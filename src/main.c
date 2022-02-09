#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>
#include <string.h>

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 192
#define MAX_LOOPS 10



void draw(double centre_x, double centre_y, double scale){
	HourGlass();
	double ci;
	double cr;
    for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
	{
		ci = ((double) i /SCREEN_HEIGHT)*scale- centre_y;
		for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
		{
			cr =((double) j /SCREEN_WIDTH)*scale*1.5 - centre_x;
			
			
			
			double zr =0.0;
			double zi = 0.0;
			
			unsigned short iter = 0;
			while ((iter < MAX_LOOPS) && (zr*zr + zi*zi < 4.0))
			{	
				double temp =zr*zr-zi*zi + cr;
				
				zi = 2.0 * zr * zi + ci;
				zr = temp;
				
				iter+=1;
			}
			unsigned short color = 0xFFFF-iter * 0xFFFF/MAX_LOOPS;
			Bdisp_SetPointWB_VRAM(j,i,color);
			
		}
	}
	Bdisp_PutDisp_DD();

}

int main(void){
	double step = 0.25;
	
    Bdisp_EnableColor(1);//Enable 16-bit mode
    
    Bdisp_AllClr_VRAM();
    
    int key;
   
	int running = 1;
	double centre_x =1.0;
	double centre_y=1.0;
	double scale = 2.0;
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
    return 0;
}


