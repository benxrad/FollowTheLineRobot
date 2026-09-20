// Streamlined version for better accuracy
// 2x scan for all sensors, and synchronized scan rate + calibrated initial state

#include <Servo.h>
#include "robotDefines.h"

// Bluetooth setup
#include <SoftwareSerial.h>
SoftwareSerial btSerial(10, 11); // RX: 10, TX: 11

String rxPayload = "";
String targetDevice = "";

// Array configuration for line sensors to streamline initialization
const uint8_t lineSensors[] = {
  lineFollowSensor0, 
  lineFollowSensor1, 
  lineFollowSensor2, 
  lineFollowSensor3, 
  lineFollowSensor4
};

//---------------------------------------------
void initializeSensors() 
{
  for (uint8_t idx = 0; idx < 5; idx++) {
    pinMode(lineSensors[idx], INPUT);
  }
}

void processIncomingBluetooth() 
{
  checkBTcmd();   // Retrieve remote command via BT
  manualCmd();    // Handle manual steering actions
  rxPayload = ""; // Clear active payload buffer
}

void executeBehavior(uint8_t currentMode) 
{
  switch (currentMode) 
  {
    case STOPPED: 
      motorStop();
      btSerial.print("The End");
      ledBlink();
      previousError = error;
      break;

    case NO_LINE:  
      motorStop();
      motorTurn(LEFT, 180);
      previousError = 0;
      break;

    case FOLLOWING_LINE:     
      calculatePID();
      motorPIDcontrol();    
      break;     
  }
}

//---------------------------------------------
void setup() 
{
  Serial.begin(9600);
  btSerial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  initializeSensors();

  // Attach drive servos
  leftServo.attach(5);
  rightServo.attach(3);

  btSerial.print("check the PID constants to be sent to Robot\n");

  // Await start trigger or mode change
  while (digitalRead(buttonPin) == HIGH && mode == 0) 
  { 
    processIncomingBluetooth();
  }

  checkPIDvalues();
  mode = STOPPED;
}

void loop() 
{
  // Standby loop until trigger condition is met
  while (digitalRead(buttonPin) == HIGH && mode == 0) 
  { 
    // Idle state
  }

  readLFSsensors();    
  executeBehavior(mode);
}
