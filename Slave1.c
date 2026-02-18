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

#define SlaveAddres 0x30

uint8_t buffer = 0;
volatile uint8_t contador = 0;
volatile uint8_t LED = 0;

volatile uint8_t estado1 = 0;
volatile uint8_t estado1_M = 0;
volatile uint8_t comando = 0;


volatile uint16_t tiempo_inicio = 0;
volatile uint16_t tiempo_fin = 0;
volatile uint8_t medicion_lista = 0;
uint16_t distancia;



uint16_t HC_SR04_getDistance(void)
{
	uint16_t ticks = tiempo_fin - tiempo_inicio;

	// cada tick = 0.5 us
	uint32_t tiempo_us = ticks / 2;

	return tiempo_us / 58;
}


int main(void)
{

	setup();
	I2C_slave_init(SlaveAddres);
	HC_SR04_init();

	
	sei();
    while (1) 
    {
		if (estado1_M == 1){
			PORTC &= ~((1<<PORTC0) | (1<<PORTC2)); // motor OFF
			

		}else{
			
			PORTC |= (1<<PORTC0);   // motor ON
			PORTC &= ~(1<<PORTC2);
			
		}
		
		
		HC_SR04_trigger();

		if(medicion_lista)
		{
			distancia = HC_SR04_getDistance();
			medicion_lista = 0;

			if(distancia < 5){  // ?? distancia umbral en cm
				PORTB |= (1<<LED);
				OCR1A = 4000; // 90 grados
			} else if(distancia > 7){
				PORTB &= ~(1<<LED);
				OCR1A = 1800; // 0 grados	
			}
			
		}
	
		
		
    }



}


//////////////////////////////INTERRUPCIONES//////////////////////////////////


ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC;
	switch(estado){
		case 0x60:
		case 0x70:
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0x80:
		case 0x90:
			buffer = TWDR;
			comando = buffer;
			if (comando == 1){
				estado1_M = 0;
			}else if(comando == 0){
				estado1_M = 1;
			}
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0xA8:
		case 0xB8:
			if(comando == 'E'){
				TWDR = estado1;   // enviar estado del motor
			}
			else if(comando == 'D'){
				TWDR = distancia;  
			}
			else{
				TWDR = 0;
			}
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
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

ISR(PCINT1_vect)
{
	if(!(PINC & (1<<PINC1)))   // botón presionado
	{
		if (estado1 == 0){
			estado1 = 1;
		}
		else{
			estado1 = 0;

		}
	}
}
ISR(TIMER1_CAPT_vect)
{
	if (TCCR1B & (1<<ICES1))
	{
		// Flanco subida
		tiempo_inicio = ICR1;
		TCCR1B &= ~(1<<ICES1); // ahora bajar
	}
	else
	{
		// Flanco bajada
		tiempo_fin = ICR1;
		medicion_lista = 1;
		TCCR1B |= (1<<ICES1); // preparar siguiente medicion
	}
}

