#include <stdint.h>
#include "lib_draw.h"

extern volatile uint8_t no_se_puede_dibujar;

int16_t Graphic_sgn(int16_t x);

// funcion signo
//    1 si x > 0
//    0 si x = 0
//   -1 si x < 0
#define Graphic_sgn(x) (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)

#define VGA_DISPLAY_X   320  // Número de píxeles en la dirección X
#define VGA_DISPLAY_Y   200  // Número de píxeles en la dirección Y
// Dibuja una línea con un color
// De x1, y1 hasta x2, y2
// 0,0 = parte superior izquierda
// (Utilizado en el algoritmo Bresenham)
void VGA_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
	int16_t x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

	// Calculo distancia entre las dos dimenciones
	dx = x2 - x1;
	dy = y2 - y1;

	// determina signo del incremento
	incx = Graphic_sgn(dx);
	incy = Graphic_sgn(dy);
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;

	// Determina que la distancia es mayor
	if (dx > dy) {
		// X es la dirección rápida
		pdx = incx;
		pdy = 0;
		ddx = incx;
		ddy = incy;
		es = dy;
		el = dx;
	} else {
		//Y es la dirección rápida
		pdx = 0;
		pdy = incy;
		ddx = incx;
		ddy = incy;
		es = dx;
		el = dy;
	}

	// Inicializacion antes del bucle
	x = x1;
	y = y1;
	err = (el >> 1);
	bsp_SetPixel(x, y, color);

	// Calculo los pixeles
	for (t = 0; t < el; ++t) {
		err -= es;
		if (err < 0) {
			err += el;
			x += ddx;
			y += ddy;
		} else {
			x += pdx;
			y += pdy;
		}
		bsp_SetPixel(x, y, color);
	}
}

// Dibuja un círculo con un color
// Center = x0, y0
// Radio = radio
// 0,0 = parte superior izquierda
// (Utilizado en el algoritmo Bresenham)
void UB_VGA_Graphic_DrawCircle(int16_t x0, int16_t y0, int16_t radius,
		uint8_t color) {
	int16_t f = 1 - radius, ddF_x = 0, ddF_y = -2 * radius, x = 0, y = radius;

	bsp_SetPixel(x0, y0 + radius, color);
	bsp_SetPixel(x0, y0 - radius, color);
	bsp_SetPixel(x0 + radius, y0, color);
	bsp_SetPixel(x0 - radius, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;

		bsp_SetPixel(x0 + x, y0 + y, color);
		bsp_SetPixel(x0 - x, y0 + y, color);
		bsp_SetPixel(x0 + x, y0 - y, color);
		bsp_SetPixel(x0 - x, y0 - y, color);
		bsp_SetPixel(x0 + y, y0 + x, color);
		bsp_SetPixel(x0 - y, y0 + x, color);
		bsp_SetPixel(x0 + y, y0 - x, color);
		bsp_SetPixel(x0 - y, y0 - x, color);
	}
}

// Dibuja una imagen de 8 bits con y sin transparencia (del flash)
// Esquina inferior izquierda = xpos, ypos
// Programa de Conversión: "ImageGenerator_UB.exe"
void VGA_DrawImageAlpha(VGA_Image *img, int16_t xpos, int16_t ypos,
		uint8_t alpha_color) {
	uint16_t xn, yn;
	uint8_t color;
	const uint8_t *wert;

	wert = &img->img[0];

	if (img->width > VGA_DISPLAY_Y || img->height > VGA_DISPLAY_X)
		return;

	for (yn = 0; yn < img->height; yn++) {
		for (xn = 0; xn < img->width; xn++) {
			color = wert[yn * img->width + xn];
			if (alpha_color != color)
				bsp_SetPixel(yn + xpos, VGA_DISPLAY_Y - xn - 1 - ypos, color);
		}
	}
}
void VGA_DrawImage(VGA_Image *img, int16_t xpos, int16_t ypos) {
	uint16_t xn, yn;
	uint8_t color;
	const uint8_t *wert;

	wert = &img->img[0];

	if (img->width > VGA_DISPLAY_Y || img->height > VGA_DISPLAY_X)
		return;

	for (yn = 0; yn < img->height; yn++) {
		for (xn = 0; xn < img->width; xn++) {
			color = wert[yn * img->width + xn];
			bsp_SetPixel(yn + xpos, VGA_DISPLAY_Y - xn - 1 - ypos, color);
		}
	}
}
