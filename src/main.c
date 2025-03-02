#include "stc8g.h"

#include "gpio.h"

#include "settings.h"

#define SECOND_COUNTER 60
#define SECOND_TIMER 65536 - (F_CPU / 12 / SECOND_COUNTER)

#define LED_ON 1
#define LED_OFF 0
#define LED_TOGGLE -1

int seconds = 0;

void writeGreen(int mode)
{
    switch (mode)
    {
    case LED_ON:
        DIGITAL_ON(5, 5);
        break;
    case LED_OFF:
        DIGITAL_OFF(5, 5);
        break;
    case LED_TOGGLE:
        DIGITAL_TOGGLE(5, 5);
        break;
    }
}

void writeYellow(int mode)
{
    switch (mode)
    {
    case LED_ON:
        DIGITAL_ON(5, 4);
        break;
    case LED_OFF:
        DIGITAL_OFF(5, 4);
        break;
    case LED_TOGGLE:
        DIGITAL_TOGGLE(5, 4);
        break;
    }
}

void writeRed(int mode)
{
    switch (mode)
    {
    case LED_ON:
        DIGITAL_ON(3, 2);
        break;
    case LED_OFF:
        DIGITAL_OFF(3, 2);
        break;
    case LED_TOGGLE:
        DIGITAL_TOGGLE(3, 2);
        break;
    }
}

void setup()
{
    // Port 3 pin 2 as output
    P3M0 |= (1 << 2);
    P3M1 = 0x00;

    // Port 5 pin 4, 5 as output
    P5M0 |= (1 << 5) | (1 << 4);
    P5M1 = 0x00;

    P3 = 0;
    P5 = 0;

    TMOD = 0; // timer 0 in mode 0

    TL0 = SECOND_TIMER & 0xFF;
    TH0 = (SECOND_TIMER >> 8) & 0xFF;

    TR0 = 1; // timer 0 enable
    ET0 = 1; // timer 0 interrupt
    EA = 1;  // global interrupt
}

void wait(int duration)
{
    int time = seconds;

    while (seconds - time < duration)
        ;
}

void automobileLoop()
{
    writeGreen(LED_ON);
    wait(CYCLE_DURATION_SECONDS);
    for (int i = 0; i < 3; i++)
    {
        writeGreen(LED_TOGGLE);
        wait(1);
    }

    writeGreen(LED_OFF);
    writeYellow(LED_ON);
    wait(3);

    writeYellow(LED_OFF);
    writeRed(LED_ON);
    wait(CYCLE_DURATION_SECONDS);

    writeYellow(LED_ON);
    wait(2);

    writeYellow(LED_OFF);
    writeRed(LED_OFF);
}

void pedestrianLoop()
{
    writeRed(LED_ON);
    wait(CYCLE_DURATION_SECONDS + 6);

    writeRed(LED_OFF);
    writeGreen(LED_ON);
    wait(CYCLE_DURATION_SECONDS - 3);

    for (int i = 0; i < 3; i++)
    {
        writeGreen(LED_TOGGLE);
        wait(1);
    }
    writeGreen(LED_OFF);
}

void loop()
{
    if (CURRENT_MODE == MODE_AUTOMOBILE)
    {
        automobileLoop();
    }
    else if (CURRENT_MODE == MODE_PEDESTRIAN)
    {
        pedestrianLoop();
    }
}

void powerDown()
{
    seconds = 0;
    P3 = 0;
    P5 = 0;
    PCON |= PD;
}

int main()
{
    int cycles = 0;
    setup();

    while (1)
    {
        loop();
        cycles++;

        if (cycles == CYCLES_COUNT)
        {
            powerDown();
            cycles = 0;
        }
    }

    return 0;
}

int counter = 0;

void TM0_Isr() __interrupt(TIMER0_INTERRUPT)
{
    counter++;

    if (counter == SECOND_COUNTER)
    {
        seconds++;
        counter = 0;
    }
}