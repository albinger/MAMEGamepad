/*
** Author: Andrew Albinger
**
** Intended for use with https://www.thingiverse.com/thing:3082120
** to remotely control a MAME installation.
**
** Based on the examples from Adafruit.
** For use with the Adafruit Feather 32u4 Bluefruit LE.
** I recommend putting the HIDKeyboard sketch on your board
** to begin as it does a factory reset and all the tweaks.
** Very little error checking in the board setup here.
**
** I take no responsibility for your implementation!
*/


#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

const int numbutts = 8;  //heh, he said butts...


int buttons[8] = {2,3,13,18,19,22,21,20};

int keys[8]= {  
  64,  //1  button7
  128, //2  button8
  32,  //3  button6
  16,  //4  button5
  4,   //5  button3
  1,   //6  button1
  2,   //7  button2
  8    //8  button4
  };

int idle=1;

// Create the bluefruit object using SPI
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


void setup(void)
{
//initialize the bluefruit le    
  ble.begin(VERBOSE_MODE);
  //ble.factoryReset();
  ble.echo(false);

  //ble.info();
  ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=MAME Gamepad" ));
  ble.sendCommandCheckOK(F( "AT+BLEHIDGAMEPADEN=On" ));
  ble.reset(); // required if we change state like the previous line

// initialize the pushbutton pins as input:
  for(int i=0;i<numbutts;i++){
    pinMode(buttons[i], INPUT_PULLUP);     
  }
  pinMode(23,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
}


void loop(void)
{
  int butts=0;
  int state=0;
  int i = 0;
  int x, y = 0;
  char thestring[9];

  y=digitalRead(23) ? 0:1; // up
  y+=digitalRead(6) ? 0:-1;  // down
  x+=digitalRead(6) ? 0:-1;  // left
  x+=digitalRead(10) ? 0:1;  //right

  for(i=0;i<numbutts;i++){ 
    state = digitalRead(buttons[i]) ? 0:1; // read all buttons one time
    {
       if (state)  
       {
        butts +=keys[i];    
       }  
    }
  }
      
  if(butts || x || y)
    {
      sprintf(thestring,"%01d,%01d,0x%02x",x,y,butts); 
      bld.print("AT+BLEHIDGAMEPAD=");
      ble.println(thestring);         //send the codes, something was pressed
      idle = 0;              // we are not idle
    }else if(!idle)   
    {
      ble.println("AT+BLEHIDGAMEPAD=0,0,0x00");  //send the all clear
      idle = 1;                   //we are idle
    }
  delay(100);    //slow down son!
}
