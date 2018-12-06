//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Script to sort M&M's //////////////////////////////////////////
////////////////////// 06/05/2017 by Mohamed ABADI ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

// Initialize necessary libraries
#include <Stepper.h>
#include <Servo.h>

// Variables to use
#define csS0 4 // Color sensore pin S0 to arduino pin 4
#define csS1 7 // Color sensore pin S1 to arduino pin 7
#define csS2 11 // Color sensore pin S2 to arduino pin 11
#define csS3 10 // Color sensore pin S3 to arduino pin 10
#define csOutPin 9 // Color sensore pin OUT to arduino pin 9
#define csLedPin 13 // Color sensore pin LED to arduino pin 13
#define motorYPin6 6  //dir
#define motorYPin3 3  //step
#define EN 8          //ENABLE
#define motorXPin5 5  //dir
#define motorXPin2 2  //step
#define servoMoteurPin 12 //definir pin servo moteur

double depMT = 45/1.8 * 64 + 1.8; // <==> 45°C change this depending on the number of steps
double depMG = 60/1.8 * 64 + 1.8; // <==> 60°C change this depending on the number of steps

//                  0    1         2         3          4         5        6
//                  Red  Orange    Move      Yellow    Brown      Blue     Green
double matDep[6] = {0,   depMG,    2*depMG,  3*depMG,  4*depMG,   5*depMG};

Servo servoMotor;  // Make servoMotor object to control the servo motor

// Initialize of the Stepper library:
Stepper motorTremis(depMT, motorXPin5, motorXPin2);
Stepper motorGobelet(depMG, motorYPin6, motorYPin3);

// Setup code:
void setup() {
  Serial.begin(9600); // Initialize the Serial port:

  pinMode(EN, OUTPUT); // Initialize ENABLE function on CNC shield:
  digitalWrite(EN, LOW);

  pinMode(csS0, OUTPUT); // Initialize color sensore
  pinMode(csS1, OUTPUT);
  pinMode(csS2, OUTPUT);
  pinMode(csS3, OUTPUT);
  pinMode(csOutPin, INPUT);
  pinMode(csLedPin, OUTPUT);

  // Setting frequency-scaling to 100% = (HIGH, HIGH) or 20% = (HIGH, LOW)
  digitalWrite(csS0, HIGH);
  digitalWrite(csS1, LOW);

  // Utilise la broche "servoMoteurPin" pour le contrôle du servoMoteur
  servoMotor.attach(servoMoteurPin);
  
  motorTremis.setSpeed(500); // Set the motor speed at 500 RPMS:
  motorGobelet.setSpeed(1000); // Set the motor speed at 1000 RPMS:
}

// Put your main code here, to run repeatedly:
double depMotorGobelet[4] = {0, 0, 0, 0};
int color;
void loop() {  
  // leftRightMotorTremis();  
  color = readColor();
  Serial.print("color = "); Serial.println(color);
  //color = -1;
  if (color >= 0) {
    depMotorGobelet[3] = matDep[color];    
  } else {
    depMotorGobelet[3] = 0;
  }

  depMotorGobelet[0] = depMotorGobelet[1];
  depMotorGobelet[1] = depMotorGobelet[2];
  depMotorGobelet[2] = depMotorGobelet[3];
  depMotorGobelet[3] = 0;

  for (int j=0; j<4; j++) {
    Serial.print(depMotorGobelet[j]); Serial.print(", ");
  }
  Serial.println("");
  
  motorGobelet.step( depMotorGobelet[0] );
  delay(100);
  motorTremis.step(depMT);
  delay(800);
  motorGobelet.step( -depMotorGobelet[0] );
  delay(100);

  //moveServoMotor();
 
  Serial.println("------------------------------------------------");
} // End loop()


