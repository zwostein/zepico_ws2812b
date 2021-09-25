#include <pico/stdlib.h>
#include <zepico/ws2812b.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>


#define MAX_BRIGHTNESS (63)


// some channels, each will get its own effect
ws2812b_t ws1 = WS2812B_STATIC_CREATE(pio0,/*pin*/18,/*freq*/800000,/*size*/45);
ws2812b_t ws2 = WS2812B_STATIC_CREATE(pio0,/*pin*/19,/*freq*/800000,/*size*/45);
ws2812b_t ws3 = WS2812B_STATIC_CREATE(pio0,/*pin*/20,/*freq*/800000,/*size*/45);
ws2812b_t ws4 = WS2812B_STATIC_CREATE(pio0,/*pin*/21,/*freq*/800000,/*size*/45);


typedef struct __attribute__((packed)) {
	uint8_t g;
	uint8_t r;
	uint8_t b;
} grb_t;


int main( void ) {
	stdio_init_all();

	ws2812b_init( &ws1 );
	ws2812b_init( &ws2 );
	ws2812b_init( &ws3 );
	ws2812b_init( &ws4 );
	
	unsigned counter = 0;
	while( true ) {
		// cycle through every LED one after another
		{
			uint8_t * leds = ws2812b_getBuffer( &ws1 );
			size_t leds_size = ws2812b_getSize( &ws1 );
			
			memset( leds, 0, leds_size );
			leds[counter/1024 % leds_size] = MAX_BRIGHTNESS;
			
			ws2812b_apply( &ws1, true );
		}
		
		// color waves
		{
			grb_t * leds = (grb_t*)ws2812b_getBuffer( &ws2 );
			size_t leds_num = ws2812b_getSize( &ws2 ) / sizeof(grb_t);
			
			for( size_t i = 0; i < leds_num; i++ ) {
				float a = ((float)i/(float)leds_num) * 2.0f * M_PI;
				leds[i].r = (0.5f+0.5f*sinf( a + counter*0.0011 )) * (float)MAX_BRIGHTNESS;
				leds[i].g = (0.5f+0.5f*sinf( a + counter*0.0012 )) * (float)MAX_BRIGHTNESS;
				leds[i].b = (0.5f+0.5f*sinf( a + counter*0.0013 )) * (float)MAX_BRIGHTNESS;
			}
			
			ws2812b_apply( &ws2, true );
		}
		
		// random
		{
			uint8_t * leds = ws2812b_getBuffer( &ws3 );
			size_t leds_size = ws2812b_getSize( &ws3 );
			
			srand(counter/256);
			for( size_t i = 0; i < leds_size; i++ ) {
				leds[i] = ((rand() & 0xff) * MAX_BRIGHTNESS) / 255;
			}
			
			ws2812b_apply( &ws3, true );
		}
		
		// racing dots
		{
			grb_t * leds = (grb_t*)ws2812b_getBuffer( &ws4 );
			size_t leds_num = ws2812b_getSize( &ws4 ) / sizeof(grb_t);

			memset( leds, 0, sizeof(grb_t)*leds_num );

			size_t r = counter / 420 % leds_num;
			size_t g = counter / 430 % leds_num;
			size_t b = counter / 440 % leds_num;
			size_t y = counter / 450 % leds_num;
			size_t c = counter / 460 % leds_num;
			size_t v = counter / 470 % leds_num;
			
			leds[r].r = MAX_BRIGHTNESS;

			leds[g].g = MAX_BRIGHTNESS;

			leds[b].b = MAX_BRIGHTNESS;

			leds[y].r = MAX_BRIGHTNESS;
			leds[y].g = MAX_BRIGHTNESS;

			leds[c].g = MAX_BRIGHTNESS;
			leds[c].b = MAX_BRIGHTNESS;

			leds[v].b = MAX_BRIGHTNESS;
			leds[v].r = MAX_BRIGHTNESS;

			ws2812b_apply( &ws4, true );
		}
		
		counter++;
	}
	return 0;
}
