//---------------------------------------------------------------------------------------------------------------[Line Sensor variables]
#define ptr int*
int start = 0;                                                                                                //check for the while loop
int black = 0;                  //highest brightness from line sensor calibration
int white = 0;                  //lowest brightness from line sensor calibration
int brightnessAvg = 0;          //average of black and white, used for reading where the line is at
int sensorRead[8];              //array keeping the current read of the line sensor
int sampleCounter = 0;          //counts the samples for pin calibration

//---------------------------------------------------------------------------------------------------------------[speed control variables]

int MaxPower = 240;             //max speed
int NoPower = 0;                //lowest speed
int LowPower = 140;             //SlowSpeed
int LowerPower = 50;            //Slowest Speed
int ReverseMaxPower = -240;     //max power on reverse
int SpeedR = MaxPower;          //Right wheel speed
int SpeedL= MaxPower;           //Left wheel speed
int SpeedBR = NoPower;          //Right wheel speed reverse
int SpeedBL = NoPower;          //Left wheel speed reverse
int SpeedWR = NoPower;          //Right wheel speed retention
int SpeedWL = NoPower;          //Left wheel speed retention
int RTurnStack = 0;             //determines the sharpness of the turn
int LTurnStack = 0;             //determines the sharpness of the turn
int RAccelerationStack = 0;     //negates the turnstacks after the turn
int LAccelerationStack = 0;     //negates the turnstacks after the turn

//---------------------------------------------------------------------------------------------------------------[start/end transition check variables]

int BlackBoxSense = 0;          //Sensing the starting and end points
int BlackBoxStart = 0;          //Check for end condition

//---------------------------------------------------------------------------------------------------------------[timer variables]

float Interval = 0;          //main time gate interval
int Counter = 0;                //check for the main timegate

//---------------------------------------------------------------------------------------------------------------[pin variables]

const int LB = 10;                                      //A1 = Left Back
const int LF = 11;                                      //A2 = Left Forward
const int RB = 6;                                       //B1 = Right Back
const int RF = 9;                                       //B2 = Right Forward
const int sensorPins[]={A0,A1,A2,A3,A4,A5,A6,A7};       //maps the pins for the line sensors

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
  StartSetup();                                   //flag checking, sensor calibration, transition into the main part [function below]
}


void loop() 
{
  if(millis()- Counter >= Interval)               //Time Gate
  {
    Counter = millis();
    ReadPins();
    LineCases();
    //Serial.print("   Speeds Right front{");
    Serial.print(SpeedR);
    Serial.print("   ");
    //Serial.print(SpeedBR);
    //Serial.print("}back  Left front{");
    Serial.print(SpeedL);
    Serial.print("   ");
    //Serial.print(SpeedBL);
    //Serial.print("}");
    EngineControl(SpeedR, SpeedBR, RF, RB);
    EngineControl(SpeedL, SpeedBL, LF, LB);

    //Serial.print("SPEED LEFT - ");
    Serial.print(SpeedL);                                                           //print out current speeds
    Serial.print(" ");
    //Serial.print("SPEED RIGHT - ");
    Serial.print(SpeedR);
    Serial.print(" ");
    
  }
  Serial.println();
}

//===============================================================================================================[Start Setup]

void StartSetup()
{
  int startCounter = 0;                                                                                         //saves millis after every loop
  int startSpeed = 200;                                                                                         //sets speed during the setup calibration
  int startInterval = 0;                                                                                        //interval for setup time gate
  analogWrite(RF,startSpeed);                                                                                   //starts engines
  analogWrite(LF,startSpeed);
  while(start == 0)                                                                                             //setup while loop
  {
    if(millis()-startCounter > startInterval)                                                                   //setup time gate
    {
      startCounter = millis();
      //Serial.print("CALIBRATING");
      //Serial.println();
      CalibratingPins();                                                                                        //calibrates the sensors
      if(millis() > 1000)
      {
        start = StartupLookForLine(start);
        sampleCounter++;                                                                                        //counts up the loop iterations
      }
    }
  }
}

