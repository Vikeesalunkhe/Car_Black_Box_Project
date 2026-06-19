/* 
 * File:   main.h
 * Author: 25014
 *
 * Created on 26 November, 2025, 5:53 PM
 */

#ifndef MAIN_H
#define	MAIN_H
#include "adc.h"
#include "digital_keypad.h"
#include "clcd.h"
#include "i2c.h"
#include "ds1307.h"
#include "external_eeprom.h"
#include "timers.h"
#include "uart.h"

void dashboard(unsigned char key);
void store_event();
void login_screen(unsigned char key);
void clear_clcd();
void display_menu(unsigned char key);

void view_log(unsigned char key);
void load_logs(unsigned char index);
void download_log(unsigned char key);
void clear_log();
void change_password(unsigned char key);
void set_time(unsigned char key);

#endif	/* MAIN_H */

