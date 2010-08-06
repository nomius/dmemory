
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

#ifndef MEMORY_C
#define MEMORY_C

#include <string.h>
#include <errno.h>
#include <signal.h>
#include "debug.h"
#include "dmemory.h"
#include "list.h"
#include "exceptions.h"

static void add_signature_to_variable(void *ptr, size_t size);
static int CheckSignatures(void *ptr, size_t size);

static stack_variable *stack;
static void *last_checked;
static char *last_file_ok;
static int last_line_ok;
static FILE *report;
static stack_variable *ptr_last_var;

void sigsegv_handler(int sig)
{
	fprintf(report, "There's a major corruption at 0x%X right after %s at %d.\n", last_checked, last_file_ok, last_line_ok);
	fprintf(report, "This normally means that you really screwed things up.\n");
	fclose(report);
	exit(1);
}

static void add_signature_to_variable(void *ptr, size_t size)
{
    char *tmp = (char *)ptr;

    /* Add the signature to the beginning */
    memcpy(tmp, SIGNATURE, SIZE_SIGNATURE);

    /* Add the signature to the end */
    memcpy(tmp + SIZE_SIGNATURE + size, SIGNATURE, SIZE_SIGNATURE);
}

static int CheckSignatures(void *ptr, size_t size)
{
    char *tmp = (char *)ptr;

    /* Check the signature in the beginning and the end */
    if (!memcmp(tmp, SIGNATURE, SIZE_SIGNATURE) && !memcmp(tmp + SIZE_SIGNATURE + size, SIGNATURE, SIZE_SIGNATURE))
        return 1;

    return 0;
}

void *__xmalloc(void **ptr, size_t size, char *file, int line)
{
#ifdef MEM_DEBUG
    /* A sanity check */
    if (*ptr != NULL && search_pointer(stack, ((char *)*ptr) - SIZE_SIGNATURE) != NULL)
        debug(WARNING, " Pointer in use. Memory leak ahead\n", file, line);

    /* Get that memory! */
    if ((*ptr = malloc(size + SIZE_SIGNATURE * 2)) != NULL) {
        /* Register the variable in the stack */
        if (stack->next == NULL)
            ptr_last_var = add_pointer_to_stack(stack, *ptr, size, file, line);
        else
            add_pointer_to_stack(stack, *ptr, size, file, line);

        /* Save the first pointer */

        add_signature_to_variable(*ptr, size);
        return ((char *)*ptr) + SIZE_SIGNATURE;
    }

    /* Ok, something went wrong */
    debug(ERROR, " Memory exausted\n", file, line);
    abort();
    return NULL;
#else
    return malloc(size);
#endif
}

void *__xrealloc(void **ptr, size_t size, char *file, int line)
{
#ifdef MEM_DEBUG
    void *tptr = NULL;
    stack_variable *myvar = NULL;

    if (*ptr == NULL) {
        if (size != 0)
            return __xmalloc(ptr, size, file, line);
        return NULL;
    }
    else if (size == 0) {
        __xfree(ptr, file, line);
        return NULL;
    }

    /* ptr != NULL and size != 0. Classic realloc */

    /* If the pointer doesn't exists but he stills want that memory let's just give it to him */
    if ((myvar = search_pointer(stack, ((char *)*ptr) - SIZE_SIGNATURE )) == NULL)
        return __xmalloc(ptr, size, file, line);

    /* It does exists in our stack, let's just increase it and leave */
    if ((tptr = realloc(((char *)*ptr) - SIZE_SIGNATURE, size + SIZE_SIGNATURE * 2)) != NULL) {
        add_signature_to_variable(tptr, size);
        myvar->size = size;
        myvar->variable = tptr;
        return ((char *)tptr) + SIZE_SIGNATURE;
    }

    /* Ok, something went wrong */
    debug(ERROR, " Memory exausted\n", file, line);
    abort();
    return NULL;
#else
    return realloc(*ptr, size);
#endif
}

