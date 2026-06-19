# Car_Black_Box_Project

 A car black box (event data recorder) built on a PIC16F877A microcontroller. 
It continuously monitors vehicle gear position and speed, logs timestamped 
events to an external I2C EEPROM, and provides a password-protected menu 
to view, download, or clear the log.

---

## Features

- **Real-time dashboard** — displays current time (HH:MM:SS), gear position, and speed
- **Event logging** — stores up to 10 events in external EEPROM with timestamp, gear, and speed
- **View log** — scroll through stored events on a 16×2 LCD
- **Clear log** — wipe all stored events
- **Download log** — send all events over UART to a terminal
- **Password protection** — 4-digit binary password to access the menu
- **Login lockout** — blocks user for 30 seconds after 3 failed attempts
- **Change password** — update the access password from the menu
- **Set time** — configure the RTC time (HH:MM:SS)

---

## Hardware

| Component | Details |
|---|---|
| Microcontroller | PIC16F877A (XC8 compiler) |
| Display | 16×2 Character LCD (4-bit mode) |
| RTC | DS1307 (I2C) |
| External EEPROM | I2C EEPROM (stores 10 events) |
| Keypad | 5-button digital keypad (SW1–SW5) |
| Speed sensor | Potentiometer via ADC (Channel 0) |
| Communication | UART @ 9600 baud |

---

## Pin Configuration

| Peripheral | Port |
|---|---|
| LCD Data | PORTD |
| LCD RS / EN | RE2 / RE1 |
| Keypad | PORTB (bits 0–5) |
| I2C (RTC + EEPROM) | RC3 (SCL) / RC4 (SDA) |
| UART | RC6 (TX) / RC7 (RX) |
| ADC Speed Input | AN0 |

---

## Menu Structure

SW4 (from dashboard) → Login Screen
↓
Main Menu
├── View Log       (scroll with SW1/SW2, back SW5)
├── Clear Log
├── Download Log   (UART output)
├── Set Time
└── Change Password



## Button Map
-----------------------------------------------------
| Button |  Dashboard  |    Menu     |    View Log   |
|--------|-------------|------------ |---------------|
| SW1    | Gear Up     | Scroll Up   | Previous Event|
| SW2    | Gear Down   | Scroll Down | Next Event    |
| SW3    | Clutch (GR) |     —       |       —       |
| SW4    | Enter Menu  | Select      |     —         |
| SW5    |      —      | Back to Dash| Back to Menu  |
-----------------------------------------------------

## Event Format

Each event stored in EEPROM is 10 bytes:

HH MM SS GG SS
│  │  │  │  └─ Speed (2 digits)
│  │  │  └──── Gear / Event code (2 chars: G1, G2, GN, GR, ON, C )
│  │  └─────── Seconds
│  └────────── Minutes
└───────────── Hours



Displayed on LCD as:
TIME EV SP
0 12:34:56 G2 75



---

## EEPROM Memory Map

Address 0x00 – 0x09 : Event 0
Address 0x0A – 0x13 : Event 1
...
Address 0x5A – 0x63 : Event 9


Circular buffer — oldest event is overwritten when full.

---

## Building

1. Open in **MPLAB X IDE**
2. Select XC8 compiler
3. Build → Program to device

---

## Default Password

1 1 0 0


SW1 = `1`, SW2 = `0`

---

## Project Structure

Car_Black_Box.X/

├── main.c              # Main loop, operation state machine
├── main.h              # Function prototypes
├── dashboard.c         # Dashboard, event store, menu, view/clear/download log
├── clcd.c / clcd.h     # 16x2 LCD driver
├── ds1307.c / ds1307.h # RTC driver
├── external_eeprom.c   # I2C EEPROM read/write
├── i2c.c / i2c.h       # I2C driver + Timer ISR
├── adc.c / adc.h       # ADC driver
├── uart.c / uart.h     # UART driver
├── timers.c            # Timer0 init
└── digital_keypad.c    # Keypad driver
