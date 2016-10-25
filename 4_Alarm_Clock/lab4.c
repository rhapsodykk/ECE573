// lab4.c
// Sihe Yang
// 11.15.2015

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


// ADC board             Mega128 board         
// -------------------   ------------------ 
//       CDS             PORTF bit 0 (ADC0)                     
//   GND(analogy)              GND
//   VCC(analogy)              VDD 


// Audio_amp board       Mega128 board         
// -------------------   ------------------ 
//       VIN             PORTD bit 7 (T2) 
//       VOL             PORTE bit 3 (OC3A)            


// Amplifier board       Audio_amp board         
// -------------------   ------------------ 
//       LOUT                  LIN
//       ROUT                  RIN             
//   GND(analogy)          GND(analogy)
//   VCC(analogy)          VCC(analogy) 


// Audio_amp board       Speaker
// -------------------   ------------------



#define F_CPU 16000000 // cpu speed in hertz 
#define TRUE 1
#define FALSE 0
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "kellen_music.c"         ///tcnt1 is in this file, provided by  Kellen Arb 10.18.08
#include "LCDDriver.c"

uint8_t a[4];   // reset arrary a, hold data about which digit display
uint8_t SEG[11] = {0xFF};   //reset array SEE, hold data about decimal to 7-sgement LED
uint8_t display_count = 0x00; //holds count for display on bar graph. After reseting, it shows 1
int8_t count = 1;   // the initial value for incrementing or decrementing sum
int8_t count_sign;   // the sign of counting number, its value is 1 or -1.
uint8_t coefficient;   // For increment/ decrement 2 or 4, is changed by pushing button.
uint8_t left_state; //state of left encoder, it initial state is 3
uint8_t past_left_state;  //It duplicates the value of encoder_data received by ISP
uint8_t right_state; //state of encoder, it initial state is 3(0xFF & 0x03).
uint8_t past_right_state;  //It is for duplicating the value of encoder_data received by ISP
uint8_t count_1s= 0;
int16_t sum = 0;   //the value of sum
uint8_t timer = 1;     //execuate per 500ms for tcnt0
uint8_t colon;     //decide that colon is lighted or not 
uint8_t hours = 0; 
uint8_t minutes = 0; 
uint8_t seconds = 0;
uint8_t adc_count = 0;
uint8_t mode = 0;  //switch different modes based on what button you push
uint8_t timer_s2, display_s2, display_s3;
uint8_t switch_s2 = 0;
uint8_t switch_s4 = 0;
uint8_t alarm_s = 0;
uint8_t alarm_m = 0;
uint8_t alarm_h = 0;
uint8_t ms;
uint8_t format_12;
uint8_t am_flag = 0;
uint8_t am_flag2 = 0;
uint8_t ampm = 2;    //initialize bar graph showing a.m. or p.m.
uint8_t alarm = 0;  //switch of alarm for getting started
uint8_t stop = 0;
uint8_t snooze_flag = 0;
uint16_t volume;    //change volume
uint8_t volume_sign;


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
            return 1; /////
        return 0;
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
            case 11: SEG[k] = 0x04; break;  //display colon
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
        case 2: PORTB = 0x20; PORTA = SEG[2]; _delay_ms(2); break;  //display colon
            case 3: PORTB = 0x30; PORTA = SEG[3]; _delay_ms(2); break;  //display hundreds
            case 4: PORTB = 0x40; PORTA = SEG[4]; _delay_ms(2); break;  //display thousands
            }//switch 4 digits | PORTB: digit | PORTA: value
    }//for
}//segment_sum


/***********************************************************************/
//                            spi_init                               
// This function initalizes the SPI port on the mega128. The following 
// parameters is set up: master mode, clock=clk/2, cycle half phase,                    
// low polarity, MSB first interrupts disabled, poll SPIF bit in SPSR to
// check xmit completion.
/***********************************************************************/
void spi_init(void){
    SPCR |= (1<<SPE) | (1<<MSTR);  //set up SPI mode      
    SPSR |= (1<<SPI2X);          // double speed operation   
 }//spi_init


