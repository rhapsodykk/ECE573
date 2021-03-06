ECE573 Lab2:

Purpose:
  *Review C programming/compiling.
  *Learn how to address ports/pins and set i/o configuration.
  *Gain experience at prototyping 
  *Learn how to correctly connect uC to peripherals.
  *Understand I/O pin drive and input levels.
  *Using multiplexed displays. 
  *Debouncing push button switches. 
  *Use a software timing loop to create delays.
  *Gain experience creating a user interface.
  *Understand the limitations of using software timed loops

Project Specifications: 
  *Build a four digit multiplexed LED display with eight push button switches.
  *Program uC to display (on LEDs) values created by the push button switches.
  *Display will operate in base ten. 
  *The i/o board push buttons will be utilized as follows:
     Button S0 increments the value displayed on the 4 digit display by one.  
     Button S1 increments by two, button S2 by four, etc.  Button S7 increments 
     by 128.  The display top count is 1023 and the next count in sequence would 
     be 1.  Initially, the count indicates zero.  

Before Lab:
 *Draw a complete schematic diagram of how you are going to hook up the 
  display and switches to port A and B.  This is not a sloppy crayon
  drawing.  This is a fully complete schematic with all part numbers, values
  and pin numbers.  If spot checks of your working schematic find that your
  drawings are not up to standard, you may loose points.
 *Port A will be used to drive the segments (by pulling them to ground) and also to
  sense the state of the push button switches.  
 *Determine the value of the PORTA LED segment current limiting resistors such that:
   -current through an LED segment will not exceed maximum ratings
   -Mega128 total port/device current capacity is not exceeded
   -display is easily readable in lab light
 *Port B bits 4-6 will select the digit to display via a 74HC138 3-to-8 decoder.  
  Look up how the 74HC138 works by reading its datasheet.  
 *Port B bit 7 in will be used to implement a PWM dimming control in another lab.  
  It will be driven continuously low for now to saturate the 2N4403 it drives
  and keep the LEDs at full brilliance.  Determine the value of the base bias resistor 
  to allow this transistor to source the necessary current for all the digits.  
 *Some of the 74HC138 outputs drive the digit driver transistors.  Determine the value 
  of the base bias resistor to allow these transistors to source the necessary current 
  for each digit.
 *PORTB bits 0-3 are not to be used at this time.  They are reserved for other control 
  and for SPI use.
 *The Y7 decoder output is used to enable the tri-state buffer on the push button board.
  The purpose of this is to allow the pushbuttons to pull the PORTA inputs low...but only
  under control of the uP.  Otherwise, the push buttons could blank some
  segments.

Assembly notes: 
 *Think ahead and carefully consider what you are doing.
 *Place and solder in the LED display on the ->BACK SIDE<- of the display board. This is 
  the board side ->WITHOUT<- the IC.
 *Use the ribbon cables to make connections to PORTA and PORTB.  Connector goes on one end 
  and split out the necessary wires to make the connections to the boards.
 *The 2x5 connection block (A_2-G_2) are best connected to the mega128 board.
 *The 1x8 connection block (A-G) is best connected to the push button board.
 *Mark your cables so you don't connect them to the wrong place.
 *Make sure unconnected wires don't short out to anything.
 *Remember to connect power to the push button board.
 *Use the power and ground available on the mega128 board and through the connectors to get 
  power to your boards.
 *Carefully check all your connections for bad solder joints and bridged pads.

Software Notes:
  *Write some test code to test the display and the push buttons.  This will check your wiring 
   and avoid you spending too much time fiddling with software that is exercising broken hardware.
   (recommendation only) 
  *Write out pseudocode or flowchart for lab2 code. Don't just start writing code. 
   (recommendation only) 
  *Write C code to perform desired function.  
  *Use of interrupts or timers is not allowed.

Testing:
  *Test and debug code.
  *Measure the actual current flowing through one segment.  Try and account for any differences.
  *Measure the duty cycle (on time versus off time) for each digit.  Is it what you expected?  

What to turn in (at beginning of your lab section)
  *Professional quality schematic diagram you used to wire up your circuit.  Hand drawn schematics 
   are entirely acceptable and even preferable.
  *Sheet(s) showing calculations done for:
    -segment current limiting resistors
    -base resistors for digit driver transistors (Q1-Q5)
    -base resistor for PWM transistor (Q6)
    -show that Iol spec for mega128 is not exceeded by driving the segments
    -show that neither Iol or Ioh spec for mega128 is not exceeded by the push buttons 
  *Printout of your code.  Use a2ps to make easily readable.
  *Sheet(s) with actual measurement of:
    -segment current and its deviation from calculated values
    -duty cycle for display digits
  
Grading:
  *Circuit function                   +70% (demo system in lab)
    -performs as expected,switches debouncing cleanly  
    -if count "jumps"                 -deduct 10%
    -dim display/flickers             -deduct  5%
    -other abnormality                -deduct  5%

  *Documentation                      +30%
    -complete, neat, correct    
    -code well written, commented     
    -sloppy documentation             -deduct 10%
    -poorly commented code            -deduct 10%

Lab weighting = 0.15



schematic_note
Remember:

Be sure to include:
  -reference designators (i.e. R1, R2, C1, U1, etc)
  -component values or part numbers
  -for ICs, each pin should be labeled with the pin name *and* number
  -straight lines (use a ruler)

Other advice:
  -If you scan/copy your schematic and turn in the copy, make sure its readable.
  -Don't just print the schematics provided on the course webpage and turn them in.
   This shows no effort.
  -You are drawing schematics, not a block diagram. You should not (for example) 
   replace the entire switch board with a block labeled "switch board."
  -If it is appropriate to combine multiple wires into a bus, you may do that. If 
   you combine multiple elements that are wired similarly (the eight diode+switch 
   pairs on the switch board) into a single element, make it very clear in the 
   schematic.

Bottom line, remember that these are to be "professional quality schematics."

