#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); //instance of rfid class

MFRC522::MIFARE_Key key;

int code[][4] = {{67, 82, 110, 139}}; //this is the stored UID
int buzzer = 2;
int moto1 = 7;
int moto2 = 6;
int moto3 = 5;
int moto4 = 4;
boolean lock = true;
int stage = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin(); //intialize SPI bus
  rfid.PCD_Init(); //initializing MFRC522
  
  pinMode(buzzer, OUTPUT); //initialize LED
  pinMode(moto1, OUTPUT); //initialize motors
  pinMode(moto2, OUTPUT);
  pinMode(moto3, OUTPUT);
  pinMode(moto4, OUTPUT);
}

void loop() { //generic looping function
  if (rfid.PICC_IsNewCardPresent()) { //if card is by reader, activate rfid reader
    readCode();
  }
  delay(100);
}

void readCode() {
  if (Serial.available() > 0) { //meant to be used for reading rfid codes from node.js database (incomplete)
    int num = Serial.parseInt();
    boolean added = false;
    int a = 0;
    int i = 0;
    while(!added) { //runs through the code array and fills in empty spaces 
      if(code[a][i] == 0){
        code[a][i] = num;
        added = true;
      } else {
        i++;
        if(i == 5){
          i = 0;
          a++;
        }
      }
    }
  }
  
  rfid.PICC_ReadCardSerial(); //displays rfid card information
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  Serial.println("Scanned PICC's UID:");
  printCode(rfid.uid.uidByte, rfid.uid.size);

  boolean match = false;
  int correct = 0;
  for (int a = 0; a < (sizeof(code)/sizeof(code[0])); a++) { //for each rfid code in the code array
    int i = 0;
    while (i < rfid.uid.size) { //check the values of the current code with the given code
      if (rfid.uid.uidByte[i] != code[a][i]) {
        correct = 0;
      } else { //if all 4 are correct, code is correct and break from loops
        correct++;
      }
      if (correct == 4){
        match = true;
        break;
      }
      i++;
    }
    if (correct == 4) {
      break;
    }
  }

  if (match) {
    Serial.println("\nGood Card!");
    for (int r = 0; r < 3000; r++) {
      runMotor(); //moving the gears activated by the motor
      if (lock) { 
        stage++; 
      } else { 
        stage--; 
      } 
      if (stage > 7) { 
        stage = 0; 
      } 
      if (stage < 0) { 
        stage = 7; 
      } 
    }
    if (lock) { //switching mechanism, if the lock is locked unlock it, if unlocked lock it
      lock = false;
    } else {
      lock = true;
    }
  } else {
    Serial.println("\nCard Not Registered");
    tone(buzzer, 1000, 500); //if the card is wrong set off buzzer
  }

  //Halt PICC
  rfid.PICC_HaltA();

  //Stop encryption
  rfid.PCD_StopCrypto1();
}

void printCode(byte * buffer, byte bufferSize) { //prints out the uid code each piece at a time
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0":" ");
    Serial.print(buffer[i], DEC);
  }
}

void runMotor() { //moves the motor gears in a sequencial order
  switch(stage){ 
   case 0: 
     digitalWrite(moto1, LOW);  
     digitalWrite(moto2, LOW); 
     digitalWrite(moto3, LOW); 
     digitalWrite(moto4, HIGH); 
   break;  
   case 1: 
     digitalWrite(moto1, LOW);  
     digitalWrite(moto2, LOW); 
     digitalWrite(moto3, HIGH); 
     digitalWrite(moto4, HIGH); 
   break;  
   case 2: 
     digitalWrite(moto1, LOW);  
     digitalWrite(moto2, LOW); 
     digitalWrite(moto3, HIGH); 
     digitalWrite(moto4, LOW); 
   break;  
   case 3: 
     digitalWrite(moto1, LOW);  
     digitalWrite(moto2, HIGH); 
     digitalWrite(moto3, HIGH); 
     digitalWrite(moto4, LOW); 
   break;  
   case 4: 
     digitalWrite(moto1, LOW);  
     digitalWrite(moto2, HIGH); 
     digitalWrite(moto3, LOW); 
     digitalWrite(moto4, LOW); 
   break;  
   case 5: 
     digitalWrite(moto1, HIGH);  
     digitalWrite(moto2, HIGH); 
     digitalWrite(moto3, LOW); 
     digitalWrite(moto4, LOW); 
   break;  
     case 6: 
     digitalWrite(moto1, HIGH);  
     digitalWrite(moto2, LOW); 
     digitalWrite(moto3, LOW); 
     digitalWrite(moto4, LOW); 
   break;  
   case 7: 
     digitalWrite(moto1, HIGH);  
     digitalWrite(moto2, LOW); 
     digitalWrite(moto3, LOW); 
     digitalWrite(moto4, HIGH); 
   break;  
   default: 
     digitalWrite(moto1, LOW);  
     digitalWrite(moto2, LOW); 
     digitalWrite(moto3, LOW); 
     digitalWrite(moto4, LOW); 
   break;  
 }
 delay(1); 
}
















