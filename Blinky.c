#include <stdio.h>
#include "LPC17xx.H"                       
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include "KBD.h"
#include "toast.c"
#include "avocado.c"
#include <LPC17xx.h>                        /* LPC17xx definitions */
#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbaudio.h"

extern int game(void);


#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0										/* Uncomment to use the LCD */

//ITM Stimulus Port definitions for printf //////////////////
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}


//Use to trace the pot values in Debug
uint16_t ADC_Dbg;
//Use to trace the joystick value
uint32_t joystick;
uint32_t joystickInit = 0;

/* Import external variables from IRQ.c file                                  */
extern uint8_t  clock_ms;

int screen = 0;
int position = 1;
int pic = 1;

//For USB sound player
extern  void SystemClockUpdate(void);
extern uint32_t SystemFrequency;  
uint8_t  Mute;                                 
uint32_t Volume;                               

#if USB_DMA
uint32_t *InfoBuf = (uint32_t *)(DMA_BUF_ADR);
short *DataBuf = (short *)(DMA_BUF_ADR + 4*P_C);
#else
uint32_t InfoBuf[P_C];
short DataBuf[B_S];                         
#endif

uint16_t  DataOut;                              
uint16_t  DataIn;                              

uint8_t   DataRun;                              
uint16_t  PotVal;                               
uint32_t  VUM;                                  
uint32_t  Tick;

void get_potval (void) {
  uint32_t val;

  LPC_ADC->CR |= 0x01000000;              
  do {
    val = LPC_ADC->GDR;                   
  } while ((val & 0x80000000) == 0);      
  LPC_ADC->CR &= ~0x01000000;            
  PotVal = ((val >> 8) & 0xF8) +         
           ((val >> 7) & 0x08);
}



void TIMER0_IRQHandler(void) 
{
  long  val;
  uint32_t cnt;

  if (DataRun) {                           
    val = DataBuf[DataOut];                
    cnt = (DataIn - DataOut) & (B_S - 1);   
    if (cnt == (B_S - P_C*P_S)) {           
      DataOut++;                            
    }
    if (cnt > (P_C*P_S)) {                 
      DataOut++;                            
    }
    DataOut &= B_S - 1;                   
    if (val < 0) VUM -= val;                
    else         VUM += val;                
    val  *= Volume;                         
    val >>= 16;                             
    val  += 0x8000;                         
    val  &= 0xFFFF;                         
  } else {
    val = 0x8000;                           
  }

  if (Mute) {
    val = 0x8000;                           
  }

  LPC_DAC->CR = val & 0xFFC0;            

  if ((Tick++ & 0x03FF) == 0) {             
    get_potval();                          
    if (VolCur == 0x8000) {                 
      Volume = 0;                           
    } else {
      Volume = VolCur * PotVal;             
    }
    val = VUM >> 20;                        
    VUM = 0;                                
    if (val > 7) val = 7;                  
  }

  LPC_TIM0->IR = 1;
	if (get_button()==KBD_SELECT){
		NVIC_DisableIRQ(TIMER0_IRQn);
		NVIC_DisableIRQ(USB_IRQn);
		USB_Connect(0);
		USB_Connect(1);
	}
}
int audio (void)
{	

  volatile uint32_t pclkdiv, pclk;

  
  SystemClockUpdate();

  LPC_PINCON->PINSEL1 &=~((0x03<<18)|(0x03<<20));  
 
  LPC_PINCON->PINSEL1 |= ((0x01<<18)|(0x02<<20));

  
  LPC_SC->PCONP |= (1 << 12);

  LPC_ADC->CR = 0x00200E04;		
  LPC_DAC->CR = 0x00008000;		

  
  pclkdiv = (LPC_SC->PCLKSEL0 >> 2) & 0x03;
  switch ( pclkdiv )
  {
	case 0x00:
	default:
	  pclk = SystemFrequency/4;
	break;
	case 0x01:
	  pclk = SystemFrequency;
	break; 
	case 0x02:
	  pclk = SystemFrequency/2;
	break; 
	case 0x03:
	  pclk = SystemFrequency/8;
	break;
  }

  LPC_TIM0->MR0 = pclk/DATA_FREQ - 1;	
  LPC_TIM0->MCR = 3;					
  LPC_TIM0->TCR = 1;					
  NVIC_EnableIRQ(TIMER0_IRQn);

  USB_Init();				
  USB_Connect(TRUE);		
}


