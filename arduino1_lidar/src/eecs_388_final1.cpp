/******************************************************************************
 *   Header Includes
 *******************************************************************************/
#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

#include "eecs_388_lib.h"

/******************************************************************************
 *   Function: setup() - Initializes the Arduino System
 *      Pre condition:
 *          Hardware must be properly connected (sensors, LEDS, etc.)
 *      Post condition:
 *          Runs initialization calls one time on power up
 *          Input/Output (IO) pins are configured
 *******************************************************************************/
void setup()
{
    uart_init();

    return;
}

/******************************************************************************
 *   Function: loop() - Main execution loop
 *      Pre condition:
 *          setup() has been executed and system is initialized
 *      Post condition:
 *          Performs a single iteration of the system's function
 *          Repeates indefinetely unless the board is reset or powered off
 *******************************************************************************/
void loop()
{
    uint16_t dist = 0; /* LIDAR distance data is 16 bits. */

    ser_printline("Setup completed.");
    ser_write('\n');

    while (1)
    {
        /*
            - read a data frame from the TFmini sensor

            - from Table 6, we know each LIDAR measurement sends 9 bytes
            - the function `ser_read()` can process 1 byte per function call

            - This if-statement below will read (and check) the first two bytes of a valid data frame
            - You will need to write code to read the subsequent bytes to get the distance data
            - You will need to process the data received from the sensor to get the correct distance value
        */

        /*

                - print the measured value to the console (i.e. the serial monitor)
                - You can use ser_printf() or ser_printline() or ser_write()

                - Helpful guide for printing with variadic function similiar to printf()
                - https://en.cppreference.com/w/cpp/io/c/fprintf


                - Send the distance to Arduino2
        */

        // Snippet from Lab 4 tfmini.cpp lines 61-81
        uint8_t one = ser_read();
        if ('Y' == one)
        {
            uint8_t two = ser_read();
            if ('Y' == two)
            {
                uint8_t Dist_L = ser_read(); // third byte
                uint8_t Dist_H = ser_read(); // fourth byte

                uint8_t five = ser_read();
                uint8_t six = ser_read();
                uint8_t seven = ser_read();
                uint8_t eight = ser_read();
                uint8_t NINE = ser_read();

                uint8_t sum = one + two + Dist_L + Dist_H + five + six + seven + eight;

                if (NINE == sum)
                {

                    dist = Dist_L | ((uint16_t)Dist_H << 8);
                    ser_printf("Distance: %u cm", dist); //Print distance readout to terminal, can remove later
                }
            }
        }
        // End snippet from Lab 4 tfmini.cpp lines 61-81
    }

    return;
}