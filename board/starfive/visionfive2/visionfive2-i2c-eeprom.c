/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 Red Hat, Inc. All Rights Reserved.
 * Written by Wei Fu (wefu@redhat.com)
 */
 

#include <common.h>
#include <command.h>
#include <env.h>
#include <i2c.h>
#include <init.h>
#include <linux/ctype.h>
#include <linux/delay.h>

#define CONFIG_SYS_EEPROM_BUS_NUM		0

#define FORMAT_VERSION				0x2
#define PCB_VERSION				0xB1
#define BOM_VERSION				'A'
/*
 * BYTES_PER_EEPROM_PAGE: the 24FC04H datasheet says that data can
 * only be written in page mode, which means 16 bytes at a time:
 * 16-Byte Page Write Buffer
 */
#define BYTES_PER_EEPROM_PAGE			16

/*
 * EEPROM_WRITE_DELAY_MS: the 24FC04H datasheet says it takes up to
 * 5ms to complete a given write:
 * Write Cycle Time (byte or page) ro Page Write Time 5 ms, Maximum
 */
#define EEPROM_WRITE_DELAY_MS			5000
/*
 * StarFive OUI. Registration Date is 20xx-xx-xx
 */
#define STARFIVE_OUI_PREFIX			"6C:CF:39:"
#define STARFIVE_DEFAULT_MAC0			{0x6c, 0xcf, 0x39, 0x6c, 0xde, 0xad}
#define STARFIVE_DEFAULT_MAC1			{0x6c, 0xcf, 0x39, 0x7c, 0xae, 0x5d}

/* Magic number at the first four bytes of EEPROM HATs */
#define STARFIVE_EEPROM_HATS_SIG	"SFVF" /* StarFive VisionFive */

#define STARFIVE_EEPROM_HATS_SIZE_MAX	256 /* Header + Atom1&4(v1) */
#define STARFIVE_EEPROM_WP_OFFSET	0 /* Read only field */
#define STARFIVE_EEPROM_ATOM1_PSTR	"VF7110A1-2228-D008E000-00000001\0"
#define STARFIVE_EEPROM_ATOM1_PSTR_SIZE	32
#define STARFIVE_EEPROM_ATOM1_SN_OFFSET	23
#define STARFIVE_EEPROM_ATOM1_VSTR	"StarFive Technology Co., Ltd.\0\0\0"
#define STARFIVE_EEPROM_ATOM1_VSTR_SIZE	32

/*
 * MAGIC_NUMBER_BYTES: number of bytes used by the magic number
 */
#define MAGIC_NUMBER_BYTES			4

/*
 * MAC_ADDR_BYTES: number of bytes used by the Ethernet MAC address
 */
#define MAC_ADDR_BYTES				6

/*
 * MAC_ADDR_STRLEN: length of mac address string
 */
#define MAC_ADDR_STRLEN				17

/*
 * Atom Types
 * 0x0000 = invalid
 * 0x0001 = vendor info
 * 0x0002 = GPIO map
 * 0x0003 = Linux device tree blob
 * 0x0004 = manufacturer custom data
 * 0x0005-0xfffe = reserved for future use
 * 0xffff = invalid
 */

#define HATS_ATOM_INVALID	0x0000
#define HATS_ATOM_VENDOR	0x0001
#define HATS_ATOM_GPIO		0x0002
#define HATS_ATOM_DTB		0x0003
#define HATS_ATOM_CUSTOM	0x0004
#define HATS_ATOM_INVALID_END	0xffff

struct eeprom_hats_header {
	char signature[MAGIC_NUMBER_BYTES];	/* ASCII table signature */
	u8 version;		/* EEPROM data format version */
				/* (0x00 reserved, 0x01 = first version) */
	u8 reversed;		/* 0x00, Reserved field */
	u16 numatoms;		/* total atoms in EEPROM */
	u32 eeplen;		/* total length in bytes of all eeprom data */
				/* (including this header) */
};

struct eeprom_hats_atom_header {
	u16 type;
	u16 count;
	u32 dlen;
};

/**
 * static eeprom: EEPROM layout for the StarFive platform I2C format
 */
struct starfive_eeprom_atom1_data {
	u8 uuid[16];
	u16 pid;
	u16 pver;
	u8 vslen;
	u8 pslen;
	uchar vstr[STARFIVE_EEPROM_ATOM1_VSTR_SIZE];
	uchar pstr[STARFIVE_EEPROM_ATOM1_PSTR_SIZE]; /* product SN */
};

