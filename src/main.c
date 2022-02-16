#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>
#include <string.h>
#include <stdlib.h>


#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 192
#define MAX_LOOPS 30


short unsigned int heightcolor(float z, float z_min, float z_max);

unsigned short Bdisp_GetPointWB_VRAM(int x, int y)
{
	return Bdisp_GetPoint_VRAM(x, y+24);
}
// Optimisation check if it is in main cardiods 
unsigned int inCardiod(const float r, const float i)
{
	float x = r-0.25f;
	float y = i;
	if ((x*x + y*y + 0.5f*x) * (x*x + y*y + 0.5f*x) - 0.25f * (x*x + y*y)<0.0)
		return 1;
	
	x = r+1.0f;
	if (x*x + y*y < (1.0/16.0))
		return 1;
	
	x = r+1.3125f;
	if (x*x + y*y < (1.0/(16.0*16.0)))
		return 1;
	return 0;
}

// draws into VRAM rectangle of mandelbrot corner t_x, t_y ...
// scale zooms into c_x, c_y
void mandelbrot(unsigned int t_x, unsigned int t_y, unsigned int w, unsigned int h, float scale, float c_x, float c_y)
{
	
	HourGlass();
	
	register float ci;
	register float cr;
	
	register float zr;
	register float zi;
	
	register float zrsqrd;
	register float zisqrd;
	
	for(unsigned int i = t_y; i <= t_y + h; i++)
	{
		ci = c_y - scale * 0.5 + scale * (float) i/SCREEN_WIDTH;
		for(unsigned int j = t_x; j <= t_x + w; j++)
		{
			cr =c_x - scale * 0.5 + scale * (float)j/SCREEN_WIDTH ;
			
			
			if (!inCardiod(cr,ci))
			{
				zr=0.0;
				zi=0.0;
				zrsqrd=0.0;
				zisqrd = 0.0;
				
				
				unsigned short iter = 0;
				while ((iter < MAX_LOOPS) && (zrsqrd + zisqrd < 4.0))
				{	
					float temp =zrsqrd-zisqrd + cr;
					
					zi = 2.0 * zr * zi + ci;
					zr = temp;
					zrsqrd = zr * zr;
					zisqrd = zi * zi;
					
					iter+=1;
				}
				unsigned short colour = heightcolor((float)iter, 0.0f, (float) MAX_LOOPS);
			
				Bdisp_SetPointWB_VRAM(j,i,colour);
			}
			else
			{
				unsigned short colour = heightcolor((float) MAX_LOOPS, 0.0f, (float) MAX_LOOPS);
			
				Bdisp_SetPointWB_VRAM(j,i,colour);
			}
			
			
			
		}
	}
			
}

// off_x number of pixels to see to the right
void draw_offset_x(int off_x, float scale, float c_x, float c_y){
	
	HourGlass();
	
	if (off_x>0)
	{
		for(unsigned int j = off_x; j<SCREEN_WIDTH; j++)
		{
			for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
			{
				unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
				Bdisp_SetPointWB_VRAM(j-off_x,i,colour);
			}
		}
		mandelbrot(SCREEN_WIDTH-off_x, 0,off_x, SCREEN_HEIGHT, scale, c_x, c_y );
	}
	else 
	{
		off_x*=-1;
		for(unsigned int j = SCREEN_WIDTH-off_x; j>=0; j--)
		{
			for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
			{
				unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
				Bdisp_SetPointWB_VRAM(j+off_x,i,colour);
			}
		}
		mandelbrot(0,0,off_x, SCREEN_HEIGHT, scale, c_x, c_y );
	}
	
	Bdisp_PutDisp_DD();
	
}

