#define __ARIKKEI_UTILS_C__

/*
 * Miscellaneous utilities
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <assert.h>
#include <crtdbg.h>

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "arikkei-utils.h"

void
arikkei_check_integrity (void)
{
	assert (_CrtCheckMemory());
}

unsigned int
arikkei_emit_fail_warning (const unsigned char *file, unsigned int line, const unsigned char *method, const unsigned char *expr)
{
	fprintf (stderr, "File %s line %d (%s): Assertion %s failed\n", file, line, method, expr);
	return 1;
}

