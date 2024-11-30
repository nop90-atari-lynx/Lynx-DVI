#ifndef _TMDS_ENCODE_H_
#define _TMDS_ENCODE_H_

#include "hardware/interp.h"
#include "dvi_config_defs.h"

// Functions from tmds_encode.c
void tmds_encode_data_channel_16bpp(const uint32_t *pixbuf, uint32_t *symbuf, size_t n_pix, uint channel_msb, uint channel_lsb);


// Uses interp0:
void tmds_encode_loop_16bpp(const uint32_t *pixbuf, uint32_t *symbuf, size_t n_pix);
void tmds_encode_loop_16bpp_leftshift(const uint32_t *pixbuf, uint32_t *symbuf, size_t n_pix, uint leftshift);

#endif
