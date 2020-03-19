#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "spi.h"
#include "mfrc522.h"
#include <string.h>

uint8_t SelfTestBuffer[64];

char card1[11];
char card2[11];
char card3[11];
char empty[] = "0000000000";

int card1_empty = 0;
int card2_empty = 0;
int card3_empty = 0;

int scan = 0;
int delete = 0;
int add = 0;
int flush = 0;

void eeprom_read() {
	eeprom_read_block((void*)&card1, (const void*)0, 10);
	eeprom_read_block((void*)&card2, (const void*)10, 10);
	eeprom_read_block((void*)&card3, (const void*)20, 10);
} 

void eeprom_flush(int n) {
	if(n == 1 || n == 4) eeprom_write_block((void*)&empty, (void*)0, 10);
	if(n == 2 || n == 4) eeprom_write_block((void*)&empty, (void*)10, 10);
	if(n == 3 || n == 4) eeprom_write_block((void*)&empty, (void*)20, 10);
}

void write_card(int n){
	if(n == 1) eeprom_write_block((void*)&card1, (void*)0, 10);
	if(n == 2) eeprom_write_block((void*)&card2, (void*)10, 10);
	if(n == 3) eeprom_write_block((void*)&card3, (void*)20, 10);
}


int card_empty_check(int n) {
	if(n == 1) {
		if(!strcmp(card1,empty)) return 1;
	}
	if(n == 2) {
		if(!strcmp(card2,empty)) return 1;
	}
	if(n == 3) {
		if(!strcmp(card3,empty)) return 1;
	}
	
	return 0;
}

int full_card_empty_check() {
	for(int i = 1; i < 4; i++) {
		if(!card_empty_check(i)) return 0;
	}
	
	return 1;
}

void menu_reset() {
	lcd_clrscr();
	lcd_puts("1-Scan 2-Add");
	lcd_gotoxy(0,1);
	lcd_puts("3-Delete 4-Flush");
	
	scan = 0;
	add = 0;
	delete = 0;
	flush = 0;
}

void init() {
	DDRD |= _BV(4);
	
	DDRB = 0x00;
	PORTB = 0x0f;
	
	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 96;
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
}

