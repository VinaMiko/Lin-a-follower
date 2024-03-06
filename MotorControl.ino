//-----[startup variables]--------------------------------------------------------
int start = 0;
int brightnessSum = 0;
int sample[6];
int sampleCounter = 0;
int startCounter = 0;
int startSpeed = 200;
int startInterval = 1000;

//-----[motors]--------------------------------------------------------------------
const int LB = 10;              //A1 = Left Back
const int LF = 11;              //A2 = Left Forward
const int RB = 6;               //B1 = Right Back
const int RF = 9;               //B2 = Right Forward

//-----[sensors]--------------------------------------------------------------------
const int sensorPins[]={A0,A1,A2,A3,A4,A5,A6,A7};
int sensorRead[8]={0,0,0,0,0,0,0,0};
int Counter = 0;

//-----[speed control variables]--------------------------------------------------------------------
int MaxPower = 240;             //max speed
int NoPower = 0;                //lowest speed
int ReverseMaxPower = -240;     //max power on reverse
int SpeedR = MaxPower;          //Right wheel speed
int SpeedL= MaxPower;           //Left wheel speed
int SpeedBR = NoPower;          //Right wheel speed reverse
int SpeedBL = NoPower;          //Left wheel speed reverse
int SpeedWR = NoPower;          //Right wheel speed retention
int SpeedWL = NoPower;          //Left wheel speed retention
int RTurnStack = 0;
int LTurnStack = 0;
int RAccelerationStack = 0;
int LAccelerationStack = 0;

//-----[break checks]--------------------------------------------------------------------
int EmergencyStopR = 0;         //Right turn help
int EmergencyStopL = 0;         //Left turn help
int BlackBoxStop = 0;

//-----[timer]--------------------------------------------------------------------
float Interval = 0.25;

//-----[pin setup]--------------------------------------------------------------------
void setup() 
{
  pinMode(RB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  Serial.begin(9600);
  for(int i = 0;i<=7;i++) 
  {
    pinMode(sensorPins[i], INPUT);
  }
//-----------------------------------------------[start line setup}------------------
  While(start == 0)
  {
    analogWrite(RF,startSpeed);
    analogWrite(LF,startSpeed);
    if(millis()-startCounter > startInterval)
    {
      startCounter = millis();
      for(int i = 0;i<7,i++)
      {
        brightnessSum += analogRead(sensorPins[i])
      }
      sample[sampleCounter] = brightnessSum;
      brightnessSum = 0;
      sampleCounter++;
    }
    if( &&  &&  && )
    {
      start = 1;
    }
  }
}

//-----------------------------------------------[mainloop]---------------------------
void loop() 
{
//-----------------------------------------------[timer]-----------------------------
  if(millis()- Counter >= Interval) //Time Gate
  {
    Counter = millis();
    
//-----------------------------------------------[Reading Pins]----------------------
    for(int i = 0; i <= 7;i++)
    {
      if(analogRead(sensorPins[i])>=900)
      {
        sensorRead[i] = 1;
      }else if(analogRead(sensorPins[i]) <=800)
      {
        sensorRead[i] = 0;
      }
    }
//-----------------------------------------------[BlackBoxSensing]----------------------
    if(sensorRead[0] == 1 && sensorRead[7] == 1) //Black Horizontal
    {
      if(BlackBoxStop > 10)
      {
        SpeedL = NoPower;
        SpeedR = NoPower;
        Serial.println("STOP");
      }else
      {
        BlackBoxStop++;
      }
    }else
    {
      BlackBoxStop = 0;
//-----------------------------------------------[Right turn]----------------------
      if(sensorRead[0] == 1 && sensorRead[1] == 0 && SpeedL > 140)
      {
        LturnStack = 4;
      }
      if(sensorRead[0] == 1) //Black Right
      {
        RTurnStack = 3;
      }else if(sensorRead[1] == 1 && SpeedR > 0)
      {
        RTurnStack = 2;
      }else if(sensorRead[2] == 1 && SpeedR > 140)
      {
        RTurnStack=1;
      }
//-----------------------------------------------[Left turn]----------------------
      if(sensorRead[7] == 1 && sensorRead[6] == 0 && SpeedR > 140)
      {
        RturnStack = 4;
      }
      if(sensorRead[7] == 1) //Black Right
      {
        LTurnStack = 3;
      }else if(sensorRead[6] == 1 && SpeedL > 0)
      {
        LTurnStack = 2;
      }else if(sensorRead[5] == 1 && SpeedL > 140)
      {
        LTurnStack=1;
      }
//-----------------------------------------------[Static acceleration right]----------------------
      if(sensorRead[2] == 0 && sensorRead[0] == 0) //Black Right
      {
        RAccelerationStack = 3;
      }else if(sensorRead[1] == 0 && SpeedR < 140)
      {
        RAccelerationStack = 3;
      }else if(sensorRead[0] == 0 && SpeedR < 0)
      {
        RAccelerationStack=5;
      }
//-----------------------------------------------[Static acceleration left]----------------------
      if(sensorRead[5] == 0 && sensorRead[7] == 0) //Black Right
      {
        LAccelerationStack = 3;
      }else if(sensorRead[6] == 0 && SpeedL < 140)
      {
        LAccelerationStack = 3;
      }else if(sensorRead[7] == 0 && SpeedL < 0)
      {
        LAccelerationStack = 5;
      }
//-----------------------------------------------[Speed calculation]----------------------
      SpeedR = ModularTurn(SpeedR, RTurnStack);
      SpeedL = ModularTurn(SpeedL, LTurnStack);
      SpeedR = ModularAcceleration(SpeedR, RAccelerationStack);
      SpeedL = ModularAcceleration(SpeedL, LAccelerationStack);
      SpeedBR = SpeedR*(-1);
      SpeedBL = SpeedL*(-1);
      RTurnStack = 0;
      LTurnStack = 0;
      RAccelerationStack = 0;
      LAccelerationStack = 0;
//-----------------------------------------------[Speed bounds check]----------------------
      if(SpeedR>MaxPower)SpeedR = MaxPower;
      if(SpeedR<ReverseMaxPower)SpeedR = ReverseMaxPower;
      if(SpeedL>MaxPower)SpeedL = MaxPower;
      if(SpeedL<ReverseMaxPower)SpeedL = ReverseMaxPower;
//-----------------------------------------------[Right Engine control]----------------------
      if(SpeedR >= 0)
      {
        analogWrite(RF,SpeedR);
        analogWrite(RB,0);
      }else
      {
        analogWrite(RF,0);
        analogWrite(RB,SpeedBR);
      }
//-----------------------------------------------[Left Engine control]----------------------
      if(SpeedL >= 0)
      {
        analogWrite(LF,SpeedL);
        analogWrite(LB,0);
      }else
      {
        analogWrite(LF,0);
        analogWrite(LB,SpeedBL);
      }
//-----------------------------------------------[Speed Notation]----------------------
      Serial.print(SpeedL);
      Serial.print(" ");
      Serial.print(SpeedR);
      Serial.print(" ");
    }
  }
  Serial.println();
}

int ModularTurn(int Speed, int Multiplier)
{
  Speed = Speed - (10*Multiplier);
  return Speed;
}
int ModularAcceleration(int Speed, int Multiplier)
{
  Speed = Speed + (10*Multiplier);
  return Speed;
}
