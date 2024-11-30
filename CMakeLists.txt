# Replace TMDS with 10 bit UART (same baud rate):
# add_definitions(-DDVI_SERIAL_DEBUG=1)
# add_definitions(-DRUN_FROM_CRYSTAL)

add_executable(LynxDVI
	main.c
	dvi.c
	dvi.h
	dvi_config_defs.h
	dvi_serialiser.c
	dvi_serialiser.h
	dvi_timing.c
	dvi_timing.h
	tmds_encode.S
	tmds_encode.c
	tmds_encode.h
	tmds_table.h
	util_queue_u32_inline.h
)

target_compile_options(LynxDVI PRIVATE -Wall)

target_compile_definitions(LynxDVI PRIVATE
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	DVI_VERTICAL_REPEAT=4
	)

target_include_directories(LynxDVI PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(LynxDVI
	pico_stdlib
	pico_multicore
	pico_util
	pico_base_headers
	pico_util
	hardware_dma
	hardware_interp
	hardware_pio
	hardware_pwm
)

pico_generate_pio_header(LynxDVI ${CMAKE_CURRENT_LIST_DIR}/dvi_serialiser.pio)
pico_generate_pio_header(LynxDVI ${CMAKE_CURRENT_LIST_DIR}/lynx.pio)

# create map/bin/hex file etc.
pico_add_extra_outputs(LynxDVI)
