/*
 * Copyright (c) 2011 Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <image.h>
#include <android_image.h>
#include <malloc.h>

static char andr_tmp_str[ANDR_BOOT_ARGS_SIZE + 1];

/*
*************************************************************************
*
*                       function
*
*   name        :   check_env_in_cmdline
*
*   paremeters  :
*                   cmdline : cmdline buf
*                   env_name: env name
*
*   description :   check if the env_name is in cmdline
*
*   return      :
*                   0       : this env not in cmdline
*                   1       : this env already in cmdline
*
*************************************************************************
*/
#if 0
/*still have bugs when using strtok*/
int check_env_in_cmdline(char * cmdline, char * env_name)
{
    char * name = NULL;
    char * tmpbuf = NULL;
    char * cmdline_tmp = NULL;

    cmdline_tmp = (char *)malloc(sizeof(char) * 2048);
    if(cmdline_tmp == NULL)
    {
        printf("In %s : malloc fail\n", __func__);
        return -1;
    }
    memset(cmdline_tmp, 0, 2048);
    strncpy(cmdline_tmp, cmdline, strlen(cmdline));

    tmpbuf = strtok(cmdline_tmp, " =");
    while( tmpbuf != NULL)
    {

        name = tmpbuf;
        if(!strcmp(name, env_name))
        {
            debug("find the env: %s in cmdline\n", name);
            free(cmdline_tmp);
            return 1;
        }
        tmpbuf = strtok(NULL, " =");
        tmpbuf = strtok(NULL, " =");
    }

    free(cmdline_tmp);
    return 0;
}
#endif
int check_env_in_cmdline(char * cmdline, char * env_name)
{
    char *src = cmdline;
    char *p = NULL;
    char tmp[1024];
    int i = 0;
    memset(tmp, 0 , 1024);

    for( p = src; *p != '\0'; p++)
    {
        if( *p == '=' )
        {
            tmp[i++] = '\0';
            debug("In %s : tmp = %s, env_name = %s\n", __func__, tmp, env_name);
            if(!strcmp(tmp, env_name))
                return 1;

            i = 0;
            memset(tmp, 0 ,1024);
            while(*p != ' ')
                p++;

            continue;
        }

        tmp[i++] = *p;
    }

    return 0;
}

/*
 *************************************************************************
 *
 *                          function
 *
 *   name        :   update_cmdline_value
 *
 *   paremeters  :
 *                   cmdline : cmdline buf
 *                   env_name: env name
 *                   env_val : env value
 *
 *   description :   use env_val to udpate env_name' value in cmdline
 *
 *   return      :
 *
 *************************************************************************
 */
int update_cmdline_value(char *cmdline, char *env_name, char *env_val)
{
    char * name = NULL;
    char * val = NULL;
    char * tmpbuf = NULL;
    char * cmdline_tmp = NULL;
    char cmdline_store[2048] = {0};

    cmdline_tmp = (char *)malloc(sizeof(char) * 2048);
    if(cmdline_tmp == NULL)
    {
        printf("In %s : malloc fail\n", __func__);
        return -1;
    }
    memset(cmdline_tmp, 0, 2048);
    strncpy(cmdline_tmp, cmdline, strlen(cmdline));

    tmpbuf = strtok(cmdline_tmp, " ");
    while(tmpbuf != NULL)
    {

        name = strtok(tmpbuf, "=");
        val = tmpbuf + strlen(name) + 1;
        strcat((char *)cmdline_store, (char *)name);
        strcat((char *)cmdline_store, "=");

        if(!strncmp(name, env_name, strlen(env_name)))
        {
            debug("update env: %s wirh val: %s\n", env_name, env_val);
            strcat((char *)cmdline_store, (char *)env_val);
        }
        else
        {
            strcat((char *)cmdline_store, (char *)val);
        }

        tmpbuf = tmpbuf + strlen(name) + 1 + strlen(val) + 1;
        tmpbuf = strtok(tmpbuf, " ");
        if(tmpbuf != NULL)
            strcat((char *)cmdline_store, " ");
    }

    cmdline_store[strlen(cmdline_store) + 1] = '\0';
    strcpy(cmdline, cmdline_store);

    free(cmdline_tmp);
    return 0;
}

/*
 *******************************************************************************************************
 *
 *                                  function
 *
 *   name       :  update_bootargs_with_android_cmdline
 *
 *   paremeters :
 *                 env_cmdline     : defined by env.cfg,at this point,it means bootargs in environmment
 *                 android_cmdline : defined by BOARD_KERNEL_CMDLINE in $cdevice/BoardConfig.mk
 *
 *   description:  use android_cmdline to update bootargs
 *
 *   return     :  return new bootargs
 *
 *******************************************************************************************************
 */
