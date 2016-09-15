#ifndef __LIB_VGA_H
#define __LIB_VGA_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_dma.h"

#define  VGA_COL_VERDE          0x1C
#define  VGA_COL_NEGRO          0x00
#define  VGA_COL_AZUL           0x03
#define  VGA_COL_BLANCO         0xFF
#define  VGA_COL_ROJO           0xE0

#define  VGA_COL_CYAN           0x1F
#define  VGA_COL_MAGENTA        0xE3
#define  VGA_COL_AMARILLO         0xFC

// Define a las resoluciones:
// El monitor VGA se ejecutará en modo 640x400
// La imagen que aparece es, pero sólo 320x200 pixeles
// Cada píxel se muestra como un punto de 2x2 en el monitor
#define VGA_DISPLAY_X   320  // Número de píxeles en la dirección X
#define VGA_DISPLAY_Y   200  // Número de píxeles en la dirección Y

// Estructura VGA
typedef struct {
	uint16_t hsync_cnt; // Contador Lineas
	uint32_t start_adr; // Direccion de Inicio
	uint32_t dma2_cr_reg; // Contenido del registro CR
} VGA_t;
VGA_t VGA;

// Doble Buffer de la imagen
// Se agrega un pixel mas por linea para el negro
uint8_t VGA_RAM1[2][(VGA_DISPLAY_X + 1) * VGA_DISPLAY_Y];

// TIM1
// Función: pixel Clock (reloj para la DMA Transver)
//
// Grundfreq = 2 * APB2 (APB2 = 84MHz) => TIM_CLK = 168MHz
// Freq = 168 MHz / 1/12 = 14 MHz
#define VGA_TIM1_PERIODE      11
#define VGA_TIM1_PRESCALE      0

// ------------------------------------------------ --------------
// Temporizador 2
// Función =
//     CH4: señal Hsync para generar PB11
//     CH3: el punto de disparo para el arranque DMA
//
// Grundfreq = 2 * APB1 (APB1 = 48 MHz) => TIM_CLK = 84MHz
// Freq = 84MHz / 1/2668 = 31,48kHz => T = 31,76us
// 1TIC = 11,90ns
#define  VGA_TIM2_HSYNC_PERIODE   2670
#define  VGA_TIM2_HSYNC_PRESCALE     0

#define  VGA_TIM2_HSYNC_IMP       320  // Hsync longitud del pulso (3,81us)
#define  VGA_TIM2_HTRIGGER_START  480  // HSync + BackPorch (5,71us)
#define  VGA_TIM2_DMA_DELAY        30  // Compensa el retraso desde el inicio de la DMA

// Señal VSync
// Disparo = Timer2 Update (f = 31,48kHz => T = 31,76us)
// 1TIC = 31,76us
#define  VGA_VSYNC_PERIODE        449  // Número de líneas por cuadro
#define  VGA_VSYNC_IMP              2  // VSync longitud del pulso (63,64us)
#define  VGA_VSYNC_BILD_START      37  // VSync+BackPorch (1,11ms)
#define  VGA_VSYNC_BILD_STOP      437  // VSync + + BACKPORCH longitud de la imagen (16,38ms)

// Direcciones de PORTE (registro ODR) para acceder a ella a través de DMA
// (Consulte la página 53 + 204 Manual de referencia)
//
// Nota: PE0 y PE1 están tan ocupados en el bordo del Discovery,
// Los datos de 8 bits es la salida a PE8 a PE15
// (Y por lo tanto empujado a 8bit = Desplazamiento izquierdo)
// Data Bit 0 => PE8
// Datos bit 7 => PE15
#define VGA_GPIOE_BASE_ADR     ((uint32_t)0x40021000) // ADR de Port-E
#define VGA_GPIO_ODR_OFFSET    ((uint32_t)0x00000014) // ADR registro de ODR
#define VGA_GPIO_BYTE_OFFSET   ((uint32_t)0x00000001) // Mover datos a 8bit
#define VGA_GPIOE_ODR_ADDRESS   (VGA_GPIOE_BASE_ADR | VGA_GPIO_ODR_OFFSET | VGA_GPIO_BYTE_OFFSET)

// Definir el color "negro" al tema PE8 hasta PE15
#define VGA_GPIO_HINIBBLE  ((uint16_t)0xFF00) // GPIO_Pin_8 - GPIO_Pin_15

// Funciones
void VGA_Screen_Init(void);
void VGA_FillScreen(uint8_t color);
void VGA_SetPixel(uint16_t xp, uint16_t yp, uint8_t color);
void change_buffer();

#endif
