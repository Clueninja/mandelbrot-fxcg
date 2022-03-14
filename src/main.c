#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>
#include <string.h>
#include <stdlib.h>

#define FIXEDPT_BITS 32
#include "fixedptc.h"


#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 192
#define MAX_LOOPS 30


short unsigned int heightcolor(fixedpt z, fixedpt z_min, fixedpt z_max);

unsigned short Bdisp_GetPointWB_VRAM(int x, int y)
{
	return Bdisp_GetPoint_VRAM(x, y+24);
}
// Optimisation check if it is in main cardiods 
unsigned int inCardiod(const fixedpt r, const fixedpt i)
{
	fixedpt x =  r - (FIXEDPT_ONE>>2);
	fixedpt y = i;
	
	if (
	    fixedpt_mul( //TODO: Optimise
	        fixedpt_mul(x, x) + fixedpt_mul(y, y) + fixedpt_mul(FIXEDPT_ONE>>1, x),
	        fixedpt_mul(x, x) + fixedpt_mul(y, y) + fixedpt_mul(FIXEDPT_ONE>>1, x)
	    ) - fixedpt_mul(FIXEDPT_ONE>>2,  fixedpt_mul(x, x) + fixedpt_mul(y, y))<0)
		return 1;
	
	x = r+FIXEDPT_ONE;
	if (fixedpt_mul(x, x) + fixedpt_mul(y, y) < FIXEDPT_ONE>>4)
		return 1;
	    // 1.3125
	x = r+ 0x00015000 ;
	if (fixedpt_mul(x, x) + fixedpt_mul(y, y) < FIXEDPT_ONE>>8)
		return 1;
	return 0;
}

// draws into VRAM rectangle of mandelbrot corner t_x, t_y ...
// scale zooms into c_x, c_y
void mandelbrot(unsigned int t_x, unsigned int t_y, unsigned int w, unsigned int h, fixedpt scale, fixedpt c_x, fixedpt c_y)
{
	
	HourGlass();
	
	register fixedpt ci;
	register fixedpt cr;
	
	register fixedpt zr;
	register fixedpt zi;
	
	register fixedpt zrsqrd;
	register fixedpt zisqrd;
	
	for(unsigned int i = t_y; i <= t_y + h; i++)
	{
		ci =  c_y - fixedpt_mul(scale, FIXEDPT_ONE>>1) + fixedpt_div(fixedpt_mul (scale,  fixedpt_fromint(i)), fixedpt_fromint(SCREEN_WIDTH));
		for(unsigned int j = t_x; j <= t_x + w; j++)
		{
			cr =  c_x - fixedpt_mul(scale, FIXEDPT_ONE>>1) + fixedpt_div(fixedpt_mul (scale,  fixedpt_fromint(j)), fixedpt_fromint(SCREEN_WIDTH));
			
			if (!inCardiod(cr,ci))
			{
				zr=0;
				zi=0;
				zrsqrd=0;
				zisqrd = 0;
				
				
				unsigned short iter = 0;
				while ((iter < MAX_LOOPS) && (zrsqrd + zisqrd < FIXEDPT_ONE<<2))
				{	
					fixedpt temp =zrsqrd-zisqrd + cr;
					
					zi = fixedpt_mul(fixedpt_mul(FIXEDPT_ONE<<1, zr),  zi) + ci;
					zr = temp;
					zrsqrd = fixedpt_mul(zr, zr);
					zisqrd = fixedpt_mul(zi, zi);
					
					iter+=1;
				}
				unsigned short colour = heightcolor(fixedpt_fromint(iter), 0, fixedpt_fromint(MAX_LOOPS));
			
				Bdisp_SetPointWB_VRAM(j,i,colour);
			}
			else
			{
				unsigned short colour = heightcolor(fixedpt_fromint(MAX_LOOPS), 0, fixedpt_fromint(MAX_LOOPS));
			
				Bdisp_SetPointWB_VRAM(j,i,colour);
			}
		}
	}			
}

