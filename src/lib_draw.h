#ifndef __LIB_DRAW_H
#define __LIB_DRAW_H
#include "bsp/bsp.h"

// Estructura de Imagenes.
typedef struct Image_t {
	const uint8_t *img; // Imagen
	uint16_t width; // Ancho
	uint16_t height; // Alto
} VGA_Image;

void VGA_DrawPixel(int16_t xpos, int16_t ypos, uint8_t color);
void VGA_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
void VGA_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint8_t color);

void VGA_DrawImage(VGA_Image *img, int16_t xpos, int16_t ypos);
void VGA_DrawImageAlpha(VGA_Image *img, int16_t xpos, int16_t ypos,
		uint8_t alpha_color);

#endif
