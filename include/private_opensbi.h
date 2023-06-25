/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef __OPENSBI_H
#define __OPENSBI_H

#define FW_DYNAMIC_INFO_MAGIC_VALUE 0x4942534f
#define FW_DYNAMIC_INFO_NEXT_MODE_S 0x1

/** Representation dynamic info passed by previous booting stage */
struct fw_dynamic_info {
	/** Info magic */
	unsigned long magic;
	/** Info version */
	unsigned long version;
	/** Next booting stage address */
	unsigned long next_addr;
	/** Next booting stage mode */
	unsigned long next_mode;
	/** Options for OpenSBI library */
	unsigned long options;
	/**
	* Preferred boot HART id
	*
	* It is possible that the previous booting stage uses same link
	* address as the FW_DYNAMIC firmware. In this case, the relocation
	* lottery mechanism can potentially overwrite the previous booting
	* stage while other HARTs are still running in the previous booting
	* stage leading to boot-time crash. To avoid this boot-time crash,
	* the previous booting stage can specify last HART that will jump
	* to the FW_DYNAMIC firmware as the preferred boot HART.
	*
	* To avoid specifying a preferred boot HART, the previous booting
	* stage can set it to -1UL which will force the FW_DYNAMIC firmware
	* to use the relocation lottery mechanism.
	*/
	unsigned long boot_hart;
} __packed;

#endif
