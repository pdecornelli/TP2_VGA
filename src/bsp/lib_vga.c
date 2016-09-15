#include "lib_vga.h"

// Prototipos
void VGA_InitIO(void);
void VGA_InitTIM(void);
void VGA_InitINT(void);
void VGA_InitDMA(void);

// Variables globales de la libreria
uint8_t buffer_mostrado = 0;
uint8_t buffer_escondido = 1;
volatile uint8_t flag_frame = 1;

// Funcion para decir que buffer se dibuja y cual se oculta
void change_buffer() {
	while (flag_frame)
		;
	flag_frame = 1;
	if (buffer_mostrado) {
		buffer_mostrado = 0;
		buffer_escondido = 1;
	} else {
		buffer_mostrado = 1;
		buffer_escondido = 0;
	}
}

// ------------------------------------------------ --------------
// Init del módulo VGA
// ------------------------------------------------ --------------
void VGA_Screen_Init(void) {
	uint16_t xp, yp;

	// Init todas las variables
	VGA.hsync_cnt = 0;
	VGA.start_adr = 0;
	VGA.dma2_cr_reg = 0;

	// RAM borrar con negro (completa)
	for (yp = 0; yp < VGA_DISPLAY_Y; yp++) {
		for (xp = 0; xp < (VGA_DISPLAY_X + 1); xp++) {
			VGA_RAM1[0][(yp * (VGA_DISPLAY_X + 1)) + xp] = 0;
			VGA_RAM1[1][(yp * (VGA_DISPLAY_X + 1)) + xp] = 0;
		}
	}

	// Init del los pins IO
	VGA_InitIO();
	// Init del temporizador
	VGA_InitTIM();
	// Init de la DMA
	VGA_InitDMA();
	// Init la interrupción
	VGA_InitINT();

	// -----------------------
	// Secure  Registrarse
	// -----------------------
	//Leer y asegurar  contenido de CR registro
	VGA.dma2_cr_reg = DMA2_Stream5->CR;
}

// ------------------------------------------------ --------------
// Llena la pantalla con un color
// ------------------------------------------------ --------------
void VGA_FillScreen(uint8_t color) {
	uint16_t xp, yp;

	for (yp = 0; yp < VGA_DISPLAY_Y; yp++) {
		for (xp = 0; xp < VGA_DISPLAY_X; xp++) {
			VGA_SetPixel(xp, yp, color);
		}
	}
}

// ------------------------------------------------ --------------
// Establece un píxel en la pantalla con un color
// Importante: Recuerda el último píxel de cada fila + 1 debe ser "negro"
// ------------------------------------------------ --------------
void VGA_SetPixel(uint16_t xp, uint16_t yp, uint8_t color) {
	if (xp >= VGA_DISPLAY_X)
		return;
	if (yp >= VGA_DISPLAY_Y)
		return;

	// Modifico el pixel
	VGA_RAM1[buffer_escondido][((uint32_t) yp * (VGA_DISPLAY_X + 1))
			+ (uint32_t) xp] = color;
}

// Configuro las entradas Salidas
void VGA_InitIO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Reloj Habilitar
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// Config como salida digital PE8 a PE15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
			| GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// Inicia nivel (Lo en todos los pines)
	GPIOE->BSRRH = VGA_GPIO_HINIBBLE;

	// H-Sync PB7 TIM4-CH2
	// PWM por Timer4 y CH3

	// Habilitar Reloj
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// Pin Config como salida digital
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configuro la Funcion Alternativa
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);

	// V-Sync PB8 Como GPIO normales

	// Habilitar Reloj
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// Pin Config como salida digital
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Inicia nivel (Hi)
	GPIOB->BSRRL = GPIO_Pin_8;
}

