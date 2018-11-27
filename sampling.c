/*************************************************************************
 *			Билинейное ресэмплирование.
 *	Входные параметры:
 *   depth	 - цветовая глубина
 *   OldWidth    - старый размер сетки
 *   OldHeight   - старый размер сетки
 *   NewWidth    - новый размер сетки
 *   NewHeight   - новый размер сетки
 *   A           - массив значений функции на старой сетке.
 *                 Нумерация элементов [0..OldHeight-1, 0..OldWidth-1]
 *
 *      Выходные параметры:
 *  B           - массив значений функции на новой сетке.
 *                 Нумерация элементов [0..NewHeight-1,0..NewWidth-1]
 * Допустимые значения параметров:
 *   OldWidth>1, OldHeight>1
 *   NewWidth>1, NewHeight>1
 *************************************************************************/
#include <stdio.h>
#include <math.h>
#include <sys/types.h>


/* 8 бит на пиксел-пиксел занимает ровно 1 байт */
#define PIXEL8 *((u_char*)b + (i * newwidth) + j)
#define A8_1   *((u_char*)a + (l * oldwidth) + c)
#define A8_2   *((u_char*)a + (l * oldwidth) + c + 1) 	
#define A8_3   *((u_char*)a + ((l + 1)* oldwidth) + c + 1)
#define A8_4   *((u_char*)a + ((l + 1)* oldwidth) + c)	      	 


/* 15 и 16 бит на пиксел-пиксел занимает ровно 2 байта */
#define PIXEL16	*((u_short*)b + (i * newwidth) + j)
#define A16_1   *((u_short*)a + (l * oldwidth) + c)
#define A16_2   *((u_short*)a + (l * oldwidth) + c + 1) 	
#define A16_3   *((u_short*)a + ((l + 1)* oldwidth) + c + 1)
#define A16_4   *((u_short*)a + ((l + 1)* oldwidth) + c)	      	 


/* 24 или 32 бит на пиксел-пиксел занимает ровно 4 байта */
#define PIXEL32 *((u_int*)b + (i * newwidth) + j)
#define A32_1   *((u_int*)a + (l * oldwidth) + c)
#define A32_2   *((u_int*)a + (l * oldwidth) + c + 1) 	
#define A32_3   *((u_int*)a + ((l + 1)* oldwidth) + c + 1)
#define A32_4   *((u_int*)a + ((l + 1)* oldwidth) + c)	      	 



void
bilinear_resample (int depth, int oldwidth, int oldheight, int newwidth, int newheight, void *a, void *b) 
{
  int i;
  int j;
  int l;
  int c;
  float t;
  float u;
  float tmp;
  u_char red, green, blue;

  for (i = 0; i <= newheight - 1; i++) {
        for (j = 0; j <= newwidth - 1; j++) {
	  
          tmp = (float) (i) / (float) (newheight - 1) * (oldheight - 1);
	  l = (int) floor (tmp);
  	     if (l < 0) {
	      l = 0;
	    }else  {
	      if (l >= oldheight - 1) {
		  l = oldheight - 2;
		}
	    }
	    
	  u = tmp - l;
	  tmp = (float) (j) / (float) (newwidth - 1) * (oldwidth - 1);
	  c = (int) floor(tmp);
	  if (c < 0){
	      c = 0;
	    } else {
	      if (c >= oldwidth - 1) {
		  c = oldwidth - 2;
		}
	    }
   	  t = tmp - c;

	 /* По компонентная интерполяция: R G B */
           if (depth == 8) {	   	   
   	   /* Используется таблица цветов-это пока не сделано */  
   		;  
   	  
	   } else if (depth == 15) {	   	   
   	   /* По 5 бит на каждый цвет */

		/* Red color */		
	    red = (1 - t) * (1 - u) *  (A16_1 >> 10) + t * (1 - u) * (A16_2 >> 10) + t * u * (A16_3 >> 10) + (1 - t) * u * (A16_4 >> 10);

	       /* Green color */
	    green = (1 - t) * (1 - u) * ((A16_1 >> 5) & 0x1f) + t * (1 - u) * ((A16_2 >> 5) & 0x1f) + 
	    t * u * ((A16_3 >> 5) & 0x1f) + (1 - t) * u * ((A16_4 >> 5) & 0x1f); 

	      /* Blue color */
	    blue = (1 - t) * (1 - u) * (A16_1 & 0x1f) + t * (1 - u) * (A16_2 & 0x1f) + t * u * (A16_3 & 0x1f) + (1 - t) * u * (A16_4 & 0x1f);     	  

	      /* Full pixel from R G B */
	    PIXEL16 =  (red << 10) | (green << 5) | (blue);    	  	    
  	   } else if (depth == 16) { 
   	   /* Red 5 бит, Green 6 бит, Blue 5 бит */

		/* Red color */		
	    red = (1 - t) * (1 - u) *  (A16_1 >> 11) + t * (1 - u) * (A16_2 >> 11) + t * u * (A16_3 >> 11) + (1 - t) * u * (A16_4 >> 11);

	       /* Green color */
	    green = (1 - t) * (1 - u) * ((A16_1 >> 5) & 0x3f) + t * (1 - u) * ((A16_2 >> 5) & 0x3f) + 
	    t * u * ((A16_3 >> 5) & 0x3f) + (1 - t) * u * ((A16_4 >> 5) & 0x3f); 

	      /* Blue color */
	    blue = (1 - t) * (1 - u) * (A16_1 & 0x1f) + t * (1 - u) * (A16_2 & 0x1f) + t * u * (A16_3 & 0x1f) + (1 - t) * u * (A16_4 & 0x1f); 
	    
	      /* Full pixel from R G B */
	    PIXEL16 =  (red << 11) | (green << 5) | (blue);    	  

    	   } else if (depth >= 24) {

		/* Red color */		
	    red = (1 - t) * (1 - u) *  (A32_1 >> 16) + t * (1 - u) * (A32_2 >> 16) + t * u * (A32_3 >> 16) + (1 - t) * u * (A32_4 >> 16);


	       /* Green color */
	    green = (1 - t) * (1 - u) * ((A32_1 >> 8) & 0xff) + t * (1 - u) * ((A32_2 >> 8) & 0xff) + 
	    t * u * ((A32_3 >> 8) & 0xff) + (1 - t) * u * ((A32_4 >> 8) & 0xff); 
	    

	      /* Blue color */
	    blue = (1 - t) * (1 - u) * (A32_1 & 0xff) + t * (1 - u) * (A32_2 & 0xff) + t * u * (A32_3 & 0xff) + (1 - t) * u * (A32_4 & 0xff);   


	    /* Full pixel from R G B */
	    PIXEL32 =  (red << 16) | (green << 8) | (blue);	     
	    } else {
		printf("This depth = %d's not supported\n", depth);
		return NULL;
	    }
	}
    }
}
