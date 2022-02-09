#define PLL_24x 0b010111 // 87 MHz
#define PLL_18x 0b010001 // 65.25 MHz
#define PLL_16x 0b001111 // 58 MHz (NORMAL SPEED)
#define PLL_15x 0b001110 // 54.37 MHz
#define PLL_12x 0b001011 // 43.5 MHz
#define PLL_8x  0b000111 // 29 MHz
#define PLL_6x  0b000101 // 21.75 MHz
#define PLL_4x  0b000011 // 14.5 MHz
#define PLL_3x  0b000010 // 10.8 MHz
#define PLL_2x  0b000001 // 7.25 MHz
#define PLL_1x  0b000000 // 3.6 MHz


void change_freq(int mult) {
	__asm__(
		"mov r4, r0\n\t"
		"and #0x3F, r0\n\t" 
		"shll16 r0\n\t" 
		"shll8 r0\n\t" 
		"mov.l frqcr, r1\n\t"  
		"mov.l pll_mask, r3\n\t"  
		"mov.l @r1, r2\n\t"  
		"and r3, r2\n\t"  
		"or r0, r2\n\t" 
		"mov.l r2, @r1\n\t" 
		"mov.l frqcr_kick_bit, r0\n\t" 
		"mov.l @r1, r2\n\t"
		"or r0, r2\n\t"
		"rts\n\t"
		"mov.l r2, @r1\n\t"
		".align 4\n\t"
		"frqcr_kick_bit: .long 0x80000000\n\t"
		"pll_mask: .long 0xC0FFFFFF\n\t" 
		"frqcr: .long 0xA4150000\n\t"
	);
}
