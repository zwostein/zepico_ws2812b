add_library(zepico_ws2812b INTERFACE)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/docs)

target_sources(zepico_ws2812b INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/ws2812b.c
)

target_include_directories(zepico_ws2812b INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(zepico_ws2812b INTERFACE
	pico_stdlib
	hardware_clocks
	hardware_pio
	hardware_dma
)

pico_generate_pio_header(zepico_ws2812b ${CMAKE_CURRENT_LIST_DIR}/ws2812b.pio)
