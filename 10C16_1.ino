#include <Servo.h>

#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

Servo myServo;

int closedAngle = 0;    // 닫힘 각도
int openedAngle = 90;   // 열림 각도
bool gateOpened = false;  // 현재 게이트 상태 저장

// Sigmoid 함수
float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

// 거리 측정 함수
long getDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH);
  long distance = duration * 0.034 / 2; // cm 단위
  return distance;
}

void setup() {
  Serial.begin(9600);
  myServo.attach(PIN_SERVO);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  myServo.write(closedAngle); // 처음엔 닫힘
}

void loop() {
  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // 차량이 15cm 이내 → 열림
  if (distance > 0 && distance < 30 && !gateOpened) {
    moveServoSmooth(closedAngle, openedAngle);
    gateOpened = true;
  }
  // 차량이 15cm 밖 → 닫힘
  else if ((distance >= 30 || distance <= 0) && gateOpened) {
    moveServoSmooth(openedAngle, closedAngle);
    gateOpened = false;
  }

  delay(200);
}

// Sigmoid 기반 부드러운 움직임
void moveServoSmooth(int fromAngle, int toAngle) {
  int steps = 50; // 단계 수
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    float s = sigmoid((t - 0.5) * 8);
    int angle = fromAngle + (toAngle - fromAngle) * s;
    myServo.write(angle);
    delay(30);
  }
}