/***********************************************************************/
//                              tcnt0_init                             
// This function initalizes timer/counter0 (TCNT0). TCNT0 is for running 
// in async mode with external 32khz crystal. If it runs in normal mode,
// there is no prescaling. Interrupt occurs at overflow 0xFF.
/***********************************************************************/
void tcnt0_init(void){
      ASSR  |= (1<<AS0);  //ext osc TOSC                        
      TIMSK |= (1<<TOIE0);  //enable timer/counter0 overflow interrupt  
      TCCR0 |= (1<<CS00);  //normal mode, no prescale  
}



/***********************************************************************/
//                              encoders  
// Switching left encoder will increment or decrement the volume playing
// on audio-amplifier board.                          
// Switching right encoder will increment or decrement the number showing
// on 7-segment LCD board.   
// To judge the data sent from 74HC165 can decide which encoder is switched.  
// Turning counterclockwise of any encoder will increments the count, Other- 
// wise it will be decremented. If the user does not push any button, this  
// count changed by one.
/***********************************************************************/
uint8_t encoders(uint8_t SPI_output){
    uint8_t temp_right, old_right;
    uint8_t temp_left, old_left;
    uint8_t encoder_mode;
    uint8_t left_encoder;
    PORTE = 0x00;                   // SH/LD = 0 CLK_INH = 0 
    PORTE |= 0x80;                  // SH/LD = 1 CLK_INH = 0

    temp_left = SPI_output & 0x03;    //back up left encoder data
    temp_right = SPI_output & 0x0C;   //back up right encoder data
    // match old_encoder_data with new_temp_encoder_data to know which encoder is switched
    // if the data are same, it means that this one is not switched
    if (temp_left == old_left) encoder_mode = 1;        //keep left one and switch right one    
    if (temp_right == old_right) encoder_mode = 0;     //keep right one and switch left one

    if(encoder_mode == 0) {  //switch left
        SPI_output &= 0x03; 
        switch (left_state){
            case 2: 
                if (past_left_state == 3) {  
                    count = 1;
                    count_sign = 1; 
                }//trun clockwise, the sum is incremented
                if (past_left_state == 0) {  
                    count = 1;
                    count_sign = -1; 
                }//trun counterclockwise, the sum is incremented
                past_left_state = left_state;
                if (SPI_output == 0) left_state = 0;
                if (SPI_output == 1) left_state = 1;
                if (SPI_output == 3) left_state = 3;
                break;
            case 0:
                if (past_left_state == 2) {  
                    count = 1;
                    count_sign = 1; 
                }//CW
                if (past_left_state == 1) {  
                    count = 1;
                    count_sign = -1; 
                }//CCW
                past_left_state = left_state;
                if (SPI_output == 1) left_state = 1;
                if (SPI_output == 2) left_state = 2;
                if (SPI_output == 3) left_state = 3;
                break;
            case 1:
                if (past_left_state == 0) {  
                    count = 1;
                    count_sign = 1; 
                }//CW
                if (past_left_state == 3) {  
                    count = 1;
                    count_sign = -1; 
                }//CCW
                past_left_state = left_state;
                if (SPI_output == 0) left_state = 0;
                if (SPI_output == 2) left_state = 2;
                if (SPI_output == 3) left_state = 3; 
                break;
            case 3: 
                if (past_left_state == 1) {  
                    count = 1;
                    count_sign = 1; 
                }//CW
                if (past_left_state == 2) {  
                    count = 1;
                    count_sign = -1; 
                }//CCW
                past_left_state = left_state; 
                //count = 0; 
                if (SPI_output == 0) left_state = 0;
                if (SPI_output == 1) left_state = 1;
                if (SPI_output == 2) left_state = 2;
                break;  
            default: left_state = 3;
        }//switch
        left_encoder &= 0xC0;
        left_encoder |= (SPI_output << 6);
        if ((left_encoder == 0xC0) & (mode == 0)) display_count |= 0x80;   
        //show that left enconder can work underlying this state of right one 
    }
    if(encoder_mode == 1) {  //switch right
        SPI_output = (SPI_output & 0x0C) >> 2; 
        switch (right_state){
            case 2: 
                if (past_right_state == 3) {  
                    count = 1;
                    count_sign = 1; 
                }//trun clockwise, the sum is incremented
                if (past_right_state == 0) {  
                    count = 1;
                    count_sign = -1; 
                }//trun counterclockwise, the sum is incremented
                past_right_state = right_state;
                if (SPI_output == 0) right_state = 0;
                if (SPI_output == 1) right_state = 1;
                if (SPI_output == 3) right_state = 3;
                break;
            case 0:
                if (past_right_state == 2) {  
                    count = 1;
                    count_sign = 1; 
                }//CW
                if (past_right_state == 1) {  
                    count = 1;
                    count_sign = -1; 
                }//CCW
                past_right_state = right_state;
                if (SPI_output == 1) right_state = 1;
                if (SPI_output == 2) right_state = 2;
                if (SPI_output == 3) right_state = 3;
                break;
            case 1:
                if (past_right_state == 0) {  
                    count = 1;
                    count_sign = 1; 
                }//CW
                if (past_right_state == 3) {  
                    count = 1;
                    count_sign = -1; 
                }//CCW
                past_right_state = right_state;
                if (SPI_output == 0) right_state = 0;
                if (SPI_output == 2) right_state = 2;
                if (SPI_output == 3) right_state = 3; 
                break;
            case 3: 
                if (past_right_state == 1) {  
                    count = 1;
                    count_sign = 1; 
                }//CW
                if (past_right_state == 2) {  
                    count = 1;
                    count_sign = -1; 
                }//CCW
                past_right_state = right_state; 
                //count = 0; 
                if (SPI_output == 0) right_state = 0;
                if (SPI_output == 1) right_state = 1;
                if (SPI_output == 2) right_state = 2;
                break;      
            default: right_state = 3;
        }//switch
    }
    old_left = SPI_output;    
    old_right = SPI_output & 0x0C;
}


