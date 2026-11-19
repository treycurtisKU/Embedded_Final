/******************************************************************************
 *   Header Includes
 *******************************************************************************/
#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

#include "eecs_388_lib.h"

/******************************************************************************
 *   Constant definitions
 *******************************************************************************/

// Straight forward GPIO+LED naming scheme:
#define RED_13 GPIO_13
#define GREEN_12 GPIO_12
#define BLUE_11 GPIO_11

// Snippet from Lab 9 eecs_388_interrupts.cpp lines 5-6
// ADAPTED: Replace red/green selection with flashing enable, and start with flashing disabled.
volatile uint8_t flash_enabled = 0;
volatile uint8_t led_state = 0;
// End snippet from Lab 9 eecs_388_interrupts.cpp lines 5-6

/*ServoMotor constants*/
#define SERVO_PULSE_MAX (2400) /* 2400 us */
#define SERVO_PULSE_MIN (544)  /* 544 us */
#define SERVO_PERIOD (20000)   /* 20000 us (20ms) */
#define MIN_ANGLE (0)          /* degrees */
#define MAX_ANGLE (180)        /* degrees */

/******************************************************************************
 *   Local function prototypes
 *******************************************************************************/
static void auto_brake();
static void steering(int pos);

/******************************************************************************
 *   Function: auto_brake() - Auto Brake
 *      Pre condition:
 *          None
 *      Post condition:
 *          Checks the LiDAR distance and configures LEDs
 *******************************************************************************/
static void auto_brake()
{
  // Task-2:
  // Your code goes here (Use Lab 2 & 4 for reference)
  // Check the project document to understand the task

  uint16_t dist = 0;
  // Snippet from Lab 4 eecs_388_tfmini.cpp lines 61-81, 107-109
  // ADAPTED: Use AA/55 headers and XOR checksum; return on invalid packets.
  uint8_t one = ser_read();
  if (0xAA == one)
  {
    uint8_t two = ser_read();
    if (0x55 == two)
    {
      uint8_t Dist_L = ser_read(); // third byte
      uint8_t Dist_H = ser_read(); // fourth byte

      uint8_t checksum = ser_read();

      uint8_t sum = Dist_L ^ Dist_H;

      if (checksum == sum)
      {
        dist = Dist_L | ((uint16_t)Dist_H << 8);
      }
      else
      {
        return;
      }
    }
    else
    {
      return;
    }
  } // Extra else returns to prevent false conditions from breaking the program
  else
  {
    return;
  }
  // End snippet from Lab 4 eecs_388_tfmini.cpp lines 61-81, 107-109

  // Snippet from Lab 4 tfmini.cpp lines 83-106 (ADAPTED: New values for parameters, and new LED states)
  if (dist <= 60)
  {
    gpio_write(GREEN_12, OFF);
    gpio_write(BLUE_11, OFF);

    // Red flashing (interrupt driven, supplemented by lab 9)
    flash_enabled = 1;
  }
  if (dist > 60 && dist <= 100)
  {
    // Red static
    flash_enabled = 0;
    gpio_write(RED_13, ON);
    gpio_write(GREEN_12, OFF);
    gpio_write(BLUE_11, OFF);
  }
  if (dist > 100 && dist <= 200)
  {
    // Red static + green static = yellow static
    flash_enabled = 0;
    gpio_write(RED_13, ON);
    gpio_write(GREEN_12, ON);
    gpio_write(BLUE_11, OFF);
  }
  if (dist > 200)
  {
    // Green static
    flash_enabled = 0;
    gpio_write(RED_13, OFF);
    gpio_write(GREEN_12, ON);
    gpio_write(BLUE_11, OFF);
  }
  // End snippet from Lab 4 tfmini.cpp lines 83-106
}

/******************************************************************************
 *   Function: steering() - Steering
 *      Pre condition:
 *          None
 *      Post condition:
 *          Control the servomotor with GPIO_6
 *******************************************************************************/
static void steering(int pos)
{
  // Task-4:
  // Your code goes here (Use Lab 05 for reference)
  // Check the project document to understand the task

  // Snippet from Lab 5 eecs_388_pwm.cpp line 123
  // ADAPTED: Change from 30 to 5 degree increments, and lower the time per step from 309
  int32_t high_time = SERVO_PULSE_MIN + ((pos / 5) * 51.56);
  // End snippet from Lab 5 eecs_388_pwm.cpp line 123
}

/******************************************************************************
 *   Function: setup() - Initializes the Arduino System
 *      Pre condition:
 *          Hardware must be properly connected (BMP180 sensors, etc.)
 *      Post condition:
 *          Runs initialization calls one time on power up
 *          UART is initialized for ser_printf()
 *          I2C is initialized for communication with BMP180
 *          calib_data is filled with calibration data from BMP180
 *******************************************************************************/
void setup()
{
  uart_init(); // Initialize UART for serial output

  // Setup Auto-break LEDs for Distance
  gpio_mode(GPIO_13, GPIO_OUTPUT); // RED
  gpio_mode(GPIO_12, GPIO_OUTPUT); // GREEN
  gpio_mode(GPIO_11, GPIO_OUTPUT); // BLUE

  // Setup GPIO_6 for PWM output
  gpio_mode(GPIO_6, GPIO_OUTPUT);

  // Snippet from Lab 9 eecs_388_interrupts.cpp lines 20-24
  // ADAPTED: Disable interrupts during setup; use 50 ms interrupts for a 100 ms complete red flash cycle.
  disable_interrupt();
  enable_timer_interrupt();
  set_cycles(12499);

  enable_interrupt();
  // Disable interrupts while configuring the timer.
  // End snippet from Lab 9 eecs_388_interrupts.cpp lines 20-24

  ser_printf("System Initialized");
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
  // Task-4:
  // Setup simulated code for the angles from the lab sheet
  int angle;

  // Task-1&2:
  // Receive data from arudino1 for the distance and setup the LEDs
  auto_brake();

  //  Task-4:
  for (int i = 0; i < 10; i++)
  {
    // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
    // and 0 if the prediction is a negative angle.
    // This is so that it is easier to see the movement of the servo.
    // You are welcome to pass the angle values directly to the steering function.
    // If the servo function is written correctly, it should still work,
    // only the movements of the servo will be more subtle
    if (angle > 0)
    {
      steering(180);
    }
    else
    {
      steering(0);
    }

    // Uncomment the line below to see the actual angles on the servo.
    // Remember to comment out the if-else statement above!
    // steering(angle);
  }
}

// Snippet from Lab 9 eecs_388_interrupts.cpp lines 30, 49-65, 76
// ADAPTED: Only need to toggle red, removed green logic
ISR(TIMER1_COMPA_vect)
{
  if (flash_enabled == 1)
  {
    gpio_write(RED_13, led_state);
    if (led_state == 0)
    {
      led_state = 1;
    }
    else
    {
      led_state = 0;
    }
  }
}
// End snippet from Lab 9 eecs_388_interrupts.cpp lines 30, 49-65, 76