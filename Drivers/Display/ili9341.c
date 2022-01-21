/**
  ******************************************************************************
  * @file    ili9341.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    02-December-2014
  * @brief   This file includes the LCD driver for ILI9341 LCD.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"
#include "font/button_pic.h"
/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup ILI9341
  * @brief This file provides a set of functions needed to drive the 
  *        ILI9341 LCD.
  * @{
  */

/** @defgroup ILI9341_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup ILI9341_Private_Defines
  * @{
  */
/**
  * @}
  */ 
  
/** @defgroup ILI9341_Private_Macros
  * @{
  */
/**
  * @}
  */  

/** @defgroup ILI9341_Private_Variables
  * @{
  */ 

LCD_DrvTypeDef   ili9341_drv = 
{
  ili9341_Init,
  ili9341_ReadID,
  ili9341_DisplayOn,
  ili9341_DisplayOff,
  0,
  0,
  0,
  0,
  0,
  0,
  ili9341_GetLcdPixelWidth,
  ili9341_GetLcdPixelHeight,
  0,
  0,    
};

/**
  * @}
  */ 
  
/** @defgroup ILI9341_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup ILI9341_Private_Functions
  * @{
  */   
volatile uint16_t LCD_HEIGHT = ILI9341_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH	 = ILI9341_SCREEN_WIDTH;

