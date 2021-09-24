cmake_minimum_required(VERSION 3.12)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

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
)

foreach( EXAMPLE_SOURCE ${EXAMPLE_SOURCES} )
	get_filename_component( ExampleExecutable ${EXAMPLE_SOURCE} NAME_WLE )
	add_executable( ${ExampleExecutable} ${EXAMPLE_SOURCE} )
	target_link_libraries( ${ExampleExecutable}
		PRIVATE
			pico_stdlib
			hardware_clocks
			hardware_pio
			hardware_dma
			zepico_ws2812b
	)
	pico_enable_stdio_usb(${ExampleExecutable} 1)
	pico_enable_stdio_uart(${ExampleExecutable} 1)
	pico_add_extra_outputs(${ExampleExecutable})
endforeach()


option(DOCUMENTATION "Enable generation of documentation" OFF)
if(DOCUMENTATION)
	find_package(Doxygen
		REQUIRED dot
		OPTIONAL_COMPONENTS mscgen dia
	)
	set(DOXYGEN_PROJECT_NAME ws2812b)
	set(DOXYGEN_PROJECT_BRIEF "A library implementing a driver for WS2812B (and compatible) LED chips.")
	set(DOXYGEN_EXAMPLE_PATH ${EXAMPLE_SOURCES})
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
	set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
	set(DOXYGEN_EXTRACT_STATIC YES)
	set(DOXYGEN_STRIP_FROM_PATH "../include/")
	doxygen_add_docs(doxygen
		${PROJECT_SOURCE_DIR}/../include/
		${PROJECT_SOURCE_DIR}/../README.md
	)
endif()
