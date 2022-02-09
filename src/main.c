#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <string.h>
#include <stdlib.h>


#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 192
#define MAX_LOOPS 10



void draw(float centre_x, float centre_y, float scale){
	HourGlass();
	register float ci;
	register float cr;
    for(unsigned int i = 0; i<SCREEN_HEIGHT; i++)
	{
		ci = ((float) i /SCREEN_HEIGHT)*scale - centre_y;
		for(unsigned int j = 0; j<SCREEN_WIDTH; j++)
		{
			cr =((float) j /SCREEN_WIDTH)*scale*1.5 - centre_x;
			
			
			
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
			
			

			unsigned short shade = 31- 31 * iter/ MAX_LOOPS ;
			
			Bdisp_SetPointWB_VRAM(j,i,shade*0x0841);
			
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
	float step = 0.25;
	
    Bdisp_EnableColor(1);//Enable 16-bit mode
    
    Bdisp_AllClr_VRAM();
    
    int key;
   
	int running = 1;
	float centre_x =2.0;
	float centre_y=1.0;
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
    return 0;
}


