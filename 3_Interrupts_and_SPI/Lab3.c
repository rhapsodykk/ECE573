// lab3.c 
// Sihe Yang
// 11.1.2015

// 7_segment board       Mega128 board
// --------------        ----------------------   
//       A               PORTA bit 0 (AD0)
//       B               PORTA bit 1 (AD1)
//       C               PORTA bit 2 (AD2)
//       D               PORTA bit 3 (AD3)
//       E               PORTA bit 4 (AD4)
//       F               PORTA bit 5 (AD5)
//       G               PORTA bit 6 (AD6)
//       DP              PORTA bit 7 (AD7)
//      SEL0             PORTB bit 4 (AD8)
//      SEL1             PORTB bit 5 (OC1A)
//      SEL2             PORTB bit 6 (OC1B)
//      EN_N                   GND
//       EN                    VDD
//       GND                   GND
//       VDD                   VDD


// 8_push_Button board   7_segment board         
// -------------------   ------------------ 
//      A_2                     J8  
//      B_2                     J6
//      C_2                     J7
//      D_2                     J1
//      E_2                     J2
//      F_2                     J3
//      G_2                     J4
//      DP_2                    J5
//     COM_EN                  DEC7


// 8_push_Button board   Mega128 board         
// -------------------   ------------------ 
//      COM_LVL                GND                
//       GND                   GND
//       VDD                   VDD
     

// Bar graph board       Mega128 board         
// -------------------   ------------------ 
//      REGCLK           PORTB bit 0 (|S|S)
//      SRCLK            PORTB bit 1 (SCK)
//      SDIN             PORTB bit 2 (MOSI)
//      OE_N             PORTB bit 7 (OC2/OC1C)                      
//       GND                   GND
//       VDD                   VDD


// Encoder board         Mega128 board         
// -------------------   ------------------ 
//      SCK              PORTE bit 6 (T3/INT6)
//      SH/LD            PORTE bit 7 (ICP3/INT7)
//      CKINH            PORTB bit 1 (SCK)
//      SOUT             PORTB bit 3 (MISO)                      
//       GND                   GND
//       VDD                   VDD

#define F_CPU 16000000 // cpu speed in hertz 
#define TRUE 1
#define FALSE 0
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t a[4] = {0};   // reset arrary a, hold data about which digit display
uint8_t SEG[10] = {0xFF};   //reset array SEE, hold data about decimal to 7-sgement LED
uint8_t display_count = 0x00; //holds count for display on bar graph. After reseting, it shows 1
int8_t count = 1;   // the initial value for incrementing or decrementing sum
int8_t count_sign;   // the sign of counting number, its value is 1 or -1.
uint8_t past_encoder_state;  //It is for duplicating the value of encoder_data received by ISP
uint8_t coefficient = 1;   // For increment/ decrement 2 or 4, is changed by pushing button.
uint8_t encoder_state = 3; //state of encoder, it initial state is 3(0xFF & 0x03).


//*********************************************************************************
//                            chk_buttons                                      
//This code function debounces the pushed buttons. i is the button number. If the  
//buttons are not pushed, state[i] will be equal to E000 all the time. When any 
//button is pushed, the corresponding state[i] will become 1 from 0 by checking 
//PINA, and it will come back to 1 if pushed button is released. In this function,
//the right most bit is always shifted to the left side. So state[i] is changed from
//E000 to F000 if any button is released after pushing. 
//Because button 1 cannot be debounce by using the function for the rest of buttons,
//button 1 uses an alternative function to be debounced.
//*********************************************************************************
int16_t chk_buttons(uint8_t i) {  //check which button is pushed
static uint16_t state[8] = {0x0000}; //holds present state
	//the way of debounce S2-8 is not useful to S1, use another way
	if (!(PINA&0x01)){   
	state[i] = (state[i] << 1) | (!(PINA&0x01)) | 0xE000;
		if (state[0] == 0xF000)
			return 0;
		return 1;
	}
	else {
	// S2-8 debounce
	state[i] = (state[i] << 1) | (!bit_is_clear(PINA,i)) | 0xE000;
  	if (state[i] == 0xF000) 
		return 1;
  	return 0;
	}
}


