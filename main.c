// Microcontrollers Laboratory
// Practice 10
// Esteban Lavín Treviño

// Libraries and Headers
#include "Config_header.h" // Include the config header file

// Directives
#define _XTAL_FREQ 10000000 // frequency


// Constants

        
        
// Function Prototypes
void Ports_configuration(void);                     // Ports configuration.
void dutyCycle(void);                               // duty cycle
void ADC_Init (void);                               // ADC initialization
void TMR2_Init (void);                              // timer 2 initialization
void PWM_Init(void);                                // PWM initialization
void __interrupt (high_priority) high_isr(void);    // High priority Interrupt Service Request.
void __interrupt (low_priority) low_isr(void);      // Low priority Interrupt Service Request.

int valor_adc;          // stores ADC result
float pw;               // pulse width
int dc = 0;             // duty cycle

// Main Function
void main(void){
    Ports_configuration();          // ports configuration function
    ADC_Init();                     // initialize ADC
    TMR2_Init();                    // initialize Timer 2
    PWM_Init();                     // PWM Initalization
    
    while (1) {                     // infinite loop
        ADCON0bits.DONE = 1;        // initialize conversion
        while(ADCON0bits.DONE==1);  // waits until A/D conversion is done
        valor_adc = ADRESH;         // assign new duty cycle
        CCPR1L = valor_adc;         // assign new duty cycle to PWM
        dutyCycle();                // calculate duty cycle and turn on/off LED
    }              
}     
 

// Function Definition
void Ports_configuration(void){//            Ports configuration.
    // PORTA
    TRISAbits.RA0 = 1;      // Potentiometer input
    ANSELAbits.ANSA0 = 1;   // analog
    TRISAbits.RA5 = 0;      // LED output
    ANSELAbits.ANSA5 = 0;   // Digital
    TRISCbits.RC2 = 0;      // output
    ANSELCbits.ANSC2 = 0;   // Digital
    
    // Oscillator Configuration
    OSCCON = 0b01010010; // 4 MHz
    
}

void dutyCycle (void){
    pw = (CCPR1L<<2)|(CCP1CON>>4); // Pulse width
    dc = ((pw)/(4*(PR2+1)))*100;   // duty cycle calculation

    if(dc > 75){            // if grater than 75%
        PORTAbits.RA5 = 1;  // turn on LED
    }
    else{
        PORTAbits.RA5 = 0;  // turn of LED
    }
}

void ADC_Init (void) {
    ADCON0 = 0b00000001;    // Select channel AN0; turn on ADC
    ADCON1 = 0b00000000;    // Vref+ (Vcc), Vref- (GND)
    ADCON2 = 0b00111110;    // right justified, 20TAD, TAD = Fosc/64
}

void TMR2_Init (void){
    RCONbits.IPEN = 1;      // enable priority
    PIE1bits.TMR2IE = 1;    // enable interrupt tmr2
    IPR1bits.TMR2IP = 1;    // interrupt high priority
    PIR1bits.TMR2IF = 0;    // clear flag
    INTCONbits.GIEH = 1;    // global priority
    INTCONbits.GIEL = 0;    // no priority assigned yet
    PR2 = 249;              // PR2 starting value
    T2CON = 0b00011111;     // 1:4 post-scaler, TMR2 ON, 1:16 pre-scaler
}

void PWM_Init(void) { 
    CCP1CON = 0b00001100;           // PWM 
    CCPTMRS = 0x00;                 // use timer 2
    CCPR1L = 0;                     // start at 0
}

void __interrupt (high_priority) high_isr(void){//  High priority Interrupt Service Request.
   PIR1bits.TMR2IF = 0;         // clear flag
}
void __interrupt (low_priority)  low_isr(void){//   Low priority Interrupt Service Request.
    Nop();
}
