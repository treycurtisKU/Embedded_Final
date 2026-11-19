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

// Snippet from Lab 9 eecs_388_interrupts.cpp lines 4-6
// ADAPTED: Replace red/green selection with flashing enable, and start with flashing disabled.
volatile uint8_t interrupt_count = 0;
volatile uint8_t flash_enabled = 0;
volatile uint8_t led_state = 0;
// End snippet from Lab 9 eecs_388_interrupts.cpp lines 4-6

//Breaking distances
#define no_break 0 //distance >200 cm (green)
#define slight_break 1 // 100 < distance <= 200 cm (yellow)
#define hard_break 2 //60 < distance <= 100cm (red)
#define max_break 3 // distance <= 60cm (flash red)
//Start off in no brake
volatile uint8_t braking_state = no_break;

/*ServoMotor constants*/
#define SERVO_PULSE_MAX (2400) /* 2400 us */
#define SERVO_PULSE_MIN (544)  /* 544 us */
#define SERVO_PERIOD (20000)   /* 20000 us (20ms) */
#define MIN_ANGLE (0)          /* degrees */
#define MAX_ANGLE (180)        /* degrees */

// Servo pulse durations. To be shared with timer ISR (microseconds)
volatile uint16_t servo_high_time = SERVO_PULSE_MIN;
volatile uint16_t servo_low_time = SERVO_PERIOD - SERVO_PULSE_MIN;
uint8_t servo_status = 0; // 0 = GPIO 6 LOW, 1 = GPIO 6 HIGH
uint16_t servo_active_low_time = SERVO_PERIOD - SERVO_PULSE_MIN;

// Task 4. Steering angles, to loop in this order.
const int angle_values[] = {10, 25, 75, 45, 100, 40, 125, 15, 150, 50, 170};
const uint8_t angle_count = 11;
uint8_t angle_indexer = 0; // Track which angle we're iterating on
unsigned long lastAngleChange = 0;
const unsigned long angle_interval = 1000; // holy each angle for one second

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

  // Snippet from Lab 5 eecs_388_pwm.cpp line 123-124
  // ADAPTED: Change from 30 to 5 degree increments, and lower the time per step from 309
  int32_t high_time = SERVO_PULSE_MIN + ((pos / 5) * 51.56);
  int32_t low_time = SERVO_PERIOD - high_time;
  // End snippet from Lab 5 eecs_388_pwm.cpp line 123-124

  // Share calculations outside this function without interrupt
  disable_interrupt();
  servo_high_time = high_time;
  servo_low_time = low_time;
  enable_interrupt();
}

// Snippet from Lab 9 eecs_388_lib.c lines 314-326
// ADAPTED: Configure Timer2 for 1 ms interrupts instead of Timer1.
void enable_led_timer_interrupt(void)
{
  TCCR2B = 0; // stop timer 2 while configing it

  // Task 2.3

  // In TCCR1B: Setup CTC mode, set the prescaler you will use with set_cycles() compare value
  TCCR2A = (1 << WGM21);  // timer2 in ctc mode. resetafter ocr2a match
  TCNT2 = 0;              // Start the counter at zero
  OCR2A = 249;            // 16MHz/64. 250 ticks for a 1ms period
  TIFR2 = (1 << OCF2A);   // Clear any pending Timer2 compare A interrupt before enabling it.
  TIMSK2 = (1 << OCIE2A); // Enable timer2. Compare A interrupts
  TCCR2B = (1 << CS22);   // Start timer2 with prescaler 64
}
// End snippet from Lab 9 eecs_388_lib.c lines 314-326

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

  steering(angle_values[0]);
  lastAngleChange = millis();
  // Snippet from Lab 9 eecs_388_interrupts.cpp lines 20-24
  // ADAPTED: Configure Timer2 for 1 ms interrupts; count 50 ticks between brake toggles.
  disable_interrupt();
  enable_led_timer_interrupt();

  gpio_write(GPIO_6, OFF); // Start the servo signal low before pulses
  servo_status = 0;
  enable_timer_interrupt();
  set_cycles((servo_low_time / 4) - 1); // First interrupt starts high pulse
  TIFR1 = (1 << OCF1A);                 // Clear Timer1 compare A interrupt before enabling more

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
  //  Task-4:
  // pasted and adapted from final1.cpp. Advance thru different steering angles. One per second
  unsigned long currentTime = millis(); // reads current time

  if (currentTime - lastAngleChange >= angle_interval) // check if 1 second has passed
  {
    lastAngleChange = currentTime;
    // advance to the next angle in the array. loop to start if at the end
    angle_indexer = angle_indexer + 1;
    if (angle_indexer >= angle_count)
    {
      angle_indexer = 0;
    }

    steering(angle_values[angle_indexer]);
  }
  // Tasks 2 and 3. Recieve distance from first arduino and update brake LEDs
  auto_brake();
}

// Snippet from Lab 9 eecs_388_interrupts.cpp lines 30, 49-65, 76
// ADAPTED: Only need to toggle red, removed green logic
ISR(TIMER2_COMPA_vect) // Runs when Timer2 reachers compare value. Handles brake LED timing
{
  // Snippet from Lab 9 eecs_388_interrupts.cpp lines 32-36
  // ADAPTED: Use Timer2 and count 50 one-millisecond ticks; toggle only red.
  interrupt_count = interrupt_count + 1;
  // Task 3.2 - Write code to blink the red LED every 100ms for 1 second.  Then
  if (interrupt_count == 50)
  {
    // End snippet from Lab 9 eecs_388_interrupts.cpp lines 32-36
    interrupt_count = 0;
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
}
// End snippet from Lab 9 eecs_388_interrupts.cpp lines 30, 49-65, 76

// Inspired by the above snippet.
// Runs when Timer1 reaches compare value and handles servo pulse timing
ISR(TIMER1_COMPA_vect)
{
  // Start a new pulse when the servo signal is LOW
  if (servo_status == 0)
  {
    // Capture matching durations before starting pulse
    uint16_t high_time = servo_high_time;
    servo_active_low_time = servo_low_time;

    gpio_write(GPIO_6, ON);

    servo_status = 1;

    // Timer1 ticks every 4 microsecs. Subtract 1 because counting starts at zero.
    set_cycles((high_time / 4) - 1);
  }
  else
  {
    // end the high pulse and hold low
    gpio_write(GPIO_6, OFF);

    servo_status = 0;

    set_cycles((servo_active_low_time / 4) - 1);
  }
}