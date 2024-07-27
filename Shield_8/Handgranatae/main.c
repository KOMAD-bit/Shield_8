/*
 * Handgranatae.c
 *
 * Created: 04.01.2024 10:16:56
 * Author : Katharina B�hm-Klamt
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <math.h>

#include "RS232.h"
#include "twimaster.h"
#include "Commands.h"
#include "softwaretimerISRTIM2.h"
#include "LCD.h"


#define Pieps	4
#define Trigger	2
#define LED		7
#define Game	6
	


void shot(void);
void do1000ms(void);
void do5000ms(void);
void trigger(void);

void Init_Register(void)
{
	// Input/Output Ports initialization
cli();
	// PORT B 
	PORTB = 0x00; 
	DDRB  = 0xC;  

	// Port C initialization
	PORTC = 0x00;	//
	DDRC  = 0x70;	//     

	// Port D initialization
	PORTD = 0x00;
	DDRD  = 0xD2;  //       
	

	
	TCCR1B = (1<<WGM12)  |(1<<CS10); //
	TCCR1A |= (1<<COM1B0);
	OCR1A=230;			
	
	
	
	TCCR2 = 0x03;
	TCNT2 =0x00;
	TIMSK = (1<<TOIE2);
		UCSRA=0x00;
		UCSRB=0xD8; 
		UCSRC=0x86;
		
		UBRRH=BAUD_H;
		UBRRL=BAUD_L;
		
		

		SFIOR = (1<<PUD);	
		SPCR = 0x00;
		
		
		
		soft_s_zeiten[0].ms_max = 6; 
		soft_s_zeiten[0].fsync = trigger; 

		
		soft_s_zeiten[1].ms_max = 1024; 
		soft_s_zeiten[1].fasync = do1000ms; 
		
		soft_s_zeiten[2].ms_max = 99;
		soft_s_zeiten[2].fasync = shot;
		
		soft_s_zeiten[3].ms_max = 5120; 
		soft_s_zeiten[3].fasync = do5000ms;

		soft_enable_fsync	= 0b00000001;
		soft_enable_fasync	= 0b00001110;
		soft_enable			= 0b00001111;

	
	sei();
}


#define shd_pieps_off()		PORTD &=~(1<<Pieps)
#define shd_pieps_on()		PORTD |=(1<<Pieps)
#define shd_led_off()		PORTD &=~(1<<LED)
#define shd_led_on()		PORTD |=(1<<LED)
#define shd_game_yes()		PORTD |=(1<<Game)
#define shd_game_no()		PORTD &=~(1<<Game)


uint16_t triggerpressed = 0;
uint16_t	munition = 0;
uint8_t		d1=0;
uint8_t		d2=0;
uint8_t		d3=0;
struct S_SETTINGS
{
	
	uint16_t max_ammo;		
	uint8_t  Player_NR;		
	uint8_t	 Delay;
	} E_gamesettings EEMEM = { 0, 8, 0 };
	struct S_SETTINGS gamesettings;	
		// State definiition
uint8_t     current_state;		
#define     STATE_ADMIN	0x00	
#define		STATE_GAME	0x01	
#define		STATE_OUT	0x02	
#define TRIGGERPRESSED    (1<<PD2)


#define DevSSD1306	0x78;
void    setup_i2c()
{
	i2c_write(0x80);
	i2c_write(0xAE);                    

	i2c_write(0x00 | 0x0);            
	i2c_write(0x10 | 0x0);          
	i2c_write(0x40 | 0x0);           

	i2c_write(0x80);
	i2c_write(0x81);                   
	i2c_write(0xFF);					
	
	// flips display
	i2c_write(0xA1);                    
	
	i2c_write(0x80);
	i2c_write(0xC8);                    
	i2c_write(0xA6);                    
	
	//	i2c_write(0xA4);                
	i2c_write(0xA8);                    
	i2c_write(0x3F);                    

	i2c_write(0xD3);                    
	i2c_write(0x00);                     
	i2c_write(0xD5);                    
	i2c_write(0x80);                   


	i2c_write(0xD9);                 
	i2c_write(0x11);

	i2c_write(0xDA);                   
	
	i2c_write(0x12);

	i2c_write(0xDB);                
	i2c_write(0x40);

	
	
	i2c_write(0x20);                    
	i2c_write(0x02);                   

	//	i2c_write(0x40 | 0x0);              
	i2c_write(0x8D);                    
	i2c_write(0x14);

	//	i2c_write(0xA4);              
	i2c_write(0xAF);                
}

void Write_EEPROM()
{
	cli();
	eeprom_write_block(&gamesettings,&E_gamesettings,sizeof(gamesettings));
	sei();
}

//----------------------------------------------------------------------------------------------------
void Read_EEPROM()
{
	cli();
	eeprom_read_block(&gamesettings,&E_gamesettings,sizeof(gamesettings));
	sei();
	munition = gamesettings.max_ammo;
}


int main(void)
{
		Init_Register();
		i2c_init();
		my_delay(50);

	restart:
	Read_EEPROM();
	RX_ClearBuffer();
	i2c_init();
	my_delay(50);
	cleardisplay();
	digitdraw(1);
	my_delay(1000);
	current_state =STATE_GAME;
	d1=gamesettings.Delay*1000/3;
	d2=gamesettings.Delay*1000/6;
	d3=gamesettings.Delay*1000/12;
	shd_game_yes();
	my_delay(500);
	shd_game_no();
	digitdraw(2);
	/* Replace with your application code */
	while (1)
	{		soft_chk_do();

		TX_SendCommand(IRCMD_FIRE, 13, IRCMD_NOP);
		my_delay(1000);
		
		
		if (!RX_GetNextCommand(&serial_in))
		{
			
			switch (serial_in.cmd)
			{
				case IRCMD_REFILL_ENERGY:{
					munition = serial_in.id;
					munition = munition << 8;
					munition += serial_in.val;
					current_state = STATE_GAME;
					shd_game_yes();
					my_delay(500);
					shd_game_no();

				}
				
				
				
				break;
				
				case IRCMD_REFILL_BOTH_EN:{
					munition = serial_in.id;
					munition = munition << 8;
					munition += serial_in.val;
					current_state = STATE_GAME;
					shd_game_yes();
					my_delay(500);
					shd_game_no();
				}
				
				break;
				
				
				
				case IRCMD_SET_ADMIN:
				if (serial_in.id==IRCMD_NOP)
				{
					if ( (serial_in.val == IRVAL_ADMIN))
					{
						current_state =  STATE_ADMIN;
						cleardisplay();
						admin();
						my_delay(1000);
						
					}
					else
					if (!(serial_in.val & IRVAL_ADMIN) && (current_state==STATE_ADMIN))
					{
						
						current_state =(1<<STATE_GAME);
					}
				}
				break;
				
				
				
				#pragma region SIGNAL AMDIN
				
				case IRCMD_HIT_GUN:
				if(serial_in.id == serial_in.val)
				{
					gamesettings.Delay = serial_in.id;
					digitdraw(gamesettings.Delay);
					my_delay(500);
					
				}
				break;
				
				case IRCMD_SET_PLAYER:			
				if (serial_in.id == serial_in.val)
				{
					gamesettings.Player_NR = serial_in.id;
					digitdraw(gamesettings.Player_NR); my_delay(500);
					
				}
				break;
				
				case IRCMD_EEPROM_BURN:			
				if (serial_in.id == serial_in.val && serial_in.val == IRCMD_NOP)
				{
					Write_EEPROM();
					my_delay(500);
					cleardisplay();
					admin();
					my_delay(500);
					goto restart;
				}
				break;
				
				
				#pragma endregion SIGNAL ADMIN

			}
		}

	}
}

