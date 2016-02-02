
#include <msp430x14x.h>

#define Beep_on  P2OUT&=~BIT2;
#define Beep_OFF P2OUT|=BIT2;
unsigned char beep_flag=1;
unsigned int beep_timer;


void Clock_init()
{
   unsigned char iq0;
   BCSCTL1 = RSEL0 + RSEL1 + RSEL2;          // XT2on, max RSEL
   BCSCTL1&=~XT2OFF; //打开XT2振荡器
   do
   {
      IFG1 &= ~OFIFG; // 清除振荡器失效标志
      for (iq0 = 255; iq0 > 0; iq0--); // 延时，等待XT2起振
   }
    while ((IFG1 & OFIFG) != 0); // 判断XT2是否起振
   
   BCSCTL2 |= SELS+SELM_2;                          // SMCLK = XT2
}


void timerA_init()
{
   CCTL0 = CCIE;                               // CCR0 interrupt enabled
   CCR0 = 8000;                                // 晶振8M   
   TACTL = TASSEL_2 + MC_1+TACLR;              // SMCLK, upmode
}

void main(void)
{
   WDTCTL = WDTPW + WDTHOLD;                  // Stop WDT
  
   Clock_init();

   P2DIR |= BIT2;                             // P1.0 output
   timerA_init();
   _BIS_SR(LPM0_bits + GIE);                   // Enter LPM0 w/ interrupt

}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
   beep_timer++;
   if(beep_timer==500)
   {
        beep_timer=0;
        switch(beep_flag)
       {
          case 1:
                  beep_flag=2;
                  Beep_on;
                  break;
           case 2:
                  Beep_OFF;
                  beep_flag=1;
                  break;
          default:break;
        }
   }
  
}