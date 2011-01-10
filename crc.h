/*
 * crc.h - CRC calculation routine
 *
 * Version 1.0
 */

/*
 * Copyright (c) 1995, Edward B. Hamrick
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that
 *
 * (i)  the above copyright notice and the text in this "C" comment block
 *      appear in all copies of the software and related documentation, and
 *
 * (ii) any modifications to this source file must be sent, via e-mail
 *      to the copyright owner (currently hamrick@primenet.com) within 
 *      30 days of such modification.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL EDWARD B. HAMRICK BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This CRC algorithm is the same as that used in zip.  Normally it
 * should be initialized with 0xffffffff, and the final CRC stored
 * should be crc ^ 0xffffffff.
 *
 * It implements the polynomial:
 *
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 */

#ifndef __CRC_H
#define __CRC_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned long CrcUpdate(          /* returns updated crc         */
  unsigned long crc,              /* starting crc                */
  unsigned char *buffer,          /* buffer to use to update crc */
  long length                     /* length of buffer            */
);

#ifdef __cplusplus
}
#endif

#endif