/***********************************************************************/
//                              ISR                             
// This TIMER0 vector will be run when the interrupt happens, it excuates
// the encoders function and send the data of bar graph out by using SPI.
// The colon is lighted on and off at 1s intervals. The second count is 
// incremented every 1s.
// 1/32768         = 30.517578uS
//(1/32768)*256    = 7.8125ms
//(1/32768)*256*64 = 500ms
//***********************************************************************/
ISR(TIMER0_OVF_vect){  

    mode_switch(1);      //this function can switch different modes
    
    encoders(SPDR);
    SPDR = display_count; //send to display_test function
    
    while (bit_is_clear(SPSR, SPIF)) {}   //wait till data is sent out (while spin loop)    
    PORTB |=   (1 << PORTB0);         //strobe output data reg in HC595 - rising edge   0x02
        PORTB &=  ~(1 << PORTB0);          //falling edge 

    volume = count_sign * 0x0E74;  //3700
    OCR3A = 0x0F00 - volume;    //output volume
    if (OCR3A < 200) volume_sign = 0;
    else volume_sign = 1;
}


//***********************************************************************/
//                          tcnt2_init                             
// This function is to initialize in normal mode, Timer/Counter2 uses CTC 
// waveform mode with toggle Compare Output mode to dim LEDs.
// 
//***********************************************************************/
void tcnt2_init(void){      
    TCCR2 &= (0<<FOC2);                        //no forced compare 
                 //fast pwm mode           //Set OC2 on compare match
    TCCR2 |= (1<<WGM21) | (1<<WGM20) | (1<<COM21) | (1<<COM20); 
    TCCR2 |= (1<<CS20);                 //normal mode, no prescaler
    OCR2  = 0x00;             //initialize Output Compare Register 
}


