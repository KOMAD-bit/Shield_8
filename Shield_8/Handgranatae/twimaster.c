/*
 * twimaster.c
 *
 * Created: 13/03/2022 22:26:21
 *  Author: Katharina B�hm-Klamt
 */ 

#include <inttypes.h>
#include <compat/twi.h>
#include "twimaster.h"

#ifndef F_CPU
#define F_CPU 8000000
#endif

/* I2C clock in Hz */
#define SCL_CLOCK  1000000



void i2c_init(void)
{
 
  
  TWSR = 0;                         /* no prescaler */

	TWBR =10;
}



signed char i2c_start(unsigned char address)
{
    uint8_t   twst;

	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	while(!(TWCR & (1<<TWINT)));

	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) return 2;

	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while(!(TWCR & (1<<TWINT)));

	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

	return 0;

}/* i2c_start */

void i2c_start_wait(unsigned char address)
{
    uint8_t   twst;


    while ( 1 )
    {
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    
    	while(!(TWCR & (1<<TWINT)));
    
    	twst = TW_STATUS & 0xF8;
    	if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
    
    	TWDR = address;
    	TWCR = (1<<TWINT) | (1<<TWEN);
    
    	while(!(TWCR & (1<<TWINT)));
    
    	twst = TW_STATUS & 0xF8;
    	if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) ) 
    	{    	    
	        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	        
	        while(TWCR & (1<<TWSTO));
	        
    	    continue;
    	}
    	break;
     }

}
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );

}/* i2c_rep_start */



void i2c_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	

}
unsigned char i2c_write( unsigned char data )
{	
    uint8_t   twst;
    
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while(!(TWCR & (1<<TWINT)));

	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return 1;
	return 0;

}/* i2c_write */



unsigned char i2c_readAck(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));    

    return TWDR;

}/* i2c_readAck */


unsigned char i2c_readNak(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
    return TWDR;

}
