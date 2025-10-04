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
#define EMA_ALPHA 0.5

#define N 3
//#define N 10
//#define N 30
float samples[N];
int sample_index = 0;
bool samples_filled = false;

unsigned long last_sampling_time;
float dist_prev = (DIST_MIN + DIST_MAX) / 2.0;
float dist_ema;

void sort_array(float arr[], int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = i + 1; j < size; j++) {
      if (arr[i] > arr[j]) {
        float temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

float get_median() {
  int size = samples_filled ? N : sample_index;
  if (size == 0) return 0.0;

  float sorted_samples[size];
  for (int i = 0; i < size; i++) {
    sorted_samples[i] = samples[i];
  }
  
  sort_array(sorted_samples, size);

  if (size % 2 == 0) {
    return (sorted_samples[size / 2 - 1] + sorted_samples[size / 2]) / 2.0;
  } else {
    return sorted_samples[size / 2];
  }
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  
  Serial.begin(57600);

  dist_ema = (DIST_MIN + DIST_MAX) / 2.0;
}

void loop() {
  float dist_raw, dist_filtered, dist_median;
  
  if (millis() < last_sampling_time + INTERVAL)
    return;
  
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if ((dist_raw == 0.0) || (dist_raw > DIST_MAX) || (dist_raw < DIST_MIN)) {
      dist_filtered = dist_prev;
  } else {
      dist_filtered = dist_raw;
      dist_prev = dist_raw;
  }

  dist_ema = EMA_ALPHA * dist_filtered + (1 - EMA_ALPHA) * dist_ema;

  samples[sample_index] = dist_raw;
  sample_index++;
  if (sample_index >= N) {
    sample_index = 0;
    samples_filled = true;
  }
  dist_median = get_median();
  
  Serial.print("Min:");      Serial.print(DIST_MIN);
  Serial.print(",raw:");     Serial.print(dist_raw);
  Serial.print(",ema:");     Serial.print(dist_ema);
  Serial.print(",median:");  Serial.print(dist_median);
  Serial.print(",Max:");     Serial.print(DIST_MAX);
  Serial.println("");
  
  if ((dist_raw < DIST_MIN) || (dist_raw > DIST_MAX))
    digitalWrite(PIN_LED, 1);
  else
    digitalWrite(PIN_LED, 0);
  
  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