/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void ili9341_Init(void)
{
  /* Initialize ILI9341 low level bus layer ----------------------------------*/
  LCD_IO_Init();
  
  /* Configure LCD */
/*  ili9341_WriteReg(0xCA);
  ili9341_WriteData(0xC3);
  ili9341_WriteData(0x08);
  ili9341_WriteData(0x50);
  ili9341_WriteReg(LCD_POWERB);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0xC1);
  ili9341_WriteData(0x30);
  ili9341_WriteReg(LCD_POWER_SEQ);
  ili9341_WriteData(0x64);
  ili9341_WriteData(0x03);
  ili9341_WriteData(0x12);
  ili9341_WriteData(0x81);
  ili9341_WriteReg(LCD_DTCA);
  ili9341_WriteData(0x85);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x78);
  ili9341_WriteReg(LCD_POWERA);
  ili9341_WriteData(0x39);
  ili9341_WriteData(0x2C);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x34);
  ili9341_WriteData(0x02);
  ili9341_WriteReg(LCD_PRC);
  ili9341_WriteData(0x20);
  ili9341_WriteReg(LCD_DTCB);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_FRMCTR1);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x1B);
  ili9341_WriteReg(LCD_DFC);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0xA2);
  ili9341_WriteReg(LCD_POWER1);
  ili9341_WriteData(0x10);
  ili9341_WriteReg(LCD_POWER2);
  ili9341_WriteData(0x10);
  ili9341_WriteReg(LCD_VCOM1);
  ili9341_WriteData(0x45);
  ili9341_WriteData(0x15);
  ili9341_WriteReg(LCD_VCOM2);
  ili9341_WriteData(0x90);
  ili9341_WriteReg(LCD_MAC);
  ili9341_WriteData(0xC8);
  ili9341_WriteReg(LCD_3GAMMA_EN);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_RGB_INTERFACE);
  ili9341_WriteData(0xC2);
  ili9341_WriteReg(LCD_DFC);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0xA7);
  ili9341_WriteData(0x27);
  ili9341_WriteData(0x04);
  

  ili9341_WriteReg(LCD_COLUMN_ADDR);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0xEF);
  ili9341_WriteReg(LCD_PAGE_ADDR);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x01);
  ili9341_WriteData(0x3F);
  ili9341_WriteReg(LCD_INTERFACE);
  ili9341_WriteData(0x01);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x06);
  
  ili9341_WriteReg(LCD_GRAM);
  LCD_Delay(200);
  
  ili9341_WriteReg(LCD_GAMMA);
  ili9341_WriteData(0x01);
  
  ili9341_WriteReg(LCD_PGAMMA);
  ili9341_WriteData(0x0F);
  ili9341_WriteData(0x29);
  ili9341_WriteData(0x24);
  ili9341_WriteData(0x0C);
  ili9341_WriteData(0x0E);
  ili9341_WriteData(0x09);
  ili9341_WriteData(0x4E);
  ili9341_WriteData(0x78);
  ili9341_WriteData(0x3C);
  ili9341_WriteData(0x09);
  ili9341_WriteData(0x13);
  ili9341_WriteData(0x05);
  ili9341_WriteData(0x17);
  ili9341_WriteData(0x11);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_NGAMMA);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x16);
  ili9341_WriteData(0x1B);
  ili9341_WriteData(0x04);
  ili9341_WriteData(0x11);
  ili9341_WriteData(0x07);
  ili9341_WriteData(0x31);
  ili9341_WriteData(0x33);
  ili9341_WriteData(0x42);
  ili9341_WriteData(0x05);
  ili9341_WriteData(0x0C);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0x28);
  ili9341_WriteData(0x2F);
  ili9341_WriteData(0x0F);
  
  ili9341_WriteReg(LCD_SLEEP_OUT);
  LCD_Delay(200);
  ili9341_WriteReg(LCD_DISPLAY_ON);

  ili9341_WriteReg(LCD_GRAM);  */
	ILI9341_Enable();
	ILI9341_Reset();

	//SOFTWARE RESET
	ILI9341_WriteCommand(0x01);
	HAL_Delay(10);

	//POWER CONTROL A
	ILI9341_WriteCommand(0xCB);
	ILI9341_WriteData(0x39);
	ILI9341_WriteData(0x2C);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x34);
	ILI9341_WriteData(0x02);

	//POWER CONTROL B
	ILI9341_WriteCommand(0xCF);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0xC1);
	ILI9341_WriteData(0x30);

	//DRIVER TIMING CONTROL A
	ILI9341_WriteCommand(0xE8);
	ILI9341_WriteData(0x85);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x78);

	//DRIVER TIMING CONTROL B
	ILI9341_WriteCommand(0xEA);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x00);

	//POWER ON SEQUENCE CONTROL
	ILI9341_WriteCommand(0xED);
	ILI9341_WriteData(0x64);
	ILI9341_WriteData(0x03);
	ILI9341_WriteData(0x12);
	ILI9341_WriteData(0x81);

	//PUMP RATIO CONTROL
	ILI9341_WriteCommand(0xF7);
	ILI9341_WriteData(0x20);

	//POWER CONTROL,VRH[5:0]
	ILI9341_WriteCommand(0xC0);
	ILI9341_WriteData(0x23);

	//POWER CONTROL,SAP[2:0];BT[3:0]
	ILI9341_WriteCommand(0xC1);
	ILI9341_WriteData(0x10);

	//VCM CONTROL
	ILI9341_WriteCommand(0xC5);
	ILI9341_WriteData(0x3E);
	ILI9341_WriteData(0x28);

	//VCM CONTROL 2
	ILI9341_WriteCommand(0xC7);
	ILI9341_WriteData(0x86);

	//MEMORY ACCESS CONTROL
	ILI9341_WriteCommand(0x36);
	ILI9341_WriteData(0x48);

	//PIXEL FORMAT
	ILI9341_WriteCommand(0x3A);
	ILI9341_WriteData(0x55);

	//FRAME RATIO CONTROL, STANDARD RGB COLOR
	ILI9341_WriteCommand(0xB1);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x18);

	//DISPLAY FUNCTION CONTROL
	ILI9341_WriteCommand(0xB6);
	ILI9341_WriteData(0x08);
	ILI9341_WriteData(0x82);
	ILI9341_WriteData(0x27);

	//3GAMMA FUNCTION DISABLE
	ILI9341_WriteCommand(0xF2);
	ILI9341_WriteData(0x00);

	//GAMMA CURVE SELECTED
	ILI9341_WriteCommand(0x26);
	ILI9341_WriteData(0x01);

	//POSITIVE GAMMA CORRECTION
	ILI9341_WriteCommand(0xE0);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x31);
	ILI9341_WriteData(0x2B);
	ILI9341_WriteData(0x0C);
	ILI9341_WriteData(0x0E);
	ILI9341_WriteData(0x08);
	ILI9341_WriteData(0x4E);
	ILI9341_WriteData(0xF1);
	ILI9341_WriteData(0x37);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x10);
	ILI9341_WriteData(0x03);
	ILI9341_WriteData(0x0E);
	ILI9341_WriteData(0x09);
	ILI9341_WriteData(0x00);

	//NEGATIVE GAMMA CORRECTION
	ILI9341_WriteCommand(0xE1);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x0E);
	ILI9341_WriteData(0x14);
	ILI9341_WriteData(0x03);
	ILI9341_WriteData(0x11);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x31);
	ILI9341_WriteData(0xC1);
	ILI9341_WriteData(0x48);
	ILI9341_WriteData(0x08);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x0C);
	ILI9341_WriteData(0x31);
	ILI9341_WriteData(0x36);
	ILI9341_WriteData(0x0F);

	//EXIT SLEEP
	ILI9341_WriteCommand(0x11);
	HAL_Delay(100);

	//TURN ON DISPLAY
	ILI9341_WriteCommand(0x29);

	//STARTING ROTATION
	//ILI9341_SetRotation(SCREEN_VERTICAL_1);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval LCD Register Value.
  */
