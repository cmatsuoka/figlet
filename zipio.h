/*
 * zipio.h - stdio emulation library for reading zip files
 *
 * Version 1.1.1
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
 * Changes from 1.1 to 1.1.1:
 * Changed "z*" functions to "Z*" to avoid namespace pollution.
 * Added "zungetc" macro.
 * John Cowan <cowan@ccil.org>
 */

/*
 * This library of routines has the same calling sequence as
 * the stdio.h routines for reading files.  If these routines
 * detect that they are reading from a zip file, they transparently
 * unzip the file and make the application think they're reading
 * from the uncompressed file.
 *
 * Note that this library is designed to work for zip files that
 * use the deflate compression method, and to read the first file
 * within the zip archive.
 *
 * There are a number of tunable parameters in the reference
 * implementation relating to in-memory decompression and the
 * use of temporary files.
 *
 * Particular care was taken to make the Zgetc() macro work
 * as efficiently as possible.  When reading an uncompressed
 * file with Zgetc(), it has exactly the same performance as
 * when using getc().  WHen reading a compressed file with
 * Zgetc(), it has the same performance as fread().  The total
 * CPU overhead for decompression is about 50 cycles per byte.
 *
 * The Zungetc() macro is quite limited.  It ignores the character
 * specified for pushback, and essentially just forces the last
 * character read to be re-read.  This is essential when parsing
 * numbers and such.  (1.1.1)
 *
 * There are a few stdio routines that aren't represented here, but
 * they can be layered on top of these routines if needed.
 */

#ifndef __ZIPIO_H
#define __ZIPIO_H

#include <stdio.h>

typedef struct {
  int            len;
  unsigned char *ptr;
} ZFILE;

#define Zgetc(f)                   \
  ((--((f)->len) >= 0)             \
    ? (unsigned char)(*(f)->ptr++) \
    : _Zgetc (f))

#define Zungetc(c,f) \
  ((f)->ptr--, (f)->len++, (c))

#ifdef __cplusplus
extern "C" {
#endif

ZFILE  *Zopen(const char *path, const char *mode);
int    _Zgetc(ZFILE *stream);
size_t  Zread(void *ptr, size_t size, size_t n, ZFILE *stream);
int     Zseek(ZFILE *stream, long offset, int whence);
long    Ztell(ZFILE *stream);
int     Zclose(ZFILE *stream);

#ifdef __cplusplus
}
#endif

#endif
