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

void enviar(char data);
void cadena();
void iniciar_USART();
void initADC();
void initCON();



#endif /* LIBRERIA_H_ */