//**********************************************************************************
//                                   segment_value                                    
//This function is for displaying number on 7-segement board by using a 16-bit binary
//value. So the code utilizes the binary codes to display decimal numbers from 0 to 9.                  
//**********************************************************************************
uint8_t segment_value() {    
int k;
	//choose the 7-segmaent LEDs to display based on its value of the corresponding array
	for (k = 0; k < 5; k++) {
		switch (a[k]){
			case 0: SEG[k] = 0xC0; break;  //display 0 		
			case 1: SEG[k] = 0xF9; break;  //display 1
	  		case 2: SEG[k] = 0xA4; break;  //display 2
  			case 3: SEG[k] = 0xB0; break;  //display 3
			case 4: SEG[k] = 0x99; break;  //display 4
  			case 5: SEG[k] = 0x92; break;  //display 5
  			case 6: SEG[k] = 0x82; break;  //display 6
 	 		case 7: SEG[k] = 0xF8; break;  //display 7
			case 8: SEG[k] = 0x80; break;  //display 8
			case 9: SEG[k] = 0x90; break;  //display 9
			case 10: SEG[k] = 0xFF; break;  //display null
  			default: SEG[k] = 0xFF;
  		}//switch
	}//for    
}//segment_value


//**********************************************************************************
//                                   segment_sum     
//The 4 digit number cannot be display at the same time, so microcontroller should 
//display each digit underlying fast frequency to look like they are showed at the
//same time.
//The array is loaded at exit as:  |digit3|digit2|colon|digit1|digit0|
//**********************************************************************************
void  segment_sum() {   //display 4 digits of final sum
int n;
	for (n = 0; n < 5; n++) {
    		switch (n) {
    		case 0: PORTB = 0x00; PORTA = SEG[0]; _delay_ms(2); break;  //display units
    		case 1: PORTB = 0x10; PORTA = SEG[1]; _delay_ms(2); break;  //display tens
    		case 3: PORTB = 0x30; PORTA = SEG[3]; _delay_ms(2); break;  //display hundreds
    		case 4: PORTB = 0x40; PORTA = SEG[4]; _delay_ms(2); break;  //display thousands
    		default : PORTB = 0x20; PORTA = 0xFF; _delay_ms(2);
	    	}//switch 4 digits | PORTB: digit | PORTA: value
	}//for
}//segment_sum


/***********************************************************************/
//                            spi_init                               
// This function initalize the SPI port on the mega128. The following 
// parameters is set up: master mode, clock=clk/2, cycle half phase,                    
// low polarity, MSB first interrupts disabled, poll SPIF bit in SPSR to
// check xmit completion.
/***********************************************************************/
void spi_init(void){
  	SPCR  |=   (1<<SPE)|(1<<MSTR);  //set up SPI mode      
  	SPSR  |=   (1<<SPI2X);          //double speed operation   
 }//spi_init


/***********************************************************************/
//                              tcnt0_init                             
// This function initalize timer/counter0 (TCNT0). TCNT0 is for running 
// in async mode with external 32khz crystal. If it runs in normal mode,
// there is no prescaling. Interrupt occurs at overflow 0xFF.
/***********************************************************************/
void tcnt0_init(void){
  ASSR   |=  (1<<AS0);  //ext osc TOSC                        
  TIMSK  |=  (1<<TOIE0);  //enable timer/counter0 overflow interrupt  
  TCCR0  |=  (1<<CS00);  //normal mode, no prescalering   
}


