
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
volatile uint8_t Serial = 0;
volatile uint8_t comandoConsulta = 0;

volatile uint8_t estado1_M = 0;
volatile uint8_t comando = 0;
volatile uint8_t paso = 0;

float lux;
uint8_t porcentaje_luz = 0;

volatile int posicion_actual = 0;
volatile int posicion_objetivo = 0;
volatile uint8_t motor_activo = 0;

#define POS_ROJO   0
#define POS_VERDE  100
#define POS_NARANJA 200

char color = 0;

///////////////////////////////////////////////////////////



int main(void)
{
	
    
	DDRC |= (1<<LED)|(1<<LED_ROJO)|(1<<LED_VERDE)|(1<<LED_NARANJA);
	
	
	I2C_slave_init(SlaveAddres);
///////////////CONFIGURACION MOTOR STEPPER/////////////////////////////////
	// Salidas stepper
	DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB3);


	// ?? Timer0 CTC
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS02) | (1<<CS00); // prescaler 1024
	OCR0A = 30;  // frecuencia pasos (ajusta velocidad)
	TIMSK0 |= (1<<OCIE0A);
/////////////////////////////////////////



	sei();
    while (1) 
    {
		if(estado1_M == 1){
		PORTD &= ~((1<<LED_ROJO)|(1<<LED_VERDE)|(1<<LED_NARANJA));
		// Rojo
		if(color == 'R'){
			PORTD |= (1<<LED_VERDE);
			PORTD &= ~((1<<LED_ROJO)|(1<<LED_NARANJA));
			
		}else if(color == 'V'){ //Verde
			PORTD |= (1<<LED_VERDE);
			PORTD &= ~((1<<LED_ROJO)|(1<<LED_NARANJA));
		}
		if(lux < 75)   // oscuro
			PORTD |= (1<<LED);
		else
			PORTD &= ~(1<<LED);

		
		}
		_delay_ms(50);
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
			if(buffer == 1){
				estado1_M = 1;
			}
			else if(buffer == 0){
				estado1_M = 0;
			}else if(buffer == 'R'){
			color = buffer;
			posicion_objetivo = POS_ROJO;
			motor_activo = 1;
			}
			else if(buffer == 'V'){
			color = buffer;
			posicion_objetivo = POS_VERDE;
			motor_activo = 1;
			}
			else if(buffer == 'N'){
			color = buffer;
			posicion_objetivo = POS_NARANJA;
			motor_activo = 1;
			}else {
				lux = buffer;
			}
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0xA8:
		case 0xB8:
			TWDR = 0;
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


ISR(TIMER0_COMPA_vect)
{
	if(motor_activo)
	{
		if(posicion_actual < posicion_objetivo)
		{
			// girar adelante
			paso++;
			if(paso > 3) paso = 0;
			posicion_actual++;
		}
		else if(posicion_actual > posicion_objetivo)
		{
			// girar atrás
			if(paso == 0) paso = 3;
			else paso--;
			posicion_actual--;
		}
		else
		{
			motor_activo = 0; // llegó
		}

		// Secuencia bobinas
		switch(paso)
		{
			case 0:
			PORTB = (PORTB & 0xF0) | 0b00000001;
			break;
			case 1:
			PORTB = (PORTB & 0xF0) | 0b00000010;
			break;
			case 2:
			PORTB = (PORTB & 0xF0) | 0b00000100;
			break;
			case 3:
			PORTB = (PORTB & 0xF0) | 0b00001000;
			break;
		}
	}
}
