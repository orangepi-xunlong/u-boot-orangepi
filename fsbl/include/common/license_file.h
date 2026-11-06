#ifndef __LICENSE_FILE_H__
#define __LICENSE_FILE_H__

#define LICENSE_FILE_DIGEST_SIZE 32
#define LICENSE_FILE_MAGIC "; LF_MAGIC"
#define LICENSE_FILE_MAGIC_SIZE 10

#define LICENSE_FILE_RUNTIME_SIZE 0x100000

#define LICENSE_FILE_IMAGE_DESC {				\
	.image_id = CV_LICENSE_FILE_IMAGE_ID,		\
	SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,	\
		VERSION_1, image_info_t, 0),		\
	.image_info.image_base = LICENSE_FILE_BUF_BASE,		\
	SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,	\
		VERSION_1, entry_point_info_t, SECURE),	\
	.ep_info.pc = LICENSE_FILE_BUF_BASE,				\
}

extern const __aligned(16) unsigned char cv_lf_key[];

#endif /* __LICENSE_FILE_H__ */
