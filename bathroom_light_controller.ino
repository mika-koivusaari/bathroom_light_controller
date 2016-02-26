
const int LDRPIN=A0;
const int PWM1PIN=10;
const int PWM2PIN=11;
const int FORCELIGHTPIN=5;
/* muuttujat
 * pwm tavoite
 * pwm nykyinen
 */
 
int pwm1Goal=0;
int pwm1Current=0;
int pwm2Goal=0;
int pwm2Current=0;

//value of the LDR
int ldrValue=0;
//value of the force light switch
int forceLightValue=LOW;

//high values of the led strips, read on startup
int pwm1High=255;
int pwm2High=255;

//low values of the led strips, read on startup
int pwm1Low=50;
int pwm2low=50;

//value of the ldr between dark and light
int ldrDark=800;

int debug=true;

void setup() {
  // start serial port
  Serial.begin(9600);
  Serial.println("");
  //lue pimeän raja
  //lue kanavan 1 pwm (kirkas ja hämärä)
  //lue kanavan 2 pwm (kirkas ja hämärä)
  
  //aseta ldr lukupin
  pinMode(LDRPIN,INPUT);
  //aseta pwm kanavat 1 ja 2
  pinMode(PWM1PIN,OUTPUT);
  pinMode(PWM2PIN,OUTPUT);
  //aseta kytkimen luku pin
  pinMode(FORCELIGHTPIN,INPUT);
}

void loop() {
  //lue kirkkaaksi pakottavan kytkimen tila
  forceLightValue=digitalRead(FORCELIGHTPIN);
  //lue ldr
  ldrValue=analogRead(LDRPIN);

  if (debug) {
    Serial.print("Forcelight=");
    Serial.print(forceLightValue==HIGH?"HIGH":"LOW");
    Serial.print(" ldrValue=");
    Serial.println(ldrValue);
  }  
  
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
    pwm2Goal=pwm2low;
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

  Serial.print("pwm1Current=");
  Serial.print(pwm1Current);
  Serial.print(" pwm2Current=");
  Serial.println(pwm2Current);
  delay(100);  
  
}
