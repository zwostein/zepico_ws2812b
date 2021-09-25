cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

# Pull in SDK (must be before project)
include(pico_sdk_import)

project(zepico_ws2812b_example C CXX ASM)
set(CMAKE_C_STANDARD 11)

# Initialize the SDK
pico_sdk_init()

add_compile_options(
	-Wall
	-Wextra
)

add_subdirectory(${CMAKE_SOURCE_DIR}/../ zepico_ws2812b)

set(EXAMPLE_SOURCES
	${CMAKE_SOURCE_DIR}/dynamic.c
	${CMAKE_SOURCE_DIR}/static.c
	${CMAKE_SOURCE_DIR}/parallel.c
)

foreach( EXAMPLE_SOURCE ${EXAMPLE_SOURCES} )
	get_filename_component( EXAMPLE_EXECUTABLE ${EXAMPLE_SOURCE} NAME_WLE )
	add_executable( ${EXAMPLE_EXECUTABLE} ${EXAMPLE_SOURCE} )
	target_link_libraries( ${EXAMPLE_EXECUTABLE}
		PRIVATE
			pico_stdlib
			hardware_clocks
			hardware_pio
			hardware_dma
			zepico_ws2812b
	)
	pico_enable_stdio_usb(${EXAMPLE_EXECUTABLE} 1)
	pico_enable_stdio_uart(${EXAMPLE_EXECUTABLE} 1)
	pico_add_extra_outputs(${EXAMPLE_EXECUTABLE})
endforeach()
