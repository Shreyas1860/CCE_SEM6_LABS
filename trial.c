#include <stdio.h>
#include <LPC17xx.h>
#include <string.h>

// Defining Constants
#define LED_Pinsel 0xFF    // P0.4-0.11 (LEDs)
#define TRIGGER_PIN (1 << 15)     // P0.15 (Trigger Pin)
#define ECHO_PIN (1 << 16)     // P0.16 (Echo Pin)

// Variable Declarations
char ans[20] = "";
int temp, temp1, temp2 = 0;
int flag = 0, flag_command = 0;
int i, j, r, echoTime = 5000;
float distance = 0;

// Function Declarations
void lcd_wr(void);
void port_wr(void);
void delay(int r1);
void timer_init(void);
void dealy_in_US(unsigned int microseconds);
void dealy_in_MS(unsigned int milliseconds);

// Functions
void dealy_in_US(unsigned int microseconds)
{
    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->PR = 0;
    LPC_TIM0->MR0 = microseconds - 1; // Set match register for 10us
    LPC_TIM0->MCR = 0x04; // Reset timer on match
    LPC_TIM0->TCR = 0x01; // Enable timer
    while ((LPC_TIM0->TCR & 0x01) == 0);
}

void dealy_in_MS(unsigned int milliseconds) // Using Timer0
{
    dealy_in_US(milliseconds * 1000);
}

void timer_init(void)
{
    LPC_SC->PCONP |= (1 << 1); // Power on Timer 0
    LPC_TIM0->CTCR = 0x0;
    LPC_TIM0->PR = 0; // PCLK divided by 1
    LPC_TIM0->TCR = 0x02; // Reset Timer
}

void delay(int r1)
{
    for (r = 0; r < r1; r++);
}

void port_wr()
{
    int j;
    LPC_GPIO0->FIOPIN = temp2 << 23;
    if (flag_command == 0)
    {
        LPC_GPIO0->FIOCLR = 1 << 27;
    }
    else
    {
        LPC_GPIO0->FIOSET = 1 << 27;
    }
    LPC_GPIO0->FIOSET = 1 << 28;
    for (j = 0; j < 50; j++);
    LPC_GPIO0->FIOCLR = 1 << 28;
    for (j = 0; j < 10000; j++);
}

void lcd_wr()
{
    temp2 = (temp1 >> 4) & 0xF;
    port_wr();
    temp2 = temp1 & 0xF;
    port_wr();
}

// Main Program
int main()
{
    SystemInit();
    SystemCoreClockUpdate();
    timer_init();
    LPC_PINCON->PINSEL0 &= ~(0xFF << 8);    // Clear bits 8-15 for GPIO P0.4-P0.11
    LPC_PINCON->PINSEL0 &= ~(3 << 30);    // Clear bits 30-31 for GPIO P0.15
    LPC_PINCON->PINSEL1 &= ~(3 << 0);    // Clear bits 0-1 for GPIO P0.16
    LPC_GPIO0->FIODIR |= TRIGGER_PIN; // Set Trigger pin as output
    LPC_GPIO1->FIODIR &= ~(1 << 16); // Set Echo pin as input

    // Direction for ECHO PIN
    LPC_GPIO0->FIODIR |= LED_Pinsel << 4; // Direction for LED
    LPC_PINCON->PINSEL1 &= ~(0xFFFF); // Configure P0.23-P0.28 as GPIO
    LPC_GPIO0->FIODIR |= 0xF << 23 | 1 << 27 | 1 << 28; // Direction For LCDs
    flag_command = 0;

    // Initialization commands for LCD
    int command_init[] = {3, 3, 3, 2, 2, 0x01, 0x06, 0x0C, 0x80};
    for (i = 0; i < 9; i++)
    {
        temp1 = command_init[i];
        lcd_wr();
        delay(30000);
    }

    // Main loop
    while (1)
    {
        // Triggering Ultrasonic Sensor
        LPC_GPIO0->FIOSET = TRIGGER_PIN;
        delay(10);
        LPC_GPIO0->FIOCLR = TRIGGER_PIN;

        // Measure echo time
        while (!(LPC_GPIO1->FIOPIN & ECHO_PIN)); // Wait till ECHO PIN becomes high
        dealy_in_US(10); // Wait for a short time
        timer_init(); // Reinitialize timer
        LPC_TIM0->TCR = 0x01; // Start timer
        while (LPC_GPIO1->FIOPIN & ECHO_PIN); // Wait till ECHO PIN becomes low
        echoTime = LPC_TIM0->TC; // Read timer value
        distance = (0.00343 * echoTime) / 2; // Calculate distance in cm
        sprintf(ans, " Distance: %.3f", distance);

        // Display distance on LCD
        flag_command = 1;
        i = 0;
        flag_command = 0;
        temp1 = 0x01;
        lcd_wr();
        flag_command = 1;

        while (ans[i] != '\0')
        {
            temp1 = ans[i];
            lcd_wr();
            delay(30000);
            i++;
        }

        // Control LED based on distance
        if (distance < 20)
        {
            LPC_GPIO0->FIOSET = LED_Pinsel << 4;
            LPC_GPIO0->FIOSET = 1 << 17;
        }
        else
        {
            LPC_GPIO0->FIOCLR = LED_Pinsel << 4;
            LPC_GPIO0->FIOCLR = 1 << 17;
        }
        delay(88000);
    }
}