void draw_offset_y(int off_y, float scale, float c_x, float c_y){
	
	HourGlass();
	if (off_y>0)
	{
		for(unsigned int i = off_y; i<SCREEN_HEIGHT; i++)
		{
			for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
			{
				unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
				Bdisp_SetPointWB_VRAM(j,i-off_y,colour);
			}
		}
		mandelbrot(0,SCREEN_HEIGHT-off_y,SCREEN_WIDTH, off_y, scale, c_x, c_y );
	}
	else
	{	
		off_y*=-1;
		for(unsigned int i = SCREEN_HEIGHT-off_y; i>=0; i--)
		{
			for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
			{
				unsigned short colour =  Bdisp_GetPointWB_VRAM(j, i);
				Bdisp_SetPointWB_VRAM(j,i+off_y,colour);
			}
		}
		mandelbrot(0,0,SCREEN_WIDTH, off_y, scale, c_x, c_y );
	}
	
	Bdisp_PutDisp_DD();
}

void draw_scale(float scale, float c_x, float c_y)
{
	mandelbrot(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, scale, c_x, c_y);
	Bdisp_PutDisp_DD();
	
}

int main(void){
	//change_freq(PLL_24x);
	int step = 60;	
    Bdisp_EnableColor(1);//Enable 16-bit mode
    
    Bdisp_AllClr_VRAM();
    
    int key;
   
	int running = 1;
	float c_x =-2.0;
	float c_y = -1.0;
	float scale = 4.0;
	float zoom = 0.25f;
	draw_scale(scale, c_x,c_y);
    while(running)
    {
    	
		GetKey(&key);
		switch (key){
			 case KEY_CTRL_EXE:
			 	running = 0;
			 	break;
			 	
			 case KEY_CTRL_UP:
			 	c_y = c_y - scale * ((float)step/SCREEN_WIDTH);
			 	draw_offset_y(-step,scale, c_x, c_y);
			 	break;
			 	
			 case KEY_CTRL_DOWN:
			 	c_y = c_y + scale * ((float)step/SCREEN_WIDTH);
			 	draw_offset_y(step,scale, c_x, c_y);
			 	break;
			 	
			 case KEY_CTRL_LEFT:
			 	c_x = c_x - scale * ((float)step/SCREEN_WIDTH);
			 	draw_offset_x(-step,scale, c_x, c_y);
			 	break;
			 
			 case KEY_CTRL_RIGHT:
			 	c_x = c_x + scale * ((float)step/SCREEN_WIDTH);
			 	draw_offset_x(step, scale,c_x, c_y);
			 	break;
			 
			 case KEY_CHAR_PLUS:
				scale*=zoom;
				draw_scale(scale, c_x, c_y);
				break;
				
			 case KEY_CHAR_MINUS:
				scale*=1.0/zoom;
				draw_scale(scale, c_x, c_y);
				break;
				
		}
		
    }
    //change_freq(PLL_16x);
    return 0;
}

/*Created by Christopher "Kerm Martian" Mitchell*/
short unsigned int heightcolor(float z, float z_min, float z_max) {
         float frac = ((z-z_min)/(z_max-z_min));
         
         //color!
         float r = (0.25f)-frac;
         float g = (0.5f)-frac;
         float b = (0.75f)-frac;

         //calculate the R/G/B values
         r = (r>0.f)?r:-r; g = (g>0.f)?g:-g; b = (b>0.f)?b:-b;   //absolute value
         r = (0.25f)-r; g = (1.f/3.f)-g; b = (0.25f)-b;   //invert
         r = (r>0.f)?(6.f*r):0.f; g = (g>0.f)?(6.f*g):0.f; b = (b>0.f)?(6.f*b):0.f;   //scale the chromatic triangles
         r = (r>1.f)?1.f:r; g = (g>1.f)?1.f:g; b = (b>1.f)?1.f:b;   //clip the top of the chromatic triangles
         if (frac < 0.25f) r = (r+1.f)/2.f;   //adjust the bottom end of the scale so that z_min is red, not black
         if (frac > 0.75f) b = (b+1.f)/2.f;   //adjust the top end of the scale so that z_max is blue, not black
         return (short unsigned int)(0x0000ffff & (((int)(31.f*r) << 11) | ((int)(63.f*g) << 5) | ((int)(31.f*b))));   //put the bits together
}


