#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>
#include <string.h>
#include <stdlib.h>
#include "fxcg_freq.h"


#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 192
#define MAX_LOOPS 30

short unsigned int heightcolor(float z, float z_min, float z_max);

void draw(float centre_x, float centre_y, float scale){
	
	HourGlass();
	register float ci;
	register float cr;
    for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
	{
		ci = ((float) i /SCREEN_HEIGHT -centre_y)*scale ;
		for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
		{
			cr =((float) j /SCREEN_WIDTH -centre_x)*scale*1.5 ;
			
			
			
			register float zr =0.0;
			register float zi = 0.0;
			
			unsigned short iter = 0;
			while ((iter < MAX_LOOPS) && (zr*zr + zi*zi < 4.0))
			{	
				float temp =zr*zr-zi*zi + cr;
				
				zi = 2.0 * zr * zi + ci;
				zr = temp;
				
				iter+=1;
			}
			
			

			unsigned short colour = heightcolor((float)iter, 0.0f, (float) MAX_LOOPS);
			
			Bdisp_SetPointWB_VRAM(j,i,colour);
			
		}
	}
	
	int x = 0; // This function uses pixels
	int y = 0;
	PrintMini(&x, &y, "Number Of Iterations:", 0, 0xffffffff, 0,0,COLOR_DARKGREEN, COLOR_GRAY, 1,0);
	
	char buffer[14]; 
	itoa(MAX_LOOPS,(unsigned char*)buffer); // Leave the garbage bytes alone
	PrintMini(&x,&y,buffer, 0, 0xffffffff, 0,0,COLOR_DARKGREEN, COLOR_GRAY, 1,0);
	
	Bdisp_PutDisp_DD();

}

int main(void){
	//change_freq(PLL_24x);
	float step = 0.25;
	
    Bdisp_EnableColor(1);//Enable 16-bit mode
    
    Bdisp_AllClr_VRAM();
    
    int key;
   
	int running = 1;
	float centre_x =0.5;
	float centre_y = 0.5;
	float scale = 2.0;
	draw(centre_x,centre_y, scale);
    while(running)
    {
    	
		GetKey(&key);
		switch (key){
			 case KEY_CTRL_EXE:
			 	running = 0;
			 	break;
			 	
			 case KEY_CTRL_UP:
			 	centre_y+=step;
			 	draw(centre_x, centre_y, scale);
			 	break;
			 	
			 case KEY_CTRL_DOWN:
			 	centre_y-=step;
			 	draw(centre_x, centre_y,scale);
			 	break;
			 	
			 case KEY_CTRL_LEFT:
			 	centre_x+=step;
			 	draw(centre_x, centre_y,scale);
			 	break;
			 
			 case KEY_CTRL_RIGHT:
			 	centre_x-=step;
			 	draw(centre_x, centre_y,scale);
			 	break;
			 
			 case KEY_CHAR_PLUS:
				scale*=0.75;
				draw(centre_x, centre_y, scale);
				break;
				
			 case KEY_CHAR_MINUS:
				scale*=1.25;
				draw(centre_x, centre_y, scale);
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


