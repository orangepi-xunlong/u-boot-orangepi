
#include <common.h>
#include <asm/io.h>
#include <asm/arch/twi.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/gpio.h>

#define	I2C_WRITE		0
#define I2C_READ		1

#define I2C_OK			0
#define I2C_NOK			1
#define I2C_NACK		2
#define I2C_NOK_LA		3	/* Lost arbitration */
#define I2C_NOK_TOUT	4	/* time out */

#define I2C_START_TRANSMIT     0x08
#define I2C_RESTART_TRANSMIT   0x10
#define I2C_ADDRWRITE_ACK	   0x18
#define I2C_ADDRREAD_ACK	   0x40
#define I2C_DATAWRITE_ACK      0x28
#define I2C_READY			   0xf8
#define I2C_DATAREAD_NACK	   0x58
#define I2C_DATAREAD_ACK	   0x50
/* status or interrupt source */
/*------------------------------------------------------------------------------
* Code   Status
* 00h    Bus error
* 08h    START condition transmitted
* 10h    Repeated START condition transmitted
* 18h    Address + Write bit transmitted, ACK received
* 20h    Address + Write bit transmitted, ACK not received
* 28h    Data byte transmitted in master mode, ACK received
* 30h    Data byte transmitted in master mode, ACK not received
* 38h    Arbitration lost in address or data byte
* 40h    Address + Read bit transmitted, ACK received
* 48h    Address + Read bit transmitted, ACK not received
* 50h    Data byte received in master mode, ACK transmitted
* 58h    Data byte received in master mode, not ACK transmitted
* 60h    Slave address + Write bit received, ACK transmitted
* 68h    Arbitration lost in address as master, slave address + Write bit received, ACK transmitted
* 70h    General Call address received, ACK transmitted
* 78h    Arbitration lost in address as master, General Call address received, ACK transmitted
* 80h    Data byte received after slave address received, ACK transmitted
* 88h    Data byte received after slave address received, not ACK transmitted
* 90h    Data byte received after General Call received, ACK transmitted
* 98h    Data byte received after General Call received, not ACK transmitted
* A0h    STOP or repeated START condition received in slave mode
* A8h    Slave address + Read bit received, ACK transmitted
* B0h    Arbitration lost in address as master, slave address + Read bit received, ACK transmitted
* B8h    Data byte transmitted in slave mode, ACK received
* C0h    Data byte transmitted in slave mode, ACK not received
* C8h    Last byte transmitted in slave mode, ACK received
* D0h    Second Address byte + Write bit transmitted, ACK received
* D8h    Second Address byte + Write bit transmitted, ACK not received
* F8h    No relevant status information or no interrupt
*-----------------------------------------------------------------------------*/



/* AXP806 */
#define   BOOT_POWER806_VERSION         	   			(0x03)
#define   BOOT_POWER806_DCDOUT_VOL          			(0x15)


static  struct sunxi_twi_reg *i2c  = NULL;

static __s32 i2c_sendbyteaddr(__u32 byteaddr)
{
	__s32  time = 0xffff;
	__u32  tmp_val;

	i2c->data = byteaddr & 0xff;
        i2c->ctl |= (0x01<<3);//write 1 to clean int flag
        while( (time--) && (!(i2c->ctl & 0x08)) );
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c->status;
	if(tmp_val != I2C_DATAWRITE_ACK)
	{
		return -I2C_DATAWRITE_ACK;
	}

	return I2C_OK;
}

static __s32 i2c_sendstart(void)
{
    __s32  time = 0xfffff;
    __u32  tmp_val;

    i2c->eft  = 0;
    i2c->srst = 1;
    i2c->ctl |= 0x20;

    while((time--)&&(!(i2c->ctl & 0x08)));
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c->status;
    if(tmp_val != I2C_START_TRANSMIT)
    {
		return -I2C_START_TRANSMIT;
    }

    return I2C_OK;
}

