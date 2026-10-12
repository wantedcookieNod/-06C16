#include <Servo.h>

#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

Servo myServo;

int closedAngle = 0;     // 차단기 닫힘 각도
int openedAngle = 90;    // 차단기 열림 각도
bool gateOpened = false; // 현재 차단기 상태 저장

// 거리 측정 함수
long getDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH);
  long distance = duration * 0.034 / 2; // cm 변환
  return distance;
}

void setup() {
  Serial.begin(9600);
  myServo.attach(PIN_SERVO);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  myServo.write(closedAngle); // 시작은 닫힘 상태
}

// Cosine 기반 부드러운 움직임
void moveServoSmoothCosine(int fromAngle, int toAngle) {
  int steps = 50; // 단계 수
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;           // 0 ~ 1
    float c = (1 - cos(t * PI)) / 2.0;    // Cosine easing
    int angle = fromAngle + (toAngle - fromAngle) * c;
    myServo.write(angle);
    delay(30); // 속도 조절
  }
}

void loop() {
  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // 차량이 30cm 이내로 들어오면 열림
  if (distance > 0 && distance < 30 && !gateOpened) {
    moveServoSmoothCosine(closedAngle, openedAngle);
    gateOpened = true;
  }
  // 차량이 30cm 밖으로 나가면 닫힘
  else if ((distance >= 30 || distance <= 0) && gateOpened) {
    moveServoSmoothCosine(openedAngle, closedAngle);
    gateOpened = false;
  }

  delay(200);
}
