/*
 * Copyright (c) 2015-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <auth_mod.h>
#include <platform_def.h>
#if USE_TBBR_DEFS
#include <tbbr_oid.h>
#else
#include <platform_oid.h>
#endif
#include <stddef.h>

/*
 * Maximum key and hash sizes (in DER format)
 */
#define PK_DER_LEN			294
#define HASH_DER_LEN			51
#define KEY_DER_LEN			50

/*
 * The platform must allocate buffers to store the authentication parameters
 * extracted from the certificates. In this case, because of the way the CoT is
 * established, we can reuse some of the buffers on different stages
 */
static unsigned char tb_fw_hash_buf[HASH_DER_LEN];
static unsigned char bld_hash_buf[HASH_DER_LEN];
static unsigned char soc_fw_hash_buf[HASH_DER_LEN];
static unsigned char tos_fw_hash_buf[HASH_DER_LEN];
static unsigned char bl33_hash_buf[HASH_DER_LEN];
static unsigned char bld1_pk_buf[PK_DER_LEN];
static unsigned char bld_legacy_pk_buf[PK_DER_LEN];
static unsigned char content_pk_buf[PK_DER_LEN];
static unsigned char cv_trusted_pk_buf[PK_DER_LEN];
static unsigned char cv_non_trusted_pk_buf[PK_DER_LEN];

unsigned char cv_non_trusted_ek_buf[KEY_DER_LEN] __aligned(16);
unsigned char bl33_ek_buf[KEY_DER_LEN] __aligned(16);

/*
 * Parameter type descriptors
 */
static auth_param_type_desc_t trusted_nv_ctr = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_NV_CTR, TRUSTED_FW_NVCOUNTER_OID);
static auth_param_type_desc_t non_trusted_nv_ctr = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_NV_CTR, NON_TRUSTED_FW_NVCOUNTER_OID);

static auth_param_type_desc_t subject_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, 0);
static auth_param_type_desc_t sig = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_SIG, 0);
static auth_param_type_desc_t sig_alg = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_SIG_ALG, 0);
static auth_param_type_desc_t raw_data = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_RAW_DATA, 0);

static auth_param_type_desc_t bld1_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, BLD1_PK_OID);
static auth_param_type_desc_t bld_legacy_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, BLD_LEGACY_PK_OID);

static auth_param_type_desc_t bld_content_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, BLD_CONTENT_CERT_PK_OID);


static auth_param_type_desc_t cv_trusted_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, CV_TRUSTED_PK_OID);
static auth_param_type_desc_t cv_non_trusted_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, CV_NON_TRUSTED_PK_OID);
static auth_param_type_desc_t cv_non_trusted_ek = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_EK, CV_NON_TRUSTED_EK_OID);

static auth_param_type_desc_t soc_fw_content_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, SOC_FW_CONTENT_CERT_PK_OID);
static auth_param_type_desc_t tos_fw_content_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, TRUSTED_OS_FW_CONTENT_CERT_PK_OID);
static auth_param_type_desc_t bl33_content_pk = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_PUB_KEY, NON_TRUSTED_FW_CONTENT_CERT_PK_OID);
static auth_param_type_desc_t bl33_ek = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_EK, BL33_EK_OID);

static auth_param_type_desc_t tb_fw_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, TRUSTED_BOOT_FW_HASH_OID);
static auth_param_type_desc_t bld_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, BLD_HASH_OID);
static auth_param_type_desc_t soc_fw_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, SOC_AP_FW_HASH_OID);
static auth_param_type_desc_t tos_fw_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, TRUSTED_OS_FW_HASH_OID);
static auth_param_type_desc_t bl33_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, NON_TRUSTED_WORLD_BOOTLOADER_HASH_OID);
static auth_param_type_desc_t scp_bl2u_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, SCP_FWU_CFG_HASH_OID);
static auth_param_type_desc_t bl2u_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, AP_FWU_CFG_HASH_OID);
static auth_param_type_desc_t ns_bl2u_hash = AUTH_PARAM_TYPE_DESC(
		AUTH_PARAM_HASH, FWU_HASH_OID);

/*
 * TBBR Chain of trust definition
 */