struct starfive_eeprom_atom1 {
	struct eeprom_hats_atom_header header;
	struct starfive_eeprom_atom1_data data;
	u16 crc16;
};

struct starfive_eeprom_atom4_v1_data {
	u16 version;
	u8 pcb_revision;		/* PCB version */
	u8 bom_revision;		/* BOM version */
	u8 mac0_addr[MAC_ADDR_BYTES];	/* Ethernet0 MAC */
	u8 mac1_addr[MAC_ADDR_BYTES];	/* Ethernet1 MAC */
	u8 reserved[2];
};

struct starfive_eeprom_atom4_v1 {
	struct eeprom_hats_atom_header header;
	struct starfive_eeprom_atom4_v1_data data;
	u16 crc16;
};

/* Set to 1 if we've read EEPROM into memory
 * Set to -1 if EEPROM data is wrong
 */
static int has_been_read;

/**
 * helper struct for getting info from the local EEPROM copy.
 * most of the items are pointers to the eeprom_wp_buff.
 * ONLY serialnum is the u32 from the last 8 Bytes of product string
 */
struct starfive_eeprom_info {
	char *vstr;		/* Vendor string in ATOM1 */
	char *pstr;		/* product string in ATOM1 */
	u32 serialnum;		/* serial number from in product string*/
	u16 *version;		/* custom data version in ATOM4 */
	u8 *pcb_revision;	/* PCB version in ATOM4 */
	u8 *bom_revision;	/* BOM version in ATOM4 */
	u8 *mac0_addr;		/* Ethernet0 MAC in ATOM4 */
	u8 *mac1_addr;		/* Ethernet1 MAC in ATOM4 */
};
static struct starfive_eeprom_info einfo;


static uchar eeprom_wp_buff[STARFIVE_EEPROM_HATS_SIZE_MAX];
static struct eeprom_hats_header starfive_eeprom_hats_header_default = {
	.signature = STARFIVE_EEPROM_HATS_SIG,
	.version = FORMAT_VERSION,
	.numatoms = 2,
	.eeplen = sizeof(struct eeprom_hats_header) +
		  sizeof(struct starfive_eeprom_atom1) +
		  sizeof(struct starfive_eeprom_atom4_v1)
};
static struct starfive_eeprom_atom1 starfive_eeprom_atom1_default = {
	.header = {
		.type = HATS_ATOM_VENDOR,
		.count = 1,
		.dlen = sizeof(struct starfive_eeprom_atom1_data) + sizeof(u16)
	},
	.data = {
		.uuid = {0},
		.pid = 0,
		.pver = 0,
		.vslen = STARFIVE_EEPROM_ATOM1_VSTR_SIZE,
		.pslen = STARFIVE_EEPROM_ATOM1_PSTR_SIZE,
		.vstr = STARFIVE_EEPROM_ATOM1_VSTR,
		.pstr = STARFIVE_EEPROM_ATOM1_PSTR
	}
};
static struct starfive_eeprom_atom4_v1 starfive_eeprom_atom4_v1_default = {
	.header = {
		.type = HATS_ATOM_CUSTOM,
		.count = 2,
		.dlen = sizeof(struct starfive_eeprom_atom4_v1_data) + sizeof(u16)
	},
	.data = {
		.version = FORMAT_VERSION,
		.pcb_revision = PCB_VERSION,
		.bom_revision = BOM_VERSION,
		.mac0_addr = STARFIVE_DEFAULT_MAC0,
		.mac1_addr = STARFIVE_DEFAULT_MAC1,
		.reserved = {0}
	}
};

//static u8 starfive_default_mac[MAC_ADDR_BYTES] = STARFIVE_DEFAULT_MAC;

/**
 * is_match_magic() - Does the magic number match that of a StarFive EEPROM?
 *
 * @hats:		the pointer of eeprom_hats_header
 * Return:		status code, 0: Yes, non-0: NO
 */
static inline int is_match_magic(char *hats)
{
	return strncmp(hats, STARFIVE_EEPROM_HATS_SIG, MAGIC_NUMBER_BYTES);
}

