#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define DIST_MIN 100.0
#define DIST_MAX 300.0
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);
  last_sampling_time = millis() - INTERVAL;
}

void loop() {
  float distance;
  int ledValue = 255;
  
  if (millis() < (last_sampling_time + INTERVAL))
    return;
  
  distance = USS_measure(PIN_TRIG, PIN_ECHO);
  
  if (distance < DIST_MIN || distance > DIST_MAX || distance == 0.0) {
    ledValue = 255;
  } 
  else if (distance <= 200.0) {
    ledValue = map(distance, 100, 200, 255, 0);
  } 
  else {
    ledValue = map(distance, 200, 300, 0, 255);
  }
  
  analogWrite(PIN_LED, ledValue);
  
  Serial.print("Min:"); Serial.print(DIST_MIN);
  Serial.print(",distance:"); Serial.print(distance);
  Serial.print(",Max:"); Serial.print(DIST_MAX);
  Serial.println("");
  
  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  unsigned long pulse = pulseIn(ECHO, HIGH, (unsigned long)TIMEOUT);
  return pulse * SCALE;
}