int main (void) {
  ADC_Init();                                /* ADC Initialization            */
	KBD_Init();																 /* Joystick Initialization 			*/
	LED_Init();                                /* LED Initialization            */
	#ifdef __USE_LCD
  GLCD_Init();                               /* Initialize graphical LCD (if enabled) */
  GLCD_Clear(White);                         /* Clear graphical LCD display   */
  GLCD_SetBackColor(Blue);
	GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(1, 3, __FI, "1.Photo Gallery");
	GLCD_SetTextColor(White);
  GLCD_DisplayString(2, 3, __FI, "2.MP3 Player");
	GLCD_DisplayString(3, 3, __FI, "3.Game");
	GLCD_DisplayString(5, 0, __FI, "Press to select");
	#endif


  while (1) {		/* Loop forever                  */
		joystick = get_button();								 /* Get joystick value */
    while (joystick != joystickInit){
		if (joystick == KBD_SELECT){ //make selection
				if (screen == 0)
					screen = 1;
				else
					screen = 0;
		}
		if (joystick == KBD_UP){ //Joystick pressed up
				position--;
				if (position == 0)
					position = 1;
		}
		if (joystick == KBD_DOWN){ //Joystick pressed down
				position++;
				if (position == 4)
					position = 3;
		}
		
		if (screen == 0){	//Main Menu
			#ifdef __USE_LCD
			GLCD_SetBackColor(Blue);
			GLCD_SetTextColor(White);
			GLCD_DisplayString(1, 3, __FI, "1.Photo Gallery");
			GLCD_DisplayString(2, 3, __FI, "2.MP3 Player");
			GLCD_DisplayString(3, 3, __FI, "3.Game");
			GLCD_DisplayString(5, 0, __FI, "Press to select");
			#endif
			if (position == 1){ //Photo Gallery
						#ifdef __USE_LCD
						GLCD_SetTextColor(Yellow);
						GLCD_DisplayString(1, 3, __FI, "1.Photo Gallery");
						#endif
			}
			if (position == 2){ //MP3 Player
						#ifdef __USE_LCD
						GLCD_SetTextColor(Yellow);
						GLCD_DisplayString(2, 3, __FI, "2.MP3 Player");
						#endif
			}
			if (position == 3){ //Game
						#ifdef __USE_LCD
						GLCD_SetTextColor(Yellow);
						GLCD_DisplayString(3, 3, __FI, "3.Game");
						#endif
			}
		}
		if (screen == 1){ //	Individual Option
			#ifdef __USE_LCD
			GLCD_Clear(White);
			#endif
			if (position == 1){ //Photo Gallery
					joystick = get_button();
					while(joystick != KBD_SELECT){
					if (joystick == KBD_LEFT){
							#ifdef __USE_LCD
							GLCD_Clear(White);
							#endif
							pic = 1;
					}
					if (joystick == KBD_RIGHT){
							#ifdef __USE_LCD
							GLCD_Clear(White);
							#endif
							pic = 2;
					}
					if (pic == 1){
						#ifdef __USE_LCD
						GLCD_Bitmap(0, 0, 296, 155, TOAST_PIXEL_DATA);
						GLCD_DisplayString(7, 0, __FI, "Right to next pic");
						GLCD_DisplayString(8, 0, __FI, "Press to exit");
						#endif
					}
					if (pic == 2){
						#ifdef __USE_LCD
						GLCD_Bitmap(0, 0, 296, 167, AVOCADO_PIXEL_DATA);
						GLCD_DisplayString(7, 0, __FI, "Left to next pic");
						GLCD_DisplayString(8, 0, __FI, "Press to exit");
						#endif
					}
					joystick = get_button();
				 }
					screen = 0;
					#ifdef __USE_LCD
							GLCD_Clear(White);
							#endif
			}
			if (position == 2){ //MP3 Player
					
					#ifdef __USE_LCD
					GLCD_DisplayString(1, 3, __FI, "MP3 Playing");
					GLCD_DisplayString(3, 3, __FI, "Press to exit");
					#endif
					audio();
					screen = 0;
					#ifdef __USE_LCD
					GLCD_Clear(White);
					#endif
			}
			if (position == 3){ //Game
					game();
					screen = 0;
					#ifdef __USE_LCD
							GLCD_Clear(White);
							#endif
			}
		}
		joystickInit = joystick;
	}
 }		 
}