//***********************************************************************/
//                           adc_init                            
// This function is to initialize the ADC. In free running mode, ADC is to
// constantly sample and update the ADC Data Register. The ADC data register
// is 15-8 digit for ADCH, and 7-6 for ADCL.
// The ADC module contains a prescaler which generates a acceptable ADC clock
// frequency from CPU. This prescaler is decided by ADPSn.
//***********************************************************************/
void adc_init(void)
{
    DDRF &= 0xFE;
    PORTF &= 0xFE;
        ADMUX |= (1<<REFS0) | (1<<ADLAR); //AVCC with external capacitor at AREF pin. 
                    //left adjusted, just read ADCH, otherwise,
                    //need to read ADCL at first, then read ADCH
            //set ADC enable, start conversion, free running mode, enable adc interrupt
        ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADFR) | (1<<ADIE) ;   
        ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // set clk/128 prescaler
}


//***********************************************************************/
//                             ADC                             
// This function set the range of ADC output from 30 to 500. Whenever TCNT2 
// equals OCR2, the comparator signals a match, and the output compare flag 
// generates an output compare interrupt for OCnA.
// 
//***********************************************************************/
ISR(ADC_vect){  
uint32_t adc_out;
    adc_out = ADCH;   // ADC data
    //adc_out -= 10;
        //if(adc_out < 30) adc_out = 30;
        //if(adc_out > 500) adc_out = 500;
    OCR2 = adc_out ;
}


//***********************************************************************/
//                          tcnt3_init                             
// This function initializes Timer/Counter3 to control the output compare
// pin OCnA behavior. In fast mode, OCnA is set on compare match, and is 
// cleared at BOTTOM. This part is for automatically changing volume of    
// alarm on Audio amplifier board.
// 
//***********************************************************************/
void tcnt3_init(void){      
    TCCR3A |= ( 1<<COM3A1 ) | ( 1<<COM3A0 ) | ( 1<<WGM31 ); // fast pwm, set on match.
    TCCR3B |= ( 1<<WGM33 ) | ( 1<<WGM32 ) | ( 1<<CS30 );    // ICR3 for TOP, use clk/1, no prescale
    TCCR3C  = 0x00;                            //no forced compare 
    ICR3 = 0x0FFF;
}



//***********************************************************************/
//                          mode_switch                            
// This function switches the LEDs on the bar graph board and set time counter
// in different modes. 
// In mode 0, 3 and 4, adding timer means that clock runs in normal.
// In mode 1, 2 and snooze, adding timer_s2 could bink some digit numbers 
// to observe accurately which number is changeable, and stopping to add timer
// means that the clock does not run.
//***********************************************************************/
void mode_switch(uint8_t mode_c){
    if (mode_c) {
            if (mode == 0) {                    //push S1
            timer++;     //increment count every 7.8125 ms
            coefficient = 0;
            display_count &= 0x70;
            display_count |= 0x01;
        }
        if ((mode == 1) || (mode == 2)) {   //push S2
            timer_s2++;
            display_count &= 0x70;
            display_count |= 0x02; 
        }
        if ((mode == 3) || (mode == 4)) {   //push S3
            timer++;
            display_count &= 0x70;
            display_count |= 0x04; 
        }
        if (snooze_flag == 1) {                    //push S7
            timer_s2++;
            snooze(timer_s2);
        }
    }
    clk_count(timer);

    switch (mode) {
        case 0: 
            a[0] = minutes%10; //get the number of units 
            a[1] = minutes/10%10; //get the number of tens
            a[3] = hours%10;  //get the number of hundreds
            a[4] = hours/10%10; //get the number of thousands
            break;
        case 1:                                //blink lower 2-bit numbers
            if ((timer_s2 % 64) == 0) {    //change mode,  set seconds
                display_s2 = !display_s2;
                a[0] = 10;          //switch off 
                a[1] = 10;          
                if (display_s2 == 0) {  
                    a[0] = minutes%10; 
                    a[1] = minutes/10%10;  
                }
            }   
            a[3] = hours%10; 
            a[4] = hours/10%10;
            break;  
        case 2:                                //blink higher 2-bit numbers
            if ((timer_s2 % 64) == 0) {    //change mode,  set minutes
                display_s2 = !display_s2;
                a[3] = 10;          //switch off 
                a[4] = 10;          
                if (display_s2 == 0) {  
                    a[3] = hours%10; 
                    a[4] = hours/10%10;  
                }
            } 
            a[0] = minutes%10; 
            a[1] = minutes/10%10;   
            break;  
        case 4: 
            if ((timer % 64) == 0) {    //change mode,  set alarm hours 
                display_s3 = !display_s3;
                a[0] = 10;          //switch off 
                a[1] = 10;          
                if (display_s3 == 0) {  
                    a[0] = alarm_m%10; 
                    a[1] = alarm_m/10%10;  
                }
            }   
            a[3] = alarm_h%10; 
            a[4] = alarm_h/10%10;
            break;  
        case 3: 
            if ((timer % 64) == 0) {    //change mode,  set alarm minutes
                display_s3 = !display_s3;
                a[3] = 10;          //switch off 
                a[4] = 10;          
                if (display_s3 == 0) {  
                    a[3] = alarm_h%10; 
                    a[4] = alarm_h/10%10;  
                }
            } 
            a[0] = alarm_m%10; 
            a[1] = alarm_m/10%10;   
            break; 
    }
}