// Init Timers
void VGA_InitTIM(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// Init del Timer1
	// Datos de Píxeles por DMA

	// Reloj permite
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	// Timer1 init
	TIM_TimeBaseStructure.TIM_Period = VGA_TIM1_PERIODE;
	TIM_TimeBaseStructure.TIM_Prescaler = VGA_TIM1_PRESCALE;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// ---------------------------------------------
	// Init de Timer4
	// CH4 para la señal HSYNC
	// CH3 para DMA inicio gatillo
	// ---------------------------------------------

	// Clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	// Timer2 init
	TIM_TimeBaseStructure.TIM_Period = VGA_TIM2_HSYNC_PERIODE;
	TIM_TimeBaseStructure.TIM_Prescaler = VGA_TIM2_HSYNC_PRESCALE;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	// Timer4 Canal 3 (para empezar DMA Trigger)
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = VGA_TIM2_HTRIGGER_START - VGA_TIM2_DMA_DELAY;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Timer4 Channel 2 (PWM para HSYNC)
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = VGA_TIM2_HSYNC_IMP;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Habilitar todos los timers

	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

}

// Init interrupciones
// ------------------------------------------------ --------------
void VGA_InitINT(void) {
	NVIC_InitTypeDef NVIC_InitStructure;

	// Interrupcion de la DMA cuando termina la transmicion
	// DMA2, Stream5, Channel6

	DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);

	// NVIC config
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Interrupcion Timer 2 CH3
	// Termino un frame.
	// Para DMA gatillo START por CH3

	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);

	// NVIC config
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// Init DMA
void VGA_InitDMA(void) {
	DMA_InitTypeDef DMA_InitStructure;

	// DMA con clock de timer1
	// (Véase la página 217 del Manual de referencia)
	// DMA = 2, Canal = 6, Stream = 5

	// Reloj habilitar (DMA)
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	// DMA init (DMA2, Channel6, Stream5)
	DMA_Cmd(DMA2_Stream5, DISABLE);
	DMA_DeInit(DMA2_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_6;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)VGA_GPIOE_ODR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)
			& VGA_RAM1[buffer_mostrado];
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = VGA_DISPLAY_X + 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream5, &DMA_InitStructure);

	// DMA-Timer1 enable
	TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
}

// Interrupcion CC3 -> Inicio de la DMA
//
// NOTA: Por motivos de rendimiento, no se verifica que interrupcion
void TIM4_IRQHandler(void) {

	// Desactivo Bandera
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	VGA.hsync_cnt++;
	// 3ro Se mostraron todas las líneas
	if (VGA.hsync_cnt >= VGA_VSYNC_PERIODE) {
		VGA.hsync_cnt = 0;
		flag_frame = 0;
		// Muevo el puntero a la primera linea
		VGA.start_adr = (uint32_t)(&VGA_RAM1[buffer_mostrado]);
	}

	// 1ro Genero pulso H-Sync
	if (VGA.hsync_cnt < VGA_VSYNC_IMP) {
		// HSYNC a Lo
		GPIOB->BSRRH = GPIO_Pin_8;
	} else {
		// HSYNC a Hi
		GPIOB->BSRRL = GPIO_Pin_8;
	}

	// 2do Probar si DMA debe iniciarse
	if ((VGA.hsync_cnt > VGA_VSYNC_BILD_START)
			&& (VGA.hsync_cnt < VGA_VSYNC_BILD_STOP)) {
		// Sólo si la línea visible en la imagen => inicio transferencia DMA

		// DMA2 init
		DMA2_Stream5->CR = VGA.dma2_cr_reg;
		// Establecer dirección de inicio
		DMA2_Stream5->M0AR = VGA.start_adr;
		// Timer1 arrancar
		TIM1->CR1 |= TIM_CR1_CEN;
		// DMA2 enable
		DMA2_Stream5->CR |= DMA_SxCR_EN;

		// Prueba si la linea a dibujar es par
		if ((VGA.hsync_cnt & 0x01) != 0) {
			VGA.start_adr += (VGA_DISPLAY_X + 1);
		}
	}
}

// Interrucion cuando finaliza el DMA
// Nota: tarda en ejecutarse, por lo que no se cumple el tiempo de poner,
// los colores a Negro. Por eso se pone 1px mas por linea y ya queda en
// negro.
void DMA2_Stream5_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5)) {
		// Se transfirieron todos los datos del DMA2
		DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
		// paro el timer.
		TIM1->CR1 &= ~TIM_CR1_CEN;
		// deshabilito DMA2
		DMA2_Stream5->CR = 0;
		// Dejo en negro todo el puerto de salida de colores.
		GPIOE->BSRRH = VGA_GPIO_HINIBBLE;
	}
}