// Permet de lire les 3 composantes Rouge, Vert et Bleu : la couleur de l'échantillon et de renvoyer l'indice color
int readColor() {
  int rColorStrength = 0, gColorStrength = 0, bColorStrength = 0, color = 0;

  digitalWrite(csLedPin, HIGH);
  // Lets start by reading Red component of the color : S2 and S3 be sel LOW
  digitalWrite(csS2, LOW); digitalWrite(csS3, LOW);
  rColorStrength = getColorStrength(0, 255);

  // Read Green component of the color : S2 and S3 be sel HIGH
  digitalWrite(csS2, HIGH); digitalWrite(csS3, HIGH);
  gColorStrength = getColorStrength(0, 255);

  // Read Bleu component of the color : S2 and S3 be set LOW and HIGH respectively
  digitalWrite(csS2, LOW); digitalWrite(csS3, HIGH);
  bColorStrength = getColorStrength(0, 255);

  Serial.print(rColorStrength); Serial.print(", "); Serial.print(gColorStrength); Serial.print(", "); Serial.println(bColorStrength);

  color = getColor(rColorStrength, gColorStrength, bColorStrength);

  digitalWrite(csLedPin, LOW);
  delay(100);
  return color;
}

// Renvoie la valeur moyenne du color sensor sur 20 valeurs
int getColorStrength(int MIN, int MAX) {
  int colorStrength = 0;
  unsigned int pulseWidth = 0;
  for (int j=0; j<20; j++) {
    pulseWidth = pulseIn(csOutPin, LOW);
    colorStrength = colorStrength + map(pulseWidth, MIN, MAX, 255, 0);
  }
  colorStrength = (int) (colorStrength / 20);

  Serial.println(pulseWidth);
  return (colorStrength);
}

// Renvoie color : la couleur de l'échantillon. Avec color appartient {-1,0,1,2,3,4,5}
int getColor(int rColorStrength, int gColorStrength, int bColorStrength) {  
  int color = -1;
  if (rColorStrength > gColorStrength && rColorStrength > bColorStrength) {
    Serial.println("Rouge");
    if (134 <= rColorStrength && rColorStrength <= 152&& 32 <= gColorStrength && gColorStrength <= 48 && 80 <= bColorStrength && bColorStrength <= 97) {
      Serial.println("RED");
      color = 0;
    }
    if (gColorStrength > bColorStrength) {
      if (154 <= rColorStrength && rColorStrength <= 169 && 71 <= gColorStrength && gColorStrength <= 88 && 91 <= bColorStrength && bColorStrength <= 106) {
        Serial.println("ORANGE");
        color = 1;
      }
      if (164 <= rColorStrength && rColorStrength <= 180 && 132 <= gColorStrength && gColorStrength <= 151 && 110 <= bColorStrength && bColorStrength <= 129) {
        Serial.println("YELLOW");
        color = 3;
      }
      if (57 <= rColorStrength && rColorStrength <= 70 && 14 <= gColorStrength && gColorStrength <= 28 && 65 <= bColorStrength && bColorStrength <= 79) {
        Serial.println("BROWN");
        color = 4;
      }
    }
    if (bColorStrength > gColorStrength) {
      if (159 <= rColorStrength && rColorStrength <= 177 && 65 <= gColorStrength && gColorStrength <= 79 && 132 <= bColorStrength && bColorStrength <= 149) {
        Serial.println("MOVE");
        color = 2;
      }
    }
  }    
  if (gColorStrength > rColorStrength && gColorStrength > bColorStrength) {    
    Serial.println("Vert");
    color = -1;
  }
  if (bColorStrength > rColorStrength && bColorStrength > gColorStrength) {    
    Serial.println("BLEU");
    color = 5;
  }

  return (color);
} // End getColor

// Initialisation
void leftRightMotorTremis() {
  int dStep = 6 / 1.8 * 64;
  motorTremis.setSpeed(1000);
  for (int i = 0; i < 5; i++) {
    motorTremis.step(dStep);
    delay(100);
    motorTremis.step(-dStep);
    delay(100);
  }
  motorTremis.setSpeed(100);
}

// Faire tourner le servoMotor entre 0°C et 180°C
void moveServoMotor() {
  for(int i=0; i<=180; i=i+30){
    servoMotor.write(i);
    delay(50);                       // waits 15ms for the servo to reach the position    
  }
}
















