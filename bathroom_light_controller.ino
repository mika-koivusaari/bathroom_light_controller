#include <EEPROM.h>

const int LDRPIN=A0;
const int PWM1PIN=9;
const int PWM2PIN=10;
const int PWM3PIN=11;
const int FORCELIGHTPIN=5;

const String GET = "GET";
const String HELP = "HELP";
const String LOAD = "LOAD";
const String SAVE = "SAVE";
const String SET = "SET";
const String SET_LDRDARK = "SET LDRDARK";
const String SET_PWM1LOW = "SET PWM1LOW";
const String SET_PWM1HIGH = "SET PWM1HIGH";
const String SET_PWM2LOW = "SET PWM2LOW";
const String SET_PWM2HIGH = "SET PWM2HIGH";
const String SET_PWM3LOW = "SET PWM3LOW";
const String SET_PWM3HIGH = "SET PWM3HIGH";
const String SET_LOOPDELAY = "SET LOOPDELAY";

const int MAX_CMD_LENGTH = 20;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
 
byte pwm1Goal=0;
byte pwm1Current=0;
byte pwm2Goal=0;
byte pwm2Current=0;
byte pwm3Goal=0;
byte pwm3Current=0;

//value of the LDR
int ldrValue=0;
//value of the force light switch
int forceLightValue=LOW;

//high values of the led strips, read on startup
byte pwm1High=255;
byte pwm2High=255;
byte pwm3High=255;

//low values of the led strips, read on startup
byte pwm1Low=50;
byte pwm2Low=50;
byte pwm3Low=50;

//value of the ldr between dark and light
int ldrDark=800;

int debug=true;
byte loopDelay=50;

//Temporary variable for commands data.
int iTmp=0;
byte bTmp;

void setup() {
  // start serial port
  Serial.begin(9600);
  Serial.println("");

  inputString.reserve(MAX_CMD_LENGTH);

  readConfig();
  printConfig();
    
  //set ldr pin
  pinMode(LDRPIN,INPUT);
  //set pwm channels
  pinMode(PWM1PIN,OUTPUT);
  pinMode(PWM2PIN,OUTPUT);
  pinMode(PWM3PIN,OUTPUT);
  //set forcelight switch input pin
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
    else if (inputString.startsWith(SET)) {
      set();
    } else {
      Serial.println("Syntax error:");
      Serial.println("Try HELP");
    }
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  //Read if force light switch is on
  forceLightValue=digitalRead(FORCELIGHTPIN);
  //Read ldr
  ldrValue=analogRead(LDRPIN);
  
  //make light bright if force light is on
  if (forceLightValue==HIGH) {
    //set pwm goal to high
    pwm1Goal=pwm1High;
    pwm2Goal=pwm2High;
    pwm3Goal=pwm3High;
  } else if (ldrValue>ldrDark) { //if hallway if lighted then make lights bright
    pwm1Goal=pwm1High;
    pwm2Goal=pwm2High;
    pwm3Goal=pwm3High;
  } else {//otherwise make light dim
    pwm1Goal=pwm1Low;
    pwm2Goal=pwm2Low;
    pwm3Goal=pwm3Low;
  }

  //advance pwm values to goal
  if (pwm1Current>pwm1Goal)
    pwm1Current--;
  else if (pwm1Current<pwm1Goal)
    pwm1Current++;

  if (pwm2Current>pwm2Goal)
    pwm2Current--;
  else if (pwm2Current<pwm2Goal)
    pwm2Current++;

  if (pwm3Current>pwm3Goal)
    pwm3Current--;
  else if (pwm3Current<pwm3Goal)
    pwm3Current++;

  analogWrite(PWM1PIN,pwm1Current);
  analogWrite(PWM2PIN,pwm2Current);
  analogWrite(PWM3PIN,pwm3Current);

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
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low)+sizeof(pwm2High),pwm3Low);
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low)+sizeof(pwm2High)+sizeof(pwm3Low),pwm3High);
    EEPROM.get(sizeof(bTmp)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low)+sizeof(pwm2High)+sizeof(pwm3Low)+sizeof(pwm3High),loopDelay);
    if (loopDelay>50) {
      loopDelay=50;
    }
    if (loopDelay<0) {
      loopDelay=0;
    }
  }
}

