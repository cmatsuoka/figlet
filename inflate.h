/*
 * inflate.h -  inflate decompression routine
 *
 * Version 1.1
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
 * 1) All file i/o is done externally to these routines
 * 2) Routines are symmetrical so inflate can feed into deflate
 * 3) Routines can be easily integrated into wide range of applications
 * 4) Routines are very portable, and use only ANSI C
 * 5) No #defines in inflate.h to conflict with external #defines
 * 6) No external routines need be called by these routines
 * 7) Buffers are owned by the calling routine
 * 8) No static non-constant variables are allowed
 */

/*
 * Note that for each call to InflatePutBuffer, there will be
 * 0 or more calls to (*putbuffer_ptr).  All except the last
 * call to (*putbuffer_ptr) will be with 32768 bytes, although
 * this behaviour may change in the future.  Before InflatePutBuffer
 * returns, it will have output as much uncompressed data as
 * is possible.
 */

#ifndef __INFLATE_H
#define __INFLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Routine to initialize inflate decompression */
void *InflateInitialize(                      /* returns InflateState       */
  void *AppState,                             /* for passing to putbuffer   */
  int (*putbuffer_ptr)(                       /* returns 0 on success       */
    void *AppState,                           /* opaque ptr from Initialize */
    unsigned char *buffer,                    /* buffer to put              */
    long length                               /* length of buffer           */
  ),
  void *(*malloc_ptr)(long length),           /* utility routine            */
  void (*free_ptr)(void *buffer)              /* utility routine            */
);

/* Call-in routine to put a buffer into inflate decompression */
int InflatePutBuffer(                         /* returns 0 on success       */
  void *InflateState,                         /* opaque ptr from Initialize */
  unsigned char *buffer,                      /* buffer to put              */
  long length                                 /* length of buffer           */
);

/* Routine to terminate inflate decompression */
int InflateTerminate(                         /* returns 0 on success       */
  void *InflateState                          /* opaque ptr from Initialize */
);

#ifdef __cplusplus
}
#endif

#endif
