const int dataPin = 2;
const int clockPin =  3;

enum {
  PS2IWANTSTART,
  PS2IWANTDATA,
  PS2IWANTPAR,
  PS2IWANTSTOP,
};

volatile uint8_t ps2InputState;
volatile uint8_t ps2InputBuffer;
volatile uint8_t ps2InputBufferPosition;
volatile uint8_t ps2InputBufferParity;

volatile uint8_t ps2InputFIFO[32];
volatile uint8_t ps2InputFIFOWritePtr = 0;
volatile uint8_t ps2InputFIFOReadPtr = 0;

#define ps2InputFIFOWrite(DATA) (ps2InputFIFO[ps2InputFIFOWritePtr++ & 0x1F] = (DATA))
#define ps2InputFIFORead() (ps2InputFIFO[ps2InputFIFOReadPtr++ & 0x1F])
#define ps2HaveData() ((ps2InputFIFOReadPtr & 0x1F) != (ps2InputFIFOWritePtr & 0x1F))
#define ps2FullFIFO() ((ps2InputFIFOReadPtr & 0x1F) - 1 == (ps2InputFIFOWritePtr & 0x1F))

void err() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void ps2Init() {
  pinMode(dataPin, INPUT_PULLUP);
  pinMode(clockPin, INPUT_PULLUP);

  ps2Reset();
}

void ps2Reset() {
  ps2InputState = PS2IWANTSTART;
  ps2InputBuffer = 0;
  ps2InputBufferPosition = 0;
  ps2InputBufferParity = 0;
}

void ps2KeyboardInterrupt() {
  static unsigned long ps2InterruptLastTime = 0; 
  
  unsigned long ps2InterruptTime = millis();

  if (ps2InterruptTime - ps2InterruptLastTime > 100) {
    ps2Reset();

    ps2InterruptLastTime = ps2InterruptTime;
  }

  uint8_t ps2InputBit = digitalRead(dataPin);

  switch (ps2InputState) {
    case PS2IWANTSTART: {
      if (ps2InputBit != 0) {
        err(); // "ERROR: Start bit not 0"
      }

      ps2InputState = PS2IWANTDATA;

      break;
    }

    case PS2IWANTDATA: {
      ps2InputBuffer |= ps2InputBit << ps2InputBufferPosition++;
      ps2InputBufferParity ^= ps2InputBit;

      if (ps2InputBufferPosition == 8) {
        ps2InputState = PS2IWANTPAR;
      }

      break;
    }

    case PS2IWANTPAR: {
      ps2InputBufferParity ^= ps2InputBit;

      if (ps2InputBufferParity != 1) {
        err(); // "ERROR: Bad parity bit"
      }

      ps2InputState = PS2IWANTSTOP;

      break;
    }

    case PS2IWANTSTOP: {
      if (ps2InputBit != 1) {
        err(); // "ERROR: Stop bit not 1"
      }

      if (ps2FullFIFO()) {
        err(); // "ERROR: Input FIFO is full";
      } else {
        ps2InputFIFOWrite(ps2InputBuffer);
      }

      ps2Reset();
      break;
    }
  }

}


#define AWAIT_FALLING_EDGE while (digitalRead(clockPin) == HIGH) \
    ;

#define AWAIT_RISING_EDGE while (digitalRead(clockPin) == LOW) \
    ;

void ps2WriteByte(uint8_t data) {
  uint8_t parity = 0;

  cli();

  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, LOW);

  delayMicroseconds(100);

  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);

  delayMicroseconds(30);

  pinMode(clockPin, INPUT_PULLUP);

  delayMicroseconds(30);

  for (uint8_t i = 0; i < 8; i++) {
    AWAIT_FALLING_EDGE

    digitalWrite(dataPin, data & 1);
    parity ^= data & 1;
    data >>= 1;

    AWAIT_RISING_EDGE
  }

  AWAIT_FALLING_EDGE
  digitalWrite(dataPin, parity ^ 1);
  AWAIT_RISING_EDGE


  AWAIT_FALLING_EDGE
  pinMode(dataPin, INPUT_PULLUP);
  AWAIT_RISING_EDGE

  AWAIT_FALLING_EDGE
  bool ok = !digitalRead(dataPin);

  // I don't know why this is here, but it is necessary
  AWAIT_RISING_EDGE
  AWAIT_FALLING_EDGE

  sei();

  if (!ok) {
    err(); // ACK bit not zero
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  ps2Init();

  attachInterrupt(digitalPinToInterrupt(clockPin), ps2KeyboardInterrupt, FALLING);

  Serial.begin(9600);
 
  delay(1000);
  ps2WriteByte(0xED);
  delayMicroseconds(1000);
  ps2WriteByte(2);
}

void loop() {
  if (Serial.available()) {
    ps2WriteByte(Serial.read());
  }

  if (ps2HaveData()) {
    Serial.write(ps2InputFIFORead());
  }
}
