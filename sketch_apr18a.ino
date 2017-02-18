#define CLK1    _BV(CS10)               // 0.0625/4096uS
#define CLK8    _BV(CS11)               // 0.5/32768uS
#define CLK64   _BV(CS11) | _BV(CS10)   // 4/262144uS
#define CLK256  _BV(CS12)               // 16/1048576uS
#define CLK1024 _BV(CS12) | _BV(CS10)   // 64/4194304uS

#define HERTZ     60.0    // Hz
#define PULSE     50.0    // ms
#define DIM       50      // %
#define CLKMULT   0.5     // CLK8

#define GATE      PB1     // PIN9

const double _c = 1 / HERTZ / 2.0 * 1000000.0; // 8333.333...
uint16_t _v = _c * DIM / 100.0 / CLKMULT + 0.5;

ISR(_zc) {
    TCNT1 = 0;
    TCCR1B = CLK8;
}

ISR(TIMER1_COMPA_vect) {
  PORTB |= _BV(GATE);
  TCNT1 = 65536 - (PULSE / CLKMULT + 0.5);
}

ISR(TIMER1_OVF_vect) {
  PORTB &= ~_BV(GATE);
  TCCR1B = 0x00;
}

// DDRx
// 0=INPUT
// 1=OUTPUT

// PORTx
// 0=LOW/
// 1=HIGH/PULLUP

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");

  Serial.print("_v=");
  Serial.println(_v);

  DDRB = 0xFF;
  PORTB = 0;

  DDRC = 0xFF;
  PORTC = 0;

  DDRD = ~_BV(PD3); // PD3=INPUT
  PORTD = 0;

  attachInterrupt(1, _zc, RISING);

  // TIMSK1   Timer/Counter1 Interrupt Mask Register
  // OCIE1A   Timer/Counter1, Output Compare A Match Interrupt Enable
  // TOIE1    Timer/Counter1, Overflow Interrupt Enable
  TIMSK1 = _BV(OCIE1A) | _BV(TOIE1);

  // TCCR1A   Timer/Counter1 Control Register A
  // 0x00     Normal port operation, OC1A/OC1B disconnected.
  // TCCR1B   Timer/Counter1 Control Register B
  // 0x00     No clock source (Timer/Counter stopped)
  TCCR1A = 0x00;
  TCCR1B = 0x00;

  // Timer/Counter (TCNT1)
  TCNT1 = 0;
  // Output Compare Registers (OCR1A/B)
  OCR1A = _v;
}

void loop() {
  if (Serial.available()) {
    auto p = Serial.readString().toDouble();

    Serial.print((uint32_t)p);
    Serial.println("%");

    p = 100 - p;
    if (p == 0)
      p = 1;
    else if (p == 100)
      p = 99;

    uint16_t v = _c * p / 100.0 / CLKMULT + 0.5;

    cli();
    OCR1A = v;
    sei();
  }
}

