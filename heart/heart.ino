/* Animation firmware for ATTiny85 heart-shaped 
 * PCB for Penn State Harrisburg IEEE club Valentine's day
 * soldering workshop. 
 *
 * Uses charlieplexed scheme to drive 20 LEDs
 *
 * Created 31 Jan 2023 
 * Michael Cieslak, Zach Martin
 */

#include <avr/io.h>
#include <util/delay.h>

// 5 charlieplexing pins
#define LINE_A 0 //Pin 5 (PB0) on ATtiny85
#define LINE_B 1 //Pin 6 (PB1) on ATtiny85
#define LINE_C 2 //Pin 7 (PB2) on ATtiny85
#define LINE_D 3 //Pin 2 (PB3) on ATtiny85
#define LINE_E 4 //Pin 3 (PB4) on ATtiny85

// For now, 8 brightness steps
#define MAX_BRIGHTNESS 8

char led_grid[20] = {
  000 , 000 , 000 , 000 , 000 ,
  000 , 000 , 000 , 000 , 000 ,
  000 , 000 , 000 , 000 , 000 ,
  000 , 000 , 000 , 000 , 000
};
//DDRB direction config for each LED (1 = output)
const char led_dir[20] = {
  ( 1<<LINE_A | 1<<LINE_E ), //LED 0
  ( 1<<LINE_B | 1<<LINE_E ), //LED 1
  ( 1<<LINE_C | 1<<LINE_E ), //LED 2
  ( 1<<LINE_D | 1<<LINE_E ), //LED 3
  ( 1<<LINE_E | 1<<LINE_D ), //LED 4
  
  ( 1<<LINE_A | 1<<LINE_D ), //LED 5
  ( 1<<LINE_B | 1<<LINE_D ), //LED 6
  ( 1<<LINE_C | 1<<LINE_D ), //LED 7
  ( 1<<LINE_D | 1<<LINE_C ), //LED 8
  ( 1<<LINE_E | 1<<LINE_C ), //LED 9
  
  ( 1<<LINE_A | 1<<LINE_C ), //LED 10
  ( 1<<LINE_B | 1<<LINE_C ), //LED 11
  ( 1<<LINE_C | 1<<LINE_B ), //LED 12
  ( 1<<LINE_D | 1<<LINE_B ), //LED 13
  ( 1<<LINE_E | 1<<LINE_B ), //LED 14
  
  ( 1<<LINE_A | 1<<LINE_B ), //LED 15
  ( 1<<LINE_B | 1<<LINE_A ), //LED 16
  ( 1<<LINE_C | 1<<LINE_A ), //LED 17
  ( 1<<LINE_D | 1<<LINE_A ), //LED 18
  ( 1<<LINE_E | 1<<LINE_A ) //LED 19
};

//PORTB output config for each LED (1 = High, 0 = Low)
const char led_out[20] = {
  ( 1<<LINE_A ), //LED 0
  ( 1<<LINE_B ), //LED 1
  ( 1<<LINE_C ), //LED 2
  ( 1<<LINE_D ), //LED 3
  ( 1<<LINE_E ), //LED 4
  
  ( 1<<LINE_A ), //LED 5
  ( 1<<LINE_B ), //LED 6
  ( 1<<LINE_C ), //LED 7
  ( 1<<LINE_D ), //LED 8
  ( 1<<LINE_E ), //LED 9
  
  ( 1<<LINE_A ), //LED 10
  ( 1<<LINE_B ), //LED 11
  ( 1<<LINE_C ), //LED 12
  ( 1<<LINE_D ), //LED 13
  ( 1<<LINE_E ), //LED 14
  
  ( 1<<LINE_A ), //LED 15
  ( 1<<LINE_B ), //LED 16
  ( 1<<LINE_C ), //LED 17
  ( 1<<LINE_D ), //LED 18
  ( 1<<LINE_E ) //LED 19
};


void setup() {
  // put your setup code here, to run once:

}

void loop(void) {
  heart_pattern(); // change the leds in memory to follow the heart pattern. 
  draw_frame(); // PWM the LEDs. This has to run as fast as possible.
}


/* Fade heart pattern ring in, then fade to black */
void heart_pattern(void)
{
  static unsigned long timestamp = 0; // timestamp in ms
  static uint8_t head = 0; // head of animation
  uint8_t body, intensity;
  
  // Run this only every 100ms
  if(millis() - timestamp > 100)
  {
    timestamp = millis();
  }
  else
  {
    return; // don't execute code yet
  }
  
  if(head == 0) // start animation with LED 1
  {
    set_led(head, MAX_BRIGHTNESS/4);
    head = 1;
  }
  
  else if (head >= 14) // somewhere around here, fade out. TODO this doesn't actually fade, its just shutting off
  {
    uint8_t i; 
    for(i = 0; i < 19; i++)
    {
     set_led(i+1, 0);
    }
  }
  
  else  // fade remainder of heart
  {
    head ++; 
    body = head;
    intensity = MAX_BRIGHTNESS/4;
    
    while(body > 0)
    {
      if(body <= 11) set_led(body, intensity); // the two sides meet at D11
      
      body --; 
      if(intensity < MAX_BRIGHTNESS) 
      {
        intensity += MAX_BRIGHTNESS/4;
      }
    }
    
    body = 22-head; 
    intensity = MAX_BRIGHTNESS; 
    while(body < 21)
    {
      if(body <=12) set_led(body, intensity); // this side will stop at D12
      
      body ++;
      if(intensity < MAX_BRIGHTNESS)
      {
        intensity += MAX_BRIGHTNESS/4;
      }
    }
    
  }
}

void set_led(uint8_t led, uint8_t intensity)
{
  intensity = intensity < 100 ? intensity : MAX_BRIGHTNESS; // clamp the value to MAX_BRIGHTNESS
  
  led_grid[led -1] = intensity;
}




void light_led(char led_num) { //led_num must be from 0 to 19
  DDRB = led_dir[led_num];
  PORTB = led_out[led_num];
}

void leds_off() {
  DDRB = 0;
  PORTB = 0;  
}

void draw_frame(void)
{
  static uint8_t period_track = 0; // Use for janky PWM
  uint8_t led; 
  
  for(; led < 19; led ++) // write the LEDs
  {
    if(led_grid[led] >= period_track)
    {
      light_led(led); // turn it on
    }
    else
    {
      leds_off(); // everything off
    }
  }
  
  // Keep track of the PWM waveform
  if(period_track >= MAX_BRIGHTNESS)
  {
    period_track = 0;
  }
  else
  { 
    period_track ++;
  }
}
