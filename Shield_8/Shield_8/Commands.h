/*
 * Commands.h
 *
 * Created: 28/01/2022 20:45:37
 *  Author: Katharina B�hm
 
 */ 


#ifndef COMMANDS_H_
#define COMMANDS_H_


// Commands sent by RS232

#define IRCMD_CLEARHITMATRIX	IRCMD_HEADER-1	
#define IRCMD_FIRE				IRCMD_HEADER	
#define IRCMD_SHIELD_ENERGY		IRCMD_HEADER+1	
#define IRCMD_STOG_INFO			IRCMD_HEADER+2	
#define IRCMD_SETDELAYS			IRCMD_HEADER+4	
#define IRCMD_REFILL_GUN_AMMO	IRCMD_HEADER+5	
#define IRCMD_REFILL_ENERGY		IRCMD_HEADER+6
#define IRCMD_SET_PLAYER		IRCMD_HEADER+7	
#define IRCMD_EEPROM_BURN		IRCMD_HEADER+8	
#define IRCMD_SET_ADMIN			IRCMD_HEADER+9	
#define IRCMD_RUN_CHECK			IRCMD_HEADER+10 
#define IRCMD_HITMEM			IRCMD_HEADER+11 
#define IRCMD_REFILL_BOTH_EN	IRCMD_HEADER+12 
#define IRCMD_REFILL_BOTH_GUN	IRCMD_HEADER+13 
#define IRCMD_REFILL_BOTH		IRCMD_HEADER+16	
#define IRCMD_INVULNERABLE		IRCMD_HEADER+14	
#define IRCMD_HIT_GUN			IRCMD_HEADER+3	



#define IRCMD_NOP				0xAA			
#define IRVAL_GP_OUTDOOR		0x01			
#define IRVAL_GP_INDOOR			0x02			
#define IRVAL_GP_GREEN			0x04			
#define IRVAL_GP_BLUE			0x08			
#define IRVAL_ADMIN				0x01			
#define IRVAL_MEM_READ			0x00			
#define IRVAL_MEM_CLEAR			0x01			
#define IRVAL_RELOAD_PERM		0x01			
#define IRVAL_RELOAD_SWITCH		0x02			

//-----------------------------------------------------------------------------------------------------
// IR Protokoll - IRCMD_FIRE = IRCMD_HEADER
//
// Bytefolge: || FF | FF | CMD | ^CMD | ID | ^ID | VAL | ^VAL ||
//			  || 00 | 01 | 02  | 03   | 04 | 05  | 06  | 07   ||
//-----------------------------------------------------------------------------------------------------
//||	|   CMD 				|| ID			|| VALUE		|| // Comment
//-----------------------------------------------------------------------------------------------------
//|| F0	| IRCMD_FIRE			||  Player_Nr	|| IRCMD_NOP	|| // neue Idee st�rke des Schusses als 2. Wert
//-----------------------------------------------------------------------------------------------------
//|| F1 | IRCMD_SHIELD_ENERGY	||  Player_Nr	|| Energy		|| //
//-----------------------------------------------------------------------------------------------------
//|| F2 | IRCMD_STOG_INFO		||  Player_Nr	|| 1 | time		|| // time out TO = 0x80 + time in 1/10s
//||	| IRCMD_STOG_INFO		||  Player_Nr	|| 0 | 0000000	|| // reactivate Gun  [IR_STOG_TIMEOUT=0x80 + time || IR_STOG_ACTIVATE=0 ]
//-----------------------------------------------------------------------------------------------------
//|| F3 | IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxx | GB IO || // change Gameplay 00 = no change
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxx | 10 xx || // change Gameplay green
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxx | 01 xx || // change Gameplay blue
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxx | 11 xx || // change Gameplay green/blue
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxx | xx 01 || // change Gameplay outdoor
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxx | xx 10 || // change Gameplay indoor
//|| F3 | IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxxxx S M	|| // Reload Gun startup mode S=switchable M=Mode
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxxxx x 0   || // Reload Gun: reload on Hit
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxxxx x 1   || // Reload Gun: permanent reload
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxxxx 0 x   || // Reload Gun: mode defined by Starup (M)
//||	| IRCMD_GAMEPLAY		||  IRCMD_NOP	|| xxxxxx 1 x   || // Reload Gun: mode changed by switch (laser pin)
//-----------------------------------------------------------------------------------------------------
//|| F4	| IRCMD_SETDELAYS		|| Delay_Hit	|| Delay_Relaod || // delay hit "1/10s"; delay reload in "s"
//-----------------------------------------------------------------------------------------------------
//|| F5	| IRCMD_REFILL_GUN_AMMO	|| 0 Ammo_H		|| Ammo_L		|| // refil Ammo
//||	| IRCMD_REFILL_GUN_AMMO	|| 1 xxxxxx		|| xxxxxx		|| // set infinite Ammo
//||	| IRCMD_REFILL_GUN_AMMO	|| 0 000000		|| 0000000		|| // relaod ammo from settings
//-----------------------------------------------------------------------------------------------------
//|| F6	| IRCMD_REFILL_ENERGY	|| 1 xxxxxx		|| xxxxxx		|| // set infinite nergy
//||	| IRCMD_REFILL_ENERGY	|| 0 xxxxxx		|| Energy		|| // set energy
//||	| IRCMD_REFILL_ENERGY	|| 0 000000		|| 0000000		|| // relaod ammo from settings
//-----------------------------------------------------------------------------------------------------
//|| F7	| IRCMD_SET_PLAYER		|| Player_Nr	|| Player_Nr	|| // set new player numebr (2x PlayerNr zu senden)
//||  	| IRCMD_SET_PLAYER		|| Player_Ammo	|| Player_Energ	|| // set new player numebr reload gun
//-----------------------------------------------------------------------------------------------------
//|| F8	| IRCMD_EEPROM_BURN		|| IRCMD_NOP	|| IRCMD_NOP	|| start EEPROM brun, leave dmin mode
//-----------------------------------------------------------------------------------------------------
//|| F9	| IRCMD_SET_ADMIN		|| IRCMD_NOP	|| 00000001		|| set to Admin mode
//||	| IRCMD_SET_ADMIN		|| IRCMD_NOP	|| 00000000		|| reset to Gameplay mode
//-----------------------------------------------------------------------------------------------------
//|| FA	| IRCMD_RUN_CHECK		|| IRCMD_NOP	|| IRCMD_NOP	|| run internal check sequence
//-----------------------------------------------------------------------------------------------------
//|| FB	| IRCMD_HITMEM			|| IRCMD_NOP	|| 00000000		|| read dump
//|| FB	| IRCMD_HITMEM			|| IRCMD_NOP	|| 00000001		|| clear
//-----------------------------------------------------------------------------------------------------


#define BAUD_H 0x03 
#define BAUD_L 0x41




struct S_SER_IN serial_in = {0,0,0};
	







#endif /* COMMANDS_H_ */