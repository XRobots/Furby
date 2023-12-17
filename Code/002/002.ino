#include <Servo.h>
#include <DFRobot_DF1201S.h>

#define DF1201SSerial Serial3
DFRobot_DF1201S DF1201S;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;

int IR;   // IR sensor

float leftEye = 1500;
float rightEye = 1500;
float leftLid = 1500;
float rightLid = 1500;
float beak = 1500;
float rightEar = 1500;
float leftEar = 1500;

float leftEyeFiltered = 1500;
float rightEyeFiltered = 1500;
float leftLidFiltered = 1500;
float rightLidFiltered = 1500;
float beakFiltered = 1500;
float rightEarFiltered = 1500;
float leftEarFiltered = 1500;

unsigned long currentMillis;
unsigned long previousMillis = 0;        // set up timers
long interval = 10;             // time constant for timer

// state machine vars

int stepFlagBlink = 0;
unsigned long previousBlinkMillis = 0;

int stepFlagEars = 0;
unsigned long previousEarsMillis = 0;

int stepFlagBeak = 0;
unsigned long previousBeakMillis = 0;

int stepFlagRock = 0;
unsigned long previousRockMillis = 0;

int stepFlagRockBack = 0;
unsigned long previousRockBackMillis = 0;

int counterIR = 0;
unsigned long counterFlag = 0;

int count1Flag = 0;
unsigned long previousCount1Millis = 0;

int count2Flag = 0;
unsigned long previousCount2Millis = 0;

int count3Flag = 0;
unsigned long previousCount3Millis = 0;

int idle1Flag = 0;
int idle2Flag = 0;
int idleGo = 0;
int idleLoopCount = 0;
unsigned long previousIdle1Millis = 0;

// filter function

float filter(float prevValue, float currentValue, int filter) {  
  float lengthFiltered =  (prevValue + (currentValue * filter)) / (filter + 1);  
  return lengthFiltered;  
}

void setup() {

  pinMode(8, INPUT);    // IR sensor
  pinMode(2, OUTPUT);   // PWMs for rocking action
  pinMode(3, OUTPUT);
  Serial.begin(115200);

  DF1201SSerial.begin(115200);
  while (!DF1201S.begin(DF1201SSerial)) {
  }
  DF1201S.setVol(/*VOL = */20);
  DF1201S.switchFunction(DF1201S.MUSIC);
  DF1201S.setPlayMode(DF1201S.SINGLE);
  servo1.attach(22);
  servo2.attach(24);
  servo3.attach(26);
  servo4.attach(28);
  servo5.attach(30);
  servo6.attach(32);
  servo7.attach(34);
  delay(500);
  servo2.writeMicroseconds(2050);      // left eyelid - higher nmber closes
  servo3.writeMicroseconds(950);     // right eyelid - lower number closes
  delay(3000);
  stepFlagRockBack = 1;
  stepFlagBeak = 1; 
  stepFlagEars = 1; 
  stepFlagBlink = 1;
  DF1201S.playFileNum(/*File Number = */1);  
 
}

