#include <reg51.h>

#define lcd_port P2

sbit lcd_rs = P3^0;
sbit lcd_rw = P3^1;
sbit lcd_en = P3^2;

sbit entry_sensor = P1^0;
sbit exit_sensor  = P1^1;
sbit led = P1^2; // --- NEW: LED connected to Port 1 pin 2 ---

unsigned int people_count = 0;

void ms_delay(unsigned int time) {
    unsigned int i, j;
    for(i=0; i<time; i++)
        for(j=0; j<125; j++);
}

void send_command(unsigned char command) {
    lcd_port = command;
    lcd_rs = 0;
    lcd_rw = 0;
    lcd_en = 1;
    ms_delay(1);
    lcd_en = 0;
    ms_delay(1);
}

void send_data(unsigned char disp_data) {
    lcd_port = disp_data;
    lcd_rs = 1;
    lcd_rw = 0;
    lcd_en = 1;
    ms_delay(1);
    lcd_en = 0;
    ms_delay(1);
}

void lcd_init() {
    ms_delay(20);
    send_command(0x38);
    send_command(0x0C);
    send_command(0x06);
    send_command(0x01);
    ms_delay(5);
}

void show_number(unsigned int number) {
    send_command(0x86);
    send_data((number/100) + '0');
    send_data(((number/10)%10) + '0');
    send_data((number%10) + '0');
}

// --- NEW: Function to control LED based on count ---
void update_led() {
    if(people_count > 0) {
        led = 1;  // Turn LED ON (assuming Active High)
    } else {
        led = 0;  // Turn LED OFF
    }
}

void main() {
    P1 = 0xFB; // Initialize P1.2 as output (0), others as input (1)
    led = 0;   // Start with LED OFF
    
    lcd_init();
    
    send_command(0x80);
    send_data('C'); send_data('o'); send_data('u');
    send_data('n'); send_data('t'); send_data(':');
    send_data(' ');
    
    show_number(people_count);

    while(1) {
        // --- ENTRY LOGIC ---
        if(entry_sensor == 0) {
            ms_delay(10);
            if(entry_sensor == 0) {
                people_count++;
                show_number(people_count);
                update_led(); // Check if LED should turn ON
                while(entry_sensor == 0);
                ms_delay(100);
            }
        }

        // --- EXIT LOGIC ---
        if(exit_sensor == 0) {
            ms_delay(10);
            if(exit_sensor == 0) {
                if(people_count > 0)
                    people_count--;
                
                show_number(people_count);
                update_led(); // Check if LED should turn OFF
                while(exit_sensor == 0);
                ms_delay(100);
            }
        }
    }
}