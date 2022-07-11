/*
 *
 * M3 João Vitor Specht Kogut e Nicole Migliorini Magagnin
 * 
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// diretiva de configuração: pino reset externo, watchdog desabilitado, oscilador High Speed
#pragma config MCLRE = ON, WDT = OFF, OSC = HS, BOREN = OFF


#define _XTAL_FREQ 16000000
#define VAL_TIMER_H 207
#define VAL_TIMER_L 44
#define PRESCALE  128

int valorAMos;
int valor_display = 0;
int indice = 0;
int medindo = 0;
unsigned long int tempo_echo = 0;
int dig_val = 0;
int valor_tara = 0;
short flag_MEDIR = 0;
short flag_TARA = 0;
short flag_MENSURAR = 0;
int valX,valY,valZ;

void UART_Init(const uint24_t baud_rate)
{
  int24_t n = ( _XTAL_FREQ / (16 * baud_rate) ) - 1;
  
  if (n < 0)
    n = 0;
 
  if (n > 255)  // low speed
  {
    n = ( _XTAL_FREQ / (64 * baud_rate) ) - 1;
    if (n > 255)
      n = 255;
    SPBRG = n;
    TXSTA = 0x20;  // transmit enabled, low speed mode
  }
 
  else   // high speed
  {
    SPBRG = n;
    TXSTA = 0x24;  // transmit enabled, high speed mode
  }
 
  RCSTA = 0x90;  // serial port enabled, continues receive enabled
 
}
 
__bit UART_Data_Ready()
{
  return RCIF;  // return RCIF bit (register PIR1, bit 5)
}
 
uint24_t UART_GetC()
{
  while (RCIF == 0) ;  // wait for data receive
  if (OERR)  // if there is overrun error
  {  // clear overrun error bit
    CREN = 0;
    CREN = 1;
  }
  return RCREG;        // read from EUSART receive data register
}
 
void UART_PutC(const char data)
{
  while (TRMT == 0);  // wait for transmit shift register to be empty
  TXREG = data;       // update EUSART transmit data register
}
 
void UART_Print(const char *data)
{
  uint24_t i = 0;
  while (data[i] != '\0')
    UART_PutC (data[i++]);
}

void valor_equivalente(int dig, int val){
    unsigned char correspondencia;
    switch (val) {
        case 0:
            correspondencia = 0b00111111;
        break;
        
        case 1:
            correspondencia = 0b00000110;
        break;
        
        case 2:
            correspondencia = 0b01011011;
        break;
        
        case 3:
            correspondencia = 0b01001111;
        break;
        
        case 4:
            correspondencia = 0b1100110;
        break;
        
        case 5:
            correspondencia = 0b01101101;
        break;
        
        case 6:
            correspondencia = 0b01111101;
        break;
        
        case 7:
            correspondencia = 0b00000111;
        break;
        
        case 8:
            correspondencia = 0b01111111;
        break;
        
        case 9:
            correspondencia = 0b01100111;
        break;
        
        default:
            correspondencia = 0b00111111;
        }
    switch(dig){ ///Ordem dos bits ligados
        case 0:
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 0;
            PORTBbits.RB6 = 0;
            PORTBbits.RB7 = 1;
            PORTD = correspondencia;
            __delay_ms(1);
            break;
    
        case 1:
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 0;
            PORTBbits.RB6 = 1;
            PORTBbits.RB7 = 0;
            PORTD = correspondencia;
            __delay_ms(1);
            break;
        
        case 2:
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 1;
            PORTBbits.RB6 = 0;
            PORTBbits.RB7 = 0;
            PORTD = correspondencia;
            __delay_ms(1);
            break;
        case 3:
            PORTBbits.RB4 = 1;
            PORTBbits.RB5 = 0;
            PORTBbits.RB6 = 0;
            PORTBbits.RB7 = 0;
            PORTD = correspondencia;
            __delay_ms(1);
            break;
        default:
            break;
    }
    return;
}


void main(void) {
        
    TRISD = 0x00;                                   
    TRISB = 0b00000001;
    TRISC = 0b00000111;
    ADCON1 = 0x0F;                                  

    
    // timer0 display
    T0CONbits.T08BIT = 0;                        
    T0CONbits.T0CS = 0;                        
    T0CONbits.PSA = 0;                        
    T0CONbits.T0PS = PRESCALE;                
    T0CONbits.TMR0ON = 1;                         
    TMR0H = VAL_TIMER_H;                              
    TMR0L = VAL_TIMER_L;     
    
    // timer 1 echo
    T1CONbits.RD16 = 1;
    T1CONbits.T1OSCEN = 1;
    T1CONbits.TMR1CS = 0;
    T1CON = 0x10;
    
    // interrupção botão
    INTCONbits.TMR0IF = 0;                        
    INTCONbits.TMR0IE = 1;                                               
    INTCON2bits.INTEDG1 = 0;
    INTCON3bits.INT1IF = 0;                      
    INTCON3bits.INT1IE = 1;                                          
    
    // Interrupção do echo
    INTCONbits.INT0IF = 0; // Apaga flag indicador de interrupção
    INTCONbits.INT0IE = 1; // Habilita int externa 0
    INTCONbits.GIE = 1; // Habilita interrupções globais
    INTCON2bits.INTEDG0 = 1;
    RCONbits.IPEN=0;
    
    // dados para o display
    PORTBbits.RB2 = 0;
    PORTBbits.RB5 = 0;
    PORTBbits.RB6 = 0;
    PORTBbits.RB7 = 0;
    PORTD = 0b00111111;

    UART_Init(9600);

    
    while(1){
        
        if(PORTCbits.RC0 == 1 && flag_MENSURAR == 0){
            __delay_ms(20);
            valor_display = 555;
            char str[8];
            sprintf(str, "%d", valor_display);
            strcat(str, "\r\n");
            UART_Print(str);
            flag_MENSURAR = 1;
        }
        
        if(PORTCbits.RC2 == 1 && flag_MEDIR == 0){
            __delay_ms(20);
            flag_MEDIR = 1;
            PORTBbits.RB2 = 1;
            __delay_us(10);
            PORTBbits.RB2 = 0;
            
        }else if(PORTCbits.RC2 == 0){
            flag_MEDIR = 0;
        }
        
        if(PORTCbits.RC1 == 1 && flag_TARA == 0){
            __delay_ms(20);
            flag_TARA = 1;
            valor_display = 666;

        }
        
        
    }
}

void __interrupt() isr(void){
    
    if(INTCONbits.INT0IF){ // trigger RB0
        INTCONbits.INT0IE = 0;  // Desabilita novas interrupções na porta B
        if(medindo == 0 && PORTBbits.RB0 == 1){
            TMR1H = 0;  //Setting Initial Value of Timer
            TMR1L = 0;   
            TMR1ON = 1;  // LIGA O TIMER1
            INTCON2bits.INTEDG0 = 0;  // ALTERAR O TIMER PARA PEGAR NA BORDA DE DESCIDA
            medindo = 1;  // Flag que indica estar medindo
        }else if(PORTBbits.RB0 == 0 && medindo == 1){ 
            TMR1ON = 0; // DESLIGA O TIMER1
            tempo_echo = (TMR1L | (TMR1H<<8))/110; // calcula distancia 
            valor_display = tempo_echo;
            
            
            if(flag_TARA == 0 && valor_tara > 0) // Caso não seja uma operação de tara atualiza o display
                valor_display = valor_tara - tempo_echo;
            
            if(flag_MENSURAR == 4){ 
                flag_MENSURAR = 5;
            }
            if(flag_MENSURAR == 3){ // psedo maquina de estados para obter os valores de X Y Z
                valZ = valor_display;
                flag_MENSURAR = 4;
            }
            if(flag_MENSURAR == 2){
                valY = valor_display;
                flag_MENSURAR = 3;
            }
            if(flag_MENSURAR == 1){
                valX = valor_display;
                flag_MENSURAR = 2;
            }
            if(flag_MENSURAR == 5){ // calculo do volume
                valor_display = valX*valY*valZ;
                flag_MENSURAR = 0;
                __delay_ms(20);
                char str[20];
                sprintf(str, "%d", valor_display);
                strcat(str, "\r\n");
                UART_Print(str);
            }else{
                 __delay_ms(20);
                char str[20];
                sprintf(str, "%d", valor_display);
                strcat(str, "\r\n");
                UART_Print(str);
            }
       
            INTCON2bits.INTEDG0 = 1; // seta borda de subida
            medindo = 0;
            if(flag_TARA == 1){ // caso seja uma operação de tara
                valor_tara = tempo_echo;
                valor_display =  valor_tara - valor_display;
                flag_TARA = 0;
                __delay_ms(40);
            }
                
        }
        INTCONbits.INT0IE = 1; // Abilita interrupções novamente
        INTCONbits.INT0IF = 0;
    }
    
    if(INTCONbits.TMR0IF){ // trigger do timer  0
        TMR0H = VAL_TIMER_H;
        TMR0L = VAL_TIMER_L;
        
        if( medindo == 0 ){
            dig_val = 0;
            valorAMos = valor_display; 
            switch(indice){
                case 0 :
                    dig_val = (valorAMos%100) % 10;
                    valor_equivalente(0, dig_val);
                    break;

                case 1:
                    dig_val = (valorAMos%100) / 10;
                    valor_equivalente(1, dig_val);
                    break;

                case 2:
                    dig_val = (valorAMos%1000) / 100;
                    valor_equivalente(2, dig_val);
                    break;

                case 3:
                    dig_val = valorAMos / 1000;
                    valor_equivalente(3, dig_val);
                    break;

                default:
                    break;
            }

            indice ++;
            if(indice == 4){
                indice = 0;
            }
        }
        
        INTCONbits.TMR0IF = 0;
    }
}