static __s32 i2c_sendslaveaddr(__u32 saddr,  __u32 rw)
{
	__s32  time = 0xffff;
	__u32  tmp_val;

	rw &= 1;
	i2c->data = ((saddr & 0xff) << 1)| rw;
        i2c->ctl  |= (0x01<<3);//write 1 to clean int flag
	while(( time-- ) && (!( i2c->ctl & 0x08 )));
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c->status;
	if(rw == I2C_WRITE)//+write
	{
		if(tmp_val != I2C_ADDRWRITE_ACK)
		{
			return -I2C_ADDRWRITE_ACK;
		}
	}

	else//+read
	{
		if(tmp_val != I2C_ADDRREAD_ACK)
		{
			return -I2C_ADDRREAD_ACK;
		}
	}

	return I2C_OK;
}

static __s32 i2c_sendRestart(void)
{
	__s32  time = 0xffff;
	__u32  tmp_val;
	tmp_val = i2c->ctl;

	tmp_val |= 0x20;
	i2c->ctl = tmp_val;

	while( (time--) && (!(i2c->ctl & 0x08)) );
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}

	tmp_val = i2c->status;
	if(tmp_val != I2C_RESTART_TRANSMIT)
	{
		return -I2C_RESTART_TRANSMIT;
	}

	return I2C_OK;
}

static __s32 i2c_stop(void)
{
	__s32  time = 0xffff;
	__u32  tmp_val;
	i2c->ctl |= (0x01 << 4);
        i2c->ctl |= (0x01 << 3);
        while( (time--) && (i2c->ctl & 0x10) );
	if(time <= 0)
	{
		return -I2C_NOK_TOUT;
	}
	time = 0xffff;
	while( (time--) && (i2c->status != I2C_READY) );
	tmp_val = i2c->status;
	if(tmp_val != I2C_READY)
	{
		return -I2C_NOK_TOUT;
	}

	return I2C_OK;
}

static __s32 i2c_getdata(__u8 *data_addr, __u32 data_count)
{
	__s32  time = 0xffff;
	__u32  tmp_val;
	__u32  i;
	if(data_count == 1)
	{
	        i2c->ctl |= (0x01<<3);
	        while( (time--) && (!(i2c->ctl & 0x08)) );
		if(time <= 0)
		{
	            return -I2C_NOK_TOUT;
		}
		for(time=0;time<100;time++);
		*data_addr = i2c->data;

		tmp_val = i2c->status;
		if(tmp_val != I2C_DATAREAD_NACK)
		{
	            return -I2C_DATAREAD_NACK;
		}
	}
	else
	{
		for(i=0; i< data_count - 1; i++)
		{
			time = 0xffff;
			//host should send ack every time when a data packet finished
			tmp_val = i2c->ctl | (0x01<<2);
                        tmp_val = i2c->ctl | (0x01<<3);
		        tmp_val |= 0x04;
                        i2c->ctl = tmp_val;
				//i2c->ctl |=(0x01<<3);

			while( (time--) && (!(i2c->ctl & 0x08)) );
			if(time <= 0)
			{
				return -I2C_NOK_TOUT;
			}
			for(time=0;time<100;time++);
			time = 0xffff;
			data_addr[i] = i2c->data;
			while( (time--) && (i2c->status != I2C_DATAREAD_ACK) );
			if(time <= 0)
			{
                            return -I2C_NOK_TOUT;
			}
		}

		time = 0xffff;
                i2c->ctl &= 0xFb;  //the last data packet,not send ack
                i2c->ctl |= (0x01<<3);
		while( (time--) && (!(i2c->ctl & 0x08)) );
		if(time <= 0)
		{
                    return -I2C_NOK_TOUT;
		}
		for(time=0;time<100;time++);
		data_addr[data_count - 1] = i2c->data;
		while( (time--) && (i2c->status != I2C_DATAREAD_NACK) );
		if(time <= 0)
		{
                    return -I2C_NOK_TOUT;
		}
	}

	return I2C_OK;
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int   i, ret, ret0, addrlen;
	char  *slave_reg;

	ret0 = -1;
	ret = i2c_sendstart();
	if(ret)
	{
		goto i2c_read_err_occur;
	}

	ret = i2c_sendslaveaddr(chip, I2C_WRITE);
	if(ret)
	{
		goto i2c_read_err_occur;
	}
	//send byte address
	if(alen >= 3)
	{
		addrlen = 2;
	}
	else if(alen <= 1)
	{
		addrlen = 0;
	}
	else
	{
		addrlen = 1;
	}
	slave_reg = (char *)&addr;
	for (i = addrlen; i>=0; i--)
	{
		ret = i2c_sendbyteaddr(slave_reg[i] & 0xff);
		if(ret)
		{
			goto i2c_read_err_occur;
		}
	}
	ret = i2c_sendRestart();
	if(ret)
	{
		goto i2c_read_err_occur;
	}
	ret = i2c_sendslaveaddr(chip, I2C_READ);
	if(ret)
	{
		goto i2c_read_err_occur;
	}
	//get data
	ret = i2c_getdata(buffer, len);
	if(ret)
	{
		goto i2c_read_err_occur;
	}
	ret0 = 0;

i2c_read_err_occur:
	i2c_stop();

	return ret0;
}




