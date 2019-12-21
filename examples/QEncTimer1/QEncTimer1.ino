#include "QEnc.h"

/*
 * Interfacing to the rotary encoder knob using
 * Timer1 interrupts for polling with 500usec period.
 */

#define PIN_A 2
#define PIN_B 3
#define POLL_PERIOD_US 500

QEnc g_enc(PIN_A, PIN_B);

ISR(TIMER1_OVF_vect)
{
  g_enc.poll();
}

void timer_init(unsigned us_period)
{
  TCCR1A = 0;  // clear control register A 
  ICR1 = (F_CPU / 2000000) * us_period; // it will count up/down
  TCCR1B = _BV(WGM13) | _BV(CS10); // simmetric PWM, no prescaler
  TIMSK1 = _BV(TOIE1); // enable interrupt
}

void setup() {
  Serial.begin(9600);
  g_enc.begin();
  timer_init(POLL_PERIOD_US);
}

void loop() {
  Serial.println(g_enc.value());
  delay(1000);
}
