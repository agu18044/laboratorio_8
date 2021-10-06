#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>


//Variables utilizadas
char cambio;    // variable de cambio canal
char valor;     // variable para division 7seg
int  multi;     // variable del multiplexado de displays
char centena;   // variable para display centena
char decena;    // variable para display decena
char unidad;    // variable para display unidad y se usa como residuo del resto

char display[10] = {0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,
0b01101101,0b01111101,0b00000111,0b01111111,0b01101111}; 

#define _XTAL_FREQ 4000000
void setup(void);
char division(void);

void __interrupt() isr(void) { 
    if (T0IF == 1) {   
        PORTAbits.RA2 = 0; 
        PORTAbits.RA0 = 1;  
        PORTC = (display[centena]); 
        multi = 0b00000001;                       
        
        if (multi == 0b00000001) {                         
            PORTAbits.RA0 = 0;  
            PORTAbits.RA1 = 1;  
            PORTC = (display[decena]);  
            multi = 0b00000010; 
        }        
        if (multi == 0b00000010) {                             
            PORTAbits.RA1 = 0;  
            PORTAbits.RA2 = 1;  
            PORTC = (display[unidad]);  
            multi = 0x00;
        }
        INTCONbits.T0IF = 0;  
        TMR0 = 255;  //valor inicial del tmr0}
    
    if (PIR1bits.ADIF == 1){
        if (ADCON0bits.CHS == 5)
            PORTD = ADRESH;
        else 
            valor = ADRESH;
        PIR1bits.ADIF = 0;     
    }
  }
}

void main (void){
    setup();
    ADCON0bits.GO = 1;
    while (1){
        if (ADCON0bits.GO == 0){
            if (ADCON0bits.CHS == 6)
                ADCON0bits.CHS = 5;
            else 
                ADCON0bits.CHS = 6;
            __delay_us(100);
            ADCON0bits.GO = 1;
        }
        
        division();
    }
}

void setup(void){
    ANSEL = 0b01100000;
    ANSELH = 0;
    
    TRISA = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0b011;
    
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    OSCCONbits.IRCF = 0b0110; //4Mhz
    OSCCONbits.SCS = 1;
    
    //CONFIG ADC
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS = 0b01;
    ADCON0bits.CHS = 5;
    ADCON0bits.ADON = 1;
    __delay_us(50);
    
    //CONFIG TIMER0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 1;  // Prescaler 111   1:256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    
    //CONFIG INT
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;        
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;    //interrupciones del TMR0
    INTCONbits.T0IF = 0;
    return;       
}

char division(void){ 
    centena = valor/100; //centena = contador dividio 100
    unidad = valor%100;  //variable de unidad es utilizado como residuo
    decena = unidad/10; //decena = residuo(unidad) divido 10
    unidad = unidad%10; //se coloca el residuo en variable unidad para mostrar
}