# Course references

- `EECS_388_Arduino_Timers.pdf`, pages 1-2: TCNT1, OCR1A, compare-match interrupts, and the Lab 9 timer helpers.
- `EECS_388_Lab9.pdf`: original timer interrupt assignment.
- `EECS_388_Lab5.pdf`: original servo control assignment.
- `EECS_388_Lab_Final_Project.pdf`, page 6: bonus requirements, including state-driven brake LED interrupts and servo PWM without delay APIs.
- The ATmega328P datasheet is already included in each Arduino project's `docs/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf`. Timer2 references: section 17.7.2 (CTC), Table 17-8 on page 130 (mode selection), Table 17-9 on page 131 (clock/prescaler selection), and sections 17.11.6-17.11.7 on page 132 (interrupt enable and flags).

Timer2 configuration rationale: at 16 MHz with prescaler 64, the counter ticks every 4 microseconds. OCR2A = 249 yields 250 ticks per CTC period, or 1 ms. Counting 50 compare interrupts preserves the existing 50 ms brake toggle interval (100 ms full flashing cycle). This preserves the existing behavior; full Bonus 1 alignment still requires a braking-state value and ISR handling of steady LED states.
