#include <pico/stdlib.h>
#include <zepico/ws2812b.h>
#include <string.h>

#define MAX_BRIGHTNESS (63)


//! [Create an instance]
ws2812b_t ws = WS2812B_STATIC_CREATE(pio0,/*pin*/18,/*freq*/800000,/*size*/45);
//! [Create an instance]


int main( void ) {
	stdio_init_all();
	ws2812b_init( &ws );
	
	unsigned counter = 0;
	while( true ) {
		// cycle through every LED one after another
		uint8_t * leds = ws2812b_getBuffer( &ws );
		size_t leds_size = ws2812b_getSize( &ws );
		memset( leds, 0, leds_size );
		leds[counter/1024 % leds_size] = MAX_BRIGHTNESS;
		ws2812b_apply( &ws, true );
		counter++;
	}
	return 0;
}
