find_package(Doxygen QUIET)

option(ZEPICO_WS2812B_BUILD_DOCS "Build WS2812B driver documentation" OFF)
if(ZEPICO_WS2812B_BUILD_DOCS)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()
	set(DOXYGEN_PROJECT_NAME ws2812b)
	set(DOXYGEN_PROJECT_BRIEF "A driver for WS2812B (and compatible) LED chips.")
	set(DOXYGEN_EXAMPLE_PATH "../example/")
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
	set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
	set(DOXYGEN_EXTRACT_STATIC YES)
	set(DOXYGEN_STRIP_FROM_PATH "../include/")
	doxygen_add_docs(ws2812b_docs
		../include/
		../README.md
	)
endif()
