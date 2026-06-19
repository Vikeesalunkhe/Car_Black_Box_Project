/*
 * File:   dashboard.c
 * Author: 
 *
 * Created on 31 December, 2025
 * 
 */
#include <xc.h>
#include "main.h"
#include <string.h>
unsigned char clock_reg[3], speed[3] = {0};
char  *event[ ] = { "ON",  "GN", "G1", "G2", "G3", "G4", "GR", "C "};
unsigned char pos = 0;
unsigned char adc_val;

extern unsigned char operation;



void dashboard(unsigned char key) {
        clcd_print(" TIME     EV  SP", LINE1(0));
        
        // display time
        get_time(clock_reg);
        display_time(clock_reg);
        
        // event display ( Gear pos and speed )
        if (key == SW1)
        {       
            if (pos == 7){
                pos = 1;
            }
            if (pos < 6){
                pos++;
            }
            store_event();
        }
        else if (key == SW2)
        {
            if (pos == 7){
                pos = 1;
            }
            if (pos > 1){
                pos--;
            }
            store_event();
        }
        else if (key == SW3)
        {
            pos = 7;
            store_event();
        }
        clcd_print(event[pos], LINE2(10));
        
        //Speed
        adc_val = read_adc(CHANNEL0) / 10.23;
        speed[0] = (adc_val / 10) + '0';
        speed[1] = (adc_val % 10) + '0';
        speed[2] = '\0';
        clcd_print(speed, LINE2(14));
}

//Store Event in External EEPROM
unsigned char event_count = 0;
unsigned char address = 0x00;
extern char time[];
void store_event()
{
    
    for (int i = 0; i < 8; i++){
    
        if (i == 2 || i == 5)
        {
            continue;
        }
        write_external_eeprom(address++, time[i]);
    }
    
    write_external_eeprom(address++, event[pos][0]);
    write_external_eeprom(address++, event[pos][1]);
    
    write_external_eeprom(address++, speed[0]);
    write_external_eeprom(address++, speed[1]);
    
    if (address > 99){
        address = 0x00;
    }

    if (event_count < 10)
        event_count++;
}

unsigned int sec;        //for timer sec (sec variable value updated in isr function)
unsigned int field_index = 0, TOTAl_ATTEMPTs = 3, attempt = 0;
unsigned long delay = 0;
char Pass[5] = "1100";  //Main Default Password
char userPass[5];
unsigned char rem_time_sec[3] = "00";
unsigned int block_start_sec = 0;
unsigned char blocked = 0;
void login_screen(unsigned char key)
{
    if (field_index < 4)
    {
        clcd_print("Enter Password  ", LINE1(0));
        clcd_print("                ", LINE2(field_index+1));

        if (sec % 2 == 0)
        {
            clcd_putch('_', LINE2(field_index)); 
        }
        else
        {
            clcd_putch(' ', LINE2(field_index));
        } 
        
        if (key == SW1){
            userPass[field_index] = '1';
            for (int j = 0; j<field_index+1; j++){
                clcd_putch('*', LINE2(j));
            }                
            field_index++;
        }           
        else if (key == SW2){
            userPass[field_index] = '0';
            for (int j = 0; j<field_index+1; j++){
                clcd_putch('*', LINE2(j));
            }
            field_index++;
        }
            
    }
    else
    {            
        attempt++;
            
        userPass[4] = '\0';
        if (!strcmp(Pass, userPass)){
            clcd_print("Correct Password", LINE1(0)); 
            clcd_print("                ", LINE2(0));
            operation = 2;  //Enter in Menu Page
            clear_clcd();
            
        }
        else 
        {                
            if (attempt < TOTAl_ATTEMPTs)
            {
                clcd_print("Wrong Password", LINE1(0)); 

                switch(attempt){
                    
                    case 1:
                        clcd_print("Attempts rem. 2 ", LINE2(0));
                        break;
                    case 2:
                        clcd_print("Attempts rem. 1 ", LINE2(0)); 
                        break;
                    case 3:
                        clcd_print("Attempts rem. 0 ", LINE2(0));
                        break;
                        
                }  
                for (delay = 0; delay < 200000; delay++);
                delay = 0;
                field_index = 0;
                clear_clcd();
                    
            }
            else if (attempt >= TOTAl_ATTEMPTs && blocked == 0)
            {
                blocked = 1;
                block_start_sec = sec;
                
            }
            
            if (blocked)
            {
                unsigned int elapsed = sec - block_start_sec;
                
                if (elapsed < 30){
                    
                    unsigned int rem_sec = 29 - elapsed;
                    rem_time_sec[0] = (rem_sec / 10) + '0';
                    rem_time_sec[1] = (rem_sec % 10) + '0';
                    rem_time_sec[2] = '\0';

                    clcd_print("USER BLOCKED!   ", LINE1(0)); 
                    clcd_print("Wait ", LINE2(0));
                    clcd_print(rem_time_sec, LINE2(5));
                    clcd_print(" sec          ", LINE2(7));
                }
                else {
                
                    blocked = 0;
                    attempt = 0;
                    field_index = 0;
                    operation = 0;
                    pos = 0;
                    clear_clcd();
                    
                    
                }
            }
                  
        }
    }
}