int main() {
	uint8_t byte;
	uint8_t str[16];
	char rez[11];
	_delay_ms(50);
	char temp[11];
	int flag=0;

	init();
	eeprom_read(); 
	
	card1_empty = card_empty_check(1);
	card2_empty = card_empty_check(2);
	card3_empty = card_empty_check(3);
	
	lcd_puts("RFID Reader");
	
	spi_init();
	_delay_ms(1000);
	lcd_clrscr();
	mfrc522_init();
	

	byte = mfrc522_read(VersionReg);
	if(byte == 0x92)
	{
		lcd_puts("MIFARE RC522v2");
		lcd_puts("Detected");
	}else if(byte == 0x91 || byte==0x90)
	{
		lcd_puts("MIFARE RC522v1");
		lcd_puts("Detected");
	}else
	{
		lcd_puts("No reader found");
	}
	
	byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg,byte|0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg,byte|0x80);
	
	_delay_ms(1500);
	lcd_clrscr();
	
	menu_reset();
	
	while(1){		
		menu_reset();
		if(bit_is_clear(PINB, 0)) {
			scan = 1;
			add = 0;
			delete = 0;
			lcd_clrscr();
		}
		
		if(bit_is_clear(PINB, 1)) {
			scan = 0;
			add = 1;
			delete = 0;
			lcd_clrscr();
		}
		
		if(bit_is_clear(PINB, 2)) {
			scan = 0;
			add = 0;
			delete = 1;
			lcd_clrscr();
		}
		
		if(bit_is_clear(PINB, 3)) {
			scan = 0;
			add = 0;
			delete = 0;
			flush = 1;
			lcd_clrscr();
		}
		
		if(scan) {
			if(full_card_empty_check()) {
				lcd_puts("No stored cards.");
				_delay_ms(1000);
			} else {
				lcd_puts("Scan card");
				flag = 1;
				while(flag) {	
				byte = mfrc522_request(PICC_REQALL,str);
				if(byte == CARD_FOUND){
					byte = mfrc522_get_card_serial(str);
					if(byte == CARD_FOUND){
						temp[0] = '\0';
						for(byte=0;byte<5;byte++) {
							itoa(str[byte],rez,16);
							strcat(temp,rez);
						}
						flag = 0;							
						lcd_clrscr();
					}
					else{
						lcd_puts("Error");
					}
				}
			}	
				if(!strcmp(card1,temp)) lcd_puts("Card 1 OK.");
				else if(!strcmp(card2,temp)) lcd_puts("Card 2 OK.");
				else if(!strcmp(card3,temp)) lcd_puts("Card 3 OK.");
				else lcd_puts("Unknown card.");
			}		
		}
		
		if(delete) {
			if(full_card_empty_check()) {
				lcd_puts("No stored cards.");
				_delay_ms(1000);
			}else {
				lcd_puts("Delete card");
				
				flag = 1;
				while(flag) {
					byte = mfrc522_request(PICC_REQALL,str);
					if(byte == CARD_FOUND){
						byte = mfrc522_get_card_serial(str);
						if(byte == CARD_FOUND){
							temp[0] = '\0';
							for(byte=0;byte<5;byte++) {
								itoa(str[byte],rez,16);
								strcat(temp,rez);
							}
							flag = 0;
							lcd_clrscr();
						}
						else{
							lcd_puts("Error");
						}
					}
				}
				
				if(!strcmp(card1,temp)) {
					eeprom_flush(1);
					strcpy(card1,empty);
					lcd_puts("Card 1 deleted.");
				} else if(!strcmp(card2,temp)) {
					eeprom_flush(2);
					strcpy(card2,empty);
					lcd_puts("Card 2 deleted.");
				}else if(!strcmp(card3,temp)){
					eeprom_flush(3);
					strcpy(card3,empty);
					lcd_puts("Card 3 deleted.");
				} else lcd_puts("Unknown card.");
			}
		}
		
		if(add) {
			lcd_puts("Add card");
			flag = 1;
			while(flag) {	
				byte = mfrc522_request(PICC_REQALL,str);
				if(byte == CARD_FOUND){
					byte = mfrc522_get_card_serial(str);
					if(byte == CARD_FOUND){
						temp[0] = '\0';
						for(byte=0;byte<5;byte++) {
							itoa(str[byte],rez,16);
							strcat(temp,rez);
						}
						flag = 0;
						lcd_clrscr();
					}
					else{
						lcd_clrscr();
						lcd_puts("Error");
						_delay_ms(1000);
						lcd_clrscr();
					}
				}
			}
		
			
	
			if(!strcmp(card1,temp)) lcd_puts("Card 1 exists.");
			else if(!strcmp(card2,temp)) lcd_puts("Card 2 exists.");
			else if(!strcmp(card3,temp)) lcd_puts("Card 3 exists.");
			else if(card_empty_check(1)) {
				strcpy(card1,temp);
				write_card(1);
				lcd_puts("Card 1 saved.");
			}
			else if(card_empty_check(2)) {
				strcpy(card2,temp);
				write_card(2);
				lcd_puts("Card 2 saved.");
			}
			else if(card_empty_check(3)){
				strcpy(card3,temp);
				write_card(3);
				lcd_puts("Card 3 saved.");
			}else lcd_puts("No space.");
			
			_delay_ms(1000);
		}
		
		if(flush) {
			eeprom_flush(4);
			strcpy(card1,empty);
			strcpy(card2,empty);
			strcpy(card3,empty);
			lcd_puts("Flushed");
			flush = 0;
			_delay_ms(1000);
						
		}
		_delay_ms(1000);
	} 
}