/**
 * calculate_crc16() - Calculate the current CRC for atom
 * Porting from  https://github.com/raspberrypi/hats, getcrc
 * @data:		the pointer of eeprom_hats_atom_header
 * @size:		total length in bytes of the entire atom
 * 			(type, count, dlen, data)
 * Return:		result: crc16 code
 */
#define CRC16 0x8005
static u16 calculate_crc16(uchar* data, unsigned int size)
{
	int i, j = 0x0001;
	u16 out = 0, crc = 0;
	int bits_read = 0, bit_flag;

	/* Sanity check: */
	if((data == NULL) || size == 0)
		return 0;

	while(size > 0) {
		bit_flag = out >> 15;

		/* Get next bit: */
		out <<= 1;
		// item a) work from the least significant bits
		out |= (*data >> bits_read) & 1;

		/* Increment bit counter: */
		bits_read++;
		if(bits_read > 7) {
			bits_read = 0;
			data++;
			size--;
		}

		/* Cycle check: */
		if(bit_flag)
			out ^= CRC16;
	}

	// item b) "push out" the last 16 bits
	for (i = 0; i < 16; ++i) {
		bit_flag = out >> 15;
		out <<= 1;
		if(bit_flag)
			out ^= CRC16;
	}

	// item c) reverse the bits
	for (i = 0x8000; i != 0; i >>=1, j <<= 1) {
		if (i & out)
			crc |= j;
	}

	return crc;
}

/* This function should be called after each update to any EEPROM ATOM */
static inline void update_crc(struct eeprom_hats_atom_header *atom)
{
	uint atom_crc_offset = sizeof(struct eeprom_hats_atom_header) +
			       atom->dlen - sizeof(u16);
	u16 *atom_crc_p = (void *) atom + atom_crc_offset;
	*atom_crc_p = calculate_crc16((uchar*) atom, atom_crc_offset);
}

/**
 * dump_raw_eeprom - display the raw contents of the EEPROM
 */
static void dump_raw_eeprom(u8 *e, unsigned int size)
{
	unsigned int i;

	printf("EEPROM dump: (0x%x bytes)\n", size);

	for (i = 0; i < size; i++) {
		if (!(i % 0x10))
			printf("%02X: ", i);

		printf("%02X ", e[i]);

		if (((i % 16) == 15) || (i == size - 1))
			printf("\n");
	}

	return;
}

static int hats_atom_crc_check(struct eeprom_hats_atom_header *atom)
{
	u16 atom_crc, data_crc;
	uint atom_crc_offset = sizeof(struct eeprom_hats_atom_header) +
			       atom->dlen - sizeof(atom_crc);
	u16 *atom_crc_p = (void *) atom + atom_crc_offset;

	atom_crc = *atom_crc_p;
	data_crc = calculate_crc16((uchar *) atom, atom_crc_offset);
	if (atom_crc == data_crc)
		return 0;

	printf("EEPROM HATs: CRC ERROR in atom %x type %x, (%x!=%x)\n",
		atom->count, atom->type, atom_crc, data_crc);
	return -1;
}

static void *hats_get_atom(struct eeprom_hats_header *header, u16 type)
 {
	struct eeprom_hats_atom_header *atom;
	void *hats_eeprom_max = (void *)header + header->eeplen;
	void *temp = (void *)header + sizeof(struct eeprom_hats_header);

	for (int numatoms = (int)header->numatoms; numatoms > 0; numatoms--) {
		atom = (struct eeprom_hats_atom_header *)temp;
		if (hats_atom_crc_check(atom))
			return NULL;
		if (atom->type == type)
			return (void *)atom;
		/* go to next atom */
		temp = (void *)atom + sizeof(struct eeprom_hats_atom_header) +
		       atom->dlen;
		if (temp > hats_eeprom_max) {
			printf("EEPROM HATs: table overflow next@%p, max@%p\n",
				temp, hats_eeprom_max);
			break;
		}
	}

	/* fail to get atom */
	return NULL;
}

/**
 * show_eeprom - display the contents of the EEPROM
 */
