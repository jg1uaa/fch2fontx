// code by SASANO Takayoshi, CC-BY-SA

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "load.h"
#include "parse.h"
#include "convert.h"

int main(int argc, char *argv[])
{
	uint8_t *buf;
	size_t size;

	if (argc < 5) {
		printf("usage: %s [infile] [fontname] "
		       "[outfile(ascii)] [outfile(kanji)]\n", argv[0]);
		goto fin0;
	}

	buf = loadfile(argv[1], &size);
	if (buf == NULL) goto fin0;

	if (parse_command(buf, size))
		goto fin1;

	convert_ascii(argv[3], argv[2]);
	convert_kanji(argv[4], argv[2]);

fin1:
	free(buf);
fin0:
	return 0;
}