//***********************************************************************/
//                          clk_count                             
// The clk_count fucntion sets switch mechanismbetween 24hr format and 12hr 
// format. Some sepcial situations should be considered by the following
// examples.
// The special switching format from 12hr to 24hr:
//    am_flag = 0  ---- the clock is changed from 12a.m. to 0:00 a.m. midnight
// The special switching format from 24hr to 12hr:
//    am_flag = 1  ---- the clock is changed from 00:00 to 12:00 a.m.
// The special running clock for 12hr format:
//    am_flag2 = 0 ---- the clock is changed from 11:59 a.m. to 12:00 p.m.
//    am_flag2 = 1 ---- the clock is changed from 12:59 p.m. to 1:00 p.m.
//
// The clk_count fucntion also sets the mechanism of displaying the range of 
// mins (00~60) and hours (00~24) and setting alarm time (1 min).
//***********************************************************************/
void clk_count(uint8_t timer1) {
    if ((timer1 % 128) == 0) { //execuate every 1s
        colon = !colon;
        seconds += 1;   
        if (colon == 0) a[2] = 10;    //light off colon
        else a[2] = 11;               //light on colon
    }

    
    if (coefficient == 0) {
        if (seconds == 60)
         {
            minutes += 1;
            seconds = 0;
            if (minutes == 60) {
                hours++;
                minutes = 0;
                if (hours == 24) hours = 0;
            }
        }

        alarm_mode();    //compare clock time with alarm time 

        if (switch_s4 == 1) {                    //24hr--->12hr format  
            if (hours >= 12) {              // 12:00/12:00 p.m. ~ 23:59/1:59 p.m.
                if (hours > 12) {
                    if (hours == 24) {   //00:00 
                        hours = 0;
                        ampm = 0;    //A.M.
                    }
                    else {
                        hours -= 12; // 12p.m. = noon, it isn't subtracted by 12
                        ampm = 1;    //P.M.
                    }
                }
                else if (am_flag != 1) { //restrain 00:00 --> 12:00 a.m. --> 12:00p.m.
                    hours = 12;     
                    ampm = 1;            //P.M.
                }
            }
            else if (hours == 0) {     //00:00 --> 12:00 a.m.
                hours += 12;
                am_flag = 1;
                ampm = 0;          //A.M.
            }
            else if ((hours > 0) && (hours <= 11) && ((display_count & 0x10) != 0x10)) ampm = 0;   //A.M.           
        format_12 = 1;   
        }
        if (switch_s4 == 0) {                    //12hr --> 24hr format 
            if (format_12 == 1) {
                if (hours == 12) {       //12:00 a.m. --> 00:00
                    if (am_flag == 1) {
                        if (hours == 12) hours = 0;    //12a.m. --> 0 a.m. midnight
                        am_flag = 0;
                    }
                }
                else if ((hours != 12) && ((display_count & 0x10) == 0x10))  hours += 12;
                format_12 = 0;
                ampm = 2;           //P.M.
            }
        }
    }

    if ((coefficient == 1) && (mode == 1)) {   // set minutes
        if ((minutes >= 0) && (minutes < 60)) minutes += count * count_sign * coefficient;  
        else if (minutes == 60) minutes -= 60;
        else minutes -= 196;
    }
    if ((coefficient == 1) && (mode == 2)) {   // set hours
        if (format_12 == 1) {              // 12hr format mode
            if ((display_count & 0x20) == 0x20) {  //at A.M.
                if ((hours >= 1) && (hours <= 12)) {
                    if (hours == 12) {
                        if (am_flag2 == 0) ampm = 1;           //---> 12 P.M.
                        else {
                            hours += count * count_sign * coefficient; 
                            if (hours == 13) hours = 1;    // 12 a.m. -(+1)-> 1 a.m.
                            else if (hours == 11) ampm = 1;//12 a.m. -(-1)-> 11 p.m.
                        }
                    }
                    else {    //1<= hours <= 11
                        hours += count * count_sign * coefficient; 
                        if (hours == 12) am_flag2 = 0; // 11 a.m. -(+1)-> 12 p.m.
                        else am_flag2 = 1;  // A.M.
                    }
                }
                else if (hours == 0) hours = 12;    // 1 a.m.-(-1)-> 12 a.m.; 1p.m. -(-1)-> 12p.m.
            }
            else if  ((hours >= 1) && (hours <= 12)) {   // at P.M.
                if (hours == 12) {
                    if (am_flag2 == 1) ampm = 0;  //---> 12 A.M.
                    else {
                        hours += count * count_sign * coefficient; 
                        if (hours == 13) hours = 1;  // 12 p.m. -(+1)-> 1 p.m.
                        else if (hours == 11) ampm = 0;    // 12 p.m. -(-1)-> 11 a.m.
                    }
                }
                else {
                    hours += count * count_sign * coefficient; 
                    if (hours == 12) am_flag2 = 1;     // 12 p.m. -(-1)-> 1 p.m.
                    else am_flag2 = 0;
                }
            }
            else if (hours == 0) hours = 12;   // 1 p.m. -(-1)-> 12 p.m.
        }
        else if ((hours >= 0) && (hours < 24)) hours += count * count_sign * coefficient;  
        else if (hours == 24) hours -= 24;
        else hours = 23;  
    }
    if ((coefficient == 1) && (mode == 3)) {   // set alarm minutes
        if ((alarm_m >= 0) && (alarm_m < 60)) alarm_m +=  count * count_sign * coefficient;  
        else if (alarm_m == 60) alarm_m -= 60;
        else alarm_m -= 196;
    }
    if ((coefficient == 1) && (mode == 4)) {   // set alarm hours
        if (format_12 == 1) {              // 12hr format mode
            if ((display_count & 0x20) == 0x20) {  //at A.M.
                if ((alarm_h >= 1) && (alarm_h <= 12)) {
                    if (alarm_h == 12) {
                        if (am_flag2 == 0) ampm = 1;           //---> 12 P.M.
                        else {
                            alarm_h += count * count_sign * coefficient; 
                            if (alarm_h == 13) alarm_h = 1;    // 12 a.m. -(+1)-> 1 a.m.
                            else if (alarm_h == 11) ampm = 1;//12 a.m. -(-1)-> 11 p.m.
                        }
                    }
                    else {    //1<= alarm_h <= 11
                        alarm_h += count * count_sign * coefficient; 
                        if (alarm_h == 12) am_flag2 = 0; // 11 a.m. -(+1)-> 12 p.m.
                        else am_flag2 = 1;  // A.M.
                    }
                }
                else if (alarm_h == 0) alarm_h = 12;    // 1 a.m.-(-1)-> 12 a.m.; 1p.m. -(-1)-> 12p.m.
            }
            else if  ((alarm_h >= 1) && (alarm_h <= 12)) {   // at P.M.
                if (alarm_h == 12) {
                    if (am_flag2 == 1) ampm = 0;  //---> 12 A.M.
                    else {
                        alarm_h += count * count_sign * coefficient; 
                        if (alarm_h == 13) alarm_h = 1;  // 12 p.m. -(+1)-> 1 p.m.
                        else if (alarm_h == 11) ampm = 0;    // 12 p.m. -(-1)-> 11 a.m.
                    }
                }
                else {
                    alarm_h += count * count_sign * coefficient; 
                    if (alarm_h == 12) am_flag2 = 1;     // 12 p.m. -(-1)-> 1 p.m.
                    else am_flag2 = 0;
                }
            }
            else if (alarm_h == 0) alarm_h = 12;   // 1 p.m. -(-1)-> 12 p.m.
        }
        else if ((alarm_h >= 0) && (alarm_h < 24)) alarm_h += count * count_sign * coefficient;  
        else if (alarm_h == 24) alarm_h -= 24;
        else alarm_h = 23;  
    }

    switch (ampm) {    //bar graph showing a.m. or p.m.
        case 0: display_count &= 0x8F; display_count |= 0x20; break;          // A.M.
        case 1: display_count &= 0x8F; display_count |= 0x10; break;          // P.M.
        case 2: display_count &= 0x8F; break;                                 // nothing
    }
}


