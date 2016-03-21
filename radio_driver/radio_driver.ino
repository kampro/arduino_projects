const byte VOLUME_MIN = 0;
const byte VOLUME_MAX = 35;

enum EncoderPinAssignments {
  encoderPinA = 2,   // rigth
  encoderPinB = 3,   // left
  clearButton = 8    // button
};

volatile unsigned int encoderPos = 0;  // a counter for the dial
unsigned int lastReportedPos = 1;  // change management
static boolean rotating = false;  // debounce management

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

void setup() {
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(clearButton, INPUT);
  // turn on pullup resistors
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  digitalWrite(clearButton, HIGH);

  // encoder pin on interrupt 0
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1
  attachInterrupt(1, doEncoderB, CHANGE);

  Serial.begin(9600);  // output
}

void loop() {
  rotating = true;  // reset the debouncer

  if (lastReportedPos != encoderPos) {
    Serial.print("Index:");
    Serial.println(encoderPos, DEC);
    lastReportedPos = encoderPos;
  }
  if (digitalRead(clearButton) == LOW )  {
    encoderPos = 0;
  }
}

// Interrupt on A changing state
void doEncoderA() {
  // debounce
  if (rotating) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change?
  if (digitalRead(encoderPinA) != A_set) { // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if (A_set && !B_set) {
      if (encoderPos < VOLUME_MAX)
        ++encoderPos;
    }

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// Interrupt on B changing state
void doEncoderB() {
  if (rotating) delay (1);
  if (digitalRead(encoderPinB) != B_set) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if (B_set && !A_set) {
      if (encoderPos > VOLUME_MIN)
        --encoderPos;
    }

    rotating = false;
  }
}