void *__xcalloc(void **ptr, size_t nmemb, size_t size, char *file, int line)
{
#ifdef MEM_DEBUG
    stack_variable *myvar = NULL;

    /* A sanity check */
    if (*ptr != NULL && search_pointer(stack, ((char *)*ptr) - SIZE_SIGNATURE) != NULL)
        debug(WARNING, " Pointer in use. Memory leak ahead\n", file, line);

    /* Get that memory and blank it! */
    if ((*ptr = calloc(nmemb + SIZE_SIGNATURE * 2, size)) != NULL) {

        /* Register the variable in the stack */
		ptr_last_var = add_pointer_to_stack(stack, *ptr, size, file, line);
        add_signature_to_variable(*ptr, size);
        return ((char *)*ptr) + SIZE_SIGNATURE;
    }

    /* Ok, something went wrong */
    debug(ERROR, "Memory exausted\n", file, line);
    abort();
    return NULL;
#else
    return calloc(nmemb, size);
#endif
}

int __xfree(void **ptr, char *file, int line)
{
#ifdef MEM_DEBUG
    stack_variable *myvar = NULL;

    /* If the pointer doesn't exists just leave, there's nothing to free */
    if ((myvar = search_pointer(stack, ((char *)*ptr) - SIZE_SIGNATURE)) != NULL) {
        /* Remove the variable from the stack and clean up the variable */
        remove_pointer_from_stack(stack, myvar);
        free(((char *)*ptr) - SIZE_SIGNATURE);
        *ptr = NULL;
        return 0;
    }
    debug(WARNING, " You can not free this variable, it was never reserved\n",
          file, line);
    return 1;
#else
    free(*ptr);
#endif
}

void dmemory_init(int level)
{
#ifdef MEM_DEBUG
    /* Initialize the debug level */
    DEBUG_LEVEL = level;

    /* Initialize the stack */
    stack = malloc(sizeof (stack_variable));
    stack->next = stack->prev = stack->variable = NULL;
    stack->size = -1;
#endif
}

int dmemory_end(void)
{
#ifdef MEM_DEBUG
    char *filename;
    stack_variable *ptr;

    /* Load the variable in the filename */
    if ((filename = getenv(VAR_REPORT_FILENAME)) == NULL) {
        debug(WARNING, " %s not defined, no report will be generated\n", "LIBRARY", 0, VAR_REPORT_FILENAME);
        return 1;
    }

    /* Check if it was defined and not empty */
    if (*filename == '\0') {
        debug(WARNING, " %s defined but empty, no report will be generated\n", "LIBRARY", 0, VAR_REPORT_FILENAME);
        return 1;
    }

    /* Open the exceptions file */
    if ((report = fopen(filename, "w")) == NULL) {
        debug(WARNING, " opening %s", strerror(errno), errno, filename);
        return 1;
    }

    /* Load the exceptions array */
    __load_exceptions_file();

    /* We capture SIGSEV because CheckSignatures could break our program if
     * there's actually a memory corruption by the user */
    signal(SIGSEGV, sigsegv_handler);

    /* For every variable in the stack */
    /* We go from back to top in case he broke things up badley */
    for (ptr = ptr_last_var;; ptr = ptr->prev) {

        last_file_ok = ptr->filename;
        last_line_ok = ptr->line;
        last_checked = ptr->variable;

        /* Go cleaning the stack while moving through it */
        free(ptr->next);
        ptr->next = NULL;

        if (ptr->variable != NULL) {
            /* If it is not in the exception list add it to the report */
            if (!__ExceptLeak(ptr->filename, ptr->line))
                fprintf(report, "(L) [%s] [%d]\n", ptr->filename, ptr->line);
            if (!CheckSignatures((void *)ptr->variable, (int)ptr->size))
                fprintf(report, "(C) [%s] [%d]\n", ptr->filename, ptr->line);
            free(ptr->filename);
        }
        if (ptr->prev == NULL)
            break;
    }
	 /* Free the last one */
    free(ptr->next);

	 /* Free the exceptions list */
    __free_exceptions();

    fclose(report);
#endif

    return 0;
}

#endif