//***********************************************************************************
//                                    alarm_mode 
// This function sets to run alarm mode in 24hr format alarm and 12hr format. This 
// section includes 5 parts, normal 24hr foramt / 1:00 p.m. ~ 11:59 p.m./ 
// 12 a.m. = 00:00/  12 p.m. = 12:00 / 1:00 a.m. ~ 11:59 a.m. 
// Beside alarm, it also runs alarm once again after 10 sec in snooze function.
//***********************************************************************************
void alarm_mode() {
    //24hr format clock and (24hr format alarm / 12hr format alarm at p.m.)
    if ((am_flag == 0) && (am_flag2 == 0)) { 
        if ((display_count & 0x10) != 0x10) {    //24hr foramt
            if ((minutes == alarm_m) && (hours == (alarm_h))) {
                if (stop == 0)  alarm_run();  // run alarm
                else alarm_stop();  // stop alarm after running 1 min
            }
            else alarm_stop();  // stop alarm after running 1 min
        }
        else if ((display_count & 0x10) == 0x10) {     //1:00 p.m. ~ 11:59 p.m.
            if ((minutes == alarm_m) && (hours == (alarm_h - 12))) 
                alarm_run();  // run alarm
            else alarm_stop();  // stop alarm after running 1 min
        }               
    }
    else if ((am_flag == 1) && (am_flag2 == 0)) { //12hr format clock and 24hr format alarm
        if ((hours == 12) && ((display_count & 0x20) == 0x20)) {     // at 12 a.m. = 00:00
            if ((minutes == alarm_m) && (hours == (alarm_h + 12))) alarm_run();  
            else alarm_stop();  // stop alarm after running 1 min
        }
        else if ((display_count & 0x20) == 0x20) {   // at 1:00 a.m. ~ 11:59 a.m.
            if ((minutes == alarm_m) && (hours == (alarm_h + 12))) alarm_run(); 
            else alarm_stop();  
        }
        else if ((hours == 12) && ((display_count & 0x10) == 0x10)) {// at 12 p.m. = 12:00
            if ((minutes == alarm_m) && (hours == alarm_h)) alarm_run();   
            else alarm_stop();  // stop alarm after running 1 min
        }
    }
    else if (snooze_flag == 1) {
        song++;
        alarm_run();
    }
}

