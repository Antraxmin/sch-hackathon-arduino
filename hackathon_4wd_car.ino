#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

//===================================================================================//
// 초음파 센서 설정
//===================================================================================//
#define TRIG_PIN            A0
#define ECHO_PIN            A1
#define MAX_DISTANCE        700 // 200-900

//===================================================================================//
// DC 모터 설정
//===================================================================================//
#define MAX_SPEED           150 // 150~190 sets speed of DC  motors
#define MAX_SPEED_OFFSET    20
#define MOTOR_STOP          0
#define MOTOR_RUN           MAX_SPEED

//===================================================================================//
// 초음파 센서 객체 생성
//===================================================================================//
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

//===================================================================================//
// DC 모터 객체 생성
//===================================================================================//
AF_DCMotor  motor1(1, MOTOR12_1KHZ);        // DC-모터 1번 객체 생성
AF_DCMotor  motor2(2, MOTOR12_1KHZ);        // DC-모터 2번 객체 생성
AF_DCMotor  motor3(3, MOTOR34_1KHZ);        // DC-모터 3번 객체 생성
AF_DCMotor  motor4(4, MOTOR34_1KHZ);        // DC-모터 4번 객체 생성

//===================================================================================//
// 서보 모터 객체 생성
//===================================================================================//
Servo       myservo;                        // 서보모터 객체 생성

//===================================================================================//
// 초기값 설정
//===================================================================================//
boolean     goesForward = false;            // 전진 비활성
int         distance = 100;                 // 초음파 센서 초기값
int         speedSet = 0;                   // 모터 스피드 초기값

//===================================================================================//
// 초기 설정 루틴 setup()
//===================================================================================//
void setup()
{
  Serial.begin(9600);

  //---------------------------------------------------------------------------//
  // 서보모터 초기화
  // 서보모터 제어핀 : 10번
  // 서보모터 센터각도 : 115
  //---------------------------------------------------------------------------//
  myservo.attach(10);                 // 서보모터 제어핀 아두이노 10번 핀
  myservo.write(115);                 // 서보모터 초기 센터각도 : 115도

  delay(2000);

  //---------------------------------------------------------------------------//
  // 초음파센서 거리 측정 4회 실시
  //---------------------------------------------------------------------------//
  distance = readPing();              // 초음파센서 거리 측정
  delay(100);
  distance = readPing();              // 초음파센서 거리 측정
  delay(100);
  distance = readPing();              // 초음파센서 거리 측정
  delay(100);
  distance = readPing();              // 초음파센서 거리 측정
  delay(100);
}

//===================================================================================//
// loop()
//===================================================================================//
void loop()
{
  int distanceR = 0;
  int distanceL = 0;
  delay(40);

  //---------------------------------------------------------------------------//
  // 초음파 센서 거리가 50보다 작으면
  //---------------------------------------------------------------------------//
  if (distance <= 60)
  {
    moveStop();                 // 차량 정지
    delay(100);
    moveBackward();             // 차량 후진
    delay(300);
    moveStop();                 // 차량 정지
    delay(200);
    distanceR = lookRight();    // 오른쪽 거리 측정
    delay(1000);
    distanceL = lookLeft();     // 왼쪽 거리 측정
    delay(1000);

    Serial.println(distanceR);
    Serial.println(distanceL);

    //-------------------------------------------------------------------//
    // 오른쪽 거리 >= 왼쪽 거리
    //-------------------------------------------------------------------//
    if (distanceR >= distanceL)
    {
      turnRight();            // 오른쪽 회전
      moveStop();             // 차량 정지
    }
    //-------------------------------------------------------------------//
    // 왼쪽 거리 >= 오른쪽 거리
    //-------------------------------------------------------------------//
    else
    {
      turnLeft();             // 왼쪽 회전
      moveStop();             // 차량 정지
    }
  }
  else
  {
    moveForward();              // 차량 전진
  }
  distance = readPing();              // 초음파 측정
}

//===================================================================================//
// 오른쪽 초음파 측정
//===================================================================================//
int lookRight()
{
  myservo.write(50);                  // 오른쪽 쳐다보기
  delay(500);
  int distance = readPing();          // 거리 측정
  delay(100);
  myservo.write(115);                 // 센터 쳐다보기
  return distance;                    // 오른쪽 거리값 반환
}

//===================================================================================//
// 왼쪽 초음파 측정
//===================================================================================//
int lookLeft()
{
  myservo.write(170);                 // 외쪽 쳐다보기
  delay(500);
  int distance = readPing();          // 거리 측정
  delay(100);
  myservo.write(115);                 // 센터 쳐다보기
  delay(100);
  return distance;                    // 거리값 반환
}

//===================================================================================//
// 거리를 Cm로 변환
//===================================================================================//
int readPing()
{
  delay(70);
  int cm = sonar.ping_cm();           // 초음파 센서 거리를 cm로 변환
  if (cm == 0)                        // 변환 거리가 0 이면 250cm 로 간주
  {
    cm = 250;
  }
  return cm;
}

//===================================================================================//
// 모든 모터 정지
//===================================================================================//
void moveStop()
{
  motor1.run(RELEASE);                // DC 모터 1번 정지
  motor2.run(RELEASE);                // DC 모터 2번 정지
  motor3.run(RELEASE);                // DC 모터 3번 정지
  motor4.run(RELEASE);                // DC 모터 4번 정지
}

//===================================================================================//
// 모터 전진 함수
//===================================================================================//
void moveForward()
{
  if (!goesForward)
  {
    goesForward = true;
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);

    for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) // 갑작스런 전진 방지. 속도를 천천히 가속
    {
      motor1.setSpeed(speedSet);
      motor2.setSpeed(speedSet);
      motor3.setSpeed(speedSet);
      motor4.setSpeed(speedSet);
    }
  }
}

//===================================================================================//
// 모터 후진 함수
//===================================================================================//
void moveBackward()
{
  goesForward = false;
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) // 갑작스런 후진 방지. 속도를 천천히 가속
  {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    motor3.setSpeed(speedSet);
    motor4.setSpeed(speedSet);
  }
}

//===================================================================================//
// 오른쪽 회전 함수
//===================================================================================//
void turnRight()
{
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(500);                // 500 mSec 동안 오른쪽 회전
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

//===================================================================================//
// 왼쪽 회전 함수
//===================================================================================//
void turnLeft()
{
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(500);                // 500 mSec 동안 왼쪽 회전
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}