static __s32 i2c_senddata(__u8  *data_addr, __u32 data_count)
{
    __s32  time = 0xffff;
    __u32  i;

	for(i=0; i< data_count; i++)
	{
		time = 0xffff;
	        i2c->data = data_addr[i];
#if defined(CONFIG_ARCH_SUN5I)|defined(CONFIG_ARCH_SUN7I)
		i2c->ctl &= 0xF7;
#else
                i2c->ctl |= (0x01<<3);
#endif
                while( (time--) && (!(i2c->ctl & 0x08)) );
		if(time <= 0)
		{
                    return -I2C_NOK_TOUT;
		}
		time = 0xffff;
		while( (time--) && (i2c->status != I2C_DATAWRITE_ACK) );
                if(time <= 0)
		{
                    return -I2C_NOK_TOUT;
		}
	}

	return I2C_OK;
}


int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int   i, ret, ret0, addrlen;
	char  *slave_reg;

	ret0 = -1;
	ret = i2c_sendstart();
	if(ret)
	{
		goto i2c_write_err_occur;
	}

	ret = i2c_sendslaveaddr(chip, I2C_WRITE);
	if(ret)
	{
		goto i2c_write_err_occur;
	}
	//send byte address
	if(alen >= 3)
	{
		addrlen = 2;
	}
	else if(alen <= 1)
	{
		addrlen = 0;
	}
	else
	{
		addrlen = 1;
	}
	slave_reg = (char *)&addr;
	for (i = addrlen; i>=0; i--)
	{
		ret = i2c_sendbyteaddr(slave_reg[i] & 0xff);
		if(ret)
		{
			goto i2c_write_err_occur;
		}
	}

	ret = i2c_senddata(buffer, len);
	if(ret)
	{
		goto i2c_write_err_occur;
	}
	ret0 = 0;

	i2c_write_err_occur:
	i2c_stop();

	return ret0;
}

static inline int axp_i2c_write(unsigned char chip, unsigned char addr, unsigned char data)
{
	return i2c_write(chip, addr, 1, &data, 1);
}

static inline int axp_i2c_read(unsigned char chip, unsigned char addr, unsigned char *buffer)
{
	return i2c_read(chip, addr, 1, buffer, 1);
}


