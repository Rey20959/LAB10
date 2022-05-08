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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*
 * --------------------------- Librerias --------------------------
 */

#include <xc.h>
#include <stdint.h>

/*
 * ---------------------------- Constantes -----------------------------------
 */
#define _XTAL_FREQ 4000000
#define LEN_MSG_2 3             // Constante para definir largo de mensaje e iteraciones al enviarlo por el serial
#define LEN_MSG 5 

/*
 * ----------------------------- Variables ----------------------------------
 */
char variable;    //Casos
uint8_t num[3];
char num1;         //Numeros de suma 
char num2;
char num3;
char ADC;  
char valor[LEN_MSG_2] = {' ', 0x0D, 0x0A};// Variable que guarda el valor de potenciometro en Ascii
uint8_t indice = 0;  
char valor_2[LEN_MSG] = {' ', ' ', ' ', 0x0D, 0x0A}; 
/*
 * ------------------------------- Tabla ------------------------------------
 */

/*
 * -------------------------- Prototipos de funciones -----------------------
 */
void setup (void);
void TX_usart(char data);
char RX_resultado();
void cadena(char *str);
void obtener_no(void);  

/*
 * ------------------------------ Interrupción -------------------------------
 */
void __interrupt() isr (void){
    if(PIR1bits.ADIF){                                   // Verificar interrupcion de ADC
        if(ADCON0bits.CHS == 0){                         // Cambio de canal al AN0
            ADC = ADRESH; }                              // ADC valor de ADRESH
        PIR1bits.ADIF = 0;                               // Limpiar la interrucpción del ADC
    }
    return;
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
        cadena("\r Que opción desea elegir? \r"); //Texto
        cadena("1. Cadena de caracteres \r");
        cadena("2. Cambiar PORTB  \r");
        cadena("3. POT\r");
        if(ADCON0bits.GO == 0){ADCON0bits.GO = 1;}       //Mantenerse en el mismo canal siempre   

        while(PIR1bits.RCIF == 0);//Cuando la bandera este en 0 deja pasar datos
        variable = RX_resultado();
        
        switch(variable)
        {
            case('1'): //Caso 1
                cadena("HOLA, soy yo su estudiante favorito \r");
                break;

            case ('2')://Caso 2
                cadena("El numero a ingresar tiene que ser de 3 digitos" );
                cadena(" ejemplo: 001 \r");
                cadena("Ingresa un valor para el PORTB : ");
                PORTB = 0;
                while (PIR1bits.RCIF == 0);//Deja pasar datos
                num1 = RCREG -48;//Se le resta 0
                while (PIR1bits.RCIF == 0);//Deja pasar datos
                num2 = RCREG -48;//Se le resta 0
                while (PIR1bits.RCIF == 0);//Deja pasar datos
                num3 = RCREG -48;//Se le resta 0
                PORTB = num1;//Enseñar primer digito 
                __delay_ms(1000);
                PORTB = num2;//Enseñar segundo digito 
                __delay_ms(1000);
                PORTB = num3;//Enseñar tercer digito 
                __delay_ms(1000);
                PORTB = num1*100+ num2*10+num3; //Suma y que aparezca en PORTB
                break;

            case('3')://Caso 3
                valor[0]= ADRESH;                            // Guardar el valor de ADC en valor
                num[0] = (valor[0]/100);                             // Obtener centenas
                num[1] = ((valor[0] - num[0]*100)/10);               // Obtener decenas
                num[2] = (valor[0] - num[0]*100 - num[1]*10);        // Obtener unidades

                valor_2[0] = num[0] + 48;                            // Conversion a Ascii centenas
                valor_2[1] = num[1] + 48;                            // Conversion a Ascii decenas
                valor_2[2] = num[2] + 48;                            // Conversion a Ascii unidades
    
                cadena("El valor del potenciometro es:\r"); //Escribir la cadena a mostrar
                indice = 0;                                  // Limpiar indice para empezar en 0
                while(indice<LEN_MSG){                       // Loop para imprimir el mensaje completo
                if (PIR1bits.TXIF){                      // Esperamos a que esté libre el TXREG para poder enviar por el serial
                    TXREG = valor_2[indice];             // Cargamos caracter a enviar
                    indice++;                            // Incrementamos indice para enviar siguiente caracter
            
                break;

        }

    }
        return;
}
        }
    }
/*
 * -------------------------------- Funciones --------------------------------
 */
void setup (void) 
{
    //Configuración de puertos
    ANSEL = 0b00000001;         // AN0 como entrada analogica
    ANSELH = 0;                 // I/O digitales
    
    TRISB = 0;
    PORTB = 0;
    
    TRISA = 0;
    PORTA = 0;
    
    //9600 bauds
    //Configuración del oscilador a 8MHz
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    
    //Configuración de TX y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16 = 1;
    
    SPBRG = 207;                 //SPBRGH : SPBRG = 207
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;          //Modo 8 bits
    RCSTAbits.CREN = 1;         //Habilitmaos la recpeción
    TXSTAbits.TXEN = 1;         //Habilitamos la transmisión
    //TXSTAbits.TX9 = 0;
}

   
    void TX_usart(char data)
    {
        while(TXSTAbits.TRMT == 0);
        TXREG = data;//Igualar el reg a data 
    }

    char RX_resultado()
    {
        return RCREG;
    }

    void cadena (char *str){//Para las diferentes opciones 
        while(*str != '\0'){
            TX_usart(*str);
            str++;
        }
    }
