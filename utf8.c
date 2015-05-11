#ifdef TLF_FONTS
/*
 * Copyright (c) 2007 Alexey Vatchenko <av@bsdua.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>

#include <wchar.h>
#include <arpa/inet.h>	/* for htonl() */

#include "utf8.h"

#define _NXT	0x80
#define _SEQ2	0xc0
#define _SEQ3	0xe0
#define _SEQ4	0xf0
#define _SEQ5	0xf8
#define _SEQ6	0xfc

#define _BOM	0xfeff

static int __wchar_forbitten(wchar_t sym);
static int __utf8_forbitten(uint8_t octet);

static int
__wchar_forbitten(wchar_t sym)
{

	/* Surrogate pairs */
	if (sym >= 0xd800 && sym <= 0xdfff)
		return (-1);

	return (0);
}

static int
__utf8_forbitten(uint8_t octet)
{

	switch (octet) {
	case 0xc0:
	case 0xc1:
	case 0xf5:
	case 0xff:
		return (-1);
	}

	return (0);
}

/*
 * DESCRIPTION
 *	This function translates UTF-8 string into UCS-4 string (all symbols
 *	will be in local machine byte order).
 *
 *	It takes the following arguments:
 *	in	- input UTF-8 string. It can be null-terminated.
 *	insize	- size of input string in bytes.
 *	out	- result buffer for UCS-4 string. If out is NULL,
 *		function returns size of result buffer.
 *	outsize - size of out buffer in wide characters.
 *
 * RETURN VALUES
 *	The function returns size of result buffer (in wide characters).
 *	Zero is returned in case of error.
 *
 * CAVEATS
 *	1. If UTF-8 string contains zero symbols, they will be translated
 *	   as regular symbols.
 *	2. If UTF8_IGNORE_ERROR or UTF8_SKIP_BOM flag is set, sizes may vary
 *	   when `out' is NULL and not NULL. It's because of special UTF-8
 *	   sequences which may result in forbitten (by RFC3629) UNICODE
 *	   characters.  So, the caller must check return value every time and
 *	   not prepare buffer in advance (\0 terminate) but after calling this
 *	   function.
 */
size_t
utf8_to_wchar(const char *in, size_t insize, wchar_t *out, size_t outsize,
    int flags)
{
	uint8_t *p, *lim;
	wchar_t *wlim, high;
	size_t n, total, i, n_bits;

	if (in == NULL || insize == 0 || (outsize == 0 && out != NULL))
		return (0);

	total = 0;
	p = (uint8_t *)in;
	lim = p + insize;
	wlim = out + outsize;

	for (; p < lim; p += n) {
		if (__utf8_forbitten(*p) != 0 &&
		    (flags & UTF8_IGNORE_ERROR) == 0)
			return (0);

		/*
		 * Get number of bytes for one wide character.
		 */
		n = 1;	/* default: 1 byte. Used when skipping bytes. */
		if ((*p & 0x80) == 0)
			high = (wchar_t)*p;
		else if ((*p & 0xe0) == _SEQ2) {
			n = 2;
			high = (wchar_t)(*p & 0x1f);
		} else if ((*p & 0xf0) == _SEQ3) {
			n = 3;
			high = (wchar_t)(*p & 0x0f);
		} else if ((*p & 0xf8) == _SEQ4) {
			n = 4;
			high = (wchar_t)(*p & 0x07);
		} else if ((*p & 0xfc) == _SEQ5) {
			n = 5;
			high = (wchar_t)(*p & 0x03);
		} else if ((*p & 0xfe) == _SEQ6) {
			n = 6;
			high = (wchar_t)(*p & 0x01);
		} else {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			continue;
		}

		/* does the sequence header tell us truth about length? */
		if (lim - p <= n - 1) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			n = 1;
			continue;	/* skip */
		}

		/*
		 * Validate sequence.
		 * All symbols must have higher bits set to 10xxxxxx
		 */
		if (n > 1) {
			for (i = 1; i < n; i++) {
				if ((p[i] & 0xc0) != _NXT)
					break;
			}
			if (i != n) {
				if ((flags & UTF8_IGNORE_ERROR) == 0)
					return (0);
				n = 1;
				continue;	/* skip */
			}
		}

		total++;

		if (out == NULL)
			continue;

		if (out >= wlim)
			return (0);		/* no space left */

		*out = 0;
		n_bits = 0;
		for (i = 1; i < n; i++) {
			*out |= (wchar_t)(p[n - i] & 0x3f) << n_bits;
			n_bits += 6;		/* 6 low bits in every byte */
		}
		*out |= high << n_bits;

		if (*out == 0)			/* return at end of string */
			break;

		if (__wchar_forbitten(*out) != 0) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);	/* forbitten character */
			else {
				total--;
				out--;
			}
		} else if (*out == _BOM && (flags & UTF8_SKIP_BOM) != 0) {
			total--;
			out--;
		}

		out++;
	}

	return (total);
}

