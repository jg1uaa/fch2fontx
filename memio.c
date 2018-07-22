// code by SASANO Takayoshi, CC-BY-SA

#include "memio.h"

/* read 16bit value from memory, little-endian */
uint16_t peek_le16(void *ptr)
{
	uint8_t *p = ptr;

	return (p[1] << 8) | p[0];
}

/* write 16bit value to memory, little-endian */
void poke_le16(void *ptr, uint16_t data)
{
	uint8_t *p = ptr;

	p[0] = data;
	p[1] = data >> 8;
}