/***********************************************************************/
//                              encoders                             
// Switching two encoders will increment or decrement the sum. To judge  
// the data sent from 74HC165 can decide which encoder is switched. Turn-  
// ing counterclockwise of any encoder will increase the count. Oppositely,
// turning clockwise will decrease the count. If the user does not switch 
// any button, this count is changed by one.
/***********************************************************************/
uint8_t encoders(uint8_t SPI_output){
	//the value of SPI sent from 74HC165 output when right encoder is changed
	uint8_t encoder_right[3]= {0xFB, 0xF3, 0xF7, 0xFF}; 
	//the value of SPI sent from 74HC165 output when left encoder is changed
	uint8_t encoder_left[3]= {0xFE, 0xFC, 0xFD, 0xFF}; 

	PORTE = 0x00; 					// SH/LD = 0 CLK_INH = 0 
  	PORTE |= 0x80; 					// SH/LD = 1 CLK_INH = 0

	uint8_t j;
	for (j = 0; j < 3; j++) {
		if (SPI_output == encoder_left[j]) //left mode
			SPI_output &= 0x03;                       
		else if (SPI_output == encoder_right[j]) //right mode
			SPI_output = (SPI_output >> 2) & 0x03;  	
	}

	switch (encoder_state){
		case 2: 
			if (past_encoder_state == 3) {  
				count = 1;
				count_sign = 1; 
			}//trun clockwise, the sum is incremented
			if (past_encoder_state == 0) {  
				count = 1;
				count_sign = -1; 
			}//trun counterclockwise, the sum is incremented
			past_encoder_state = encoder_state;
			if (SPI_output == 0) encoder_state = 0;
			if (SPI_output == 1) encoder_state = 1;
			if (SPI_output == 0xFF) encoder_state = 3;
			break;
		case 0:
			if (past_encoder_state == 2) {  
				count = 1;
				count_sign = 1; 
			}//CW
			if (past_encoder_state == 1) {  
				count = 1;
				count_sign = -1; 
			}//CCW
			past_encoder_state = encoder_state;
			if (SPI_output == 1) encoder_state = 1;
			if (SPI_output == 2) encoder_state = 2;
			if (SPI_output == 0xFF) encoder_state = 3;
			break;
		case 1:
			if (past_encoder_state == 0) {  
				count = 1;
				count_sign = 1; 
			}//CW
			if (past_encoder_state == 3) {  
				count = 1;
				count_sign = -1; 
			}//CCW
			past_encoder_state = encoder_state;
			if (SPI_output == 0) encoder_state = 0;
			if (SPI_output == 2) encoder_state = 2;
			if (SPI_output == 0xFF) encoder_state = 3; 
			break;
		case 3: 
			if (past_encoder_state == 1) {  
				count = 1;
				count_sign = 1; 
			}//CW
			if (past_encoder_state == 2) {  
				count = 1;
				count_sign = -1; 
			}//CCW
			past_encoder_state = encoder_state; 
			count = 0; 
			if (SPI_output == 0) encoder_state = 0;
			if (SPI_output == 1) encoder_state = 1;
			if (SPI_output == 2) encoder_state = 2;
			break;	
	}//switch		  
}


/***********************************************************************/
//                              ISR                             
// This TIMER0 vector will be run when the interrupt happens, it excuates
// the encoders function and send the data of bar graph out by using SPI
//***********************************************************************/
ISR(TIMER0_OVF_vect){                                  	
	encoders(SPDR);
	SPDR = display_count; //send to display
	
	while (bit_is_clear(SPSR, SPIF)) {}   //wait till data is sent out (while spin loop)    
 	PORTB |=   (1 << PORTB0);         //strobe output data reg in HC595 - rising edge   0x02
    	PORTB &=  ~(1 << PORTB0);          //falling edge   
}