//===============================================================================================================[Looking for the line at the start]

int StartupLookForLine(int start)
{
  for(int i = 0; i <= 7;i++)                                                                              //loops through the sensors
  {
    ReadPins();                                                                                           //reads sensors
    if((sensorRead[0] == 0 || sensorRead[7] == 0) && (sensorRead[3] == 1 || sensorRead[4] == 1))          //checks for start of actual line
    {
      start = 1;                                                                                          //switches check into ending the while loop
    }
  }
  return start;
}

//===============================================================================================================[PIN CALIBRATION]

void CalibratingPins()
{
  for(int i = 0;i <= 7;i++)                                                       //loops through pins
 {
    if(sampleCounter == 0 && i == 0)                                              //sets black and white to the read value on first iteration
    {
      black, white = analogRead(sensorPins[i]);
    }
    if(black < analogRead(sensorPins[i]))                                         //overrites black if senses a higher value
    {
      black = analogRead(sensorPins[i]);
    }
    if(white > analogRead(sensorPins[i]))                                         //overrites white if senses a lower value
    {
      white = analogRead(sensorPins[i]);
    }
  }
  brightnessAvg = (black + white)/2;                                              //gets average value of black and white, to use in the brightness reading gates
}

//===============================================================================================================[PIN READING]
void ReadPins()
{
  const int brightnessGateOffset = 50;                                            //gate offset turns singular gate into a double one
  for(int i = 0; i <= 7;i++)                                                      //loops through every sensor
  {
    if(analogRead(sensorPins[i])>= brightnessAvg + brightnessGateOffset)          //sensor read set to 1 if senses black
    {
      sensorRead[i] = 1;
    }else if(analogRead(sensorPins[i]) <= brightnessAvg - brightnessGateOffset)   //sensor reat set to 0 if senses white
    {
      sensorRead[i] = 0;
    }
    //Serial.print(sensorRead[i]);
  }
}

//===============================================================================================================[READING THE LINE]

void LineCases()
{
  if(sensorRead[0] == 1 && sensorRead[7] == 1)  //senses a Black Horizontal line
  {
    if(BlackBoxSense > 10)                      //delay to avoid confusing intersections with the black box
    {
      BlackBoxActions();                        //acts based on whether its first time or second [function below]
    }else
    {
      BlackBoxSense++;                          //increases every loop it senses black box, part of the delay system
    }
  }else
  {
    if(BlackBoxSense !=0)                       //resets the delay system
    {
      if(BlackBoxSense >=10)                    //counts up after every instance of black box being sensed
      {
        BlackBoxStart++;
      }
    BlackBoxSense = 0;
    }
    StackCounting();                            //calculates the stacks based on line position
    SpeedCalculation();                         //calculates final speed based on the stacks
  }
}

//===============================================================================================================[GETTING STACKS FOR TURN AND ACCELERATION]

