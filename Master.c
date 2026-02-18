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
char buffer1[8];
char buffer2[8];
uint8_t posicion = 0;

uint16_t motor1 = 0;
uint16_t motor2 = 0;
uint16_t motor3 = 0;

uint8_t dato;

float lux;
uint8_t porcentaje_luz = 0;
int color = 0;

uint16_t extraer_numero(const char *msg) {
	return atoi(&msg[3]);
}

int main(void)
{
	initLCD8bits();

	LCD_SET(0,1);
	LCD_STRING("SD");

	LCD_SET(6,1);
	LCD_STRING("SL");
	
	LCD_SET(12,1);
	LCD_STRING("SC");
	
	iniciar_USART(103);
	I2C_master_init(100000, 1); //Inicializar el master, Frecuencia 100kHz, Prescaler 1
	DDRB |= (1<<DDB4);
	
	uint16_t r,g,b,c;
	TCS3472_init();

	BH1750_init();
	

    while (1) 
    {
				c = TCS3472_read16(CDATA);
				r = TCS3472_read16(RDATA);
				g = TCS3472_read16(GDATA);
				b = TCS3472_read16(BDATA);

				// Rojo
				if(r > g && r > b)
				{
					if(g > (r * 0.3)){

						color = 'N';
						}else{

						color = 'R';
					}
					
					}else if(g > r && g > b){ //Verde
					color = 'V';
				}
				lux = BH1750_readLux();
				porcentaje_luz = (lux * 99) / (LUX_MAX);
				if(lux < 20)   // oscuro
				PORTB |= (1<<LED);
				else
				PORTB &= ~(1<<LED);

				
		
		//////////////////////////////////////////////////////////////////////////
		if(!I2C_master_start()) continue;
		
		if(!I2C_master_write(slave1W)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_write('E');
		
		if(!I2C_master_repeatedStar()){
			I2C_master_Stop();
			continue;
		}
		
		if(!I2C_master_write(slave1R)){
			I2C_master_Stop();
			continue;
		}
		I2C_master_read(&dato, 0);
		I2C_master_Stop();
		_delay_ms(50);
		itoa(dato, buffer1, 10);

		if(dato == 1){
////////////////////////////////////Envia 1 a SLAVE 1///////////////////////////
			if(!I2C_master_start()) continue;

			if(!I2C_master_write(slave1W)){
				I2C_master_Stop();
				continue;
			}

			I2C_master_write(1);   // solo envías el dato

			I2C_master_Stop();       // termina la transmisión
////////////////////////////////////Envia 1 a SLAVE 2///////////////////////////
			if(!I2C_master_start()) continue;

			if(!I2C_master_write(slave2W)){
				I2C_master_Stop();
				continue;
			}

			I2C_master_write(1);   // solo envías el dato

			I2C_master_Stop();       // termina la transmisión
			PORTB |= (1<<PORTB4);
			
			
		}else{
////////////////////////////////////Envia 0 a SLAVE 1///////////////////////////
			if(!I2C_master_start()) continue;

			if(!I2C_master_write(slave1W)){
				I2C_master_Stop();
				continue;
			}

			I2C_master_write(0);   // solo envías el dato

			I2C_master_Stop();       // termina la transmisión
			
////////////////////////////////////Envia 0 a SLAVE 2///////////////////////////
			if(!I2C_master_start()) continue;

			if(!I2C_master_write(slave2W)){
				I2C_master_Stop();
				continue;
			}

			I2C_master_write(0);   // solo envías el dato

			I2C_master_Stop();       // termina la transmisión
			PORTB &= ~(1<<PORTB4);
		}
		
		//Esclavo 1 ///////////////////////////////////////////////////
		if(!I2C_master_start()) continue;
		
		if(!I2C_master_write(slave1W)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_write('D');
		
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
		
		
		itoa(bufferI2C, texto1, 10);
		LCD_SET(0,2);
		LCD_STRING(texto1);
		_delay_ms(10);
		//Esclavo 2 ////////////////////////////////////////////////////////
		
/////////////////////////////PREGUNTAR POR ILUMINACION////////////////////////
		if(!I2C_master_start()) continue;
		
		if(!I2C_master_write(slave2W)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_write('L');
		
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
		
		_delay_ms(50);
		itoa(porcentaje_luz, texto2, 10);
		LCD_SET(6,2);
		LCD_STRING("    ");
		LCD_SET(6,2);
		LCD_STRING(texto2);
		LCD_STRING("%");
		_delay_ms(50);
//////////////////////////////PREGUNTAR POR COLOR////////////////////
			if(!I2C_master_start()) continue;

			if(!I2C_master_write(slave2W)){
				I2C_master_Stop();
				continue;
			}

			I2C_master_write(color);   // solo envías el dato

			I2C_master_Stop();       // termina la transmisión
			PORTB &= ~(1<<PORTB4);
		
		
		if(!I2C_master_start()) continue;
		
		if(!I2C_master_write(slave2W)){
			I2C_master_Stop();
			continue;
		}
		
		I2C_master_write('C');
		
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
		
	
		
		if(color == 'R'){
			
			LCD_SET(12,2);
			LCD_STRING("Roj");
		}
		else if(color == 'V'){
			
			LCD_SET(12,2);
			LCD_STRING("Ver");
		}
		else if(color == 'N'){
			
			LCD_SET(12,2);
			LCD_STRING("Nar");
		}
		else{
			
			LCD_SET(12,2);
			LCD_STRING("N/A");
		}
		
		_delay_ms(50);
		////////////////////////////////////////////////////////
		cadena("D:");
		itoa(bufferI2C, buffer, 10);
		cadena(buffer);

		cadena(",L:");
		itoa(porcentaje_luz, buffer, 10);
		cadena(buffer);

		cadena(",C:");
		enviar(color);   // envía la letra R, V o N

		cadena("#\n");   // fin de mensaje
		}
	}
