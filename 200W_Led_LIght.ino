int motorDownPin = 11, motorUpPin = 10;
int coolerPin = 9;
int ledPin1 = 5, ledPin2 = 6;
int upPosPin = A1, downPosPin = A0, switch1 = A2;
int ledVoltagePin1 = A3, ledVoltagePin2 = A4;

unsigned long timeStart;
unsigned long switchTimeOn = 0, switchTimeOff = 0;
boolean isDown = true, transition = false;
boolean movingUp = false, movingDown = false;
boolean resetSwitch = false;
boolean ledsOn = false, flashLED = false;

int motorSpeed = 80;
int maxTime = 4000;

void setup() 
{
  Serial.begin(9600);
  
  pinMode(motorDownPin, OUTPUT);
  pinMode(motorUpPin, OUTPUT);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  pinMode(coolerPin, OUTPUT);
}


void readState()
{
  if (analogRead(upPosPin) > 100)
  {
    isDown = false;
  }
  else if (analogRead(downPosPin) > 100)
  {
    isDown = true;
  }  
}

void switchState()
{
   if (switchTimeOn - switchTimeOff > 300 && !resetSwitch && switchTimeOn - switchTimeOff < 3000) // move to transition state to turn on engine
  {
    Serial.println(switchTimeOn - switchTimeOff);
    resetSwitch = true;
    transition = true;
  }
  else if (switchTimeOff - switchTimeOn > 300 && resetSwitch && switchTimeOff - switchTimeOn < 3000 )
  { 
    Serial.println(switchTimeOff - switchTimeOn);
    resetSwitch = false;
    transition = true;
  }


  
   if (analogRead(switch1) < 100)
  {
    switchTimeOff = millis();
  }
  else if (analogRead(switch1) > 100)
  {
    switchTimeOn = millis();
  } 


 
}


void getMotorAction()
{
  if (transition && !movingDown && !movingUp) // if motor is not moving and transition state is on, find the action
  {
    transition = false;
    timeStart = millis();
    
    if (isDown)
    {
      movingUp = true; // if its in down possition it moves up
      
      digitalWrite(motorDownPin, LOW);
      analogWrite(motorUpPin, motorSpeed);
    }
    else // if its up it moves down
    {
      movingDown = true;
      
      digitalWrite(motorUpPin, LOW);      
      analogWrite(motorDownPin, motorSpeed/2);

    }
  }
  else if (!movingUp && !movingDown) // if its not moving up or down and isnt in transition state turn everything off (safety)
  {
    digitalWrite(motorDownPin, LOW);
    digitalWrite(motorUpPin, LOW);
  }
}

void getLedAction()
{
  
    if (millis() - timeStart > 500) // if 0.5s passed since motor started turn leds on or off
    {
       if (movingUp)
       {
       digitalWrite(ledPin1, HIGH);
       digitalWrite(ledPin2, HIGH);

       digitalWrite(coolerPin, HIGH);
       ledsOn = true;
       }
       else if (movingDown)
       {
       digitalWrite(ledPin1, LOW);
       digitalWrite(ledPin2, LOW);

       digitalWrite(coolerPin, LOW);
       ledsOn = false;
       }
    }
  
  
}

void finishState()
{
   if (!isDown && movingUp) // if its up and movingUp turn off motors and leave leds
  {
    digitalWrite(motorDownPin, LOW);
    digitalWrite(motorUpPin, LOW);
    digitalWrite(coolerPin, HIGH); 
    digitalWrite(ledPin1, HIGH); 
    digitalWrite(ledPin2, HIGH); 
    
    movingUp = false; movingDown = false; ledsOn = true;
    delay(100); 
  }
  else if (isDown && movingDown) // if its down and moving down turn off everything
  {
    digitalWrite(motorDownPin, LOW);
    digitalWrite(motorUpPin, LOW);   
    digitalWrite(coolerPin, LOW); 
    digitalWrite(ledPin1, LOW); 
    digitalWrite(ledPin2, LOW);    
    
    movingUp = false; movingDown = false; ledsOn = false;
    delay(100);
  }
}


void forceStop()
{
    digitalWrite(motorDownPin, LOW);
    digitalWrite(motorUpPin, LOW);   
    digitalWrite(coolerPin, LOW); 
    digitalWrite(ledPin1, LOW); 
    digitalWrite(ledPin2, LOW);  

    movingUp = false;
    movingDown = false;
    transition = false; 
    ledsOn = false;
}


void safetyCheck()
{
  if (millis() - timeStart > maxTime)
  {
    if (analogRead(upPosPin) < 100 && analogRead(downPosPin) < 100)
    {
      forceStop();
    }
    else if (movingDown && !isDown)
    {
      forceStop();
    }
    else if (movingUp && isDown)
    {
      forceStop();
    }
  }
}

void ledsFlash()
{
  if (ledsOn && flashLED)
  {
     digitalWrite(ledPin1, LOW); 
    digitalWrite(ledPin2, LOW);

    digitalWrite(ledPin1, HIGH);
    delay(25);
    digitalWrite(ledPin2, HIGH);
    delay(25);
    digitalWrite(ledPin1, LOW);
    delay(25);
     digitalWrite(ledPin2, LOW);
     delay(15);
    
  }
  else
  delay(10);
}


void loop() 
{
  switchState();
  readState();
  getMotorAction();
  getLedAction();
  finishState();

  safetyCheck();

  ledsFlash();

  Serial.println(analogRead(A0));
 Serial.println(analogRead(A1));
  Serial.println(analogRead(A2));
  

}