int set_cpus_i2c_clock(unsigned int onoff)
{
	int reg_value = 0;

	//opne CPUS PIO
	volatile unsigned int reg_val;
	// R_GPIO reset deassert
	reg_val = readl(SUNXI_RPRCM_BASE+0xb0);
	reg_val |= 1;
	writel(reg_val, SUNXI_RPRCM_BASE+0xb0);
	// R_GPIO GATING open
	reg_val = readl(SUNXI_RPRCM_BASE+0x28);
	reg_val |= 1;
	writel(reg_val, SUNXI_RPRCM_BASE+0x28);

	//R_GPIO: PL0,PL1 cfg 2
	writel(readl(0x01F02C00)& ~0xff,0x01F02C00);
	writel(readl(0x01F02C00)|0x22,0x01F02C00);
	//PL0,PL1 pull up 1
	writel(readl(0x01F02C00+0x1C)& ~0xf,0x01F02C00+0x1C);
	writel(readl(0x01F02C00+0x1C)|0x5,0x01F02C00+0x1C);
	//PL0,PL1 drv 2
	writel(readl(0x01F02C00+0x14)& ~0xf,0x01F02C00+0x14);
	writel(readl(0x01F02C00+0x14)|0xa,0x01F02C00+0x14);

	//deassert twi reset
	reg_value = readl(SUNXI_RPRCM_BASE + 0xb0);
	reg_value &= ~(0x01 << 6);
	writel(reg_value,SUNXI_RPRCM_BASE + 0xb0);
	reg_value = readl(SUNXI_RPRCM_BASE + 0xb0);
	reg_value |= 0x01 << 6;
	writel(reg_value,SUNXI_RPRCM_BASE + 0xb0);
	__usdelay(10);

	//open twi gating
	reg_value = readl(SUNXI_RPRCM_BASE + 0x28);
	if(onoff)
		reg_value |= 0x01 << 6;
	else
		reg_value &= ~(0x01 << 6);

	writel(reg_value,SUNXI_RPRCM_BASE + 0x28);
	__usdelay(10);
	return 0;

}


void i2c_set_clock(int speed, int slaveaddr)
{
	int i, clk_n, clk_m;
	    /* reset i2c control  */
	i = 0xffff;
	i2c->srst = 1;
	while((i2c->srst) && (i))
	{
	    i --;
	}
	if((i2c->lcr & 0x30) != 0x30 )
	{
	    /* toggle I2CSCL until bus idle */
	    i2c->lcr = 0x05;
	    __usdelay(500);
	    i = 10;
		    while ((i > 0) && ((i2c->lcr & 0x02) != 2))
		    {
			    i2c->lcr |= 0x08;
			    __usdelay(1000);
			    i2c->lcr &= ~0x08;
			    __usdelay(1000);
			    i--;
		    }
		    i2c->lcr = 0x0;
		    __usdelay(500);
	}

	if(speed < 100)
	{
	    speed = 100;
	}
	else if(speed > 400)
	{
	    speed = 400;
	}
	clk_n = 1;
	clk_m = (24000/10)/((2^clk_n) * speed) - 1;

	i2c->clk = (clk_m<<3) | clk_n;
	i2c->ctl = 0x40;
	i2c->eft = 0;

}

void i2c_init_cpus(int speed, int slaveaddr)
{
	i2c = (struct sunxi_twi_reg *)SUNXI_RTWI_BASE;
	set_cpus_i2c_clock(1);
	i2c_set_clock(speed,slaveaddr);
	return ;
}


static int axp_probe(void)
{
	u8	  pmu_type;

	if(axp_i2c_read(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_VERSION, &pmu_type))
	{
		printf("axp read error\n");
		return -1;
	}
	pmu_type &= 0xCF;
	if(pmu_type == 0x40)
	{
		return 0;
	}
	return -1;
}

static int set_dcdcd_voltage(int vol)
{
	int ret = -1;
	unsigned char  vol_set;

	if(vol>=1600)
	{
		vol_set = (vol - 1500) / 100;
		vol_set +=45 ;
	}
	else
	{
		vol_set=(vol - 600)/20;
	}

	ret = axp_i2c_write(CONFIG_SYS_I2C_SLAVE,BOOT_POWER806_DCDOUT_VOL,vol_set);
	printf("PMU:Set DDR Vol %dV %s.\n", vol,ret==0?"OK":"Fail");
	set_cpus_i2c_clock(0);
	return ret;
}

int set_ddr_voltage(int vol)
{

	i2c_init_cpus(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	if( !axp_probe())
	{
		return set_dcdcd_voltage(vol);
	}
	else
	{
		printf("axp not exist\n");
		return 0;
	}
}