void writeConfig() {
  EEPROM.put(0,(byte)1);

  EEPROM.put(sizeof(byte),ldrDark);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark),pwm1Low);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low),pwm1High);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High),pwm2Low);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low),pwm2High);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low)+sizeof(pwm2High),pwm3Low);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low)+sizeof(pwm2High)+sizeof(pwm3Low),pwm3High);
  EEPROM.put(sizeof(byte)+sizeof(ldrDark)+sizeof(pwm1Low)+sizeof(pwm1High)+sizeof(pwm2Low)+sizeof(pwm2High)+sizeof(pwm3Low)+sizeof(pwm3High),loopDelay);
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
  Serial.print("pwm3Low=");
  Serial.println(pwm3Low);
  Serial.print("pwm3High=");
  Serial.println(pwm3High);
  Serial.print("ldrDark=");
  Serial.println(ldrDark);
}

void set() {
  //SET LDRDARK x
  if (inputString.length()>=11 && inputString.startsWith(SET_LDRDARK)) {
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
  } else if (inputString.length()>=11 && inputString.startsWith(SET_PWM1LOW)) {
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
  } else if (inputString.length()>=11 && inputString.startsWith(SET_PWM2LOW)) {
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
  //SET PWM3LOW x
  } else if (inputString.length()>=11 && inputString.startsWith(SET_PWM3LOW)) {
    //Check that there is a parameter after command.
    if (inputString.length()>12) {
      iTmp=inputString.substring(12).toInt();
      if (iTmp>=0 && iTmp<=255) {
        pwm3Low=iTmp;
        Serial.print("PWM3LOW set to ");
        Serial.println(pwm3Low);
      } else {
        Serial.println("PWM3LOW must be between 0 - 255.");
      }
    } else {
      Serial.println("SET PWM3LOW must have a parameter between 0 - 255.");
    }
  //SET PWM1HIGH x
  } else if (inputString.length()>=12 && inputString.startsWith(SET_PWM1HIGH)) {
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
  //SET PWM2HIGH x
  } else if (inputString.length()>=12 && inputString.startsWith(SET_PWM2HIGH)) {
    //Check that there is a parameter after command.
    if (inputString.length()>13) {
      iTmp=inputString.substring(13).toInt();
      if (iTmp>=0 && iTmp<=255) {
        pwm2Low=iTmp;
        Serial.print("PWM2HIGH set to ");
        Serial.println(pwm2High);
      } else {
        Serial.println("PWM2HIGH must be between 0 - 255.");
      }
    } else {
      Serial.println("SET PWM2HIGH must have a parameter between 0 - 255.");
    }
  //SET PWM3HIGH x
  } else if (inputString.length()>=12 && inputString.startsWith(SET_PWM3HIGH)) {
    //Check that there is a parameter after command.
    if (inputString.length()>13) {
      iTmp=inputString.substring(13).toInt();
      if (iTmp>=0 && iTmp<=255) {
        pwm3Low=iTmp;
        Serial.print("PWM3HIGH set to ");
        Serial.println(pwm3High);
      } else {
        Serial.println("PWM3HIGH must be between 0 - 255.");
      }
    } else {
      Serial.println("SET PWM3HIGH must have a parameter between 0 - 255.");
    }
  //SET LOOPDELAY x
  } else if (inputString.length()>=13 && inputString.startsWith(SET_LOOPDELAY)) {
    //Check that there is a parameter after command.
    if (inputString.length()>13) {
      iTmp=inputString.substring(13).toInt();
      if (iTmp>=0 && iTmp<=255) {
        loopDelay=iTmp;
        Serial.print("LOOPDELAY set to ");
        Serial.println(loopDelay);
      } else {
        Serial.println("LOOPDELAY must be between 0 - 255.");
      }
    } else {
      Serial.println("SET PWM2HIGH must have a parameter between 0 - 255.");
    }
  } else {
   Serial.println("Unkown command after SET, must be LDRDARK, PWM1HIGH, PWM2HIGH, PWM1LOW, PWM2LOW.");
  } 
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