/*
 * DESCRIPTION
 *	This function translates UCS-4 symbols (given in local machine
 *	byte order) into UTF-8 string.
 *
 *	It takes the following arguments:
 *	in	- input unicode string. It can be null-terminated.
 *	insize	- size of input string in wide characters.
 *	out	- result buffer for utf8 string. If out is NULL,
 *		function returns size of result buffer.
 *	outsize - size of result buffer.
 *
 * RETURN VALUES
 *	The function returns size of result buffer (in bytes). Zero is returned
 *	in case of error.
 *
 * CAVEATS
 *	If UCS-4 string contains zero symbols, they will be translated
 *	as regular symbols.
 */
size_t
wchar_to_utf8(const wchar_t *in, size_t insize, char *out, size_t outsize,
    int flags)
{
	wchar_t *w, *wlim, ch;
	uint8_t *p, *lim, *oc;
	size_t total, n;

	if (in == NULL || insize == 0 || (outsize == 0 && out != NULL))
		return (0);

	w = (wchar_t *)in;
	wlim = w + insize;
	p = (uint8_t *)out;
	lim = p + outsize;
	total = 0;
	for (; w < wlim; w++) {
		if (__wchar_forbitten(*w) != 0) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			else
				continue;
		}

		if (*w == _BOM && (flags & UTF8_SKIP_BOM) != 0)
			continue;

		if (*w < 0) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			continue;
		} else if (*w <= 0x0000007f)
			n = 1;
		else if (*w <= 0x000007ff)
			n = 2;
		else if (*w <= 0x0000ffff)
			n = 3;
		else if (*w <= 0x001fffff)
			n = 4;
		else if (*w <= 0x03ffffff)
			n = 5;
		else /* if (*w <= 0x7fffffff) */
			n = 6;

		total += n;

		if (out == NULL)
			continue;

		if (lim - p <= n - 1)
			return (0);		/* no space left */

		/* make it work under different endians */
		ch = htonl(*w);
		oc = (uint8_t *)&ch;
		switch (n) {
		case 1:
			*p = oc[3];
			break;

		case 2:
			p[1] = _NXT | (oc[3] & 0x3f);
			p[0] = _SEQ2 | (oc[3] >> 6) | ((oc[2] & 0x07) << 2);
			break;

		case 3:
			p[2] = _NXT | (oc[3] & 0x3f);
			p[1] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[0] = _SEQ3 | ((oc[2] & 0xf0) >> 4);
			break;

		case 4:
			p[3] = _NXT | (oc[3] & 0x3f);
			p[2] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[1] = _NXT | ((oc[2] & 0xf0) >> 4) |
			    ((oc[1] & 0x03) << 4);
			p[0] = _SEQ4 | ((oc[1] & 0x1f) >> 2);
			break;

		case 5:
			p[4] = _NXT | (oc[3] & 0x3f);
			p[3] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[2] = _NXT | ((oc[2] & 0xf0) >> 4) |
			    ((oc[1] & 0x03) << 4);
			p[1] = _NXT | (oc[1] >> 2);
			p[0] = _SEQ5 | (oc[0] & 0x03);
			break;

		case 6:
			p[5] = _NXT | (oc[3] & 0x3f);
			p[4] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[3] = _NXT | (oc[2] >> 4) | ((oc[1] & 0x03) << 4);
			p[2] = _NXT | (oc[1] >> 2);
			p[1] = _NXT | (oc[0] & 0x3f);
			p[0] = _SEQ6 | ((oc[0] & 0x40) >> 6);
			break;
		}

		/*
		 * NOTE: do not check here for forbitten UTF-8 characters.
		 * They cannot appear here because we do proper convertion.
		 */

		p += n;
	}

	return (total);
}
#endif /* TLF_FONTS */
