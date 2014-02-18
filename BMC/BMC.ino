#include <DueTimer.h>

const int lowestPin = 2;
const int highestPin = 13;
const int m1_pole_pairs = 7;
const int m1_A_HIN = 2;
const int m1_A_LIN = 22;
const int m1_B_HIN = 3;
const int m1_B_LIN = 23;
const int m1_C_HIN = 4;
const int m1_C_LIN = 24;
const int ISR_per = 25; // us for Timer3 interrupt
int m1_state = 0;
long m1_counter = 0;
double m1_RPS = .3; // rotations per second
int m1_dc = 70;
bool pwm_high = 0;
bool pwm_state = 0;




void firstHandler(){
  digitalWrite(36, 1);
  m1_counter++;
  long m1_elec_freq = 6*m1_RPS*m1_pole_pairs;
  if (m1_counter > (int)1000000/ISR_per/m1_elec_freq) {
    m1_counter = 0;
    m1_state++;
    if (m1_state > 5) {
      m1_state = 0;
    }
    switch(m1_state) {
      case 0:
        analogWrite(m1_A_HIN, 70);
        digitalWrite(m1_A_LIN, 0);
        analogWrite(m1_B_HIN, 0);
        digitalWrite(m1_B_LIN, 0);
        analogWrite(m1_C_HIN, 0);
        digitalWrite(m1_C_LIN, 1);
        break;
        
      case 1:
        analogWrite(m1_A_HIN, m1_dc);
        digitalWrite(m1_A_LIN, 0);
        analogWrite(m1_B_HIN, 0);
        digitalWrite(m1_B_LIN, 1);
        analogWrite(m1_C_HIN, 0);
        digitalWrite(m1_C_LIN, 0);
        break;
      case 2:
        analogWrite(m1_A_HIN, 0);
        digitalWrite(m1_A_LIN, 0);
        analogWrite(m1_B_HIN, 0);
        digitalWrite(m1_B_LIN, 1);
        analogWrite(m1_C_HIN, m1_dc);
        digitalWrite(m1_C_LIN, 0);
        break;
      case 3:
        analogWrite(m1_A_HIN, 0);
        digitalWrite(m1_A_LIN, 1);
        analogWrite(m1_B_HIN, 0);
        digitalWrite(m1_B_LIN, 0);
        analogWrite(m1_C_HIN, m1_dc);
        digitalWrite(m1_C_LIN, 0);
        break;
      case 4:
        analogWrite(m1_A_HIN, 0);
        digitalWrite(m1_A_LIN, 1);
        analogWrite(m1_B_HIN, m1_dc);
        digitalWrite(m1_B_LIN, 0);
        analogWrite(m1_C_HIN, 0);
        digitalWrite(m1_C_LIN, 0);
        break;
      case 5:
        analogWrite(m1_A_HIN, 0);
        digitalWrite(m1_A_LIN, 0);
        analogWrite(m1_B_HIN, m1_dc);
        digitalWrite(m1_B_LIN, 0);
        analogWrite(m1_C_HIN, 0);
        digitalWrite(m1_C_LIN, 1);
        break;
    }  
  }
  digitalWrite(36, 0);
}

void setup() {
  
  pinMode(m1_A_HIN, OUTPUT);
  pinMode(m1_A_LIN, OUTPUT);
  pinMode(m1_B_HIN, OUTPUT);
  pinMode(m1_B_LIN, OUTPUT);
  pinMode(m1_C_HIN, OUTPUT);
  pinMode(m1_C_LIN, OUTPUT);
  pinMode(36, OUTPUT);
  Serial.begin(9600);
  analogWriteResolution(12);
  Timer3.attachInterrupt(firstHandler).start(ISR_per); 
 
  for (int thisPin = lowestPin; thisPin <= highestPin; thisPin++) {
    pinMode(thisPin, OUTPUT);
  }
}

void loop() {
  // iterate over the pins:
}