// off_x number of pixels to see to the right
void draw_offset_right(int off_x, fixedpt scale, fixedpt c_x, fixedpt c_y){
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
void draw_offset_left(int off_x, fixedpt scale, fixedpt c_x, fixedpt c_y)
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

void draw_offset_down(int off_y, fixedpt scale, fixedpt c_x, fixedpt c_y){
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
void draw_offset_up(int off_y, fixedpt scale, fixedpt c_x, fixedpt c_y)
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

void draw_scale(fixedpt scale, fixedpt c_x, fixedpt c_y)
{
	mandelbrot(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, scale, c_x, c_y);
	Bdisp_PutDisp_DD();
	
}

int main(void){
	int step = 5;
    Bdisp_EnableColor(1);//Enable 16-bit mode
    
    Bdisp_AllClr_VRAM();
    
    int key;
   
	int running = 1;
	fixedpt c_x = 0;
	fixedpt c_y = 0;
	fixedpt scale = FIXEDPT_ONE<<2;
	fixedpt zoom = FIXEDPT_ONE>>1;
	draw_scale(scale, c_x,c_y);
    while(running)
    {
		GetKey(&key);
		switch (key){
			 case KEY_CTRL_EXE:
			 	running = 0;
			 	break;
			 	
			 case KEY_CTRL_UP:
			 	c_y = c_y -fixedpt_div(fixedpt_mul(scale, fixedpt_fromint(step)), fixedpt_fromint(SCREEN_WIDTH));
			 	draw_offset_up(step,scale, c_x, c_y);
			 	break;
			 	
			 case KEY_CTRL_DOWN:
			 	c_y = c_y + fixedpt_div(fixedpt_mul(scale, fixedpt_fromint(step)), fixedpt_fromint(SCREEN_WIDTH));
			 	draw_offset_down(step,scale, c_x, c_y);
			 	break;
			 	
			 case KEY_CTRL_LEFT:
			 	c_x = c_x - fixedpt_div(fixedpt_mul(scale, fixedpt_fromint(step)), fixedpt_fromint(SCREEN_WIDTH));
			 	draw_offset_left(step,scale, c_x, c_y);
			 	break;
			 
			 case KEY_CTRL_RIGHT:
			 	c_x = c_x + fixedpt_div(fixedpt_mul(scale, fixedpt_fromint(step)), fixedpt_fromint(SCREEN_WIDTH));
			 	draw_offset_right(step, scale,c_x, c_y);
			 	break;
			 
			 case KEY_CHAR_PLUS:
				scale = fixedpt_mul(zoom, scale);
				draw_scale(scale, c_x, c_y);
				break;
				
			 case KEY_CHAR_MINUS:
				scale = fixedpt_div(scale,zoom);
				draw_scale(scale, c_x, c_y);
				break;
				
		}
		
    }
    return 0;
}

/*Created by Christopher "Kerm Martian" Mitchell*/
short unsigned int heightcolor(fixedpt z, fixedpt z_min, fixedpt z_max) {

         fixedpt frac = fixedpt_div((z-z_min), (z_max-z_min));
         
         //color!
         fixedpt r = (FIXEDPT_ONE>>2)-frac;
         fixedpt g = (FIXEDPT_ONE>>1)-frac;
         fixedpt b = ((FIXEDPT_ONE>>2) + (FIXEDPT_ONE>>1))-frac;

         //calculate the R/G/B values
         r = fixedpt_abs(r);
         g = fixedpt_abs(g);
         b = fixedpt_abs(b);   //absolute value
         
         r = (FIXEDPT_ONE>>2)-r;
         g = (fixedpt_div(FIXEDPT_ONE, fixedpt_fromint(3)))-g;
         b = (FIXEDPT_ONE>>2)-b;   //invert
         
         r=(r>0)?(fixedpt_mul(fixedpt_fromint(6),r)):0;
         g =(g>0)?(fixedpt_mul(fixedpt_fromint(6),g)):0;
         b=(b>0)?(fixedpt_mul(fixedpt_fromint(6), b)):0;   //scale the chromatic triangles
         
         
         r = (r>FIXEDPT_ONE)?FIXEDPT_ONE:r;
         g = (g>FIXEDPT_ONE)?FIXEDPT_ONE:g;
         b = (b>FIXEDPT_ONE)?FIXEDPT_ONE:b;   //clip the top of the chromatic triangles
         
         
         if (frac < (FIXEDPT_ONE>>2)) r = (r+FIXEDPT_ONE)>>1;   //adjust the bottom end of the scale so that z_min is red, not black
         
         if (frac > ((FIXEDPT_ONE>>2) + (FIXEDPT_ONE>>1))) b = (b+FIXEDPT_ONE)>>1;   //adjust the top end of the scale so that z_max is blue, not black
         return (short unsigned int)(
         0x0000ffff &(
             (fixedpt_toint(fixedpt_mul(fixedpt_fromint(31),r))<<11)|
             (fixedpt_toint(fixedpt_mul(fixedpt_fromint(63),g))<<5)|
             (fixedpt_toint(fixedpt_mul(fixedpt_fromint(31),b)))
         ));   //put the bits together
}