void loop() {

  currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {  // start timed event    
    previousMillis = currentMillis;

    IR = digitalRead(8);    // low state is active
    //Serial.println(IR);

    // *** counter loop for IR sensor
    if (IR == LOW && counterFlag == 0) {
      counterIR = counterIR + 1;
      counterFlag = 1;
    }
    else if (counterFlag == 1 && IR == HIGH) {
      delay(500);
      counterFlag = 0;      
    }

    if (counterIR == 4) {
      counterIR = 1;
      count1Flag = 0; 
      count2Flag = 0;
      count3Flag = 0;
    }

    // *** combo sequences

    // *** counter 1 ***

    if (counterIR == 1 && count1Flag == 0) {
      count1Flag = 1;
      idleGo = 1;    
    }

    else if (count1Flag == 1) {
      lookRight();      
      count1Flag = 2;      
      stepFlagBeak = 1;
      DF1201S.next();
      previousCount1Millis = currentMillis;                
    } 
    else if (count1Flag == 2 && currentMillis - previousCount1Millis > 1000) {
      stepFlagBlink = 1; 
      lookLeft();      
      count1Flag = 3;  
      previousCount1Millis = currentMillis;               
    } 
    else if (count1Flag == 3 && currentMillis - previousCount1Millis > 1000) {
      lookRight();
      stepFlagBeak = 1;
      count1Flag = 4; 
      previousCount1Millis = currentMillis;                
    } 
    else if (count1Flag == 4 && currentMillis - previousCount1Millis > 1500) {
      lookCentre();      
      count1Flag = 5;
      idleGo = 0; 
      previousCount1Millis = currentMillis;                 
    } 

    // *** counter 2 ***

    if (counterIR == 2 && count2Flag == 0) {
      count2Flag = 1; 
      idleGo = 1;     
    }

    else if (count2Flag == 1) {
      lookCrossed();
      count2Flag = 2;      
      stepFlagBeak = 1;
      DF1201S.next();
      previousCount2Millis = currentMillis;                
    } 
    else if (count2Flag == 2 && currentMillis - previousCount2Millis > 1500) {
      stepFlagEars = 1; 
      lookApart();      
      count2Flag = 3;  
      previousCount2Millis = currentMillis;               
    } 
    else if (count2Flag == 3 && currentMillis - previousCount2Millis > 1500) {
      count2Flag = 4; 
      previousCount2Millis = currentMillis;                
    } 
    else if (count2Flag == 4 && currentMillis - previousCount2Millis > 1500) {
      lookCentre();      
      count2Flag = 5;
      stepFlagBeak = 1;
      stepFlagBlink = 1; 
      idleGo = 0; 
      previousCount2Millis = currentMillis;                 
    }

    // *** counter 3 ***

    if (counterIR == 3 && count3Flag == 0) {
      count3Flag = 1; 
      idleGo = 1;     
    }
    else if (count3Flag == 1) {
      DF1201S.next();
      lookApart();
      stepFlagRock = 1;
      count3Flag = 2;      
      stepFlagBeak = 1;
      previousCount3Millis = currentMillis;                
    } 
    else if (count3Flag == 2 && currentMillis - previousCount3Millis > 1500) {
      stepFlagEars = 1; 
      lookCrossed();
      stepFlagBlink = 1;       
      count3Flag = 3;  
      previousCount3Millis = currentMillis;               
    } 
    else if (count3Flag == 3 && currentMillis - previousCount3Millis > 1500) {
      count3Flag = 4; 
      earsBack();
      previousCount3Millis = currentMillis;                
    } 
    else if (count3Flag == 4 && currentMillis - previousCount3Millis > 1500) {
      lookCentre();
      earsMid();      
      count3Flag = 5;
      stepFlagRock = 1;
      stepFlagBlink = 1; 
      idleGo = 0; 
      previousCount3Millis = currentMillis;                 
    } 

    // *** idle stuff ***

    if (idleGo == 0 && idle1Flag == 0 && currentMillis - previousIdle1Millis > 1000) {
        stepFlagBlink = 1;
        idle1Flag = 1;
        previousIdle1Millis = currentMillis;
    }
    else if (idleGo == 0 && idle1Flag == 1 && currentMillis - previousIdle1Millis > 1000) {
        idle1Flag = 2;
        previousIdle1Millis = currentMillis;
    }
    else if (idleGo == 0 && idle1Flag == 2 && currentMillis - previousIdle1Millis > 1000) {
        idle1Flag = 3;
        stepFlagEars = 1;
        previousIdle1Millis = currentMillis;
    }
    else if (idleGo == 0 && idle1Flag == 3 && currentMillis - previousIdle1Millis > 1000) {
        idle1Flag = 4;
        lookLeft();
        previousIdle1Millis = currentMillis;
    }
    else if (idleGo == 0 && idle1Flag == 4 && currentMillis - previousIdle1Millis > 1000) {
        idle1Flag = 5;
        lookRight();
        previousIdle1Millis = currentMillis;
    }
    else if (idleGo == 0 && idle1Flag == 5 && currentMillis - previousIdle1Millis > 1000) {
        idle1Flag = 6;
        lookApart();
        previousIdle1Millis = currentMillis;
    }
    else if (idleGo == 0 && idle1Flag == 6 && currentMillis - previousIdle1Millis > 1000) {
        idle1Flag = 0;
        lookCentre();
        idleLoopCount = idleLoopCount + 1;
        previousIdle1Millis = currentMillis;
    }

    if (idleLoopCount >= 3) {
      idleLoopCount = 0;
      idle2Flag = 0;
      counterIR = counterIR + 1;
    }

    if (idleLoopCount == 2 && idle2Flag == 0) {
      stepFlagRock = 1;
      idle2Flag = 2;
    }    

    // **** SEQUENCES ****

    // *** blink ***
    if (stepFlagBlink == 1) {
      eyesClosed();
      stepFlagBlink = 2;
      previousBlinkMillis = currentMillis;
    }
    else if (stepFlagBlink == 2 && currentMillis - previousBlinkMillis > 500) {
      previousBlinkMillis = currentMillis;
      stepFlagBlink = 0; 
      eyesOpen();   
    }  

    // *** ears back/forward *** 
    if (stepFlagEars == 1) {
      earsForward();
      stepFlagEars = 2;
      previousEarsMillis = currentMillis;
    }
    else if (stepFlagEars == 2 && currentMillis - previousEarsMillis > 500) {
      previousEarsMillis = currentMillis;
      stepFlagEars = 0; 
      earsBack();   
    } 

    // *** beak open/close ***
    if (stepFlagBeak == 1) {
      beakOpen();
      stepFlagBeak = 2;
      previousBeakMillis = currentMillis;
    }
    else if (stepFlagBeak  == 2 && currentMillis - previousBeakMillis > 500) {
      previousBeakMillis = currentMillis;
      stepFlagBeak  = 3; 
      beakClosed();   
    } 
    else if (stepFlagBeak  == 3 && currentMillis - previousBeakMillis > 500) {
      previousBeakMillis = currentMillis;
      stepFlagBeak  = 4; 
      beakOpen();
    }
    else if (stepFlagBeak  == 4 && currentMillis - previousBeakMillis > 500) {
      previousBeakMillis = currentMillis;
      stepFlagBeak  = 5; 
      beakClosed();   
    }
    else if (stepFlagBeak  == 5 && currentMillis - previousBeakMillis > 500) {
      previousBeakMillis = currentMillis;
      stepFlagBeak  = 6; 
      beakOpen();
    }
    else if (stepFlagBeak  == 6 && currentMillis - previousBeakMillis > 500) {
      previousBeakMillis = currentMillis;
      stepFlagBeak  = 0; 
      beakClosed();   
    }

    // *** rock back/forward *** 
    if (stepFlagRock == 1) {
      previousRockMillis = currentMillis;      
      stepFlagRock = 2;
      analogWrite(2,80);
      analogWrite(3,0);
    }
    else if (stepFlagRock == 2 && currentMillis - previousRockMillis > 300) {
      previousRockMillis = currentMillis;      
      stepFlagRock = 3;
      analogWrite(2,0);
      analogWrite(3,0);
    }
    else if (stepFlagRock == 3 && currentMillis - previousRockMillis > 300) {
      previousRockMillis = currentMillis;
      stepFlagRock = 4; 
      analogWrite(2,0);
      analogWrite(3,150);        
    } 
    else if (stepFlagRock == 4 && currentMillis - previousRockMillis > 450) {
      previousRockMillis = currentMillis;
      stepFlagRock = 0; 
      analogWrite(2,0);
      analogWrite(3,0);
    }

    // *** rock back only *** 
    if (stepFlagRockBack == 1) {
      previousRockBackMillis = currentMillis;      
      stepFlagRockBack = 2;
      analogWrite(2,0);
      analogWrite(3,150);
    }
    else if (stepFlagRockBack == 2 && currentMillis - previousRockBackMillis > 300) {
      previousRockBackMillis = currentMillis;      
      stepFlagRockBack = 3;
      analogWrite(2,0);
      analogWrite(3,0);
    }
   
    // filter values
  
    leftEyeFiltered = filter(leftEye, leftEyeFiltered, 60);
    rightEyeFiltered = filter(rightEye, rightEyeFiltered, 60);
    leftLidFiltered = filter(leftLid, leftLidFiltered, 20);
    rightLidFiltered = filter(rightLid, rightLidFiltered, 20);
    beakFiltered = filter(beak, beakFiltered, 20);
    rightEarFiltered = filter(rightEar, rightEarFiltered, 20);
    leftEarFiltered = filter(leftEar, leftEarFiltered, 20);
    
    // write filtered values to servos with trim to make the eyes look straigh etc
    
    servo1.writeMicroseconds(leftEyeFiltered-100);      // left eye - lower number goes left
    servo2.writeMicroseconds(leftLidFiltered);      // left eyelid - higher nmber closes
    servo3.writeMicroseconds(rightLidFiltered);     // right eyelid - lower number closes
    servo4.writeMicroseconds(rightEyeFiltered+50);     // right eye - lower number goes left
    servo5.writeMicroseconds(beakFiltered);         // beak - lower number closed
    servo6.writeMicroseconds(rightEarFiltered);     // right ear - lower number forwards
    servo7.writeMicroseconds(leftEarFiltered);      // left ear - higher number forwards
  
  }    // end of timed loop

}