uint16_t ili9341_ReadID(void)
{
  LCD_IO_Init();
  return ((uint16_t)ili9341_ReadData(LCD_READ_ID4, LCD_READ_ID4_SIZE));
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void ili9341_DisplayOn(void)
{
  /* Display On */
  ili9341_WriteReg(LCD_DISPLAY_ON);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void ili9341_DisplayOff(void)
{
  /* Display Off */
  ili9341_WriteReg(LCD_DISPLAY_OFF);
}

/**
  * @brief  Writes  to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void ili9341_WriteReg(uint8_t LCD_Reg)
{
  LCD_IO_WriteReg(LCD_Reg);
}

/**
  * @brief  Writes data to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void ili9341_WriteData(uint16_t RegValue)
{
  LCD_IO_WriteData(RegValue);
}

void ILI9341_WriteBuffer(uint8_t *buffer, uint16_t len){

	  /* Set WRX to send data */
	  LCD_WRX_HIGH();

	  /* Reset LCD control line(/CS) and Send data */
	  LCD_CS_LOW();
	  for(int i = 0 ; i < len;i++)
		  SPIx_Write(buffer[i]);

	  /* Deselect: Chip Select high */
	  LCD_CS_HIGH();
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  RegValue: Address of the register to read
  * @param  ReadSize: Number of bytes to read
  * @retval LCD Register Value.
  */
uint32_t ili9341_ReadData(uint16_t RegValue, uint8_t ReadSize)
{
  /* Read a max of 4 bytes */
  return (LCD_IO_ReadData(RegValue, ReadSize));
}

/**
  * @brief  Get LCD PIXEL WIDTH.
  * @param  None
  * @retval LCD PIXEL WIDTH.
  */
uint16_t ili9341_GetLcdPixelWidth(void)
{
  /* Return LCD PIXEL WIDTH */
  return ILI9341_LCD_PIXEL_WIDTH;
}

/**
  * @brief  Get LCD PIXEL HEIGHT.
  * @param  None
  * @retval LCD PIXEL HEIGHT.
  */
uint16_t ili9341_GetLcdPixelHeight(void)
{
  /* Return LCD PIXEL HEIGHT */
  return ILI9341_LCD_PIXEL_HEIGHT;
}

void ILI9341_FillScreen(uint16_t color)
{
	ILI9341_SetAddress(0, 0, LCD_WIDTH, LCD_HEIGHT);
	ILI9341_DrawColorBurst(color, LCD_WIDTH*LCD_HEIGHT);
}

void ILI9341_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint8_t buffer[4];
	buffer[0] = x1 >> 8;
	buffer[1] = x1;
	buffer[2] = x2 >> 8;
	buffer[3] = x2;

	ILI9341_WriteCommand(0x2A);
	ILI9341_WriteBuffer(buffer, sizeof(buffer));

	buffer[0] = y1 >> 8;
	buffer[1] = y1;
	buffer[2] = y2 >> 8;
	buffer[3] = y2;

	ILI9341_WriteCommand(0x2B);
	ILI9341_WriteBuffer(buffer, sizeof(buffer));

	ILI9341_WriteCommand(0x2C);
}

void ILI9341_DrawColorBurst(uint16_t color, uint32_t size)
{
	uint32_t BufferSize = 0;

	if((size*2) < BURST_MAX_SIZE)
	{
		BufferSize = size;
	}
	else
	{
		BufferSize = BURST_MAX_SIZE;
	}

	LCD_WRX_HIGH();
	LCD_CS_LOW();

	uint8_t chifted = color>>8;
	uint8_t BurstBuffer[BufferSize];

	for(uint32_t j = 0; j < BufferSize; j+=2)
	{
		BurstBuffer[j] = chifted;
		BurstBuffer[j+1] = color;
	}

	uint32_t SendingSize = size * 2;
	uint32_t SendingInBlock = SendingSize / BufferSize;
	uint32_t RemainderFromBlock = SendingSize % BufferSize;

	if(SendingInBlock != 0)
	{
		for(uint32_t j = 0; j < (SendingInBlock); j++)
		{
			HAL_SPI_Transmit(HSPI_INSTANCE, BurstBuffer, BufferSize, 10);
		}
	}

	HAL_SPI_Transmit(HSPI_INSTANCE, BurstBuffer, RemainderFromBlock, 10);
	LCD_CS_HIGH();
}
void ILI9341_Reset(void)
{
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);	//Disable
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);		//Select
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);		//Enable
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET); 		//Deselect
}

