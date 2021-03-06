ECE573 Lab3:

Purpose:
  *Try another way to use push buttons as a user interface.
  *Using interrupts with counter/timer 0
  *Learn how to use SPI I/O.

Overview:
  *This lab will extend upon the work done in lab 2.  There are two functions
   we are focusing on: interrupts and use of SPI.  
   
   In this lab we will use interrupts to replace the loop+delay structure of last
   lab.  Timer counter zero will be used to periodically interrupt the processor 
   to scan the switches.  In later labs, we will find this to be a much easier
   way to keep track of time processing events.
  
   This lab will also introduce the SPI interface.  This serial interface will
   be used to scan the values of the digital encoders and also to provide data
   to the bar graph display.

Project Specifications:
  *The two digital encoders and bar graph will be connected to the SPI interface.
  *The buttons we used last time will be used again but in a different way.
   Before we incremented a count each time the button was pushed.  Now we want
   the switches to operate as "mode" switches.  The mode selected will remain
   in effect until the next time the button is pushed.  This behavior is
   fundamentally different that the last time.  The encoders will be used to
   increment or decrement the count.
  *Directly after reset, either encoder will increment or decrement, by one, a 
   count displayed on the LED display.   
  *One button (your choice) will choose a mode for incrementing or decrementing
   the count by two instead of one.
  *Another button (your choice) will choose a mode for incrementing or decrementing
   the count by four instead of one.
  *If both modes are selected, the count will not increment or decrement.
  *The count will roll over at 1023 as before but if decremented beyond zero,
   the next count will be 1023.
  *The status of the modes will be indicated on the bar graph display in a manner
   of your choice.

Before Lab:
  *Determine the correct connection for the SPI interface for both the
   encoders and bar graph.  You will need to become familiar with the operation
   of both the 74HC165 and 74HC595 to use them correctly.  Draw that part of
   your schematic out before you wire things up.
  *The enable for the encoder shift register (74HC165) should be connected to 
   PORTE bit 6.
  *The output enable for the bar graph display will be from PORTB bit 7.
  *The clock for the HC595 output register can be PORTD bit 2.


Software: 
  *Write out pseudocode of your software (suggested).
  *Use 8-bit timer/counter 0 (TCNT0) initialization code as shown below: 

     //timer counter 0 setup, running off i/o clock
     TIMSK |= (1<<TOIE0);             //enable interrupts
     TCCR0 |= (1<<CS02) | (1<<CS00);  //normal mode, prescale by 128

  *Reuse your code from last time as appropriate.

Testing:
  *Make sure the encoders increment and decrement as expected.  They should be
   able to respond to reasonable rapid rotation of the controls as they they
   might be used as volume or tuning controls.  
  *There should be no skipping or jumping of the count.
  *Make sure all mode settings are tested for increment and decrement.

What to show for your work:
  *Schematic diagram you used to wire up your circuit.  
  *Printout of your code.  Use a2ps. 
  *Demonstrate working system to TA by before next lab.

Grading:
  *Circuit function                   +70% (demo system in lab)
    -performs as expected,switches debouncing cleanly
    -if count "jumps"                 -deduct 10%
    -dim display/flickers             -deduct  5%
    -other abnormality                -deduct  5% (for each distinct problem)

  *Documentation                      +30%
    -complete, neat, correct
    -code well written, commented
    -sloppy documentation             -deduct 10%
    -poorly commented code            -deduct 10%

Lab weighting = 0.15

