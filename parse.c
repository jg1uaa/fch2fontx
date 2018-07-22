// code by SASANO Takayoshi, CC-BY-SA

#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "memio.h"

uint8_t Xsize_Ascii;
uint8_t Ysize_Ascii;
uint8_t Xsize_Kanji;
uint8_t Ysize_Kanji;

uint8_t *Glyph_ptr[GLYPHPTR_ENTRY];

static void parse_initialize(void)
{
	Xsize_Ascii = Ysize_Ascii = 0;
	Xsize_Kanji = Ysize_Kanji = 0;

	memset(Glyph_ptr, 0, sizeof(Glyph_ptr));
}

static int _mbcjistojms(int jis)
{
	int low, high;

	high = (jis >> 8) & 0xff;
	low = jis & 0xff;

	if (high & 1) {
		low += 0x1f;
		if (low >= 0x7f) low++;
	} else {
		low += 0x7e;
	}

	if (high < 0x5f) {
		high--;
		high /= 2;
		high += 0x71;
	} else {
		high--;
		high /= 2;
		high += 0xb1;
	}

	return (high << 8) | low;
}

static int is_invalid_jiscode(int jis)
{
	int ku, ten;

	ku = ((jis >> 8) & 0xff) - 0x20;
	ten = (jis & 0xff) - 0x20;

	if (ten < 1 || ten > 94)
		return -1;

	if (ku < 1 || (ku > 94 && ku < 115) || ku > 119)
		return -1;

	return 0;
}

static int set_size(uint16_t arg, uint8_t *xsize, uint8_t *ysize)
{
	uint8_t x, y;

	x = arg >> 8;
	y = arg & 0xff;

	/* if size is 0, error */
	if (x == 0 || y == 0) {
		fprintf(stdout, "set_size: invalid XYsize (%d,%d)\n", x, y);
		return -1;
	}

	/* if size is already set and different size specified, error */
	if ((*xsize != 0 && *xsize != x) || (*ysize != 0 && *ysize != y)) {
		fprintf(stdout, "set_size: XYsize is already set "
			"(%d,%d <- %d,%d)\n", *xsize, *ysize, x, y);
		return -1;
	}

	*xsize = x;
	*ysize = y;
	return 0;
}

static int set_glyph(uint16_t arg, int xsize, int ysize, uint8_t *glyph, int glyphsize)
{
	int reqsize, code;

	/* if size is not set, error */
	if (xsize == 0 || ysize == 0) {
		fprintf(stdout, "set_glyph: undefined XYsize\n");
		return -1;
	}

	/* if glyph data is not enough, error */
	reqsize = ((xsize + 7) / 8) * ysize;
	if (glyphsize < reqsize) {
		fprintf(stdout, "set_glyph: glyph data not enough (%d/%d)\n",
			glyphsize, reqsize);
		return -1;
	}
	
	/* ignore invalid character code */
	if (arg >= ASCII_END && is_invalid_jiscode(arg)) {
		fprintf(stdout, "set_glyph: invalid code %#x\n", arg);
		return 0;
	}

	/* convert character code: ASCII/JIS -> ASCII/SJIS */
	code = (arg < ASCII_END) ? arg : _mbcjistojms(arg);

	/* check pointer slot is empty */
	if (Glyph_ptr[code] != NULL)
		fprintf(stdout, "set_glyph: code %#x override\n", arg);

	Glyph_ptr[code] = glyph;

	return 0;
}

int parse_command(uint8_t *buf, uint32_t bufsize)
{
	uint32_t i, remain;
	uint16_t len, cmd, arg;
	int result;

	parse_initialize();
	result = 0;

	for (i = 0; i < bufsize; ) {
		remain = bufsize - i;

		/* ignore trailer garbage */
		if (remain < MIN_CMDLEN) {
			fprintf(stdout, "parse_command: "
				"trailer garbage is discarded (%d byte)\n",
				remain);
			result = 0;
			break;
		}

		len = peek_le16(buf);
		if (len < MIN_CMDLEN) {
			fprintf(stdout, "parse_command: at %d, "
				"command packet is too short (%d/%d)\n",
				i, len, MIN_CMDLEN);
			result = -1;
			goto fin0;
		}

		cmd = peek_le16(buf + 2);
		arg = peek_le16(buf + 4);

		switch (cmd) {
		case CMD_SETGLYPH:
			if (arg < ASCII_END) {
				result = set_glyph(arg,
						   Xsize_Ascii, Ysize_Ascii,
						   buf + 6, len - 6);
			} else {
				result = set_glyph(arg,
						   Xsize_Kanji, Ysize_Kanji,
						   buf + 6, len - 6);
			}
			break;
		case CMD_SETASCIISIZE:
			result = set_size(arg, &Xsize_Ascii, &Ysize_Ascii);
			break;
		case CMD_SETKANJISIZE:
			result = set_size(arg, &Xsize_Kanji, &Ysize_Kanji);
			break;
		default:
			/* undefined command, error */
			fprintf(stdout, "parse_command: "
				"undefined command (%d)\n", cmd);
			result = -1;
			break;
		}

		if (result < 0) {
			fprintf(stdout, "parse_command: at %d, "
				"command processing failed (%d/%d/%#x)\n",
				i, len, cmd, arg);
			goto fin0;
		}

		i += len;
		buf += len;
	}
fin0:
	return result;
}