void shot(void){
	if(triggerpressed == TRIGGERPRESSED){
		if(current_state==STATE_GAME)		{
			
			for(int i=0; i<3; i++){
				shd_pieps_on();
				shd_led_on();
				my_delay(d1/2);
				shd_pieps_off();
				shd_led_off();
				my_delay(d1/2);
			}
			TX_SendCommand(IRCMD_FIRE, gamesettings.Player_NR, IRCMD_NOP);
			for(int i=0; i<6; i++){
				shd_pieps_on();
				shd_led_on();
				my_delay(d2/2);
				shd_pieps_off();
				shd_led_off();
				my_delay(d2/2);
			}
			TX_SendCommand(IRCMD_FIRE, gamesettings.Player_NR, IRCMD_NOP);
			for(int i=0; i<12; i++){
				shd_pieps_on();
				shd_led_on();
				my_delay(d3/2);
				shd_pieps_off();
				shd_led_off();
				my_delay(d3/2);
			}
			TX_SendCommand(IRCMD_FIRE, gamesettings.Player_NR, IRCMD_NOP);
			shd_pieps_on();
			shd_led_on();
			current_state=STATE_OUT;
			
		}
	}
}

void do1000ms(void){}

void do5000ms(void){}

void trigger(void)
{
	triggerpressed = (PIND & TRIGGERPRESSED);
}




