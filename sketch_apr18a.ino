#include <avr/io.h>
#include <avr/interrupt.h>

#define CLK1    (0x01 << CS10)                  // 0.0625/4096uS
#define CLK8    (0x01 << CS11)                  // 0.5/32768uS
#define CLK64   (0x01 << CS11) | (0x01 << CS10) // 4/262144uS
#define CLK256  (0x01 << CS12)                  // 16/1048576uS
#define CLK1024 (0x01 << CS12) | (0x01 << CS10) // 64/4194304uS

volatile long t[] = { 0, 0, 0 };
volatile unsigned short c[] = { 0, 0, 0 };
volatile unsigned short d = 0;

unsigned short TIM16_ReadTCNT1() {
  unsigned char sreg;
  unsigned short v;

  sreg = SREG;
  cli();
  v = TCNT1;
  SREG = sreg;

  return v;  
}

void TIM16_WriteTCNT1(unsigned short v) {
  unsigned char sreg;

  sreg = SREG;
  cli();
  TCNT1 = v;
  SREG = sreg;
}

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");

  TIMSK1 = (0x01 << OCIE1A) | (0x01 << TOIE1);

  TCCR1A = 0x00;
  TCCR1B = 0x00;
}

ISR(zero) {
  cli();
  t[0] = micros();
  TCCR1B = CLK64;
  OCR1A = 65534;
  TCNT1 = 0;
  sei();
}

ISR(TIMER1_COMPA_vect) {
  c[0] = TIM16_ReadTCNT1();
}

ISR(TIMER1_OVF_vect) {
  TCCR1B = 0x00;
  d = 1;
}

void loop() {
  if (Serial.available()) {
    unsigned char c = Serial.read();
    if (c == 'z' || c == 'Z')
      zero();
  }

  if (d == 1) {
    Serial.println(c[0]);
    d = 0;
  }
}

