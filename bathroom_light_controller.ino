#include <EEPROM.h>

const int LDRPIN=A0;
const int PWM1PIN=10;
const int PWM2PIN=11;
const int FORCELIGHTPIN=5;

const String GET = "GET";
const String HELP = "HELP";
const String LOAD = "LOAD";
const String SAVE = "SAVE";

const int MAX_CMD_LENGTH = 20;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/* muuttujat
 * pwm tavoite
 * pwm nykyinen
 */
 
byte pwm1Goal=0;
byte pwm1Current=0;
byte pwm2Goal=0;
byte pwm2Current=0;

//value of the LDR
int ldrValue=0;
//value of the force light switch
int forceLightValue=LOW;

//high values of the led strips, read on startup
byte pwm1High=255;
byte pwm2High=255;

//low values of the led strips, read on startup
byte pwm1Low=50;
byte pwm2Low=50;

//value of the ldr between dark and light
int ldrDark=800;

int debug=true;
int loopDelay=50;

//Temporary variable for commands data.
int iTmp=0;
byte bTmp;

void setup() {
  // start serial port
  Serial.begin(9600);
  Serial.println("");

  inputString.reserve(MAX_CMD_LENGTH);

  readConfig();
    
  //aseta ldr lukupin
  pinMode(LDRPIN,INPUT);
  //aseta pwm kanavat 1 ja 2
  pinMode(PWM1PIN,OUTPUT);
  pinMode(PWM2PIN,OUTPUT);
  //aseta kytkimen luku pin
  pinMode(FORCELIGHTPIN,INPUT);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    
    if (inputString==GET) {
      Serial.print("Forcelight=");
      Serial.println(forceLightValue==HIGH?"HIGH":"LOW");
      Serial.print("ldrValue=");
      Serial.println(ldrValue);
      printConfig();
    }
    else if (inputString==HELP) {
      Serial.println("Commands:");
      Serial.println("GET");
      Serial.println("SET");
    }
    else if (inputString==LOAD) {
      readConfig();
      printConfig();
    }
    else if (inputString==SAVE) {
      writeConfig();
    }
    else if (inputString.startsWith("SET")) {
      //SET LDRDARK x
      if (inputString.length()>=11 && inputString.startsWith("SET LDRDARK")) {
        //Check that there is a parameter after command.
        if (inputString.length()>12) {
          iTmp=inputString.substring(12).toInt();
          if (iTmp>=0 && iTmp<=1023) {
            ldrDark=iTmp;
            Serial.print("LDRDARK set to ");
            Serial.println(ldrDark);
          } else {
            Serial.println("LDRDARK must be between 0 - 1023.");
          }
        } else {
          Serial.println("SET LDRDARK must have a parameter between 0 - 1023.");
        }
      //SET PWM1LOW x
      } else if (inputString.length()>=11 && inputString.startsWith("SET PWM1LOW")) {
        //Check that there is a parameter after command.
        if (inputString.length()>12) {
          iTmp=inputString.substring(12).toInt();
          if (iTmp>=0 && iTmp<=255) {
            pwm1Low=iTmp;
            Serial.print("PWM1LOW set to ");
            Serial.println(pwm1Low);
          } else {
            Serial.println("PWM1LOW must be between 0 - 255.");
          }
        } else {
          Serial.println("SET PWM1LOW must have a parameter between 0 - 255.");
        }
      //SET PWM2LOW x
      } else if (inputString.length()>=11 && inputString.startsWith("SET PWM2LOW")) {
        //Check that there is a parameter after command.
        if (inputString.length()>12) {
          iTmp=inputString.substring(12).toInt();
          if (iTmp>=0 && iTmp<=255) {
            pwm2Low=iTmp;
            Serial.print("PWM2LOW set to ");
            Serial.println(pwm2Low);
          } else {
            Serial.println("PWM2LOW must be between 0 - 255.");
          }
        } else {
          Serial.println("SET PWM2LOW must have a parameter between 0 - 255.");
        }
      //SET PWM1HIGH x
      } else if (inputString.length()>=12 && inputString.startsWith("SET PWM1HIGH")) {
        //Check that there is a parameter after command.
        if (inputString.length()>13) {
          iTmp=inputString.substring(13).toInt();
          if (iTmp>=0 && iTmp<=255) {
            pwm2Low=iTmp;
            Serial.print("PWM1HIGH set to ");
            Serial.println(pwm1High);
          } else {
            Serial.println("PWM1HIGH must be between 0 - 255.");
          }
        } else {
          Serial.println("SET PWM1HIGH must have a parameter between 0 - 255.");
        }
      } else {
       Serial.println("Unkown command after SET, must be LDRDARK, PWM1HIGH, PWM2HIGH, PWM1LOW, PWM2LOW.");
      } 
    } else {
      Serial.println("Syntax error:");
      Serial.println("Try HELP");
    }
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  //lue kirkkaaksi pakottavan kytkimen tila
  forceLightValue=digitalRead(FORCELIGHTPIN);
  //lue ldr
  ldrValue=analogRead(LDRPIN);
  
  //make light bright if force light is on
  if (forceLightValue==HIGH) {
    //aseta pwmtavoite kirkkaaksi
    pwm1Goal=pwm1High;
    pwm2Goal=pwm2High;
  } else if (ldrValue>ldrDark) { //if hallway if lighted then make lights bright
    pwm1Goal=pwm1High;
    pwm2Goal=pwm2High;
  } else {//otherwise make light dim
    pwm1Goal=pwm1Low;
    pwm2Goal=pwm2Low;
  }
  
  if (pwm1Current>pwm1Goal)
    pwm1Current--;
  else if (pwm1Current<pwm1Goal)
    pwm1Current++;

  if (pwm2Current>pwm2Goal)
    pwm2Current--;
  else if (pwm2Current<pwm2Goal)
    pwm2Current++;

  analogWrite(PWM1PIN,pwm1Current);
  analogWrite(PWM2PIN,pwm2Current);

/*  Serial.print("pwm1Current=");
  Serial.print(pwm1Current);
  Serial.print(" pwm2Current=");
  Serial.println(pwm2Current);*/
  delay(loopDelay);  
  
}

void readConfig() {
  //get config versio, 255=no config written
  EEPROM.get(0,bTmp);
  if (debug) {
    Serial.print("Config version=");
    Serial.println(bTmp);
  }
  if (bTmp!=255) {
    EEPROM.get(sizeof(bTmp),ldrDark);
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark),pwm1Low);
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark)+sizeof(pwm1Low),pwm1High);
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High),pwm2Low);
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low),pwm2High);
  }
}

void writeConfig() {
  EEPROM.put(0,(byte)1);

  EEPROM.put(sizeof(byte),ldrDark);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark),pwm1Low);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low),pwm1High);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High),pwm2Low);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low),pwm2High);
}

void printConfig() {
  Serial.print("loopDelay=");
  Serial.println(loopDelay);
  Serial.print("pwm1Low=");
  Serial.println(pwm1Low);
  Serial.print("pwm1High=");
  Serial.println(pwm1High);
  Serial.print("pwm2Low=");
  Serial.println(pwm2Low);
  Serial.print("pwm2High=");
  Serial.println(pwm2High);
  Serial.print("ldrDark=");
  Serial.println(ldrDark);
}
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // add it to the inputString:
      inputString += inChar;
    }
  }
}

