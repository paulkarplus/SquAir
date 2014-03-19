long tstart = 0;
char c1 = 0;
char c2 = 0;
int ax = 0;
int ay = 250;
int az = 500;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tstart = micros();
}

void loop() {
  if (Serial.available() > 0) {
    if (c1 != 'q') {
      c1 = Serial.read();
    } else {
      c2 = Serial.read();
      if (c2 == '?') {
        Serial.print("s");
        char a = ax/256;
        char b = ax%256;
        Serial.print(a);
        Serial.print(b);
        a = ay/256;
        b = ay%256;
        Serial.print(a);
        Serial.print(b);
        a = az/256;
        b = az%256;
        Serial.print(a);
        Serial.print(b);
        Serial.print("78901234567890123456789012e");
        ax++;
        ay++;
        az++;
        if (ax >1000) ax = 0;
        if (ay >1000) ay = 0;
        if (az >1000) az = 0;
        c1 = 0;
        c2 = 0;
      } else {
        c1 = 0;
        c2 = 0;
      }
    }
  }
}
