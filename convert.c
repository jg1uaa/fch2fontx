// code by SASANO Takayoshi, CC-BY-SA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "memio.h"
#include "parse.h"
#include "fontx2defs.h"

#include "convert.h"

#define	CODETABLE_SIZE	512

static uint8_t codetable_entry;
static uint16_t codetable[CODETABLE_SIZE];

static int find_glyph_range(int *start, int *end)
{
	/* skip null section */
	for (; (*start < KANJI_END) && (Glyph_ptr[*start] == NULL);
	     (*start)++);

	/* no more section */
	if (*start >= KANJI_END)
		return -1;

	/* find range */
	for (*end = *start;
	     (*end < KANJI_END) && (Glyph_ptr[*end] != NULL);
	     (*end)++);
	(*end)--;

	return 0;
}

static int create_codetable(void)
{
	int i, start, end;

	codetable_entry = 0;
	start = KANJI_START;

	for (i = 0; i < CODETABLE_SIZE; i += 2) {
		if (find_glyph_range(&start, &end) < 0)
			break;

		poke_le16(&codetable[i], start);
		poke_le16(&codetable[i + 1], end);

		start = end + 1;
	}

	if (i == CODETABLE_SIZE) {
		fprintf(stdout, "create_codetable: codetable overflow\n");
		return -1;
	}

	codetable_entry = i / 2;
	return 0;
}

static int create_fontx2header(struct fontx2_header *h, char *fontname, int xsize, int ysize, int codetype)
{
	int i;

	if (xsize == 0 || ysize == 0)
		return -1;

	memcpy(h->identifier, "FONTX2", sizeof(h->identifier));
	for (i = 0; i < sizeof(h->fontname) && isprint(fontname[i]); i++)
		h->fontname[i] = fontname[i];
	for (; i < sizeof(h->fontname); i++)
		h->fontname[i] = 0x20;
	h->xsize = xsize;
	h->ysize = ysize;
	h->codetype = codetype;

	return 0;
}

/* convert (KANJI) */
void convert_kanji(char *filename, char *fontname)
{
	int i, datasize;
	FILE *fp;
	struct fontx2_header h;

	/* create header */
	if (create_fontx2header(&h, fontname, Xsize_Kanji, Ysize_Kanji,
				CODETYPE_KANJI) < 0 ||
	    create_codetable() < 0)
		return;

	datasize = ((Xsize_Kanji + 7) / 8) * Ysize_Kanji;

	/* save result */
	fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf(stdout, "convert_kanji: file open error (%s)\n",
			filename);
		goto fin0;
	}

	if (fwrite(&h, sizeof(h), 1, fp) < 1 ||
	    fwrite(&codetable_entry, sizeof(codetable_entry), 1, fp) < 1 ||
	    fwrite(&codetable, sizeof(uint16_t) * 2,
		   codetable_entry, fp) < codetable_entry) {
		fprintf(stdout, "convert_kanji: file write error (header)\n");
		goto fin1;
	}

	for (i = KANJI_START; i < KANJI_END; i++) {
		if (Glyph_ptr[i] == NULL)
			continue;

		if (fwrite(Glyph_ptr[i], datasize, 1, fp) < 1) {
			fprintf(stdout, "convert_kanji: "
				"file write error (data)\n");
			goto fin1;
		}
	}

fin1:
	fclose(fp);
fin0:
	return;
}

/* convert (ASCII) */
void convert_ascii(char *filename, char *fontname)
{
	int i, datasize;
	uint8_t *tmp;
	FILE *fp;
	struct fontx2_header h;

	/* create header */
	if (create_fontx2header(&h, fontname, Xsize_Ascii, Ysize_Ascii,
				CODETYPE_ASCII) < 0)
		return;

	datasize = ((Xsize_Ascii + 7) / 8) * Ysize_Ascii;

	/* create temporary buffer for null data */
	tmp = alloca(datasize);
	memset(tmp, 0, datasize);

	/* save result */
	fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf(stdout, "convert_ascii: file open error (%s)\n",
			filename);
		goto fin0;
	}

	if (fwrite(&h, sizeof(h), 1, fp) < 1) {
		fprintf(stdout, "convert_ascii: file write error (header)\n");
		goto fin1;
	}

	for (i = ASCII_START; i < ASCII_END; i++) {
		if (fwrite((Glyph_ptr[i] != NULL) ? Glyph_ptr[i] : tmp,
			   datasize, 1, fp) < 1) {
			fprintf(stdout, "convert_ascii: "
				"file write error (data)\n");
			goto fin1;
		}
	}

fin1:
	fclose(fp);
fin0:
	return;
}
