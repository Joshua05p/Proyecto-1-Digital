/*
 * Proyecto 1.c
 *
 * Created: 4/02/2026 17:41:54
 * Author : masco
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#define slave1 0x30
#define slave2 0x40

#define slave1W ((0x30 << 1) | 0)   // Write
#define slave1R ((0x30 << 1) | 1)   // Read

#define slave2W ((0x40 << 1) | 0)   // Write
#define slave2R ((0x40 << 1) | 1)   // Read

#include <avr/io.h>
#include "Libreria.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////
////////////////////PROTOTIPOS////////////////////////////////
uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C;
uint8_t bufferI2C2;
void refreshPort(uint8_t valor);
void setup();	
char texto1[4];
char texto2[5];
char buffer[8];
uint8_t posicion = 0;

uint16_t motor1 = 0;
uint16_t motor2 = 0;
uint16_t motor3 = 0;

uint16_t extraer_numero(const char *msg) {
	return atoi(&msg[3]);
}

int main(void)
{
	initLCD8bits();

	LCD_SET(0,1);
	LCD_STRING("S1");

	LCD_SET(6,1);
	LCD_STRING("S2");
	
	LCD_SET(12,1);
	LCD_STRING("S3");
	
	iniciar_USART(103);
	I2C_master_init(100000, 1); //Inicializar el master, Frecuencia 100kHz, Prescaler 1

    while (1) 
    {
		//Esclavo 1
		if(!I2C_master_start()) continue;
		
		if(!I2C_master_write(slave1W)){
			I2C_master_Stop();
			 continue;
		}
		
		I2C_master_write('R');
				
		if(!I2C_master_repeatedStar()){
			I2C_master_Stop();
			continue;
		}
		
		if(!I2C_master_write(slave1R)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_read(&bufferI2C, 0);
		I2C_master_Stop();
		
		_delay_ms(100);
		
		itoa(bufferI2C, texto1, 10);
		LCD_STRING(texto1);
		cadena("Valor slave 1: ");
		cadena(texto1);
		cadena("%");
		//Esclavo 2
		
		if(!I2C_master_start()) continue;
		
		if(!I2C_master_write(slave2W)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_write('D');
		
		if(!I2C_master_repeatedStar()){
			I2C_master_Stop();
			continue;
		}
		
		if(!I2C_master_write(slave2R)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_read(&bufferI2C2, 0);
		I2C_master_Stop();
		
		cadena("         ");
		_delay_ms(50);
		itoa(bufferI2C2, texto2, 10);
		LCD_STRING(texto2);
		cadena("Valor slave 2: ");
		cadena(texto2);
		cadena("\n");
		_delay_ms(500);
		
				
	}
}
///////////////////////////////////////////////////
//////////////////FUNCIONES///////////////////////

ISR(USART_RX_vect) {
		char c = UDR0;
		if (c == '#') {
			buffer[posicion] = '\0'; // Finaliza cadena
			posicion = 0;

			if (buffer[0] == 'M' && buffer[1] == '1') {
				motor1 = extraer_numero((const char *)buffer);
				
				
				} else if (buffer[0] == 'M' && buffer[1] == '2') {
				motor2 = extraer_numero((const char *)buffer);
				
				
				} else if (buffer[0] == 'M' && buffer[1] == '3') {
				motor3 = extraer_numero((const char *)buffer);
				
				
				} 
			} else {
			buffer[posicion++] = c;
		}
}