//clear CLCD function
void clear_clcd()
{
    clcd_print("                ", LINE1(0));
    clcd_print("                ", LINE2(0));
}




const char menu[5][17] = {"View Log        ", "Clear Log        ", "Download Log    ", "Set Time        ", "Change Password " };
unsigned char menu_index = 0;
unsigned char cursor = 0;
unsigned char MENU_COUNT = 5;
//display menu ti scroll up and down
void display_menu(unsigned char key)
{
 
    if(key == SW2)
    {
        if(cursor == 0)
        {
            cursor = 1;
        }
        else
        {
            if(menu_index < MENU_COUNT - 2)
            {
                menu_index++;
            }
        }
    }
    
    if(key == SW1)
    {
        if(cursor == 1)
        {
            cursor = 0;
        }
        else
        {
            if(menu_index > 0)
            {
                menu_index--;
            }
        }
    }
    
    
    clcd_print(menu[menu_index], LINE1(2));

    if(menu_index + 1 < MENU_COUNT)
        clcd_print(menu[menu_index + 1], LINE2(2));

    if(cursor == 0)
    {
        clcd_putch('*', LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    else
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch('*', LINE2(0));
    }
    
    
    
    if(key == SW4)
    {

        switch(menu_index + cursor)
        {
            case 0:     // View Log
                operation = 3;
                break;

            case 1:     // Clear Log
                operation = 4;
                break;

            case 2:     // Download Log
                operation = 5;
                break;

            case 3:     // Set Time
                operation = 6;
                break;

            case 4:     // Change Password
                operation = 7;
                break;
        }
    }
    
    if (key == SW5){
        operation = 0;
        menu_index = 0;
        cursor = 0;
        clear_clcd();
        
    }
}



//view log 17-byte single-entry buffer; reads one event at a time from EEPROM
char EEPROM_data[17];
unsigned int log_index = 0;
unsigned char log_loaded = 0;

void load_logs(unsigned char index)
{
    unsigned char load_address = index * 10;

    EEPROM_data[0]  = '0' + index;
    EEPROM_data[1]  = ' ';
    EEPROM_data[2]  = read_external_eeprom(load_address++);
    EEPROM_data[3]  = read_external_eeprom(load_address++);
    EEPROM_data[4]  = ':';
    EEPROM_data[5]  = read_external_eeprom(load_address++);
    EEPROM_data[6]  = read_external_eeprom(load_address++);
    EEPROM_data[7]  = ':';
    EEPROM_data[8]  = read_external_eeprom(load_address++);
    EEPROM_data[9]  = read_external_eeprom(load_address++);
    EEPROM_data[10] = ' ';
    EEPROM_data[11] = read_external_eeprom(load_address++);
    EEPROM_data[12] = read_external_eeprom(load_address++);
    EEPROM_data[13] = ' ';
    EEPROM_data[14] = read_external_eeprom(load_address++);
    EEPROM_data[15] = read_external_eeprom(load_address++);
    EEPROM_data[16] = '\0';
}

void view_log(unsigned char key)
{
    if (!log_loaded)
    {
        if (event_count > 0)
            load_logs(log_index);
        log_loaded = 1;
    }

    if(key == SW2)
    {
        if(event_count > 0 && log_index < (unsigned int)(event_count - 1))
        {
            log_index++;
            load_logs(log_index);
        }
    }
    else if(key == SW1)
    {
        if(log_index > 0)
        {
            log_index--;
            load_logs(log_index);
        }
    }
    else if(key == SW5)
    {
        log_index = 0;
        log_loaded = 0;
        operation = 2;
        return;
    }

    clcd_print("# TIME     EV SP", LINE1(0));
    if (event_count == 0)
    {
        clcd_print("  No Events     ", LINE2(0));
    }
    else
    {
        clcd_print(EEPROM_data, LINE2(0));
    }
}


// Clear log
void clear_log(){
    event_count = 0;
    address = 0x00;
    log_index = 0;
    log_loaded = 0;
    EEPROM_data[0] = '\0';
    clcd_print("Logs are        ", LINE1(0));
    clcd_print("Clearing........", LINE2(0));
    for (delay = 0; delay < 200000; delay++);
    delay = 0;
    operation = 2;
    clear_clcd();
}


// Download logs
void download_log(unsigned char key)
{
    clcd_print("Logs are        ", LINE1(0));
    clcd_print("Downloading.....", LINE2(0));
    puts("# TIME    EV SP\n\r");
    for (unsigned char i = 0; i < event_count; i++)
    {
        load_logs(i);
        puts(EEPROM_data);
        puts("\n\r");
    }

    for (delay = 0; delay < 200000; delay++);
    delay = 0;
    operation = 2;
    clear_clcd();
}


//change_password (Set New Password)
char temp_pass[5];
char temp1_pass[5];
unsigned char new_pass_index = 0, new_pass_index1 = 0;

void change_password(unsigned char key)
{
    if (new_pass_index < 4)
    {
        clcd_print("Enter New Password  ", LINE1(0));
        clcd_print("                ", LINE2(new_pass_index+1));

        if (sec % 2 == 0)
        {
            clcd_putch('_', LINE2(new_pass_index)); 
        }
        else
        {
            clcd_putch(' ', LINE2(new_pass_index));
        } 
        
        if (key == SW1){
            temp_pass[new_pass_index] = '1';
            for (int j = 0; j<new_pass_index+1; j++){
                clcd_putch('*', LINE2(j));
            }                
            new_pass_index++;
        }           
        else if (key == SW2){
            temp_pass[new_pass_index] = '0';
            for (int j = 0; j<new_pass_index+1; j++){
                clcd_putch('*', LINE2(j));
            }
            new_pass_index++;
        }
            
    }
    else if (new_pass_index1 < 4)
    {
        clcd_print("Re Enter New Pass   ", LINE1(0));
        clcd_print("                ", LINE2(new_pass_index1+1));

        if (sec % 2 == 0)
        {
            clcd_putch('_', LINE2(new_pass_index1)); 
        }
        else
        {
            clcd_putch(' ', LINE2(new_pass_index1));
        } 
        
        if (key == SW1){
            temp1_pass[new_pass_index1] = '1';
            for (int j = 0; j<new_pass_index1+1; j++){
                clcd_putch('*', LINE2(j));
            }                
            new_pass_index1++;
        }           
        else if (key == SW2){
            temp1_pass[new_pass_index1] = '0';
            for (int j = 0; j<new_pass_index1+1; j++){
                clcd_putch('*', LINE2(j));
            }
            new_pass_index1++;
        }           
    }
    else
    {            
            
        temp_pass[4] = '\0';
        temp1_pass[4] = '\0';
        //Compare Re-Password is same
        if (!strcmp(temp_pass, temp1_pass)){
            clcd_print("Password is       ", LINE1(0)); 
            clcd_print("Changed           ", LINE2(0));
            
            //Move(copy) Re-entered Pass in Main Pass
            strcpy(Pass, temp_pass);
            
            for (delay = 0; delay < 200000; delay++);
            delay = 0;
            operation = 2;  //return to menu selection
            clear_clcd();
            new_pass_index = 0;
            new_pass_index1 = 0;            
        }
        else 
        {                
            clcd_print("Re Entered Pass       ", LINE1(0)); 
            clcd_print("is Wrong              ", LINE2(0));
            for (delay = 0; delay < 200000; delay++);
            delay = 0;
            
            operation = 2;  //return to menu selection
            clear_clcd();   //clear CLCD
            new_pass_index = 0;    //reset index values
            new_pass_index1 = 0;
                             
        }
    }
}




//set time 
char time[9] = {"12:04:22"};
unsigned int set_time_filed = 0;
void set_time(unsigned char key){
    
    clcd_print("HH:MM:SS        ", LINE1(0)); 
    
    
    if (sec % 2 == 0){
        clcd_putch('_', LINE2(set_time_filed));
        clcd_putch('_', LINE2(set_time_filed + 1));
    }
    else {
        clcd_print(time, LINE2(0));
    }
    
    
    if (key == SW1)
    {
        unsigned char value;

        if(set_time_filed == 0)          // HH
        {
            value = (time[0] - '0') * 10 + (time[1] - '0');

            value++;

            if(value > 23)
                value = 0;

            time[0] = (value / 10) + '0';
            time[1] = (value % 10) + '0';
        }
        else if(set_time_filed == 3)     // MM
        {
            value = (time[3] - '0') * 10 + (time[4] - '0');

            value++;

            if(value > 59)
                value = 0;

            time[3] = (value / 10) + '0';
            time[4] = (value % 10) + '0';
        }
        else if(set_time_filed == 6)     // SS
        {
            value = (time[6] - '0') * 10 + (time[7] - '0');

            value++;

            if(value > 59)
                value = 0;

            time[6] = (value / 10) + '0';
            time[7] = (value % 10) + '0';
        }
    }
    else if (key == SW2)    {
        if(set_time_filed == 0)
            set_time_filed = 3;
        else if(set_time_filed == 3)
            set_time_filed = 6;
        else
            set_time_filed = 0;
    }
    else if(key ==  SW4){
        int time_value = 0;
        
        time_value = (time[0] - '0') * 10 + (time[1] - '0'); //HH
        write_ds1307(HOUR_ADDR, ((time_value/10)<<4) | (time_value % 10));
        
        time_value = (time[3] - '0') * 10 + (time[4] - '0'); //MM
        write_ds1307(MIN_ADDR, ((time_value/10)<<4) | (time_value % 10));
        
        time_value = (time[6] - '0') * 10 + (time[7] - '0'); //SS
        write_ds1307(SEC_ADDR, ((time_value/10)<<4) | (time_value % 10));
               
    }
    else if (key == SW5){
        operation = 2;  //return to menu selection
        clear_clcd();   //clear CLCD      
    }
      
}
