
#pragma config WDTE = OFF

#include <xc.h>

#include "main.h"
unsigned char operation = 0;

unsigned char menu_flag = 0;

extern unsigned int field_index;
extern char userPass[5];
extern unsigned int attempt;
extern unsigned char blocked;


void init_config()
{
    init_adc();
    init_clcd();
    init_digital_keypad();
    init_i2c(100000); 
    init_ds1307();
    init_timer0();  //initialize timer0
    init_uart(9600);
    
    puts("UART Test Code\n\r");
    
    GIE = 1;
}

void main(void) {
    init_config();
    
    unsigned char key; //operation = 0;
    
    
    while(1)
    {
        key = read_digital_keypad(STATE);        
        if(key == SW4 && menu_flag == 0)
        {
            operation = 1;           
            //reset all Userpass, attemps, and index values
            field_index = 0;
            attempt = 0;
            blocked = 0;
            userPass[0] = '\0';
        }
       
        if(operation == 0)
        {
            menu_flag = 0;           
            dashboard(key);          
        }
        else if(operation == 1)           
        {
            // login screen
            login_screen(key);
        }
        else if(operation == 2)
        {
            // menu part
            display_menu(key);
            menu_flag = 1;
        }
        else if(operation == 3)
        {
            // view log
            view_log(key);  
        } 
        else if(operation == 4)
        {
            // menu part
            clear_log();  
        } 
        else if(operation == 5)
        {
            // menu part
            download_log(key);  
        } 
        else if(operation == 6)
        {
            // set time
            set_time(key);  
        }
        else if(operation == 7)
        {
            // change password 
            change_password(key);  
        }
    }
}
