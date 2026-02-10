/*
 * Libreria.h
 *
 * Created: 4/02/2026 17:42:33
 *  Author: masco
 */ 


#ifndef LIBRERIA_H_
#define LIBRERIA_H_

#define F_CPU 16000000

#include <avr/io.h>
#include <stdint.h>

//////////////////INICIALIZAR I2C////////////////////////////
void  I2C_master_init(unsigned long SCL_Clock, uint8_t Prescaler);
//////////////////INICIO DE LA COMUNICACION//////////////////
uint8_t I2C_master_start(void);
uint8_t I2C_master_repeatedStar(void);
///////////////DETENER COMUNICACION/////////////////////////
void I2C_master_Stop(void);

uint8_t I2C_master_write(uint8_t dato);
uint8_t I2C_master_read(uint8_t *buffer, uint8_t ack);

/////////////////INICIALIZAR ESCLAVO//////////////////////
void I2C_slave_init(uint8_t address);

/////////////////PANTALLA LCD////////////////////////////
void initLCD8bits(void);
void LCD_CMD(uint8_t a);
void LCD_CHAR(char a);
void LCD_STRING(char *a);
void LCD_SHIFT_R(void);
void LCD_SHIFT_L(void);
void LCD_SET(uint8_t c, uint8_t f);

/////////////////////COMUNICACION UART////////////////////
void enviar(char data);
void cadena();
void iniciar_USART();

/////////////////ADC///////////////////////////////////////
void initADC();
void initCON();


#endif /* LIBRERIA_H_ */
