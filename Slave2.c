/*
 * Proyecto1_slave.c
 *
 * Created: 5/02/2026 19:41:40
 * Author : masco
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Libreria.h"

#define SlaveAddres 0x40

uint8_t buffer = 0;
volatile uint8_t valorADC = 0;



int main(void)
{
	
    DDRB |= (1<<DDB5);
	PORTB &= ~(1<<PORTB5);
	
	I2C_slave_init(SlaveAddres);
	initADC();
	initCON();
	
	sei();
    while (1) 
    {
		if(buffer == 'D'){
			PINB |= (1<<PINB5);
			buffer = 0;
		}

		_delay_ms(500);
    }
}





//////////////////////////////INTERRUPCIONES//////////////////////////////////
ISR(ADC_vect){
		initCON();
		valorADC = ADCH;
}
ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC;
	switch(estado){
		case 0x60:
		case 0x70:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(TWEA);
			break;
			
		case 0x80:
		case 0x90:
			buffer = TWDR;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(TWEA);
			break;
			
		case 0xA8:
		case 0xB8:
			TWDR = valorADC;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(TWEA);
			break;
		
		case 0xC0:
		case 0xC8:
			TWDR = 0;	
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0xA0:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
		
		default:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
		
	}
}

