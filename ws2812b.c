#include "zepico/ws2812b.h"
#include "ws2812b.pio.h"

#include <hardware/pio.h>
#include <hardware/clocks.h>
#include <hardware/timer.h>
#include <hardware/dma.h>

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
	bool loaded;
	uint offset;
} ws2812b_pio_program_location_t;

static const PIO ws2812b_pio_instances[] = WS2812B_PIO_INSTANCES;

static ws2812b_pio_program_location_t ws2812b_pio_program_locations[count_of(ws2812b_pio_instances)] = {{0}};


static uint ws2812b_pio_add_program_if_needed( PIO pio )
{
	uint index = pio_get_index( pio );
	assert( index < count_of(ws2812b_pio_instances) );

	if( ws2812b_pio_program_locations[index].loaded ) {
		return ws2812b_pio_program_locations[index].offset;
	}
	
	uint offset = pio_add_program( pio, &ws2812b_pio_program );
	ws2812b_pio_program_locations[index].offset = offset;
	ws2812b_pio_program_locations[index].loaded = true;
	return offset;
}


#if WS2812B_ALLOW_DYNAMIC

ws2812b_t ws2812b_create( PIO pio, unsigned pin, float freq, size_t size, uint8_t * bufferA, uint8_t * bufferB )
{
	assert( freq );
	assert( size );
	assert( ( bufferA &&  bufferB)
	     || (!bufferA && !bufferB) );

	if( !bufferA ) {
		bufferA = malloc( size );
	}
	if( !bufferB ) {
		bufferB = malloc( size );
	}

	assert( bufferA );
	assert( bufferB );

	return (ws2812b_t) {
		.pio               = pio,
		.sm                = 0,
		.dma               = 0,
		.pin               = pin,
		.freq              = freq,
		.size              = size,
		.updateBuffer      = bufferA,
		.dmaBuffer         = bufferB,
		.frame_duration_us = 0,
		.frame_started_us  = 0,
	};
}


void ws2812b_destroy( ws2812b_t * ws )
{
	assert( ws );
	if( ws->updateBuffer ) {
		free( ws->updateBuffer );
		ws->updateBuffer = NULL;
	}
	if( ws->dmaBuffer ) {
		free( ws->dmaBuffer );
		ws->dmaBuffer = NULL;
	}
}

#endif


void ws2812b_init( ws2812b_t * ws )
{
	assert( ws );

	////////////////////////////////
	// PIO
	ws->sm = pio_claim_unused_sm( ws->pio, /*required*/true );

	uint offset = ws2812b_pio_add_program_if_needed( ws->pio );
	
	pio_gpio_init( ws->pio, ws->pin );
	pio_sm_set_consecutive_pindirs( ws->pio, ws->sm, ws->pin, /*pin_count*/1, /*is_out*/true );

	pio_sm_config c = ws2812b_pio_program_get_default_config( offset );
	sm_config_set_sideset_pins( &c, ws->pin );
	sm_config_set_out_shift( &c, /*shift_right*/false, /*autopull*/true, /*pull_threshold*/8 );
	sm_config_set_fifo_join( &c, PIO_FIFO_JOIN_TX );

	float div = clock_get_hz( clk_sys ) / (ws->freq * ws2812b_pio_CYCLES_PER_BIT);
	sm_config_set_clkdiv( &c, div );

	pio_sm_init( ws->pio, ws->sm, offset, &c );
	pio_sm_set_enabled( ws->pio, ws->sm, true );
	////////////////////////////////

	////////////////////////////////
	// DMA
	ws->dma = dma_claim_unused_channel( /*required*/true );
	dma_channel_config channel_config = dma_channel_get_default_config( ws->dma );
    channel_config_set_dreq( &channel_config, pio_get_dreq(ws->pio, ws->sm, /*is_tx*/true) );
	channel_config_set_transfer_data_size( &channel_config, DMA_SIZE_8 );
	channel_config_set_irq_quiet( &channel_config, /*irq_quiet*/true );
	dma_channel_configure( ws->dma, &channel_config, &ws->pio->txf[ws->sm], /*read_addr*/NULL, ws->size, /*trigger*/false );
	////////////////////////////////

	// the time needed to transfer a complete buffer plus reset pause
	ws->frame_duration_us = ceilf( (float)(ws->size * 8) * 1000000.0f / (float)ws->freq ) + ws2812b_pio_RESET_US;
}


void ws2812b_deinit( ws2812b_t * ws )
{
	assert( ws );
	pio_sm_unclaim( ws->pio, ws->sm );
	dma_channel_unclaim( ws->dma );
}


bool ws2812b_isBusy( const ws2812b_t * ws )
{
	assert( ws );
	return time_us_32() - ws->frame_started_us < ws->frame_duration_us;
}


bool ws2812b_apply( ws2812b_t * ws, bool blocking )
{
	assert( ws );

	if( blocking ) {
		while( ws2812b_isBusy( ws ) ) {
		}
	} else {
		if( ws2812b_isBusy( ws ) ) {
			return false;
		}
	}

	assert( !dma_channel_is_busy( ws->dma ) );

	// swap buffers
	uint8_t * tmp = ws->updateBuffer;
	ws->updateBuffer = ws->dmaBuffer;
	ws->dmaBuffer = tmp;

	dma_channel_set_read_addr( ws->dma, ws->dmaBuffer, /*trigger*/true );

	ws->frame_started_us = time_us_32();
	return true;
}
