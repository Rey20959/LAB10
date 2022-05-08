/* 
 * File:   prelab.c
 * Author: juane
 *
 * Created on 1 de mayo de 2022, 03:11 PM
 */




// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

/*
 * --------------------------- Configracion de bits --------------------------
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillador Interno sin salidas, XT
#pragma config WDTE = OFF       // WDT disabled (reinicio repetitivo del pic)
#pragma config PWRTE = OFF     // PWRT enabled  (espera de 72ms al iniciar)
#pragma config MCLRE = OFF      // El pin de MCLR se utiliza como I/O
#pragma config CP = OFF         // Sin protección de código
#pragma config CPD = OFF        // Sin protección de datos
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit 
#pragma config LVP = OFF         // Low Voltage Programming Enable bit 

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit 
#pragma config WRT = OFF        // Flash Program Memory Self Write 

#include <xc.h>
#include <stdint.h>

/*
 * ---------------------------- Constantes -----------------------------------
 */
#define _XTAL_FREQ 1000000

/*
 * ----------------------------- Variables ----------------------------------
 */
const char data = 49;

/*
 * -------------------------- Prototipos de funciones -----------------------
 */
void setup (void);
/*
 * ------------------------------ Interrupción -------------------------------
 */
void __interrupt () isr (void)
{
    if(PIR1bits.RCIF)
    {
        PORTB = RCREG;
    }
}
/*
 * ----------------------------- MAIN CONFIGURACION --------------------------- 
 */
void main (void)
{
    setup ();
    
    //---------------------- Loop principal ------------------
    while (1)
    {
        __delay_ms(500);
        
        if(PIR1bits.TXIF)
        {
            TXREG = data;
        }
    }
}
/*
 * -------------------------------- Funciones --------------------------------
 */
void setup (void) 
{
    //Configuración de puertos
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    PORTB = 0;
    
    //9600 bauds
    //Configuración del oscilador a 1MHz
    OSCCONbits.IRCF = 0b100;
    OSCCONbits.SCS = 1;
    
    //Configuración de TX y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    
    BAUDCTLbits.BRG16 = 1;
    
    SPBRG = 25;                 //SPBRGH : SPBRG = 25
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;          //Modo 8 bits
    RCSTAbits.CREN = 1;         //Habilitmaos la recpeción
    
    TXSTAbits.TXEN = 1;         //Habilitamos la transmisión
    TXSTAbits.TX9 = 0;
    
    //Configuración de interrupciones
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}