static const auth_img_desc_t cot_desc[] = {
	/*
	 * BL2
	 */
	[TRUSTED_BOOT_FW_CERT_ID] = {
		.img_id = TRUSTED_BOOT_FW_CERT_ID,
		.img_type = IMG_CERT,
		.parent = NULL,
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &subject_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &tb_fw_hash,
				.data = {
					.ptr = (void *)tb_fw_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			}
		}
	},
	[BL2_IMAGE_ID] = {
		.img_id = BL2_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[TRUSTED_BOOT_FW_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &tb_fw_hash,
				}
			}
		}
	},
	/*
	 * BLD key certificate
	 */
	[BLD1_KEY_CERT_ID] = {
		.img_id = BLD1_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = NULL,
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &subject_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &bld1_pk,
				.data = {
					.ptr = (void *)bld1_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			},
			[1] = {
				.type_desc = &bld_legacy_pk,
				.data = {
					.ptr = (void *)bld_legacy_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			}
		}
	},
	/*
	 * BLD FW
	 */
	[BLD2_KEY_CERT_ID] = {
		.img_id = BLD2_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[BLD1_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &bld1_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &bld_content_pk,
				.data = {
					.ptr = (void *)content_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			}
		}
	},
	[BLD_CONTENT_CERT_ID] = {
		.img_id = BLD_CONTENT_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[BLD2_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &bld_content_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &bld_hash,
				.data = {
					.ptr = (void *)bld_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			}
		}
	},
	[BLD_IMAGE_ID] = {
		.img_id = BLD_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[BLD_CONTENT_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &bld_hash,
				}
			}
		}
	},
	/*
	 * CV Trusted Key Certificate
	 */
	[CV_TRUSTED_KEY_CERT_ID] = {
		.img_id = CV_TRUSTED_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = NULL,
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &subject_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &cv_trusted_pk,
				.data = {
					.ptr = (void *)cv_trusted_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			}
		}
	},
	/*
	 * SoC Firmware
	 */
	[SOC_FW_KEY_CERT_ID] = {
		.img_id = SOC_FW_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[CV_TRUSTED_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &cv_trusted_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &soc_fw_content_pk,
				.data = {
					.ptr = (void *)content_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			}
		}
	},
	[SOC_FW_CONTENT_CERT_ID] = {
		.img_id = SOC_FW_CONTENT_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[SOC_FW_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &soc_fw_content_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &soc_fw_hash,
				.data = {
					.ptr = (void *)soc_fw_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			}
		}
	},
	[BL31_IMAGE_ID] = {
		.img_id = BL31_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[SOC_FW_CONTENT_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &soc_fw_hash,
				}
			}
		}
	},
	/*
	 * Trusted OS Firmware
	 */
	[TRUSTED_OS_FW_KEY_CERT_ID] = {
		.img_id = TRUSTED_OS_FW_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[CV_TRUSTED_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &cv_trusted_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &tos_fw_content_pk,
				.data = {
					.ptr = (void *)content_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			}
		}
	},
	[TRUSTED_OS_FW_CONTENT_CERT_ID] = {
		.img_id = TRUSTED_OS_FW_CONTENT_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[TRUSTED_OS_FW_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &tos_fw_content_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &trusted_nv_ctr,
					.plat_nv_ctr = &trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &tos_fw_hash,
				.data = {
					.ptr = (void *)tos_fw_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			}
		}
	},
	[BL32_IMAGE_ID] = {
		.img_id = BL32_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[TRUSTED_OS_FW_CONTENT_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &tos_fw_hash,
				}
			}
		}
	},
	/*
	 * Non-Trusted Firmware
	 */
	[CV_NON_TRUSTED_KEY_CERT_ID] = {
		.img_id = CV_NON_TRUSTED_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = NULL,
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &subject_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &non_trusted_nv_ctr,
					.plat_nv_ctr = &non_trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &cv_non_trusted_pk,
				.data = {
					.ptr = (void *)cv_non_trusted_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			},
			[1] = {
				.type_desc = &cv_non_trusted_ek,
				.data = {
					.ptr = (void *)cv_non_trusted_ek_buf,
					.len = (unsigned int)KEY_DER_LEN
				}
			}
		}
	},
	[BL33_KEY_CERT_ID] = {
		.img_id = BL33_KEY_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[CV_NON_TRUSTED_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &cv_non_trusted_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &non_trusted_nv_ctr,
					.plat_nv_ctr = &non_trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &bl33_content_pk,
				.data = {
					.ptr = (void *)content_pk_buf,
					.len = (unsigned int)PK_DER_LEN
				}
			},
			[1] = {
				.type_desc = &bl33_ek,
				.data = {
					.ptr = (void *)bl33_ek_buf,
					.len = (unsigned int)KEY_DER_LEN
				}
			}
		}
	},
	[BL33_CONTENT_CERT_ID] = {
		.img_id = BL33_CONTENT_CERT_ID,
		.img_type = IMG_CERT,
		.parent = &cot_desc[BL33_KEY_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &bl33_content_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			},
			[1] = {
				.type = AUTH_METHOD_NV_CTR,
				.param.nv_ctr = {
					.cert_nv_ctr = &non_trusted_nv_ctr,
					.plat_nv_ctr = &non_trusted_nv_ctr
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &bl33_hash,
				.data = {
					.ptr = (void *)bl33_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			}
		}
	},
	[BL33_IMAGE_ID] = {
		.img_id = BL33_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[BL33_CONTENT_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &bl33_hash,
				}
			}
		}
	},
	/*
	 * FWU auth descriptor.
	 */
	[FWU_CERT_ID] = {
		.img_id = FWU_CERT_ID,
		.img_type = IMG_CERT,
		.parent = NULL,
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_SIG,
				.param.sig = {
					.pk = &subject_pk,
					.sig = &sig,
					.alg = &sig_alg,
					.data = &raw_data,
				}
			}
		},
		.authenticated_data = {
			[0] = {
				.type_desc = &scp_bl2u_hash,
				.data = {
					.ptr = (void *)bld_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			},
			[1] = {
				.type_desc = &bl2u_hash,
				.data = {
					.ptr = (void *)tb_fw_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			},
			[2] = {
				.type_desc = &ns_bl2u_hash,
				.data = {
					.ptr = (void *)bl33_hash_buf,
					.len = (unsigned int)HASH_DER_LEN
				}
			}
		}
	},
	/*
	 * SCP_BL2U
	 */
	[SCP_BL2U_IMAGE_ID] = {
		.img_id = SCP_BL2U_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[FWU_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &scp_bl2u_hash,
				}
			}
		}
	},
	/*
	 * BL2U
	 */
	[BL2U_IMAGE_ID] = {
		.img_id = BL2U_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[FWU_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &bl2u_hash,
				}
			}
		}
	},
	/*
	 * NS_BL2U
	 */
	[NS_BL2U_IMAGE_ID] = {
		.img_id = NS_BL2U_IMAGE_ID,
		.img_type = IMG_RAW,
		.parent = &cot_desc[FWU_CERT_ID],
		.img_auth_methods = {
			[0] = {
				.type = AUTH_METHOD_HASH,
				.param.hash = {
					.data = &raw_data,
					.hash = &ns_bl2u_hash,
				}
			}
		}
	}
};

/* Register the CoT in the authentication module */
REGISTER_COT(cot_desc);