void alarm_run() {
    if (alarm == 0) {
        music_on();  // start up alarm 
        alarm = 1;
        stop = 0;   //reset stop mode
    }
    else {  // run alarm 
        ms++;
        if(ms % 8 == 0) beat++;   //play notes (64th notes) 
    }
}

void alarm_stop() {
    music_off(); 
    alarm = 0;        // reset alarm
}

void snooze(uint8_t timer_s7) {
    //music off automatically and users don't stop alarm by themselves
    if ((timer_s7 % 128) == 0) {    //change mode,  set seconds 
        alarm_s += 1;
        if (alarm_s > 10) {    //wait 10s for alarming again
            if (alarm_s == 11) {
                alarm_run();
                LCD_Init();                     //initalize LCD
                LCD_PutStr("Alarm !!!!!!!!!");
                LCD_MovCursorLn2();
                LCD_PutStr("Snooze / Stop");
            }
            else alarm_run();
        }
    }
}


//***********************************************************************************
//  MAin function is for choosing different mode of alarm clcok. Pushing button 2 runs
//  normal mode, it is clock. Pushing button 3 sets clock time. Pushing button 4 sets 
//  alarm mode. Pushing button 5 switches 12hr or 24hr format. Pushing button 6 stops 
//  alarm. Pushing button 7 runs snooze function.
//***********************************************************************************
int main() {

uint8_t button;
uint8_t past_state = 0;

//set PB[7:4] as output for 74HC138 decoder; 
//set PB[3:0] to turn on MISO as input, and turn on SS, MOSI, SCLK as output
DDRB = 0xF7;       
DDRE |= 0xFF;     // Turn on CLK_INH, SH/LD as output(PE7,PE6) and OCR3A (PE3)
DDRA = 0x00;     //set port A all input for 7-segment

music_init();
spi_init();    //initalize SPI port
adc_init();    //initalize ADC port

LCD_Init();     //initalize LCD
LCD_PutStr("Alarm Clock");
LCD_MovCursorLn2();
if (volume_sign == 0) LCD_PutStr("Mute mode");
if (volume_sign == 1) LCD_PutStr("Speaking");

tcnt0_init();  //initalize counter timer0 zero
tcnt2_init();    //initalize counter timer2 zero
tcnt3_init();    //initalize counter timer3 zero
sei();         //enable interrupts before entering loop

while(1){
        _delay_ms(3);    //delay 3 ms
        PORTA = 0xFF;    //set port A to be with pullups
    PORTB = 0x70;    //set the decoder
    
    if (chk_buttons(1)) {
        mode = 0;       //normal mode
        LCD_Init();     //initalize LCD
        LCD_PutStr("Alarm Clock");
        LCD_MovCursorLn2();
        if (volume_sign == 0) LCD_PutStr("Mute mode");
        if (volume_sign == 1) LCD_PutStr("Speaking");
    }
    if (chk_buttons(2)) {           //change time mode
        alarm_stop();
        timer = 1;                      //stop clock
        coefficient = 1;        //+ or - 1
        switch_s2 = !switch_s2;         
        mode = 1;                       // set minutes
        if (switch_s2)  mode = 2;       // set hours
        LCD_Init();                     //initalize LCD
        LCD_PutStr("Set time mode");
    }
    if (chk_buttons(3)) {           //change alarm mode
        alarm_stop();
        stop = 0;                       //reset alarm 
        coefficient = 1;        //+ or - 1
        switch_s2 = !switch_s2;         
        mode = 3;                       // set alarm minutes
        if (switch_s2)  mode = 4;       // set alarm hours
        LCD_Init();                     //initalize LCD
        LCD_PutStr("Set alarm mode");
    }
    if (chk_buttons(4)) {
        switch_s4 = !switch_s4; // 12/24hr switching mode
        LCD_Init();                     //initalize LCD
        LCD_PutStr("12/24 hour ");
        LCD_MovCursorLn2();
        LCD_PutStr("format change");
    }
    if (chk_buttons(5)) {                   // stop alram mode 
        alarm_stop();     
        stop = 1;
        snooze_flag = 0;                //reset snooze
        alarm_s = 0;                    //reset snooze count 
        LCD_Init();                     //initalize LCD
        LCD_PutStr("Stop alarm");
    }   
    if  (chk_buttons(6)){                   //snooze function
        alarm_stop();     
        stop = 1;
        snooze_flag = 1;
        LCD_Init();                     //initalize LCD
        LCD_PutStr("Snooze 10s");
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
    //send value of 4 digit segments to display 
    segment_sum(SEG); 
}//while
}//main
