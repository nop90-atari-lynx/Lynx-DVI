# Lynx-DVI
Atari Lynx DVI out hardware mod using a rp2040-PiZero

This is a work in progress project in its alpha stage.

The project is based on the amazing RP2040-PiZero board and uses the PicoDVI lib (https://github.com/Wren6991/PicoDVI) to output a DVI signal with VGA resolution, so the lynx can be connected to screen with an HDMI port with a standard HDMI cable. 

My project started with the idea to output on the DVI line only the video and connect the audio jack of the lynx to the Audio IN of the TV, but last week I found that there is a similar project for the N64 (https://github.com/kbeckmann/PicoDVI-N64) that also drives the audio on the DVI line, so after tuning the video part I'll try to add the audio as well.

I have to thank the Retrosix site for the specs of the lynx video signal (https://www.retrosix.wiki/lcd-interface-atari-lynx-ii)

# WIRINGS

GPIO0 = D0
GPIO1 = D1
GPIO2 = D2
GPIO3 = D3
GPIO4 = Clock1
GPIO5 = Clock2
GPIO6 = Clock3
GPIO7 = VSYNC

RP2040-PiZero states the GPIO are not 5v tolerant; they are not full 5v tolerant, but can handle the lynx video signal so afters some test decided to not use level shifters.  