void StackCounting()
{
  int TurnStacks[2];
  ptr CalculationReciever;
  //Serial.print("   Right[");
  //Serial.print(RTurnStack);
  //Serial.print(" - ");
  //Serial.print(LTurnStack);
  //Serial.print("]Left   ");
  //Serial.print("   Right[");
  //Serial.print(RAccelerationStack);
  //Serial.print(" - ");
  //Serial.print(LAccelerationStack);
  //Serial.print("]Left   ");
  //Serial.print("   LEFT TURN -");
  CalculationReciever = Turn(SpeedR, SpeedL, sensorRead[2], sensorRead[1], sensorRead[0], RTurnStack, LTurnStack, TurnStacks);
  RTurnStack += CalculationReciever[0];
  LTurnStack += CalculationReciever[1];
  //Serial.print("   RIGHT TURN -");
  CalculationReciever = Turn(SpeedL, SpeedR, sensorRead[5], sensorRead[6], sensorRead[7], LTurnStack, RTurnStack, TurnStacks);
  LTurnStack += CalculationReciever[0];
  RTurnStack += CalculationReciever[1];
  //Serial.print("   Right[");
  //Serial.print(RTurnStack);
  //Serial.print(" - ");
  //Serial.print(LTurnStack);
  //Serial.print("]Left   ");
  //Serial.print("   RIGHT ACCELERATION -");
  RAccelerationStack = Acceleration(SpeedR, sensorRead[2], sensorRead[1], sensorRead[0], RAccelerationStack);
  //Serial.print("   LEFT ACCELERATION -");
  LAccelerationStack = Acceleration(SpeedL, sensorRead[5], sensorRead[6], sensorRead[7], LAccelerationStack);
  //Serial.print("   Right[");
  //Serial.print(RAccelerationStack);
  //Serial.print(" - ");
  //Serial.print(LAccelerationStack);
  //Serial.print("]Left   ");
}

//===============================================================================================================[START/END ACTIONS WHEN BLACK BOX IS SENSED]

void BlackBoxActions()
{
  if(BlackBoxStart == 0)                    //happens when the black box is sensed first time (starting transition)
  {
    SpeedR = MaxPower;                      //speed set to turn around in place
    SpeedL = NoPower;
    //Serial.println("RIGHT!!!!!!!!!!!!!!!!!!!!");
  }else                                     //happens when black box is sensed second time (ending transition)
  {
  SpeedL = NoPower;                         //stops the engines***************************************************************[WIP]
  SpeedR = NoPower;
  //Serial.println("STOP");
  }
}

//===============================================================================================================[COUNTING THE STACKS FOR A TURN]

ptr Turn(int mainSpeed,int oppositeSpeed, int closeSensor, int midSensor, int farSensor, int mainTurnStack, int oppositeTurnStack, int TurnStacks[])
{

  if(farSensor == 1 && midSensor == 0 && oppositeSpeed > LowPower)           //emergency sharp turn
  {
    //Serial.print("EMERGENCY");
    oppositeTurnStack += 5;
  }
  if(farSensor == 1)                                                        //sharp turn
  {
    //Serial.print("FAR");
    mainTurnStack += 3;
  }else if(midSensor == 1 && mainSpeed > LowerPower)                           //standard turn
  {
    //Serial.print("MIDDLE");
    mainTurnStack += 2;
  }else if(closeSensor == 1 && mainSpeed > LowPower)                             //light turn
  {
    //Serial.print("CLOSE");
    mainTurnStack += 1;
  }
  TurnStacks[0] = mainTurnStack;
  TurnStacks[1] = oppositeTurnStack;
  return TurnStacks;
}

//===============================================================================================================[COUNTING STACKS FOR ACCELERATION]

int Acceleration(int Speed, int closeSensor, int midSensor, int farSensor, int accelerationStack)
{
  if(closeSensor == 0 && farSensor == 0)                                  //accelerate to max speed
  {
    //Serial.print("MAX");
    accelerationStack += 3;
  }else if(midSensor == 0 && farSensor == 0 && Speed < LowPower)                             //accelerate to low speed
  {
    //Serial.print("MIDDLE");
    accelerationStack += 3;
  }else if(farSensor == 0 && Speed < LowerPower)                           //accelerate to very low speed
  {
    //Serial.print("SLOW");
    accelerationStack += 5;
  }
  return accelerationStack;
}

//===============================================================================================================[TRANSLATING TURN STACKS INTO SPEED DECREASE]

int ModularTurn(int Speed, int Multiplier)                        //turns turn stacks into speed decrease
{
  Speed = Speed - (10*Multiplier);
  return Speed;
}

//===============================================================================================================[TRANSLATING ACCELERATION STACKS INTO SPEED INCREASE]

