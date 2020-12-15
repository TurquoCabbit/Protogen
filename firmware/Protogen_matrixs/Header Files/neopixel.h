// How many NeoPixels are attached to the Arduino?
#define neo_ring_single  16
#define neo_ring_total  neo_ring_single * 2 

//*********color**************							  R  G  B  W
#define set_color_off()                   neo_ring->setColor(0, 0, 0, 0)
#define set_color_white()                 neo_ring->setColor(0, 0, 0, 255)
#define set_color_red()                   neo_ring->setColor(255, 0, 0, 0)
#define set_color_green()                 neo_ring->setColor(0, 255, 0, 0)
#define set_color_blue()                  neo_ring->setColor(0, 0, 255, 0)
#define set_color_nightlander_green()   neo_ring->setColor(0, 255, 100, 0)
//*********color**************

LedInterruptDriver<NS(250), NS(625), NS(375)> ws2812driver;  // define driver here ! important for memory allocation
														     // timing defined for 2812 chip