static void show_eeprom(struct starfive_eeprom_info *einfo)
{
	if (has_been_read != 1)
		return;

	printf("\n--------EEPROM INFO--------\n");
	printf("Vendor : %s\n", einfo->vstr);
	printf("Product full SN: %s\n", einfo->pstr);
	printf("data version: 0x%x\n", *einfo->version);
	if (2 == *einfo->version) {
		printf("PCB revision: 0x%x\n", *einfo->pcb_revision);
		printf("BOM revision: %c\n", *einfo->bom_revision);
		printf("Ethernet MAC0 address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       einfo->mac0_addr[0], einfo->mac0_addr[1],
		       einfo->mac0_addr[2], einfo->mac0_addr[3],
		       einfo->mac0_addr[4], einfo->mac0_addr[5]);
		printf("Ethernet MAC1 address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       einfo->mac1_addr[0], einfo->mac1_addr[1],
		       einfo->mac1_addr[2], einfo->mac1_addr[3],
		       einfo->mac1_addr[4], einfo->mac1_addr[5]);
	} else {
		printf("Custom data v%d is not Supported\n", *einfo->version);
	}
	printf("--------EEPROM INFO--------\n\n");
}

/**
 * parse_eeprom_info - parse the contents of the EEPROM
 * If everthing gose right,
 * 1, set has_been_read to 1
 * 2, display info
 *
 * If anything goes wrong,
 * 1, set has_been_read to -1
 * 2, dump data by hex for debug
 *
 * @buf:		the pointer of eeprom_hats_header in memory
 * Return:		status code, 0: Success, non-0: Fail
 *
 */
static int parse_eeprom_info(struct eeprom_hats_header *buf)
{
	struct eeprom_hats_atom_header *atom;
	void *atom_data;
	struct starfive_eeprom_atom1_data *atom1 = NULL;
	struct starfive_eeprom_atom4_v1_data *atom4_v1 = NULL;

	if (is_match_magic((char *)buf)) {
		printf("Not a StarFive EEPROM data format - magic error\n");
		goto error;
	};

	// parse atom1(verdor)
	atom = (struct eeprom_hats_atom_header *)
		hats_get_atom(buf, HATS_ATOM_VENDOR);
	if (atom) {
		atom_data = (void *)atom +
			    sizeof(struct eeprom_hats_atom_header);
		atom1 = (struct starfive_eeprom_atom1_data *)atom_data;
		einfo.vstr = atom1->vstr;
		einfo.pstr = atom1->pstr;
		einfo.serialnum = (u32)hextoul((void *)atom1->pstr +
					STARFIVE_EEPROM_ATOM1_SN_OFFSET,
					NULL);
	} else  {
		printf("fail to get vendor atom\n");
		goto error;
	};

	// parse atom4(custom)
	atom = (struct eeprom_hats_atom_header *)
		hats_get_atom(buf, HATS_ATOM_CUSTOM);
	if (atom) {
		atom_data = (void *)atom +
			    sizeof(struct eeprom_hats_atom_header);
		atom4_v1 = (struct starfive_eeprom_atom4_v1_data *)atom_data;
		einfo.version = &atom4_v1->version;
		if (*einfo.version == 2) {
			einfo.pcb_revision = &atom4_v1->pcb_revision;
			einfo.bom_revision = &atom4_v1->bom_revision;
			einfo.mac0_addr =  atom4_v1->mac0_addr;
			einfo.mac1_addr =  atom4_v1->mac1_addr;
		}
	} else  {
		printf("fail to get custom data atom\n");
		goto error;
	};

	// everthing gose right
	has_been_read = 1;

	return 0;

error:
	has_been_read = -1;
	return -1;
}

/**
 * read_eeprom() - read the EEPROM into memory, if it hasn't been read yet
 * @buf:		the pointer of eeprom data buff
 * Return:		status code, 0: Success, non-0: Fail
 * Note: depend on	CONFIG_SYS_EEPROM_BUS_NUM
 * 			CONFIG_SYS_I2C_EEPROM_ADDR
 * 			STARFIVE_EEPROM_WP_OFFSET
 * 			STARFIVE_EEPROM_HATS_SIZE_MAX
 */
static int read_eeprom(uint8_t *buf)
{
	int ret;
	struct udevice *dev;

	if (has_been_read == 1)
		return 0;

	ret = i2c_get_chip_for_busnum(CONFIG_SYS_EEPROM_BUS_NUM,
				      CONFIG_SYS_I2C_EEPROM_ADDR,
				      CONFIG_SYS_I2C_EEPROM_ADDR_LEN,
				      &dev);
	if (!ret) {
		ret = dm_i2c_read(dev, STARFIVE_EEPROM_WP_OFFSET,
				  buf, STARFIVE_EEPROM_HATS_SIZE_MAX);
	}

	if (ret) {
		printf("fail to read EEPROM.\n");
		return ret;
	}

	return parse_eeprom_info((struct eeprom_hats_header *)buf);
}

