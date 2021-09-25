# WS2812B driver for the Raspberry Pi Pico

This driver efficiently drives WS2812B (or compatible) LED chips on the RP2040 and integrates into the pico-sdk.

## How to get it running:

In your CMakeLists.txt add:

	add_subdirectory(path/to/zepico_ws2812b)

And link your target to *zepico_ws2812b*:

	target_link_libraries(${TARGET} ... zepico_ws2812b ...)

Have a look at one of the provided examples.