int ModularAcceleration(int Speed, int Multiplier)                //turns acceleration stacks into speed increase
{
  Speed = Speed + (10*Multiplier);
  return Speed;
}

//===============================================================================================================[GETTING ACTUAL SPEED FROM SPEED INCREASES/DECREASES]

void SpeedCalculation()
{
    
  if(LTurnStack > 0 || RTurnStack > 0)                              //if there is a turn then slow down
  {
    MaxPower = 200;
  }else
  {
    MaxPower = 240;
  }
  int SubSpeedCalculationArray[4];
  ptr CalculationReciever;
  //Serial.print("  RIGHT SPEED - ");
  CalculationReciever = SubSpeedCalculation(SpeedR, RTurnStack, RAccelerationStack, SpeedBR, SubSpeedCalculationArray);
  SpeedR = CalculationReciever[0];
  RTurnStack = CalculationReciever[1];
  RAccelerationStack = CalculationReciever[2];
  SpeedBR = CalculationReciever[3];
  //Serial.print("  {");
  //Serial.print(CalculationReciever[0]);
  //Serial.print(" _ ");
  //Serial.print(CalculationReciever[3]);
  //Serial.print("  RIGHT SPEED - ");
  CalculationReciever = SubSpeedCalculation(SpeedL, LTurnStack, LAccelerationStack, SpeedBL, SubSpeedCalculationArray);
  SpeedL = CalculationReciever[0];
  LTurnStack = CalculationReciever[1];
  LAccelerationStack = CalculationReciever[2];
  SpeedBL = CalculationReciever[3];
  //Serial.print("   AccelerationCheck(");
  //Serial.print(RAccelerationStack);
  //Serial.print(" - ");
  //Serial.print(LAccelerationStack);
  //Serial.print(")Left   ");
  //Serial.print("-");
  //Serial.print(CalculationReciever[0]);
  //Serial.print(" _ ");
  //Serial.print(CalculationReciever[3]);
  //Serial.print("}  ");
}

//===============================================================================================================[COUNTING SPEED FROM SPEED INCREASE/DECREASE AND CATCHING UNEXPECTED VALUES]

ptr SubSpeedCalculation(int Speed, int TurnStack, int AccelerationStack, int Reverse, int ReturnInfo[])
{
  Speed = ModularTurn(Speed, TurnStack);                         //calculates the speed from stacks
  Speed = ModularAcceleration(Speed, AccelerationStack);
  //Serial.print("   STACKS - ");
  //Serial.print(TurnStack);
  //Serial.print("  ");
  //Serial.print(AccelerationStack);
  //Serial.print("   ");
  TurnStack = 0;                                                   //resets the stacks
  AccelerationStack = 0;
  //Serial.print("[");
  //Serial.print(Speed);
  //Serial.print("-");
  if(Speed>MaxPower)Speed = MaxPower;                             //keeps the speeds in boundaries
  if(Speed<ReverseMaxPower)Speed = ReverseMaxPower;
  Reverse = Speed*(-1);                                            //gets the negative value of speed (in case of speed being already negative it makes it positive so that reversing can work)
  //Serial.print(Speed);
  //Serial.print("]");
  ReturnInfo[0] = Speed;
  ReturnInfo[1] = TurnStack;
  ReturnInfo[2] = AccelerationStack;
  ReturnInfo[3] = Reverse;
  return ReturnInfo;
}

//===============================================================================================================[OVERWRITING THE CURRENT SPEED WITH NEW CALCULATED SPEED]

void EngineControl(int Speed, int Reverse, int Forward, int Backward)
{
  if(Speed >= 0)                                                                 //check for negative speed(going backwards)
    {
      analogWrite(Forward,Speed);                                                       //go forwards
      analogWrite(Backward,0);
    }else
    {
      analogWrite(Forward,0);                                                            //go backwards
      analogWrite(Backward,Reverse);
    }
}
