#include <stdio.h>
#include <stdlib.h>

/*
 * Borrowed from libart_lgpl
 */

/**
 * A little utility function to generate header info.
 *
 * Yes, it would be possible to do this using more "native" autoconf
 * features, but I personally find this approach to be cleaner.
 *
 * The output of this program is generally written to art_config.h,
 * which is installed in libart's include dir.
 **/

#define progname "gen_nr_config"

static void
die (char *why)
{
	fprintf (stderr, "%s: %s\n", progname, why);
	exit (1);
}

int
main (int argc, char **argv)
{
	printf ("/* Automatically generated by %s */\n"
		"\n"
		"#ifndef __NR_CONFIG_H__\n"
		"#define __NR_CONFIG_H__\n"
		"\n"
		"#define NR_SIZEOF_CHAR %d\n"
		"#define NR_SIZEOF_SHORT %d\n"
		"#define NR_SIZEOF_INT %d\n"
		"#define NR_SIZEOF_LONG %d\n"
		"\n",
		progname,
		(int) sizeof (char), (int) sizeof (short), (int) sizeof (int), (int) sizeof (long));

	if (sizeof (char) == 1) {
		printf ("typedef signed char NRByte;\n");
		printf ("typedef unsigned char NRUByte;\n");
	} else {
		die ("sizeof (char) != 1");
	}

	if (sizeof (short) == 2) {
		printf ("typedef signed short NRShort;\n");
		printf ("typedef unsigned short NRUShort;\n");
	} else {
		die ("sizeof (short) != 2");
	}

	if (sizeof (int) == 4) {
		printf ("typedef signed int NRLong;\n");
		printf ("typedef unsigned int NRULong;\n");
	} else if (sizeof (long) == 4) {
		printf ("typedef signed long NRLong;\n");
		printf ("typedef unsigned long NRULong;\n");
	} else {
		die ("sizeof (int) != 4 and sizeof(long) != 4");
	}

	printf ("\n"
		"#endif /* __NR_CONFIG_H__ */\n");

	return 0;
}
