//Ports:
//1.0-1.3 = rows
//2.0-2.2 = columns
//1.4 = pump 1
//1.5 = pump 2
//2.3 = pump 3
//2.4 = pump 4
//Pumps run at ~100mL/min while, cycles run at ~1mil/sec
#include <msp430g2553.h>
#include <legacymsp430.h>
#include <stdint.h>

char keymap_char[12] = {'1', '2', '3',
                        '4', '5', '6',
                        '7', '8', '9',
                        '*', '0', '#'};

uint32_t keymap_hex[12] = {0x0, 0x1, 0x2,
                           0x3, 0x4, 0x5,
                           0x6, 0x7, 0x8,
                           0x9, 0xA, 0xB};


void led_output(uint8_t bits);

int main(void)
{
    // Disable watch dog timer
    WDTCTL = WDTPW+WDTHOLD;

    // Set all pins as inputs
    // pins 0, 1, 2, 3 of port 1 read values from the rows of the numpads
    P1DIR = 0;
    P2DIR = 0;

    // Pins 0, 1, 2 of port 2 feed into the columns of the numpad
    // pins 7, 6 of port 1 and pins 5 and 4 of port 2 feed the 4 LEDs
    P1DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    P2DIR |= (BIT3 | BIT4 | BIT5);

    // Set initial outputs values to 0
    P1OUT = 0;
    P2OUT = 0;
    uint8_t row = 0;
    uint8_t col = 0;

    uint32_t io_pins[4] = {BIT0, BIT1, BIT2, BIT3};

    uint32_t keypress1 = 0xC, keypress2 = 0xC; //initialize keypress to value not on keypad

    while(1) {
        for(row = 0; row < 4; row++)
        {
            P1OUT |= io_pins[row];
            for(col = 0; col < 3; col++)
            {
                if(P2IN & io_pins[col])
                {
                    //led_output(keymap_hex[row*3+col]);
                    if(keypress2 == 0xC)                //detect if first keypress or 2nd keypress
                    {
                        if(keypress1 == 0xC)
                        {
                            keypress1 = keymap_hex[row*3+col];      //record 1st keypress
                            __delay_cycles(1000000);
                        }else{
                            keypress2 = keymap_hex[row*3+col];      //record 2nd keypress (no new values will be admitted until cleared)
                            __delay_cycles(1000000);
                        }
                    }
                    if(keymap_hex[row*3+col] == 0x9)        //if user presses *, clears the keypress values
                    {
                        keypress1 = 0xC;
                        keypress2 = 0xC;
                    }
                    if(keymap_hex[row*3+col] == 0xB)        //if user presses #, executes then clears
                    {
                        if(keypress1 == 0xA && keypress2 == 0x0)        //recipe for 01
                        {
                            P2OUT |= (BIT3 | BIT4);
                            __delay_cycles(5000000);
                            P2OUT &= ~(BIT3 | BIT4);
                            P1OUT |= BIT4;
                            __delay_cycles(5000000);
                            P1OUT &= ~BIT4;
                        }
                        if(keypress1 == 0xA && keypress2 == 0x1)        //recipe for 02
                        {
                            P2OUT |= BIT3;
                            __delay_cycles(5000000);
                            P2OUT &= ~BIT3;
                        }
                        if(keypress1 == 0xA && keypress2 == 0x2)        //recipe for 03
                        {
                            P2OUT |= BIT3;
                        }
                        keypress1 = 0xC;
                        keypress2 = 0xC;
                    }
                }
            }
            P1OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);
        }
    }
}
/*
void led_output(uint8_t bits)
{
    (bits & BIT0) ? (P1OUT |= BIT7) : (P1OUT &= ~BIT7);
    (bits & BIT1) ? (P1OUT |= BIT6) : (P1OUT &= ~BIT6);
    (bits & BIT2) ? (P2OUT |= BIT5) : (P2OUT &= ~BIT5);
    (bits & BIT3) ? (P2OUT |= BIT4) : (P2OUT &= ~BIT4);
}*/
