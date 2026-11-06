/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __TPU_SECURITY_H__
#define __TPU_SECURITY_H__

#include <tpu_security_def.h>

typedef struct {
	void *buf;
	size_t size;
	uint8_t tag[AI_MODEL_TAG_SIZE];
	uint8_t nonce[AI_MODEL_NONCE_SIZE];
} aimodel_buf_t;

typedef struct {
	aimodel_buf_t cmd;
	aimodel_buf_t weight;
	uint8_t vdr_ai_pwd[AI_MODEL_RSA_N_SIZE];
	uint8_t sig_vdr_ai_pwd[AI_MODEL_RSA_N_SIZE];
} aimodel_t;

typedef struct {
	uint8_t ai_mdl_ek[AI_MODEL_KEY_SIZE];

	uint8_t vdr_ai_ek[AI_MODEL_KEY_SIZE];

	uint8_t vdr_ai_N[AI_MODEL_RSA_N_SIZE];
	uint8_t vdr_ai_E[AI_MODEL_RSA_E_SIZE];

	uint8_t cv_ai_N[AI_MODEL_RSA_N_SIZE];
	uint8_t cv_ai_P[AI_MODEL_RSA_N_SIZE / 2];
	uint8_t cv_ai_Q[AI_MODEL_RSA_N_SIZE / 2];
	uint8_t cv_ai_D[AI_MODEL_RSA_N_SIZE];
	uint8_t cv_ai_E[AI_MODEL_RSA_E_SIZE];
} tpu_keys_t;

int tpu_set_aimodel_keys(tpu_keys_t *keys);
int tpu_aimodel_decrypt_verify(aimodel_t *model, uint8_t *output_cmd, uint8_t *weight_cmd);

#endif /* __TPU_SECURITY_H__ */
