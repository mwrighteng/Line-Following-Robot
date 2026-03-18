// GLOBAL VARIABLES
// Motor Driver Pin Numbers
int IN1 = 22;    // Right Motor Forward
int IN2 = 24;    // Right Motor Backward
int ENA = 3;     // Right Motor Speed Control (PWM)
int IN3 = 26;    // Left Motor Forward
int IN4 = 28;    // Left Motor Backward
int ENB = 2;     // Left Motor Speed Control (PWM)

// IR Sensor Pin Numbers (PWM)
int IR1 = 10;   // L3 (Far Left)
int IR2 = 9;    // L2
int IR3 = 8;    // L1
int IR4 = 7;    // C  (Center)
int IR5 = 6;    // R1
int IR6 = 5;    // R2
int IR7 = 4;    // R3 (Far Right)

// Turning Strengths (for corrections)
int base = 85;
int micro = base + 55;
int turns = base + 65;
int sharp = base + 75;

// Used for remembering the previous action taken for when the line is lost or when a pattern is unrecognized
int lastAction = 0;

// Used for stopping the wheels when the line is lost for some time
int counter = 0;


// Note: 'void setup' and 'void loop' are special functions that Arduino "knows" to run automatically. Any other functions created besides the setup and loop functions must be called manually!
void setup() {            // void setup runs ONCE on startup
  Serial.begin(9600);     // Starts Serial communication at a Baud rate of 9600. Baud is how often the signal can change per second.

  // Motor Pins Setup (as outputs)
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  // IR Sensor Pins Setup (as inputs)
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
  pinMode(IR6, INPUT);
  pinMode(IR7, INPUT);

  // Wheel Direction Setup
  digitalWrite(IN1, HIGH);  // Right Motor Forward
  digitalWrite(IN2, LOW); 
  digitalWrite(IN3, HIGH);  // Left Motor Forward
  digitalWrite(IN4, LOW);

  // Power Pins Setup
  pinMode(30,OUTPUT);
  pinMode(32,OUTPUT);
  pinMode(34,OUTPUT);
  pinMode(36,OUTPUT);
  pinMode(40,OUTPUT);
  pinMode(42,OUTPUT);
  pinMode(44,OUTPUT);
  pinMode(46,OUTPUT);
  pinMode(48,OUTPUT);
  pinMode(50,OUTPUT);
  pinMode(52,OUTPUT);
  digitalWrite(30,HIGH);
  digitalWrite(32,HIGH);
  digitalWrite(34,HIGH);
  digitalWrite(36,HIGH);
  digitalWrite(40,HIGH);
  digitalWrite(42,HIGH);
  digitalWrite(44,HIGH);
  digitalWrite(46,HIGH);
  digitalWrite(48,HIGH);
  digitalWrite(50,HIGH);
  digitalWrite(52,HIGH);

  // Ground Pins Setup
  pinMode(41,OUTPUT);
  pinMode(43,OUTPUT);
  pinMode(45,OUTPUT);
  pinMode(47,OUTPUT);
  pinMode(49,OUTPUT);
  pinMode(51,OUTPUT);
  pinMode(53,OUTPUT);
  digitalWrite(31,LOW);
  digitalWrite(33,LOW);
  digitalWrite(35,LOW);
  digitalWrite(37,LOW);
  digitalWrite(41,LOW);
  digitalWrite(43,LOW);
  digitalWrite(45,LOW);
  digitalWrite(47,LOW);
  digitalWrite(49,LOW);
  digitalWrite(51,LOW);
  digitalWrite(53,LOW);
  
  startupLights();
  }


