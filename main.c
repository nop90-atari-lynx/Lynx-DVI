#define DVI_VERTICAL_REPEAT 4 // to reduce buffer height from 240 to 120

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"

#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"

#include "lynx.pio.h"


//#include "lynx.h"
uint32_t rgbbuffer[160*120] = {0};

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 120
#define VREG_VSEL VREG_VOLTAGE_1_30
#define DVI_TIMING dvi_timing_640x480p_60hz

const PIO pio = (DVI_DEFAULT_SERIAL_CONFIG.pio == pio0) ? pio1 : pio0;
const uint sm_video = 0;

uint32_t rgbtable[48] = {
							0, (1<<12)+ (1<<28), (2<<12)+ (2<<28),(3<<12)+ (3<<28), (4<<12)+ (4<<28), (5<<12)+ (5<<28), (6<<12)+ (6<<28), (7<<12)+ (7<<28), (8<<12)+ (8<<28), (9<<12)+ (9<<28), (10<<12)+ (10<<28), (11<<12)+ (11<<28), (12<<12)+ (12<<28), (13<<11)+ (13<<27), (14<<12)+ (14<<28), (15<<12)+ (15<<28),
							0, (1<<7) + (1<<23), (2<<7) + (2<<23),(3<<7) + (3<<23), (4<<7) + (4<<23), (5<<7) + (5<<23), (6<<7) + (6<<23), (7<<7) + (7<<23), (8<<7) + (8<<23), (9<<7) + (9<<23), (10<<7) + (10<<23), (11<<7) + (11<<23), (12<<7) + (12<<23), (13<<7) + (13<<23), (14<<7) + (14<<23), (15<<7) + (15<<23),
							0, (1<<1) + (1<<17), (2<<1) + (2<<17),(3<<1) + (3<<17), (4<<1) + (4<<17), (5<<1) + (5<<17), (6<<1) + (6<<17), (7<<1) + (7<<17), (8<<1) + (8<<17), (9<<1) + (9<<17), (10<<1) + (10<<17), (11<<1) + (11<<17), (12<<1) + (12<<17), (13<<1) + (13<<17), (14<<1) + (14<<17), (15<<1) + (15<<17)
						};

struct dvi_inst dvi0;

void core1_main() {

	dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
	while (queue_is_empty(&dvi0.q_colour_valid))
		__wfe();
	dvi_start(&dvi0);
	dvi_scanbuf_main_16bpp(&dvi0);
}

static void __not_in_flash_func(core1_scanline_callback)(void)
{
    // Note first two scanlines are pushed before DVI start
    static uint scanline = 2;
    // Discard any scanline pointers passed back
    uint16_t *bufptr;
    while (queue_try_remove_u32(&dvi0.q_colour_free, &bufptr))
    bufptr = (uint16_t *)&(rgbbuffer[scanline * FRAME_WIDTH/2]);
    queue_add_blocking_u32(&dvi0.q_colour_valid, &bufptr);
    scanline = (scanline + 1) % FRAME_HEIGHT;
}

int __not_in_flash_func(main)() {

	uint32_t component;
	uint32_t rgbval;
	uint32_t pixeloff;
	uint32_t vblon;
	uint32_t pinval;

	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz , true);

	component=0;
	rgbval=0;
	pixeloff=6*160;
	vblon=1;

	
	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
    dvi0.scanline_callback = core1_scanline_callback;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

	//push in the first two scanlines
    uint16_t *bufptr = (uint16_t *) rgbbuffer;
    queue_add_blocking_u32(&dvi0.q_colour_valid, &bufptr);
    bufptr += FRAME_WIDTH;
    queue_add_blocking_u32(&dvi0.q_colour_valid, &bufptr);

	// Start up the DVI signalling.
	multicore_launch_core1(core1_main);

	// initialize the pins
    for (int i = 0; i < 8; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
//        gpio_set_pulls(i, false, true);
    }

    // Video
    uint offset = pio_add_program(pio, &LynxDVI_program);
    lynx_video_program_init(pio, sm_video, offset);
    pio_sm_set_enabled(pio, sm_video, true);

	while (true) {
		
		pinval = pio_sm_get_blocking(pio, sm_video);


		if(!(pinval&(1<<7)) && vblon) {
			vblon=0;
			component=0;
			rgbval=0;
			pixeloff=6*160;
		}
		else if((pinval&(1<<7)) && !vblon) {
			vblon=1;
		}

		rgbval|=rgbtable[component + (pinval&15)];
		if(component==32) {
			component=0;
			rgbbuffer[(pixeloff++)]=rgbval;
			rgbval=0;
		} else
			component+=16;

	}
}
