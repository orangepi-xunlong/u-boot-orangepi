/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h> /* size_t */
#include <stdint.h>

/*
 * Fill @count bytes of memory pointed to by @dst with @val
 */
void *memset(void *dst, int val, size_t count)
{
	char *ptr = dst;

	while (count--)
		*ptr++ = val;

	return dst;
}

/*
 * Compare @len bytes of @s1 and @s2
 */
int memcmp(const void *s1, const void *s2, size_t len)
{
	const unsigned char *s = s1;
	const unsigned char *d = s2;
	unsigned char sc;
	unsigned char dc;

	while (len--) {
		sc = *s++;
		dc = *d++;
		if (sc - dc)
			return (sc - dc);
	}

	return 0;
}

/*
 * Copy @len bytes from @src to @dst
 */
void *memcpy(void *dst, const void *src, size_t len)
{
	void *__memcpy_aarch64(void *dst, const void *src, size_t len);
#if IMAGE_BL32 && __aarch64__
	return __memcpy_aarch64(dst, src, len);
#else
	const char *s = src;
	char *d = dst;

	while (len--)
		*d++ = *s++;

	return dst;
#endif
}

/*
 * Move @len bytes from @src to @dst
 */
void *memmove(void *dst, const void *src, size_t len)
{
	/*
	 * The following test makes use of unsigned arithmetic overflow to
	 * more efficiently test the condition !(src <= dst && dst < str+len).
	 * It also avoids the situation where the more explicit test would give
	 * incorrect results were the calculation str+len to overflow (though
	 * that issue is probably moot as such usage is probably undefined
	 * behaviour and a bug anyway.
	 */
	if ((size_t)dst - (size_t)src >= len) {
		/* destination not in source data, so can safely use memcpy */
		return memcpy(dst, src, len);
	} else {
		/* copy backwards... */
		const char *end = dst;
		const char *s = (const char *)src + len;
		char *d = (char *)dst + len;
		while (d != end)
			*--d = *--s;
	}
	return dst;
}

/*
 * Scan @len bytes of @src for value @c
 */
void *memchr(const void *src, int c, size_t len)
{
	const char *s = src;

	while (len--) {
		if (*s == c)
			return (void *) s;
		s++;
	}

	return NULL;
}

void *memset_gnuc(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	size_t k;

	/* Fill head and tail with minimal branching. Each
	 * conditional ensures that all the subsequently used
	 * offsets are well-defined and in the dest region.
	 */

	if (!n)
		return dest;
	s[0] = c;
	s[n - 1] = c;
	if (n <= 2)
		return dest;
	s[1] = c;
	s[2] = c;
	s[n - 2] = c;
	s[n - 3] = c;
	if (n <= 6)
		return dest;
	s[3] = c;
	s[n - 4] = c;
	if (n <= 8)
		return dest;

	/* Advance pointer to align it at a 4-byte boundary,
	 * and truncate n to a multiple of 4. The previous code
	 * already took care of any head/tail that get cut off
	 * by the alignment.
	 */

	k = -(uintptr_t)s & 3;
	s += k;
	n -= k;
	n &= -4;

#ifdef __GNUC__
	typedef uint32_t __attribute__((__may_alias__)) u32;
	typedef uint64_t __attribute__((__may_alias__)) u64;

	u32 c32 = ((u32)-1) / 255 * (unsigned char)c;

	/* In preparation to copy 32 bytes at a time, aligned on
	 * an 8-byte bounary, fill head/tail up to 28 bytes each.
	 * As in the initial byte-based head/tail fill, each
	 * conditional below ensures that the subsequent offsets
	 * are valid (e.g. !(n<=24) implies n>=28).
	 */

	*(u32 *)(s + 0) = c32;
	*(u32 *)(s + n - 4) = c32;
	if (n <= 8)
		return dest;
	*(u32 *)(s + 4) = c32;
	*(u32 *)(s + 8) = c32;
	*(u32 *)(s + n - 12) = c32;
	*(u32 *)(s + n - 8) = c32;
	if (n <= 24)
		return dest;
	*(u32 *)(s + 12) = c32;
	*(u32 *)(s + 16) = c32;
	*(u32 *)(s + 20) = c32;
	*(u32 *)(s + 24) = c32;
	*(u32 *)(s + n - 28) = c32;
	*(u32 *)(s + n - 24) = c32;
	*(u32 *)(s + n - 20) = c32;
	*(u32 *)(s + n - 16) = c32;

	/* Align to a multiple of 8 so we can fill 64 bits at a time,
	 * and avoid writing the same bytes twice as much as is
	 * practical without introducing additional branching.
	 */

	k = 24 + ((uintptr_t)s & 4);
	s += k;
	n -= k;

	/* If this loop is reached, 28 tail bytes have already been
	 * filled, so any remainder when n drops below 32 can be
	 * safely ignored.
	 */

	u64 c64 = c32 | ((u64)c32 << 32);

	for (; n >= 32; n -= 32, s += 32) {
		*(u64 *)(s + 0) = c64;
		*(u64 *)(s + 8) = c64;
		*(u64 *)(s + 16) = c64;
		*(u64 *)(s + 24) = c64;
	}
#else
	/* Pure C fallback with no aliasing violations. */
	for (; n; n--, s++)
		*s = c;
#endif

	return dest;
}
