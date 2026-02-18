/*
 * Libreria.c
 *
 * Created: 4/02/2026 17:42:21
 *  Author: masco
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Libreria.h"

///////////////INICIALIZAR MAESTRO//////////////////////
void I2C_master_init(unsigned long SCL_Clock, uint8_t Prescaler){
	DDRC &= ~((1<<DDC4)|(1<<DDC5)); ///Pines de entrada SDA y salida SCL
	///Seleccionar	el valor del prescaler
	switch(Prescaler){
		case 1:
			TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
			break;
		case 4:
			TWSR &= ~(1<<TWPS1);
			TWSR |= (1<<TWPS0);
			break;
		case 16:
			TWSR &= ~(1<<TWPS0);
			TWSR |= (1<<TWPS1);
			break;
		case 64:
			TWSR |= ((1<<TWPS1)|(1<<TWPS0));
			break;
		default:
			TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
			Prescaler = 1;
			break;
		
	}  
	TWBR = ((F_CPU/SCL_Clock)-16)/(2*Prescaler); ///Calcular la velocidad
	TWCR |= (1<<TWEN); // Activar interfase (TWI) I2C
		
}
////////////////////////FUNCION INICIO DE COMUNICACION I2C/////////////////
uint8_t I2C_master_start(void){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); //MASTER, REINICIAR BANDERA INT, CONDICION START
	while (!(TWCR & (1<<TWINT))); //ESPERAR A LA BANDERA
	
	return ((TWSR & 0xF8)== 0x08);
	
}

//////////////////////FUNCION DE REINICIO DE COMUNICACION I2C//////////////////
uint8_t I2C_master_repeatedStar(){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	return ((TWSR & 0xF8) == 0x10);
}


void I2C_master_Stop(void){
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO); //ENVIA EL INICIO DE SECUENCIA DE PARADA
	while (TWCR &(1<<TWSTO)); //ESPERAMOS A QUE EL BIT SE LIMPIE
	
}

//////////////////////////TRANSMITIR DE MAESTRO A ESCLAVO//////////////////////
uint8_t I2C_master_write(uint8_t dato){
	uint8_t estado;
	TWDR = dato;   ////CARGAR EL DATO
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT))); //ESPERA AL FLAG TWINT
	
	estado = TWSR & 0xF8; //////Nos quedamos unicamente con los bits de estado TWINT
	//VERIFICAR SI SE TRANSMITIO UNA SLA+W CON ACK, O UN DATO CON ACK
	if (estado == 0x18|| estado == 0x28){
		return 1;
	}else{
		return estado;
	}
}

uint8_t I2C_master_read(uint8_t *buffer, uint8_t ack){
	uint8_t estado;
	if(ack){
		//ACK: quiero mas datos
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); //Habilitamos interfase I2C CON envio de ack
	}else{
		//NACK: ultimo byte
		TWCR =  (1<<TWINT) | (1<<TWEN); //Habilitamos interfase I2C SIN envio de ack
	}
	while (!(TWCR & (1<<TWINT))); // ESPERAR EL FLAG INT
	
	estado = TWSR & 0xF8;
	//Verificar si se recibio ACK
	if (ack && estado != 0x50) return 0;
	if (!ack && estado != 0x58) return 0;
	*buffer = TWDR; // OBTENEMOS EL RESULTADO DEL REGISTRO DE DATOS
	return 1;
	
}

//////////////////////////FUNCION PARA INICIALIZAR ESCLAVO//////////////////////////
void I2C_slave_init(uint8_t address){
	DDRC &= ~((1<<DDC4)|(1<<DDC5)); // Pines I2C como salida
	
	TWAR = address << 1; //Se asgina la dirrecion que tendra
	//Se habilita la interfaz, ACK Automatico, se habilita la ISR
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
		
}



