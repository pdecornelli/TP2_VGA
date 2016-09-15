#ifndef _BSP_H
#define _BSP_H

typedef enum{
	SW_RIGHT=0, SW_LEFT=1, SW_UP=2, SW_DOWN=3, SW_DISC=4
}SW_typedef;


void bsp_init(void);

void led_on(uint8_t led);
void led_off(uint8_t led);
void led_toggle(uint8_t led);

uint8_t get_sw_state(SW_typedef sw);

uint16_t adc_get(void);

void bsp_FillScreen(uint8_t color);
void bsp_SetPixel(uint16_t xp, uint16_t yp, uint8_t color);
void bsp_draw(void);


#endif
