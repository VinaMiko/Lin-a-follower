#define LEFT_WHEEL_FORWARD 10 //A2 - 10 - left wheel forward - orange
#define LEFT_WHEEL_BACKWARDS 11 //A1 - 11 - left wheel backwards - orange

#define RIGHT_WHEEL_FORWARD 6 //B2 - 6 - right wheel forward - yellow
#define RIGHT_WHEEL_BACKWARDS 5 //B1 - 5 - right wheel backwards - yellow


int sensorPins[] = {A7,A6,A5,A4,A3,A2,A1,A0};

int sensors[] = {0,0,0,0,0,0,0,0};

void setup() {
  pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
  pinMode(LEFT_WHEEL_BACKWARDS, OUTPUT);

  pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
  pinMode(RIGHT_WHEEL_BACKWARDS, OUTPUT);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
}

void loop() {
  solveMaze();
}

void checkSensorsValues(){
  for(int i = 0; i < sizeof(sensorPins)/sizeof(sensorPins[0]);i++){
    if(analogRead(sensorPins[i])>700){
      sensors[i]=1;
        }
    else{
      sensors[i]=0;
        }
    }
}

 void solveMaze(){
    checkSensorsValues();
        if(sensors[3] && sensors[4] && (sensors[2]||sensors[5])){
            analogWrite(LEFT_WHEEL_FORWARD, 255);
            analogWrite(LEFT_WHEEL_BACKWARDS, 0);
            analogWrite(RIGHT_WHEEL_FORWARD, 255);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
          }
        else if(sensors[0]||sensors[1]){
         turnLeft();
          }
          else if(sensors[7]||sensors[6]){
         turnRight();
          }
        else if(sensors[5]){
            analogWrite(LEFT_WHEEL_FORWARD, 255);
            analogWrite(LEFT_WHEEL_BACKWARDS, 0);
            analogWrite(RIGHT_WHEEL_FORWARD, 210);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
            }
        else if(sensors[2]){
            analogWrite(LEFT_WHEEL_FORWARD, 210);
            analogWrite(LEFT_WHEEL_BACKWARDS, 0);
            analogWrite(RIGHT_WHEEL_FORWARD, 255);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
            }
        else if(sensors[4]&&sensors[3]){
            analogWrite(LEFT_WHEEL_FORWARD, 255);
            analogWrite(LEFT_WHEEL_BACKWARDS, 0);
            analogWrite(RIGHT_WHEEL_FORWARD, 255);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
          }
        else if(sensors[4] && !sensors[3]){
            analogWrite(LEFT_WHEEL_FORWARD, 255);
            analogWrite(LEFT_WHEEL_BACKWARDS, 0);
            analogWrite(RIGHT_WHEEL_FORWARD, 245);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
            }
        else if(sensors[3] && !sensors[4]){
            analogWrite(LEFT_WHEEL_FORWARD, 245);
            analogWrite(LEFT_WHEEL_BACKWARDS, 0);
            analogWrite(RIGHT_WHEEL_FORWARD, 255);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
            }
        else{
            analogWrite(LEFT_WHEEL_FORWARD, 0);
            analogWrite(LEFT_WHEEL_BACKWARDS, 255);
            analogWrite(RIGHT_WHEEL_FORWARD, 0);
            analogWrite(RIGHT_WHEEL_BACKWARDS, 255);
            }
        solveMaze();
}


void turnRight(){
  while(!(sensors[4] || sensors[3])){
    checkSensorsValues();
    analogWrite(LEFT_WHEEL_FORWARD, 255);
    analogWrite(LEFT_WHEEL_BACKWARDS, 0);
    analogWrite(RIGHT_WHEEL_FORWARD, 0);
    analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
  }
}

void turnLeft(){
  while(!(sensors[4] || sensors[3])){
    checkSensorsValues();
    analogWrite(LEFT_WHEEL_FORWARD, 0);
    analogWrite(LEFT_WHEEL_BACKWARDS, 0);
    analogWrite(RIGHT_WHEEL_FORWARD, 255);
    analogWrite(RIGHT_WHEEL_BACKWARDS, 0);
  }
}
