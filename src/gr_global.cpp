#include "gr_global.h"

static u32 holdrand = 0;

void grSRand(u32 seed)
{
	holdrand = seed;
}

u16 grRand()
{
	return (((holdrand = holdrand * 214013L + 2531011L) >>16) & 0x7fff);
}