//***********************************************************************************
// The Main function initializes PORT A B E and calculates for the result of sum, it changes 
// the value by pushing the button switch S1 and S6. Pushing S1 means to increment or
// decrement 2, linghting D1 and D2 on bar graph; Pushing S6 means to increment or 
// decrement 4, linghting D1-4
//***********************************************************************************
int main() {

int16_t sum = 0;   //the value of sum
int8_t rawkeypressed = 0;
uint8_t button;
uint8_t past_state = 0;
uint8_t mode;  // 1 -- in mode 1; 2 -- in mode 2; 4 -- in mode 4
//set PB[7:4] as output for 74HC138 decoder; 
//set PB[3:0] to turn on MISO as input, and turn on SS, MOSI, SCLK as output
DDRB = 0xF7;       
DDRE |= 0xC0;     // Turn on CLK_INH, SH/LD as output  11000000
DDRA = 0x00;     //set port A all input for 7-segment

tcnt0_init();  //initalize counter timer zero
spi_init();    //initalize SPI port
sei();         //enable interrupts before entering loop

while(1){
        _delay_ms(3);    //delay 3 ms
        PORTA = 0xFF;    //set port A to be with pullups
	PORTB = 0x70;    //set the decoder

	uint8_t i;
	//use i to check out which button is pushed  
	for (i = 6; i < 8; i++){
		//push the button
		if (chk_buttons(i)) { //if push button S6 or S7
			button = i;
			if (button == 6) {  //push S6
				if (past_state == 6) mode = 1;  //push S6 twice
				else if (past_state == 7) {
					if (mode == 0) mode = 2;
					else mode = 0; //push S6 and then push S7
				}
				else {                      // only push S6 one time
					mode = 2;
					past_state = button;			
				}
			}//if
			else if (button == 7) { //push S7
				if (past_state == 7) mode = 1;  //push S7 twice
				else if (past_state == 6) mode = 0; //push S7 and then push S6
				else {                      // only push S7 one time
					mode = 4;
					past_state = button;			
				}
			}//if
			else {  // come back to mode 1
				mode = 1;
			}
		}//if
	}//for

	switch (mode) {
		case 0: coefficient = 0; break;
		case 1: coefficient = 1; past_state = 0; display_count &= 0x00; break;
		                                           // low bytes stand for counting 1
		case 2: coefficient = 2; display_count &= 0x00; display_count |= 0x03; break;   
		                                           // low bytes stand for counting 4
		case 4: coefficient = 4; display_count &= 0x00; display_count |= 0x0F; break;   
	}

	//only showing 0-1023, sum is effected by count, sign and x2/x4
	if ((sum < 1024) && (sum >= 0)) {sum += count * count_sign * coefficient;}   
	//if the sum is larger than the max value 1024, subtract 1023 and show the remaining
	else if (sum >= 1024){sum -= 1023;}          		
	else if (sum < 0) {sum += 1024;}

	if (sum < 10) {
		a[0] = sum%10; //get the number of units
		a[1] = 10; 
		a[3] = 10;
		a[4] = 10;
	}
	else if (sum < 100) {
		a[0] = sum%10; //get the number of units 
		a[1] = sum/10%10; //get the number of tens
		a[3] = 10;
		a[4] = 10;	
	}
	else if (sum <1000) {
		a[0] = sum%10; //get the number of units
		a[1] = sum/10%10; //get the number of tens
		a[3] = sum/100%10;  //get the number of hundreds
		a[4] = 10;
	}
	else {
		a[0] = sum%10; //get the number of units 
		a[1] = sum/10%10; //get the number of tens
		a[3] = sum/100%10;  //get the number of hundreds
		a[4] = sum/1000; //get the number of thousands	
	}
	//send sum to display 7-segement
	segment_value(a); 
	//reset counter
	count = 0;  
	//use buffer to forbid from assigning value of button	
	PORTB = 0x5F;
	//set port A all output for 7-segment
	DDRA = 0xFF; 
	//reset PORTA
	PORTA = 0x00; 
	//reset button state for releasing
	rawkeypressed = 0; 
	//send value of 4 digit segments to display 
	segment_sum(SEG); 
}//while
}//main