char * update_bootargs_with_android_cmdline(char * env_cmdline, char * android_cmdline)
{
    char * tmpbuf = NULL;
    char * cmdline = NULL;
    char * env_name = NULL;
    char * env_val = NULL;

    cmdline = (char *)malloc(sizeof(char) * 2048);
    if(cmdline == NULL)
    {
        printf("In %s : malloc fail\n", __func__);
        return NULL;
    }
    memset(cmdline, 0, 2048);
    strncpy(cmdline , env_cmdline, strlen(env_cmdline));

    tmpbuf = android_cmdline;
    while(*tmpbuf != '\0')
    {
        env_name = strtok(tmpbuf, " =");
        tmpbuf = tmpbuf + strlen(env_name) + 1;
        env_val = strtok(NULL, " =");
        tmpbuf = tmpbuf + strlen(env_val) + 1;
        /*
         * if android env is not in bootargs, append it to bootargs
         * if android env is already in bootrags, use it's value to update bootargs
         */
        if(!check_env_in_cmdline(env_cmdline, env_name))
        {
            strcat((char *)cmdline, " ");
            strcat((char *)cmdline, (char *)env_name);
            strcat((char *)cmdline, "=");
            strcat((char *)cmdline, (char *)env_val);
        }
        else
        {
            update_cmdline_value(cmdline, env_name, env_val);
        }
    }

    return cmdline;
}


int android_image_get_kernel(const struct andr_img_hdr *hdr, int verify,
			     ulong *os_data, ulong *os_len)
{
	/*
	 * Not all Android tools use the id field for signing the image with
	 * sha1 (or anything) so we don't check it. It is not obvious that the
	 * string is null terminated so we take care of this.
	 */
	strncpy(andr_tmp_str, hdr->name, ANDR_BOOT_NAME_SIZE);
	andr_tmp_str[ANDR_BOOT_NAME_SIZE] = '\0';
	if (strlen(andr_tmp_str))
		printf("Android's image name: %s\n", andr_tmp_str);

	printf("Kernel load addr 0x%08x size %u KiB\n",
	       hdr->kernel_addr, DIV_ROUND_UP(hdr->kernel_size, 1024));
	strncpy(andr_tmp_str, hdr->cmdline, ANDR_BOOT_ARGS_SIZE);
	andr_tmp_str[ANDR_BOOT_ARGS_SIZE] = '\0';
	if (strlen(andr_tmp_str)) {
           printf("Kernel command line: %s, use it to update bootargs\n", andr_tmp_str);
           char *str = getenv("bootargs");
           char *cmdline = NULL;
           cmdline = (char *)malloc(2048);
           if(cmdline == NULL)
           {
                printf("In %s : malloc fail\n", __func__);
                return -1;
           }
           memset(cmdline, 0, 2048);
           strncpy(cmdline , str, strlen(str));
           char * cmdline_new = update_bootargs_with_android_cmdline(cmdline, andr_tmp_str);
           setenv("bootargs", cmdline_new);
           free(cmdline);
           free(cmdline_new);
	}
	if (hdr->ramdisk_size)
		printf("RAM disk load addr 0x%08x size %u KiB\n",
		       hdr->ramdisk_addr,
		       DIV_ROUND_UP(hdr->ramdisk_size, 1024));

	if (os_data) {
		*os_data = (ulong)hdr;
		*os_data += hdr->page_size;
	}
	if (os_len)
		*os_len = hdr->kernel_size;
	return 0;
}

int android_image_check_header(const struct andr_img_hdr *hdr)
{
	return memcmp(ANDR_BOOT_MAGIC, hdr->magic, ANDR_BOOT_MAGIC_SIZE);
}

ulong android_image_get_end(const struct andr_img_hdr *hdr)
{
	u32 size = 0;
	/*
	 * The header takes a full page, the remaining components are aligned
	 * on page boundary
	 */
	size += hdr->page_size;
	size += ALIGN(hdr->kernel_size, hdr->page_size);
	size += ALIGN(hdr->ramdisk_size, hdr->page_size);
	size += ALIGN(hdr->second_size, hdr->page_size);

	return size;
}

ulong android_image_get_kload(const struct andr_img_hdr *hdr)
{
	return hdr->kernel_addr;
}

int android_image_get_ramdisk(const struct andr_img_hdr *hdr,
			      ulong *rd_data, ulong *rd_len)
{
	if (!hdr->ramdisk_size)
		return -1;
	*rd_data = (unsigned long)hdr;
	*rd_data += hdr->page_size;
	*rd_data += ALIGN(hdr->kernel_size, hdr->page_size);

	*rd_len = hdr->ramdisk_size;
	return 0;
}
