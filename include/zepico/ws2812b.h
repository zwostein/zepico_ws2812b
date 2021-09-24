/**
 * \file
 * \brief The ws2812b driver's header file.
 *
 * \defgroup WS2812B ws2812b
 * \brief WS2812B driver using the PIO and DMA interfaces.
 *
 * This driver utilizes one or more PIO blocks and DMA controller channels to efficiently drive
 * WS2812B (or compatible) LED chips.
 *
 * Two buffers are used to exchange data between the CPU and the WS2812Bs.
 * While one buffer can be updated by the CPU, the other one is transferred to the PIO via DMA.
 *
 * \sa \ref WS2812B_examples
 * @{
 */

#ifndef WS2812B_INCLUDED
#define WS2812B_INCLUDED

#include <hardware/pio.h>

#include <stdint.h>
#include <stddef.h>

/// To allow dynamic memory allocation, set \ref WS2812B_ALLOW_DYNAMIC to 1. This will enable the \ref ws2812b_create and \ref ws2812b_destroy functions.
#ifndef WS2812B_ALLOW_DYNAMIC
#	define WS2812B_ALLOW_DYNAMIC 1
#endif

/// All available PIO instances shall be listed here. The driver keeps track of the PIO program for each block, so multiple instances of the driver can share the same program.
#ifndef WS2812B_PIO_INSTANCES
#	define WS2812B_PIO_INSTANCES { pio0, pio1 }
#endif

/** \brief Statically creates a driver instance.
 *
 * Example:
 * \snippet static.c Create an instance
 *
 * \param Pio  The PIO block to use.
 * \param Pin  The pin to use as output.
 * \param Freq Frequency of the generated signal in Hz. Usually 800kHz.
 * \param Size Number of individual 8 bit channels - usually 3 per LED.
 */
#define WS2812B_STATIC_CREATE(Pio,Pin,Freq,Size) \
{                                                \
	.pio               = (Pio),                  \
	.sm                = 0,                      \
	.dma               = 0,                      \
	.pin               = (Pin),                  \
	.freq              = (Freq),                 \
	.size              = (Size),                 \
	.updateBuffer      = (uint8_t[Size]){0},     \
	.dmaBuffer         = (uint8_t[Size]){0},     \
	.frame_duration_us = 0,                      \
	.frame_started_us  = 0,                      \
}


#ifdef __cplusplus
extern "C" {
#endif


/// The handle for a driver instance.
typedef struct
{
	PIO    pio; ///< PIO block
	uint   sm;  ///< PIO state machine
	uint   dma; ///< DMA controller
	uint   pin; ///< GPIO pin
	float  freq; ///< Frequency
	size_t size; ///< Size of buffer

	uint8_t * updateBuffer; ///< The buffer the CPU is supposed to update
	uint8_t * dmaBuffer;    ///< The buffer transferred via DMA to PIO

	uint32_t frame_duration_us; ///< Duration of a single frame
	uint32_t frame_started_us;  ///< Timestamp of the last start of a transfer
} ws2812b_t;


#if WS2812B_ALLOW_DYNAMIC

/** \brief Dynamically creates a driver instance.
 *
 * Example:
 * \snippet dynamic.c Create an instance
 *
 * \note Only available if \ref WS2812B_ALLOW_DYNAMIC is enabled.
 *
 * \param pio  PIO block to use.
 * \param pin  GPIO pin to use as output.
 * \param freq Frequency of the generated signal in Hz. Usually 800kHz.
 * \param size Number of individual 8 bit channels - usually 3 per LED.
 * \param bufferA Pointer to a preallocated buffer of \p size bytes. If \c NULL, the buffer will be allocated on creation.
 * \param bufferB Pointer to a preallocated buffer of \p size bytes. If \c NULL, the buffer will be allocated on creation.
 */
ws2812b_t ws2812b_create( PIO pio, unsigned pin, float freq, size_t size, uint8_t * bufferA, uint8_t * bufferB );

/** \brief Frees the driver instance.
 *
 * \attention This function will call \c free on the buffers provided to \ref ws2812b_create. If you do not want this, do not call this function!
 * \note Only available if \ref WS2812B_ALLOW_DYNAMIC is enabled.
 * \param ws Pointer to the instance.
 */
void ws2812b_destroy( ws2812b_t * ws );

#endif


/** \brief Initializes a driver instance. Claims and configures PIO and DMA accordingly.
 *
 * \param ws Pointer to the instance.
 */
void ws2812b_init( ws2812b_t * ws );

/** \brief Unclaims PIO and DMA resources.
 *
 * \param ws Pointer to the instance.
 */
void ws2812b_deinit( ws2812b_t * ws );

/** \brief Returns the size of a buffer in bytes.
 *
 * \param ws Pointer to the instance.
 * \return Size of buffer in bytes.
 */
static inline size_t ws2812b_getSize( ws2812b_t * ws ) { return ws->size; }

/** \brief Returns the buffer to be updated. Once done it can be transferred to the WS2812Bs using \ref ws2812b_apply.
 *
 * \param ws Pointer to the instance.
 * \return Pointer to a buffer ready to be updated.
 */
static inline uint8_t * ws2812b_getBuffer( ws2812b_t * ws ) { return ws->updateBuffer; }

/** \brief Indicates whether the previous update has been applied and a new one can be started via \ref ws2812b_apply.
 *
 * \param ws Pointer to the instance.
 * \return \c true if previous update finished.
 */
bool ws2812b_canApply( ws2812b_t * ws );

/** \brief Initiates an update of the WS2812Bs.
 *
 * \param ws       Pointer to the instance.
 * \param blocking If \c true this function will wait until the previous update finished.
 * \return \c true if the previous update finished and a new update was initiated.
 */
bool ws2812b_apply( ws2812b_t * ws, bool blocking );


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * \page WS2812B_examples ws2812b examples
 * \brief A collection of examples.
 * \tableofcontents
 *
 * \section WS2812B_example_static_sec Static Example
 * Statically initializes driver instances.
 * \include static.c
 *
 * \section WS2812B_example_dynamic_sec Dynamic Example
 * Dynamically initializes driver instances.
 * \include dynamic.c
 **/

#endif
