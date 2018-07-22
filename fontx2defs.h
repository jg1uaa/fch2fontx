// code by SASANO Takayoshi, CC-BY-SA

#include <stdint.h>

/* FONTX2 header */
#define CODETYPE_ASCII	0
#define	CODETYPE_KANJI	1

/* common */
struct fontx2_header {
	uint8_t	identifier[6];
	uint8_t	fontname[8];
	uint8_t	xsize;
	uint8_t	ysize;
	uint8_t	codetype;
} __attribute__((packed));
