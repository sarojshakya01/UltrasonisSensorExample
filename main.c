/*
 * Ultrasonic Sensor test.c
 *
 * Created: 9/25/2016 1:19:20 PM
 * Author : Er. Saroj
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define LCD_DATA_PORT PORTA
#define LCD_DATA_DDR DDRA
#define LCD_RS 2
#define LCD_EN 3

#define US_PORT PORTC
#define  US_PIN   PINC
#define US_DDR    DDRC
#define US_POS_0 PC0
#define US_POS_1 PC1
#define US_ERROR 0xffff
#define  US_NO_OBSTACLE 0xfffe

void LCD_CMND(unsigned char cmnd)
{
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd & 0xF0);
	LCD_DATA_PORT &= ~(1<<LCD_RS);
	LCD_DATA_PORT |= 1<<LCD_EN;
	_delay_us(300);
	LCD_DATA_PORT &= ~(1<<LCD_EN);
	_delay_us(500);
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd << 4);
	LCD_DATA_PORT |= 1<<LCD_EN;
	_delay_us(300);
	LCD_DATA_PORT &= ~(1<<LCD_EN);
}
void LCD_DATA(unsigned char data)
{
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data & 0xF0);
	LCD_DATA_PORT |= 1<<LCD_RS;
	LCD_DATA_PORT |= 1<<LCD_EN;
	_delay_us(300);
	LCD_DATA_PORT &= ~(1<<LCD_EN);
	_delay_us(500);
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data << 4);
	LCD_DATA_PORT |= 1<<LCD_EN;
	_delay_us(300);
	LCD_DATA_PORT &= ~(1<<LCD_EN);
}
void LCD_INITIALIZE(void)
{
	LCD_DATA_DDR = 0xFC;
	LCD_DATA_PORT &= ~(1<<LCD_EN);
	_delay_ms(200);
	LCD_CMND(0x33);
	_delay_ms(20);
	LCD_CMND(0x32);
	_delay_ms(20);
	LCD_CMND(0x28);
	_delay_ms(20);
	LCD_CMND(0x0C);
	_delay_ms(20);
	LCD_CMND(0x01);
	_delay_ms(20);
}
void LCD_CLEAR(void)
{
	LCD_CMND(0x01);
	_delay_ms(2);
}
void LCD_PRINT(char * str)
{
	unsigned char i=0;
	while(str[i] != 0)
	{
		LCD_DATA(str[i]);
		i++;
		_delay_us(10);
	}
}
void LCD_SET_CURSER(unsigned char y, unsigned char x)
{
	if(y==1)
	LCD_CMND(0x7F+x);
	else if(y==2)
	LCD_CMND(0xBF+x);
}
void LCD_NUM(unsigned char num)
{
	LCD_DATA(num/10 + 0x30);
	LCD_DATA(num%10 + 0x30);
}


uint16_t getPulseWidth()
{
      uint32_t i,result;

       //Wait for the rising edge
       for(i=0;i<600000;i++)
       {
          if(!(US_PIN & (1<<US_POS_0))) continue; else break;
       }

       if(i==600000)
          return 0xffff; //Indicates time out

       //High Edge Found

       //Setup Timer1
       TCCR1A=0X00;
       TCCR1B=(1<<CS11); //Prescaler = Fcpu/8
       TCNT1=0x00;       //Init counter

       //Now wait for the falling edge
       for(i=0;i<600000;i++)
       {
          if(US_PIN & (1<<US_POS_0))
          {
             if(TCNT1 > 60000) break; else continue;
          }
          else
             break;
       }

       if(i==600000)
         return 0xffff; //Indicates time out

       //Falling edge found

       result=TCNT1;

       //Stop Timer
       TCCR1B=0x00;

       if(result > 60000)
          return 0xfffe; //No obstacle
       else
          return (result>>1);
   }

    void Wait()
   {
      uint8_t i;
      for(i=0;i<10;i++)
         _delay_loop_2(0);
   }
   int main()
   {
      uint16_t r;

      Wait();

      //Initialize the LCD Module
      LCD_INITIALIZE();

      Wait();

      LCD_CLEAR();
      LCD_PRINT("Ultra Sonic");
      LCD_SET_CURSER(0,1);
	  LCD_PRINT("Sensor Test");

      Wait();
      Wait();
      Wait();
      Wait();
      Wait();
      Wait();

     LCD_CLEAR();


      while(1)
      {

         //Set Ultra Sonic Port as out
         US_DDR|=(1<<US_POS_1);

         _delay_us(10);

         //Give the US pin a 15us High Pulse
         US_PORT|=(1<<US_POS_1);   //High

         _delay_us(15);

         US_PORT&=(~(1<<US_POS_1));//Low

         _delay_us(20);

         //Now make the pin input
         US_DDR&=(~(1<<US_POS_0));

         //Measure the width of pulse
         r=getPulseWidth();

         //Handle Errors
         if(r==US_ERROR)
         {
            LCD_CLEAR();
			LCD_PRINT("Error !");
	     }
         else  if(r==US_NO_OBSTACLE)
         {
            LCD_CLEAR();
            LCD_PRINT("Clear !");
         }
         else
         {

            int d;

            d=(r/58.0); //Convert to cm
            LCD_CLEAR();
            LCD_NUM(d);
            LCD_PRINT(" cm");

            Wait();
         }
     }

  }