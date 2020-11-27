/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UFDT_COMMON_H
#define UFDT_COMMON_H

#include <common.h>
#include <malloc.h>
#include <libfdt.h>

#define strtoul(s, e, b)				((unsigned long int)simple_strtoul((s), (e), (b)))
#define dto_malloc(bytes)				malloc(bytes)
#define dto_free(buf)					free(buf)
#define dto_memcpy(dest, src, n)			memcpy((dest), (src), (n))
#define dto_memset(s, v, n)				memset((s), (v), (n))
#define dto_memchr(s, c, n)				memchr((s), (c), (n))
#define dto_strlen(s)					strlen(s)
#define dto_strcmp(s, d)				strcmp((s), (d))
#define dto_strncmp(s, d, count)			strncmp((s), (d), (count))
#define dto_qsort(base, nel, width, comp)		qsort((base), (nel), (width), (comp))

#define	UFDT_DEBUG
#define dto_error(fmt, args...)				pr_msg("[ufdt]: "fmt, ##args)

#ifdef UFDT_DEBUG
#define dto_print(fmt, args...)				pr_msg("[ufdt]: "fmt, ##args)
#define dto_debug(fmt, args...)				pr_msg("[ufdt]: "fmt, ##args)
#else
#define dto_print(fmt, args...)				{}
#define dto_debug(fmt, args...)				{}
#endif
#endif /* UFDT_COMMON_H */