void loop() {    // void loop runs over and over after startup
  // Read all IR sensors (Reads 1 on the line and 0 off the line)
  int L3 = digitalRead(IR1);  // Far Left
  int L2 = digitalRead(IR2);
  int L1 = digitalRead(IR3);
  int C = digitalRead(IR4);   // Middle
  int R1 = digitalRead(IR5);
  int R2 = digitalRead(IR6);
  int R3 = digitalRead(IR7);  // Far Right
  
  if (C == 1) {
    digitalWrite(30,HIGH);
    digitalWrite(32,HIGH);
    digitalWrite(34,HIGH);
    digitalWrite(36,HIGH);
  }
  else {
    digitalWrite(30,LOW);
    digitalWrite(32,LOW);
    digitalWrite(34,LOW);
    digitalWrite(36,LOW);
  }
  // Create Pattern Using Bitwise Operations
  int pattern = (L3 << 6) | (L2 << 5) | (L1 << 4) | (C  << 3) | (R1 << 2) | (R2 << 1) | (R3 << 0);

 // Displays 'pattern' in the Serial Monitor as a String so all 7 digits are seen.
  String patt = String(L3) + String(L2) + String(L1) + String(C) + String(R1) + String(R2) + String(R3);
  Serial.print(patt);
  //Serial.print(pattern, BIN); // BIN tells the Serial Monitor to read 'pattern' in a binary format

  // If the robot is off the line for too long (0b0000000), the counter will rise. It resets if a line is detected.
  if (pattern == 0b0000000) {
    counter += 1;
  }
  else {
    counter = 0;
  }

  // If the counter goes above 50 or the robot goes over a black box (0b1111111), the robot will stop.
  if (counter > 50 || pattern == 0b1111111) {
      stopMotors();
      while(true){}
    }
  
  // Read pattern -> Take action
  switch(pattern){

    case 0b0001000:
      forward();
      lastAction = 0;
      break;

    case 0b0000100:
      microRight();
      lastAction = 1;
      break;

    case 0b0000010:
      turnRight();
      lastAction = 2;
      break;

    case 0b0000001:
    case 0b0000011:
    case 0b0000111:
    case 0b0001110:
    case 0b0001111:
    case 0b0011111:
      sharpRight();  // For sharp turns (70-90 degree)
      lastAction = 3;  
      break;
    
    case 0b0010001:
    case 0b0001001:
    case 0b0001101:
    case 0b0000101:
      supersharpRight();  // For really sharp turns (30-45 degree)
      break;

    case 0b0010000:
      microLeft();
      lastAction = -1;
      break;

    case 0b0100000:
      turnLeft();
      lastAction = -2;
      break;
    
    case 0b1000000:
    case 0b1100000:
    case 0b1110000:
    case 0b0111000:
    case 0b1111000:
    case 0b1111100:
      sharpLeft();
      lastAction = -3;
      break;

    case 0b1000100:
    case 0b1001000: 
    case 0b1011000:
    case 0b1010000:
      supersharpLeft();
      break;

    // Fallback memory: If line was lost or is pattern was unrecognized, repeat last action taken
    default:
    if(lastAction == 0) forward();
    else if(lastAction == 1) microRight();
    else if(lastAction == 2) turnRight();
    else if(lastAction == 3) sharpRight();
    else if(lastAction == -1) microLeft();
    else if(lastAction == -2) turnLeft();
    else if(lastAction == -3) sharpLeft();
    break;
  }
}

// Motor Speed Control Functions
void forward(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 120);
  analogWrite(ENB, 118);
  Serial.println(String("   Forward"));
}

void microRight(){
  analogWrite(ENA, base-30);
  analogWrite(ENB, micro);
  Serial.println(String("   microRight"));
}

void microLeft(){
  analogWrite(ENA, micro);
  analogWrite(ENB, base-30);
  Serial.println(String("   microLeft"));
}

void turnRight(){
  analogWrite(ENA, base-80);
  analogWrite(ENB, turns);
  Serial.println(String("   turnRight"));
}

void turnLeft(){
  analogWrite(ENA, turns);
  analogWrite(ENB, base-80);
  Serial.println(String("   turnLeft"));
}

void sharpRight(){
  analogWrite(ENA, 0);
  analogWrite(ENB, sharp);
  Serial.println(String("   sharpRight"));
  delay(125);
}

void sharpLeft(){
  analogWrite(ENA, sharp);
  analogWrite(ENB, 0);
  Serial.println(String("   sharpLeft"));
  delay(125);
}

void supersharpRight(){
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 140);
  analogWrite(ENB, 140);
  Serial.println(String("   supersharpRight"));
  delay(750); // "Don't read anything while first turning for 750ms"
  
  while (digitalRead(IR3) != 1 && digitalRead(IR4) != 1 && digitalRead(IR5) != 1) {
    // Keep turning until L1, C, or R1 read the line
  }
  forward();
}

void supersharpLeft(){
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 140);
  analogWrite(ENB, 140);
  Serial.println(String("   supersharpLeft"));
  delay(750); // "Dont read anything while first turning for 750ms"
  
  while (digitalRead(IR3) != 1 && digitalRead(IR4) != 1 && digitalRead(IR5) != 1) {
    // Keep turning until L1, C, or R1 read the line
  }
  forward();
}

void stopMotors(){
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(30,LOW);   // Turn the LED off by making the voltage LOW
  digitalWrite(32,LOW);
  digitalWrite(34,LOW);
  digitalWrite(36,LOW);
  Serial.println(String("   stopMotors"));
}

void startupLights(){
  delay(6000);               // Initial delay
  int count = 0;             // Counting variable fo while loop
  while (count < 4){         // Run loop x - 1 times
    digitalWrite(30,HIGH);   // Turn the LED on (HIGH is the voltage level)
    digitalWrite(32,HIGH);
    digitalWrite(34,HIGH);
    digitalWrite(36,HIGH);
    delay(500);              // Wait for half a second
    digitalWrite(30,LOW);    // Turn the LED off by making the voltage LOW
    digitalWrite(32,LOW);
    digitalWrite(34,LOW);
    digitalWrite(36,LOW);
    delay(500);
    count += 1;              // Count up by 1 at end of loop
  }
}