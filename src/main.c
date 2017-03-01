#include <sys/_stdint.h>
#include <sys/types.h>
#include <stdlib.h>

#include "bsp/bsp.h"
#include "imagenes/auto2.h"
#include "imagenes/bache.h"
#include "imagenes/fondo.h"
#include "imagenes/fondo1.h"
#include "imagenes/inicio.h"
#include "imagenes/explosion.h"
#include "imagenes/gameover.h"
#include "lib_draw.h"

uint16_t tiempo = 0;
uint ban = 0, ban4 = 0, ban3 = 0;

int main(void) {
	uint8_t posx = 95, posy = 0;
	uint8_t persx = 80, persy = 200;
	uint8_t ban2 = 0;
	uint32_t nave = 0;
	uint32_t ruta = 0;
	uint32_t in = 0;

	bsp_init();

	while (1) {
		if (ban4 == 0) {
			if (tiempo < 3000) {
				if (in++ % 8 > 3) {
					VGA_DrawImage(&inicio, 0, 0);
				} else {
					VGA_DrawImage(&inicio, 0, 0);
				}
			} else {
				ban = 1;
				if (ruta++ % 8 > 3) {
					VGA_DrawImage(&fondo, 0, 0);
				} else {
					VGA_DrawImage(&fondo1, 0, 0);
				}
				if ((abs(persy - posy) <= 70)
						&& ((((posx - persx) <= 45) &&((posx - persx) > 0))
								|| (((persx - posx) <= 35) && ((persx - posx) > 0)))) {
					ban3 = 1;
					if (nave++ % 8 > 3) {
						VGA_DrawImage(&explosion, (posx - 13), posy);
					} else {
						VGA_DrawImage(&explosion, (posx - 13), posy);
					}
				} else {
					if (nave++ % 8 > 3) {
						persy = persy - 2;
						VGA_DrawImage(&bache, persx, persy);
						VGA_DrawImageAlpha(&auto2, posx, posy, 0x1F);

					} else {
						VGA_DrawImage(&bache, persx, persy);
						VGA_DrawImageAlpha(&auto2, posx, posy, 0x1F);
					}
				}
			}
			if (ban3 == 0) {
				if ((get_sw_state(0) == 0) && (posx <= 225)) {
					posx++;
				}
				if ((get_sw_state(1) == 0) && (posx >= 55)) {
					posx--;
				}
				if ((get_sw_state(2) == 0) && (posy <= 125)) {
					posy++;
				}
				if ((get_sw_state(3) == 0) && (posy > 0)) {
					posy--;
				}
			}
			if (persy == 0) {
				ban2 = 0;
				persy = 200;
			}
			if (ban2 == 0) {
				persx = (rand() % (225 - 55) + 55);
				ban2 = 1;
			}
			if ((persy == 0) || (persy > 200)) {
				persy = 200;
			}
		} else {
			if (in++ % 8 > 3) {
				VGA_DrawImage(&gameover, 0, 0);
			} else {
				VGA_DrawImage(&gameover, 0, 0);
			}
		}
		bsp_draw();
	}
}

void APP_1ms(void) {
	if (((ban == 0) || (ban3 == 1)) && (ban4 == 0)) {
		tiempo++;
		if (tiempo >= 6000) {
			ban4 = 1;
		}
	}
}