/**
 * prog_eeprom() - write the EEPROM from memory
 */
static int prog_eeprom(uint8_t *buf, unsigned int size)
{
	unsigned int i;
	void *p;
	uchar tmp_buff[STARFIVE_EEPROM_HATS_SIZE_MAX];
	struct udevice *dev;
	int ret = i2c_get_chip_for_busnum(CONFIG_SYS_EEPROM_BUS_NUM,
					  CONFIG_SYS_I2C_EEPROM_ADDR,
					  CONFIG_SYS_I2C_EEPROM_ADDR_LEN,
					  &dev);

	if (is_match_magic(buf)) {
		printf("MAGIC ERROR, Please check the data@%p.\n", buf);
		return -1;
	}

	for (i = 0, p = buf; i < size;
	     i += BYTES_PER_EEPROM_PAGE, p += BYTES_PER_EEPROM_PAGE) {
		if (!ret)
			ret = dm_i2c_write(dev,
					   i + STARFIVE_EEPROM_WP_OFFSET,
					   p, min((int)(size - i),
					   BYTES_PER_EEPROM_PAGE));
		if (ret)
			break;
		udelay(EEPROM_WRITE_DELAY_MS);
	}

	if (!ret) {
		/* Verify the write by reading back the EEPROM and comparing */
		ret = dm_i2c_read(dev,
				  STARFIVE_EEPROM_WP_OFFSET,
				  tmp_buff,
				  STARFIVE_EEPROM_HATS_SIZE_MAX);
		if (!ret && memcmp((void *)buf, (void *)tmp_buff,
				   STARFIVE_EEPROM_HATS_SIZE_MAX))
			ret = -1;
	}

	if (ret) {
		has_been_read = -1;
		printf("Programming failed.Temp buff:\n");
		dump_raw_eeprom(tmp_buff,
				STARFIVE_EEPROM_HATS_SIZE_MAX);
		return -1;
	}

	printf("Programming passed.\n");
	return 0;
}

/**
 * set_mac_address() - stores a MAC address into the local EEPROM copy
 *
 * This function takes a pointer to MAC address string
 * (i.e."XX:XX:XX:XX:XX:XX", where "XX" is a two-digit hex number),
 * stores it in the MAC address field of the EEPROM local copy, and
 * updates the local copy of the CRC.
 */
static void set_mac_address(char *string, int index)
{
	unsigned int i;
	struct eeprom_hats_atom_header *atom4;
	atom4 = (struct eeprom_hats_atom_header *)
		hats_get_atom((struct eeprom_hats_header *)eeprom_wp_buff,
			      HATS_ATOM_CUSTOM);

	if (strncasecmp(STARFIVE_OUI_PREFIX, string,
			strlen(STARFIVE_OUI_PREFIX))) {
		printf("The MAC address doesn't match StarFive OUI %s\n",
		       STARFIVE_OUI_PREFIX);
		return;
	}

	for (i = 0; *string && (i < MAC_ADDR_BYTES); i++) {
		if (index == 0) {
			einfo.mac0_addr[i] = hextoul(string, &string);
		} else {
			einfo.mac1_addr[i] = hextoul(string, &string);
		}
		if (*string == ':')
			string++;
	}

	update_crc(atom4);
}

/**
 * set_pcb_revision() - stores a StarFive PCB revision into the local EEPROM copy
 *
 * Takes a pointer to a string representing the numeric PCB revision in
 * decimal ("0" - "255"), stores it in the pcb_revision field of the
 * EEPROM local copy, and updates the CRC of the local copy.
 */
static void set_pcb_revision(char *string)
{
	u8 p;
	uint base = 16;
	struct eeprom_hats_atom_header *atom4;
	atom4 = (struct eeprom_hats_atom_header *)
		hats_get_atom((struct eeprom_hats_header *)eeprom_wp_buff,
			      HATS_ATOM_CUSTOM);

	p = (u8)simple_strtoul(string, NULL, base);
	if (p > U8_MAX) {
		printf("%s must not be greater than %d\n", "PCB revision",
		       U8_MAX);
		return;
	}

	*einfo.pcb_revision = p;

	update_crc(atom4);
}

