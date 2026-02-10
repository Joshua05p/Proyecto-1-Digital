/*
 * Proyecto 1.c
 *
 * Created: 4/02/2026 17:41:54
 * Author : masco
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#define slave1 0x30
#define slave2 0x40

#define slave1W ((0x30 << 1) | 0)   // Write
#define slave1R ((0x30 << 1) | 1)   // Read

#define slave2W ((0x40 << 1) | 0)   // Write
#define slave2R ((0x40 << 1) | 1)   // Read

#include <avr/io.h>
#include "Libreria.h"
#include <util/delay.h>

//////////////////////////////////////////////////////////////
////////////////////PROTOTIPOS////////////////////////////////
uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C;
uint8_t bufferI2C2;
void refreshPort(uint8_t valor);
void setup();	



int main(void)
{
    setup();
	I2C_master_init(100000, 1); //Inicializar el master, Frecuencia 100kHz, Prescaler 1

    while (1) 
    {
		//Esclavo 1
		PORTB |= (1<<PORTB5);
		
		if(!I2C_master_start()) return;
		
		if(!I2C_master_write(slave1W)){
			I2C_master_Stop();
			 return;
		}
		
		I2C_master_write('R');
				
		if(!I2C_master_repeatedStar()){
			I2C_master_Stop();
			return;
		}
		
		if(!I2C_master_write(slave1R)){
			I2C_master_Stop();
			return;
		}
		
		I2C_master_read(&bufferI2C, 0);
		I2C_master_Stop();
		
		PORTB &= ~(1<<PORTB5);
		
		refreshPort(bufferI2C);
		
		//Esclavo 2

		PORTB |= (1<<PORTB5);
		
		if(!I2C_master_start()) return;
		
		if(!I2C_master_write(slave2W)){
			I2C_master_Stop();
			return;
		}
		
		I2C_master_write('D');
		
		if(!I2C_master_repeatedStar()){
			I2C_master_Stop();
			return;
		}
		
		if(!I2C_master_write(slave2R)){
			I2C_master_Stop();
			return;
		}
		
		I2C_master_read(&bufferI2C2, 0);
		I2C_master_Stop();
		
		PORTB &= ~(1<<PORTB5);
		refreshPort2(bufferI2C2);
		
		_delay_ms(500);

		LCD_SET(0,2);
		LCD_STRING(bufferI2C);
		_delay_ms(50);
		cadena("Valor slave 1:");
		cadena(bufferI2C); 
		
		cadena("         ");
		
		LCD_SET(0,6);
		LCD_STRING(bufferI2C2);
		_delay_ms(50);
		cadena("Valor slave 2:");
		cadena(bufferI2C2);
		cadena("\n");
		
}
}
///////////////////////////////////////////////////
//////////////////FUNCIONES///////////////////////

void setup(){
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
	DDRB |= (1<<DDB0)|(1<<DDB1);
	
	DDRB |= (1<<DDB5);
	
	PORTB &= ~((1<<PORTB0)|(1<<PORTB1));
	PORTD &= ~((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7));
	
}

void refreshPort(uint8_t valor){
	if(valor & 0b10000000){
		PORTB |= (1<<PORTB1);
		}else{
		PORTB &= ~(1<<PORTB1);
	}
	if(valor & 0b01000000){
		PORTB |= (1<<PORTB0);
		}else{
		PORTB &= ~(1<<PORTB0);
	}
	if(valor & 0b00100000){
		PORTD |= (1<<PORTD7);
		}else{
		PORTD &= ~(1<<PORTD7);
	}
	if(valor & 0b00010000){
		PORTD |= (1<<PORTD6);
		}else{
		PORTD &= ~(1<<PORTD6);
	}

}
void refreshPort2(uint8_t valor){
	if(valor & 0b00001000){
		PORTD |= (1<<PORTD5);
		}else{
		PORTD &= ~(1<<PORTD5);
	}
	if(valor & 0b00000100){
		PORTD |= (1<<PORTD4);
		}else{
		PORTD &= ~(1<<PORTD4);
	}
	if(valor & 0b00000010){
		PORTD |= (1<<PORTD3);
		}else{
		PORTD &= ~(1<<PORTD3);
	}
	if(valor & 0b00000001){
		PORTD |= (1<<PORTD2);
		}else{
		PORTD &= ~(1<<PORTD2);
	}

}
	