void ILI9341_Enable(void)
{
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);		//Enable
}

void ILI9341_DrawPixel(uint16_t x,uint16_t y,uint16_t color){

	if((x >=LCD_WIDTH) || (y >=LCD_HEIGHT)) return;

	uint8_t bufferX[4] = {x>>8, x, (x+1)>>8, (x+1)};
	uint8_t bufferY[4] = {y>>8, y, (y+1)>>8, (y+1)};
	uint8_t bufferC[2] = {color>>8, color};

	ILI9341_WriteCommand(0x2A);						//ADDRESS
	ILI9341_WriteBuffer(bufferX, sizeof(bufferX));	//XDATA

	ILI9341_WriteCommand(0x2B);						//ADDRESS
	ILI9341_WriteBuffer(bufferY, sizeof(bufferY));	//YDATA

	ILI9341_WriteCommand(0x2C);						//ADDRESS
	ILI9341_WriteBuffer(bufferC, sizeof(bufferC));	//COLOR
}

void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor){
	if ((ch < 31) || (ch > 127)) return;

	uint8_t fOffset, fWidth, fHeight, fBPL;
	uint8_t *tempChar;

	fOffset = font[0];
	fWidth = font[1];
	fHeight = font[2];
	fBPL = font[3];

	tempChar = (uint8_t*)&font[((ch - 0x20) * fOffset) + 4]; /* Current Character = Meta + (Character Index * Offset) */

	/* Clear background first */
	ILI9341_DrawRectangle(X, Y, fWidth, fHeight, bgcolor);

	for (int j=0; j < fHeight; j++)
	{
		for (int i=0; i < fWidth; i++)
		{
			uint8_t z =  tempChar[fBPL * i + ((j & 0xF8) >> 3) + 1]; /* (j & 0xF8) >> 3, increase one by 8-bits */
			uint8_t b = 1 << (j & 0x07);
			if (( z & b ) != 0x00) // выводим только 1
			{
				ILI9341_DrawPixel(X+i, Y+j, color);
			}
		}
	}
}
void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor){

	uint8_t charWidth;			/* Width of character */
	uint8_t fOffset = font[0];	/* Offset of character */
	uint8_t fWidth = font[1];	/* Width of font */

	while (*str)
	{
		ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);

		/* Check character width and calculate proper position */
		uint8_t *tempChar = (uint8_t*)&font[((*str - 0x20) * fOffset) + 4];
		charWidth = tempChar[0];

		if(charWidth + 2 < fWidth)
		{
			/* If character width is smaller than font width */
			X += (charWidth + 2);
		}
		else
		{
			X += fWidth;
		}

		str++;
	}
}

