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
	DDRC |= (1<<DDC4)|(1<<DDC5); // Pines I2C como salida
	
	TWAR = address << 1; //Se asgina la dirrecion que tendra
	//Se habilita la interfaz, ACK Automatico, se habilita la ISR
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
		
}
void initADC(void)
{
	cli();

	ADMUX = 0;
	ADMUX |= (1 << REFS0);                 // AVcc
	ADMUX |= (1 << MUX2) | (1 << MUX1);    // ADC6
	ADMUX |= (1<< ADLAR);

	ADCSRA = (1 << ADEN)  |                // ADC enable
	(1 << ADIE)  |                // Interrupt enable
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128

	sei();
}
void initCON(){
	ADCSRA |= (1 << ADSC);
}
void initLCD8bits(void)
{
	// Datos
	DDRD |= 0b11111100;               // PD2–PD7
	DDRB |= (1<<PORTB0)|(1<<PORTB1);        // D6–D7

	// Control
	DDRB |= (1<<PORTB2)|(1<<PORTB3);

	_delay_ms(20);

	LCD_CMD(0x38); // 8 bits, 2 líneas
	_delay_ms(10);
	LCD_CMD(0x0C); // Display ON
	_delay_ms(10);
	LCD_CMD(0x06); // Auto incremento
	_delay_ms(10);
	LCD_CMD(0x01); // Clear
	_delay_ms(10);
}
//Funcion para enviar datos
void LCD_CMD(uint8_t a)
{
	PORTB &= ~(1 << PORTB2); // RS = 0

	PORTB |= (1 << PORTB3);
	_delay_ms(4);
	PORTB &= ~(1 << PORTB3);
}

void LCD_CHAR(char a)
{
	PORTB |= (1 << PORTB2); // RS = 1
	PORTB |= (1 << PORTB3);
	_delay_ms(4);
	PORTB &= ~(1 << PORTB3);
}

//Funcion para enviar cadena
void LCD_STRING(char *a){
	int i;
	for(i=0; a[i] !='\0'; i++)
	LCD_CHAR(a[i]);
}
//Desplazamiento hacia la derecha
void LCD_SHIFT_R(void){
	LCD_CMD(0x01);
	LCD_CMD(0x0C);
}
void LCD_SHIFT_L(void){
	LCD_CMD(0x01);
	LCD_CMD(0x08);
}
//Cursor
void LCD_SET(uint8_t col, uint8_t fila)
{
	uint8_t addr;

	if (fila == 1)
	addr = 0x80 + col;
	else if (fila == 2)
	addr = 0xC0 + col;
	else
	return;

	LCD_CMD(addr);
}

void iniciar_USART(unsigned int ubrr) {
	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;

	UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0) ; // Habilitar transmisión

	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

void enviar(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;

}

void cadena(char *caracteres) {
	while (*caracteres != '\0'){
		enviar(*caracteres);
		caracteres++;
	}

}
