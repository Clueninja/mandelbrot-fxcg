#include "../../../include/fxcg/display.h"
#include "../../../include/fxcg/keyboard.h"
#include "../../../include/fxcg/misc.h"
#include <string.h>
#include <stdlib.h>


#define FIX_FBITS 25
#include "fixed.h"


#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 192
#define MAX_LOOPS 100


short unsigned int heightcolor(fix z, fix z_min, fix z_max);

unsigned short Bdisp_GetPointWB_VRAM(int x, int y)
{
	return Bdisp_GetPoint_VRAM(x, y+24);
}
// Optimisation check if it is in main cardiods 
unsigned int inCardiod(const fix r, const fix i)
{
	fix x =  r - (FIX_ONE>>2);
	fix y = i;
	// Main Bulb
	if (fix_sq( fix_sq(x) + fix_sq(y) + (x>>1)) - ((fix_sq(x) + fix_sq(y))>>2)<0)
		return 1;
	/*
	x = r+FIX_ONE;
	if (fix_sq(x) + fix_sq(y) < FIX_ONE>>4)
		return 1;
	    // 1.3125
	x = r+ FIX_ONE + (FIX_ONE>>2) + (FIX_ONE>>4);
	if (fix_sq(x) + fix_sq(y) < FIX_ONE>>8)
		return 1;
*/
	return 0;
}

// draws into VRAM rectangle of mandelbrot corner t_x, t_y ...
// scale zooms into c_x, c_y
void mandelbrot(unsigned int t_x, unsigned int t_y, unsigned int w, unsigned int h, fix scale, fix c_x, fix c_y)
{
	HourGlass();
	
	register fix ci;
	register fix cr;
	
	register fix zr;
	register fix zi;
	
	register fix zrsqrd;
	register fix zisqrd;
	
	
	for(unsigned int i = t_y; i <= t_y + h; i++)
	{
		ci =  c_y - (scale>>2) + fix_div(fix_mul (scale,  fix_fromint(i)), fix_fromint(SCREEN_WIDTH));
		for(unsigned int j = t_x; j <= t_x + w; j++)
		{
		    
			cr =  c_x - (scale>>1) + fix_div(fix_mul (scale,  fix_fromint(j)), fix_fromint(SCREEN_WIDTH));
			
			if (!inCardiod(cr,ci))
			{
				zr=0;
				zi=0;
				zrsqrd=0;
				zisqrd = 0;
				
				int iter = 0;
				while ((iter < MAX_LOOPS) && (zrsqrd + zisqrd < FIX_ONE<<2))
				{	
					fix temp =zrsqrd-zisqrd + cr;
					
					zi = fix_mul(zr<<1, zi) + ci;
					zr = temp;
					zrsqrd = fix_sq(zr);
					zisqrd = fix_sq(zi);
					
					iter+=1;
				}
				unsigned short colour = heightcolor( fix_fromint(iter), 0, fix_fromint(MAX_LOOPS) );
				Bdisp_SetPointWB_VRAM(j,i,colour);
			}
			else
			{
				unsigned short colour = heightcolor(fix_fromint(MAX_LOOPS), 0, fix_fromint(MAX_LOOPS));
				Bdisp_SetPointWB_VRAM(j,i,colour);
			}
		}
	}
}

// off_x number of pixels to see to the right
void draw_offset_right(int off_x, fix scale, fix c_x, fix c_y){
	HourGlass();
	for(unsigned int j = off_x; j<SCREEN_WIDTH; j++)
	{
		for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
		{
			unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
			Bdisp_SetPointWB_VRAM(j-off_x,i,colour);
		}
	}
	mandelbrot(SCREEN_WIDTH-off_x, 0,off_x, SCREEN_HEIGHT, scale, c_x, c_y );
	Bdisp_PutDisp_DD();
}
void draw_offset_left(int off_x, fix scale, fix c_x, fix c_y)
{
    HourGlass();
	for(unsigned int j = SCREEN_WIDTH-off_x; j>0; j--)
	{
		for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
		{
			unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
			Bdisp_SetPointWB_VRAM(j+off_x,i,colour);
		}
	}
	mandelbrot(0,0,off_x, SCREEN_HEIGHT, scale, c_x, c_y );
	Bdisp_PutDisp_DD();
}

void draw_offset_down(int off_y, fix scale, fix c_x, fix c_y){
	HourGlass();
	for(unsigned int i = off_y; i<SCREEN_HEIGHT; i++)
	{
		for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
		{
			unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
			Bdisp_SetPointWB_VRAM(j,i-off_y,colour);
		}
	}
	mandelbrot(0,SCREEN_HEIGHT-off_y,SCREEN_WIDTH, off_y, scale, c_x, c_y );
	Bdisp_PutDisp_DD();
}

