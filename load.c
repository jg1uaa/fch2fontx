// code by SASANO Takayoshi, CC-BY-SA

#include <stdio.h>
#include <stdlib.h>

#include "load.h"

uint8_t *loadfile(char *filename, size_t *loadsize)
{
	FILE *fp;
	uint8_t *buf;
	off_t size;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stdout, "loadfile: file open error (%s)\n", filename);
		goto fin0;
	}

	fseeko(fp, 0, SEEK_END);
	size = ftello(fp);
	fseeko(fp, 0, SEEK_SET);

	buf = malloc(size);
	if (buf == NULL) {
		fprintf(stdout, "loadfile: malloc failed\n");
		goto fin1;
	}

	if (fread(buf, size, 1, fp) < 1) {
		fprintf(stdout, "loadfile: file read error (%s)\n", filename);
		goto fin2;
	}

	fclose(fp);
	*loadsize = size;
	return buf;

fin2:
	free(buf);
fin1:
	fclose(fp);
fin0:
	*loadsize = 0;
	return NULL;
}
