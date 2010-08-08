
/* vim: set sw=4 sts=4 : */

/* 
 * Copyright (c) 2009, David B. Cortarello
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the David B. Cortarello.
 * 4. Neither the name of the David B. Cortarello nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DAVID B. CORTARELLO ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DAVID B. CORTARELLO BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DEBUG_H
#define DEBUG_H

int DEBUG_LEVEL;

#define DEBUG 1

#define WARNING 5
#define ERROR 1


#if DEBUG
/* This function calculate how many variable arguments were passed */
/* #define NUMARGS(...) (sizeof((int *){0, ##__VA_ARGS__})/sizeof(int)-1) */

/* A more compliant C89 vresion of derror */
/* #define derror(file, line, fmt...) { \ if (NUMARGS(__VA_ARGS__) == 3) \ fprintf(stderr, "ERROR [%s:%d] " fmt, file, line); \ else \ fprintf(stderr, "ERROR [%s:%d] " fmt, file, line, ##__VA_ARGS__); \ fflush(NULL); \ exit(1); \ } */

#define derror(fmt, ...)  { fprintf(stderr, "ERROR [%s:%d] " fmt, __VA_ARGS__); fflush(NULL); exit(1); }

/* A more compliant C89 vresion of debug */
/* #define debug(LEVEL, file, line, fmt...) { \ if (LEVEL > DEBUG) { \ if(NUMARGS(__VA_ARGS__) == 4) \ fprintf(stderr, "DEBUG: [%s:%d]" fmt, file, line); \ else \ fprintf(stderr, "DEBUG: [%s:%d]" fmt, file, line, ##__VA_ARGS__); \ } \ fflush(NULL); \ } */

#define debug(LEVEL, fmt, ...) { \
	if (LEVEL > DEBUG) { \
		fprintf(stderr, "DEBUG: [%s:%d] " fmt, __VA_ARGS__); \
		fflush(NULL); \
	} \
}

#else
#define derror(...)
#define debug(...)
#endif

#endif