void draw_offset_up(int off_y, fix scale, fix c_x, fix c_y)
{
    HourGlass();
    for(unsigned int i = SCREEN_HEIGHT-off_y; i>0; i--)
	{
		for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
		{
			unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
			Bdisp_SetPointWB_VRAM(j,i+off_y,colour);
		}
	}
	mandelbrot(0,0,SCREEN_WIDTH, off_y, scale, c_x, c_y );
	Bdisp_PutDisp_DD();
}

void draw_scale(fix scale, fix c_x, fix c_y)
{
	mandelbrot(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, scale, c_x, c_y);
	Bdisp_PutDisp_DD();
}

int main(void){
	int step = 10;
	
    Bdisp_EnableColor(1);//Enable 16-bit mode
    Bdisp_AllClr_VRAM();
    
    int key;
	int running = 1;
	
	fix c_x = 0;
	fix c_y = 0;
	
	fix scale = FIX_ONE<<2;
	fix zoom = FIX_ONE>>1;
	
	draw_scale(scale, c_x,c_y);
    while(running)
    {
		GetKey(&key);
		switch (key){
			 case KEY_CTRL_EXE:
			 	running = 0;
			 	break;
			 	
			 case KEY_CTRL_UP:
			 	c_y = c_y - fix_div(fix_mul(scale, fix_fromint(step)), fix_fromint(SCREEN_WIDTH));
			 	draw_offset_up(step,scale, c_x, c_y);
			 	break;
			 	
			 case KEY_CTRL_DOWN:
			 	c_y = c_y + fix_div(fix_mul(scale, fix_fromint(step)), fix_fromint(SCREEN_WIDTH));
			 	draw_offset_down(step,scale, c_x, c_y);
			 	break;
			 	
			 case KEY_CTRL_LEFT:
			 	c_x = c_x - fix_div(fix_mul(scale, fix_fromint(step)), fix_fromint(SCREEN_WIDTH));
			 	draw_offset_left(step,scale, c_x, c_y);
			 	break;
			 
			 case KEY_CTRL_RIGHT:
			 	c_x = c_x + fix_div(fix_mul(scale, fix_fromint(step)), fix_fromint(SCREEN_WIDTH));
			 	draw_offset_right(step, scale,c_x, c_y);
			 	break;
			 
			 case KEY_CHAR_PLUS:
				scale = fix_mul(zoom, scale);
				draw_scale(scale, c_x, c_y);
				break;
				
			 case KEY_CHAR_MINUS:
				scale = fix_div(scale,zoom);
				draw_scale(scale, c_x, c_y);
				break;
		}
    }
    return 0;
}


/*Modified Algorithm Created by Christopher "Kerm Martian" Mitchell*/
short unsigned int heightcolor(fix z, fix z_min, fix z_max) {
         fix frac = fix_div((z-z_min),(z_max-z_min));
         
         //color!
         //calculate the R/G/B values
         fix r = (FIX_ONE>>2)-frac;
         fix g = (FIX_ONE>>1)-frac;
         fix b = ((FIX_ONE>>1) + (FIX_ONE>>2))-frac;
         
         //absolute value
         r = (r>0)?r:-r;
         g = (g>0)?g:-g;
         b = (b>0)?b:-b;
         
         //invert
         r = (FIX_ONE>>2)-r;
         g = (fix_div(FIX_ONE, fix_fromint(3)))-g;
         b = (FIX_ONE>>2)-b;
         
         //scale the chromatic triangles
         r = (r>0)?(fix_mul(fix_fromint(6), r)):0;
         g = (g>0)?(fix_mul(fix_fromint(6), g)):0;
         b = (b>0)?(fix_mul(fix_fromint(6), b)):0;   
         
         //clip the top of the chromatic triangles
         r = (r>FIX_ONE)?FIX_ONE:r;
         g = (g>FIX_ONE)?FIX_ONE:g;
         b = (b>FIX_ONE)?FIX_ONE:b;
         
         //adjust the bottom end of the scale so that z_min is red, not black
         if (frac < FIX_ONE>>2) r = (r+FIX_ONE)>>1;
         
         //adjust the top end of the scale so that z_max is blue, not black
         if (frac > (FIX_ONE>>2) + (FIX_ONE>>1)) b = (b+FIX_ONE)>>1;
         
         //put the bits together
         return (short unsigned int)(0x0000ffff & (
            fix_toint(fix_mul(fix_fromint(31), r))<<11 |
            fix_toint(fix_mul(fix_fromint(63), g))<<5 |
            fix_toint(fix_mul(fix_fromint(31), b))
            ));   
}