/**
 * set_bom_revision() - stores a StarFive BOM revision into the local EEPROM copy
 *
 * Takes a pointer to a uppercase ASCII character representing the BOM
 * revision ("A" - "Z"), stores it in the bom_revision field of the
 * EEPROM local copy, and updates the CRC of the local copy.
 */
static void set_bom_revision(char *string)
{
	struct eeprom_hats_atom_header *atom4;
	atom4 = (struct eeprom_hats_atom_header *)
		hats_get_atom((struct eeprom_hats_header *)eeprom_wp_buff,
			      HATS_ATOM_CUSTOM);

	if (string[0] < 'A' || string[0] > 'Z') {
		printf("BOM revision must be an uppercase letter between A and Z\n");
		return;
	}

	*einfo.bom_revision = string[0];

	update_crc(atom4);
}

/**
 * set_product_id() - stores a StarFive product ID into the local EEPROM copy
 *
 * Takes a pointer to a string representing the numeric product ID  in
 * string ("VF7100A1-2150-D008E000-00000001\0"), stores it in the product string
 * field of the EEPROM local copy, and updates the CRC of the local copy.
 */
static void set_product_id(char *string)
{
	struct eeprom_hats_atom_header *atom1;
	atom1 = (struct eeprom_hats_atom_header *)
		hats_get_atom((struct eeprom_hats_header *)eeprom_wp_buff,
			      HATS_ATOM_VENDOR);

	memcpy((void *)einfo.pstr, (void *)string,
		STARFIVE_EEPROM_ATOM1_PSTR_SIZE);

	update_crc(atom1);
}

/**
 * init_local_copy() - initialize the in-memory EEPROM copy
 *
 * Initialize the in-memory EEPROM copy with the magic number.  Must
 * be done when preparing to initialize a blank EEPROM, or overwrite
 * one with a corrupted magic number.
 */
static void init_local_copy(uchar *buff)
{
	struct eeprom_hats_header *hats = (struct eeprom_hats_header *)buff;
	struct eeprom_hats_atom_header *atom1 = (void *)hats +
					sizeof(struct eeprom_hats_header);
	struct eeprom_hats_atom_header *atom4_v1 = (void *)atom1 +
					sizeof(struct starfive_eeprom_atom1);

	memcpy((void *)hats, (void *)&starfive_eeprom_hats_header_default,
	       sizeof(struct eeprom_hats_header));
	memcpy((void *)atom1, (void *)&starfive_eeprom_atom1_default,
	       sizeof(struct starfive_eeprom_atom1));
	memcpy((void *)atom4_v1, (void *)&starfive_eeprom_atom4_v1_default,
	       sizeof(struct starfive_eeprom_atom4_v1));

	update_crc(atom1);
	update_crc(atom4_v1);
}

static int print_usage(void)
{
	printf("display and program the system ID and MAC addresses in EEPROM\n"
	"[read_eeprom|initialize|write_eeprom|mac_address|pcb_revision|bom_revision|product_id]\n"
	"mac read_eeprom\n"
	"    - read EEPROM content into memory data structure\n"
	"mac write_eeprom\n"
	"    - save memory data structure to the EEPROM\n"
	"mac initialize\n"
	"    - initialize the in-memory EEPROM copy with default data\n"
	"mac mac0_address <xx:xx:xx:xx:xx:xx>\n"
	"    - stores a MAC0 address into the local EEPROM copy\n"
	"mac mac1_address <xx:xx:xx:xx:xx:xx>\n"
	"    - stores a MAC1 address into the local EEPROM copy\n"
	"mac pcb_revision <?>\n"
	"    - stores a StarFive PCB revision into the local EEPROM copy\n"
	"mac bom_revision <A>\n"
	"    - stores a StarFive BOM revision into the local EEPROM copy\n"
	"mac product_id <VF7110A1-2228-D008E000-xxxxxxxx>\n"
	"    - stores a StarFive product ID into the local EEPROM copy\n");
	return 0;
}