void ILI9341_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color){

	if((x >=LCD_WIDTH) || (y >=LCD_HEIGHT)) return;

	if((x+width-1)>=LCD_WIDTH)
	{
		width=LCD_WIDTH-x;
	}

	if((y+height-1)>=LCD_HEIGHT)
	{
		height=LCD_HEIGHT-y;
	}

	ILI9341_SetAddress(x, y, x+width-1, y+height-1);
	ILI9341_DrawColorBurst(color, height*width);
}

void ILI9341_SetRotation(uint8_t rotation){
	ILI9341_WriteCommand(0x36);
		HAL_Delay(1);

		switch(rotation)
		{
		case SCREEN_VERTICAL_1:
			ILI9341_WriteData(0x40|0x08);
			LCD_WIDTH = 240;
			LCD_HEIGHT = 320;
			break;
		case SCREEN_HORIZONTAL_1:
			ILI9341_WriteData(0x20|0x08);
			LCD_WIDTH  = 320;
			LCD_HEIGHT = 240;
			break;
		case SCREEN_VERTICAL_2:
			ILI9341_WriteData(0x80|0x08);
			LCD_WIDTH  = 240;
			LCD_HEIGHT = 320;
			break;
		case SCREEN_HORIZONTAL_2:
			ILI9341_WriteData(0x40|0x80|0x20|0x08);
			LCD_WIDTH  = 320;
			LCD_HEIGHT = 240;
			break;
		default:
			break;
		}
}

void draw_custom(){
	short X = 10;
	short Y = 10;
	union MyUnion
		{
			char tmpout[96];
			long l[24];
			uint32_t lu[24];
		}s;
for(int i = 0 ; i < 25;i++){
	ILI9341_DrawRectangle(X, Y, 32, 24, WHITE);
	conv_lcd(s.tmpout, (unsigned char*)button_pic[i], 32, 24, 96);

		for (int y = 0; y < 24; y++) {
			for (int x = 0; x < 32; x++) {
				if (s.l[y] & (1 << x))
					ILI9341_DrawPixel(X + x, Y + y, BLACK);
				//else
					//ILI9341_DrawPixel(x, y, WHITE);
				}
			}
	HAL_Delay(4000);
};

}

void conv_lcd(char* out, unsigned char* in, short width_px, short height_px, short sz){
	unsigned char tmp = 0;
	int width_b = width_px/8;// байтов в строке
	int height_b = height_px / 8;
	for (int a = 0; a < height_b; a++) {
		for (int z = 0; z < 8; z++) {
			for (int y = 0; y < width_b; y++) {
				tmp = 0;
				for (int i = 0; i < 8; i++) {

					if (in[i + (y * 8) + (a * width_px)] & (1 << z))
						tmp |= (1 << i);

					//std::cout << std::hex << (int)in[i + (y * 8)] << "    " << (1 << y) << "      " << (int)tmp << '\r' << '\n';
				}
				out[y + (z * width_b) + (a * width_b * 8)] = tmp;
			}
		}
	}
}
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
