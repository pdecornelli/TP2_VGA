#include <stdint.h>
#include "bsp/bsp.h"
#include "lib_draw.h"
#include "imagenes/fondo.h"
#include "imagenes/nave0.h"
#include "imagenes/nave1.h"

int main(void) {
	uint8_t posx = 142, posy = 100;
	uint32_t nave = 0;

	bsp_init();

	while (1) {
		VGA_DrawImage(&fondo, 0, 0);
		if (nave++ % 8 > 3) {
			VGA_DrawImageAlpha(&nave1, posx, posy, 0x1C);
		} else {
			VGA_DrawImageAlpha(&nave0, posx, posy, 0x1C);
		}

		bsp_draw();
	}
}
