
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

#ifndef EXCEPTIONS_C
#define EXCEPTIONS_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "exceptions.h"
#include "debug.h"

/* This should be strnlen... THANKS GNU PEOPLE FOR SCREWING THE NAMESPACE! */
static inline int max_strlen(const char *str, int maxlen)
{
	int i = 0;

	while (str[i] != '\0')
		if (i == maxlen)
			break;
		else
			i += 1;
	return i;
}

int __load_exceptions_file(void)
{
	char *filename = NULL;
	FILE *FExceptions = NULL;
	char tmpbuf[MAX_BUF];
	char *itmp = NULL;
	int line;

	NumExceptions = 0;
	Exceptions = NULL;

	/* Load the variable in the filename */
	if ((filename = getenv(VAR_EXCEPTIONS_FILENAME)) == NULL) {
		debug(INFO, "%s not defined, no exceptions defined\n", "LIBRARY", 0, VAR_EXCEPTIONS_FILENAME);
		return 1;
	}

	/* Check if it was defined and not empty */
	if (*filename == '\0') {
		debug(INFO, "%s defined but empty, no exceptions defined\n", "LIBRARY", 0, VAR_EXCEPTIONS_FILENAME);
		return 1;
	}

	/* Open the exceptions file */
	if ((FExceptions = fopen(filename, "r")) == NULL) {
		debug(INFO, "opening %s\n", strerror(errno), errno, filename);
		return 1;
	}

	/* Load the exceptions to an array */
	while (fgets(tmpbuf, MAX_BUF, FExceptions) != NULL) {

		/* Check if the user try to screw us with junk */
		if (max_strlen(tmpbuf, MAX_BUF) == MAX_BUF) {
			debug(INFO, "Exception filename too large at line %d\n", "LIBRARY", 0, NumExceptions + 1);
			continue;
		}
		/* Save space for one more */
		Exceptions = realloc(Exceptions, sizeof(TExceptions) * (NumExceptions + 1));

		/* If no field separator was given, then he wants the whole exception */
		if ((itmp = strchr(tmpbuf, FIELD_SEPARATOR)) != NULL) {
			Exceptions[NumExceptions].line = atoi(itmp + 1);
			*itmp = '\0';
		}
		else
			Exceptions[NumExceptions].line = -1;
		Exceptions[NumExceptions].filename = strdup(tmpbuf);
		if ((itmp = strchr(Exceptions[NumExceptions].filename, '\n')) != NULL)
			*itmp = '\0';

		NumExceptions += 1;
	}
	return 0;
}

int __ExceptLeak(char *filename, int line)
{
	int i = 0;

	/* Go through the Exceptions array and check it */
	for (i = 0; i < NumExceptions; i++)
		if (!strcmp(filename, Exceptions[i].filename) && (line == Exceptions[i].line || Exceptions[i].line == -1))
			return 1;
	return 0;
}

void __free_exceptions(void)
{
	int i = 0;

	/* Go through the Exceptions array and free it */
	for (i = 0; i < NumExceptions; i++)
		free(Exceptions[NumExceptions].filename);
	free(Exceptions);
}

#endif
