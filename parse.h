// code by SASANO Takayoshi, CC-BY-SA

#include <stdint.h>

extern uint8_t Xsize_Ascii;
extern uint8_t Ysize_Ascii;
extern uint8_t Xsize_Kanji;
extern uint8_t Ysize_Kanji;

#define	ASCII_START	0x00000
#define	ASCII_END	0x00100
#define	KANJI_START	0x08000
#define	KANJI_END	0x10000

#define	GLYPHPTR_ENTRY	KANJI_END
extern uint8_t *Glyph_ptr[GLYPHPTR_ENTRY];

int parse_command(uint8_t *, uint32_t);

#define	MIN_CMDLEN		6

#define	CMD_SETGLYPH		0
#define	CMD_SETASCIISIZE	1
#define	CMD_SETKANJISIZE	2
