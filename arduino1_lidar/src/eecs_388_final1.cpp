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
uint16_t dist = 0;              /* LIDAR distance data is 16 bits. */

ser_printline("Setup completed.");
ser_write( '\n' );

while( 1 )
    { 
    /* 
        - read a data frame from the TFmini sensor
        
        - from Table 6, we know each LIDAR measurement sends 9 bytes
        - the function `ser_read()` can process 1 byte per function call

        - This if-statement below will read (and check) the first two bytes of a valid data frame
        - You will need to write code to read the subsequent bytes to get the distance data
        - You will need to process the data received from the sensor to get the correct distance value
    */
    if ('Y' == ser_read() && 'Y' == ser_read()) 
        {
        //read the distance bytes
        uint8_t distance_low = ser_read();
        uint8_t distance_high = ser_read();

        //read the remaining bytes
        uint8_t strength_low = ser_read();
        uint8_t strength_high = ser_read();
        uint8_t reserved = ser_read();
        uint8_t quality = ser_read();
        uint8_t checksum = ser_read();

        //calculate the checksum
        uint8_t sum = 0x59 + 0x59;
        sum += distance_low;
        sum += distance_high;
        sum += strength_low;
        sum += strength_high;
        sum += reserved;
        sum += quality;

        //checks if the data is valid
        if (sum == checksum)
        {
            //combine distance bytes
            dist = (distance_high << 8) | distance_low;

            //print the distance
            ser_printf("Distance: %u cm\n", dist);
        }
        }
    }

return;
}