
/* vim: set sw=4 sts=4 tw=80 */

/* 
 * Copyright (c) 2010 , David B. Cortarello <dcortarello@gmail.com>
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
 *    This product includes software developed by David B. Cortarello.
 * 4. Neither the name of David B. Cortarello nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DAVID B. CORTARELLO 'AS IS' AND ANY
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

#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdio.h>

#define SIGNATURE "(D)(M).\0"
#define SIZE_SIGNATURE ((int)(strlen(SIGNATURE)+1))
#define VAR_REPORT_FILENAME "DMEMORY_REPORT"

void *__xmalloc(void **ptr, size_t size, char *file, int line);
void *__xcalloc(void **ptr, size_t nmemb, size_t size, char *file, int line);
void *__xrealloc(void **ptr, size_t size, char *file, int line);
int __xfree(void **ptr, char *file, int line);
void memory_init(int level);
int memory_end(void);

#ifdef C89
#define xmalloc(ptr, size) ((typeof(ptr)) __xmalloc((void **)&(ptr), (size), (char *)__FILE__, __LINE__))
#define xcalloc(ptr, nmemb, size) ((typeof(ptr)) __xcalloc((void **)&(ptr), (nmemb), (size), (char *)__FILE__, __LINE__))
#define xrealloc(ptr, size) ((typeof(ptr)) __xrealloc((void **)&(ptr), (size), (char *)__FILE__, __LINE__))
#define xfree(ptr) __xfree((void **)&(ptr), (char *)__FILE__, __LINE__)
#elif C99
#define xmalloc(ptr, size) __xmalloc((void **)&(ptr), (size), (char *)__FILE__, __LINE__)
#define xcalloc(ptr, nmemb, size) __xcalloc((void **)&(ptr), (nmemb), (size), (char *)__FILE__, __LINE__)
#define xrealloc(ptr, size) __xrealloc((void **)&(ptr), (size), (char *)__FILE__, __LINE__)
#define xfree(ptr) __xfree((void **)&(ptr), (char *)__FILE__, __LINE__)
#endif

#endif