int do_mac(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	char *cmd;

	if (argc == 1) {
		show_eeprom(&einfo);
		return 0;
	}

	if (argc > 3)
		return print_usage();

	cmd = argv[1];

	/* Commands with no argument */
	if (!strcmp(cmd, "read_eeprom")) {
		has_been_read = 0;
		return read_eeprom(eeprom_wp_buff);
	} else if (!strcmp(cmd, "initialize")) {
		init_local_copy(eeprom_wp_buff);
		return 0;
	} else if (!strcmp(cmd, "write_eeprom")) {
		return prog_eeprom(eeprom_wp_buff,
				   STARFIVE_EEPROM_HATS_SIZE_MAX);
	}

	if (argc != 3)
		return print_usage();

	if (is_match_magic(eeprom_wp_buff)) {
		printf("Please read the EEPROM ('read_eeprom') and/or initialize the EEPROM ('initialize') first.\n");
		return 0;
	}

	if (!strcmp(cmd, "mac0_address")) {
		set_mac_address(argv[2], 0);
		return 0;
	} else if (!strcmp(cmd, "mac1_address")) {
		set_mac_address(argv[2], 1);
		return 0;
	} else if (!strcmp(cmd, "pcb_revision")) {
		set_pcb_revision(argv[2]);
		return 0;
	} else if (!strcmp(cmd, "bom_revision")) {
		set_bom_revision(argv[2]);
		return 0;
	} else if (!strcmp(cmd, "product_id")) {
		set_product_id(argv[2]);
		return 0;
	}

	return print_usage();
}

/**
 * mac_read_from_eeprom() - read the MAC address & the serial number in EEPROM
 *
 * This function reads the MAC address and the serial number from EEPROM and
 * sets the appropriate environment variables for each one read.
 *
 * The environment variables are only set if they haven't been set already.
 * This ensures that any user-saved variables are never overwritten.
 *
 * If CONFIG_ID_EEPROM is enabled, this function will be called in
 * "static init_fnc_t init_sequence_r[]" of u-boot/common/board_r.c.
 */
int mac_read_from_eeprom(void)
{
	/**
	 * try to fill the buff from EEPROM,
	 * always return SUCCESS, even some error happens.
	 */
	if (read_eeprom(eeprom_wp_buff)) {
		dump_raw_eeprom(eeprom_wp_buff, STARFIVE_EEPROM_HATS_SIZE_MAX);
		return 0;
	}

	// 1, setup ethaddr env
	eth_env_set_enetaddr("eth0addr", einfo.mac0_addr);
	eth_env_set_enetaddr("eth1addr", einfo.mac1_addr);

	/**
	 * 2, setup serial# env, reference to hifive-platform-i2c-eeprom.c,
	 * serial# can be a ASCII string, but not just a hex number, so we
	 * setup serial# in the 32Byte format:
	 * "VF7100A1-2201-D008E000-00000001;"
	 * "<product>-<date>-<DDR&eMMC>-<serial_number>"
	 * <date>: 4Byte, should be the output of `date +%y%W`
	 * <DDR&eMMC>: 8Byte, "D008" means 8GB, "D01T" means 1TB;
	 *     "E000" means no eMMC，"E032" means 32GB, "E01T" means 1TB.
	 * <serial_number>: 8Byte, the Unique Identifier of board in hex.
	 */
	if (!env_get("serial#"))
		env_set("serial#", einfo.pstr);

	printf("StarFive EEPROM format v%u\n", *einfo.version);
	show_eeprom(&einfo);
	return 0;
}

/**
 * get_pcb_revision_from_eeprom - get the PCB revision
 *
 * Read the EEPROM to determine the board revision.
 */
u8 get_pcb_revision_from_eeprom(void)
{
	u8 pv = 0xFF;

	if (read_eeprom(eeprom_wp_buff))
		return pv;

	if (einfo.pcb_revision) {
		pv = *einfo.pcb_revision;
	}
	return pv;
}

/**
 * get_data_from_eeprom
 *
 * Read data from eeprom, must use int mac_read_from_eeprom(void) first
 *
 * offset: offset of eeprom
 * len: count of data
 * data: return data
 *
 * return the len of valid data
 */
int get_data_from_eeprom(int offset, int len, unsigned char *data)
{
	int cp_len = -1;

	if (read_eeprom(eeprom_wp_buff))
		return cp_len;

	if (offset < STARFIVE_EEPROM_HATS_SIZE_MAX) {
		cp_len = (offset + len > STARFIVE_EEPROM_HATS_SIZE_MAX) ?
			(offset + len - STARFIVE_EEPROM_HATS_SIZE_MAX) : len;
		memcpy(data, &eeprom_wp_buff[offset], cp_len);
	}

	return cp_len;
}
