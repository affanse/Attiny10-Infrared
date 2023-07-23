#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

#define LED PB1

volatile uint8_t NextBit;
volatile uint32_t RecdData;

void ReceivedCode(bool Repeat) {
  if ((RecdData & 0xFFFF) != 0xff00) return;
  uint8_t key = RecdData>>16 & 0xFF;
  if ((key == 0x0C) && !Repeat) PORTB ^= 1<<LED;
  else if ((key == 0x18) && !Repeat) PORTB ^= 1<<LED;
  else if ((key == 0x16) && !Repeat) PORTB &= ~1<<LED;
}

ISR(INT0_vect) {
  uint8_t Time = TCNT0;
  uint8_t Overflow = TIFR0 & 1<<TOV0;

  if (NextBit == 32) {
    if ((Time >= 194) && (Time <= 228) && (Overflow == 0)) {
      RecdData = 0; 
      NextBit = 0;
    } else if ((Time >= 159) && (Time <= 193) && (Overflow == 0)) ReceivedCode(1);
  } else {
    if ((Time > 44) || (Overflow != 0)) NextBit = 32; // Invalid - restart
    else {
      if (Time > 26) RecdData = RecdData | ((uint32_t) 1<<NextBit);
      if (NextBit == 31) ReceivedCode(0);
      NextBit++;
    }
  }
  TCNT0 = 0;           // Clear counter
  TIFR0 |= 1<<TOV0;    // Clear overflow
  EIFR |= 1<<INTF0;    // Clear INT0 flag
}

int main(void) {
  DDRB = 1<<LED;
  TCCR0B = 3<<CS00;    // Prescaler /64
  EIMSK |= 1<<INT0;    // Enable INT0/PB2
  EICRA |= 1<<ISC01;   // Interrupt on falling edge
  NextBit = 32;        // Wait for AGC start pull

  while(1) {
    PORTB ^= 1<<LED;
    _delay_ms(1000);
  }
}

