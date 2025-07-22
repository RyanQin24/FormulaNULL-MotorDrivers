#include <Arduino.h>
// #include <stdio.h>                      //TESTING, COMMENT OUT LATER

//Motor pin declarations
int en_A{6};                               //Digital PWM pin -> speed control for the motor
int en_B{3};             

int in1_A{9};                              //control pin for  motor polarity

int chl1{10};                              //reading values for motor power channel from the RC reciever

int enable{11};                            //Safety Motor Driver deactivation pin

int va_pwm{0};                             //reading pwm ranging
int va2_pwm{0};                            //value after calculations done on it
float va2_pwmB{0};                         //speed value for the second motor with different rpm (shifted from an unknown RPM to 25000rpm)
float down_shift{0.76};                       //ratio that pwmB is being shifted by (assuming that the unknown rpm is 33000)

int va_low{1440};                          //low value of the undefined range
int va_high{1560};                         //high value of the undefined range
int lowest{970};                           //highest value for the receiver
int highest{1940};                         //lowest value for the receiver 

//controls for changing directions 

bool curr_dir{true};                       //current direction, where true is forwards and false is backwards 
bool prev_dir{true};                       //previous direction

void setup() {
  //Setup the motor control pins to output and the power channel to input
  pinMode(en_A, OUTPUT);
  pinMode(in1_A, OUTPUT);
  // pinMode(in2_A, OUTPUT);
  pinMode(en_B, OUTPUT);
  pinMode(chl1, INPUT);

  //Turning all the motors to LOW and deactivating driver to begin with
  digitalWrite(in1_A, LOW);
  digitalWrite(in2_A, LOW);
  digitalWrite(enable, HIGH);


  bool checkSwitch();
  checkSwitch();
  Serial.begin(9600);                   //TESTING, COMMENT OUT LATER
  // va_pwm = 1000;                     //TESTING, COMMENT OUT LATER
}

bool checkSwitch(){
  prev_dir = curr_dir;
  if (va2_pwm >= 0){
    curr_dir = true;
  } else {
    curr_dir = false;
  } 
  if (curr_dir != prev_dir){ 
    return true;
  } else{
    return false;
  }
}

// TESTING, COMMENT OUT LATER
void testSpeed(){            
  delay(10);

//     //speed tests
Serial.print("input value is: ");
Serial.println(va_pwm);
Serial.print("speed A is: ");
Serial.println(va2_pwm);
Serial.print("speed B is: ");
Serial.println(va2_pwmB);
//     //direction tests
Serial.print("current direction is: ");
Serial.println(curr_dir);
//     Serial.print("previous direction was: ");
//     Serial.println(prev_dir);
//     Serial.print("check switch triggered: ");
//     Serial.println(checkSwitch());
   
}


void relaySwitch(bool relayState){
  analogWrite(en_A, 0);
  analogWrite(en_B, 0);

  if (relayState != false) {
    digitalWrite(in1_A, LOW);
  } else {
    digitalWrite(in1_A, HIGH);
  }
  
}

void vectorControl(){
  if (va2_pwm >= 0) {
    analogWrite(en_A, va2_pwm);            //writing the speed when it is forwards
    analogWrite(en_B, va2_pwmB);


  } else {
    analogWrite(en_A, -(va2_pwm));         //writing the speed when it is backwards
    analogWrite(en_B, -(va2_pwmB)); 
  }
}

void loop() {
  testSpeed();                          //TESTING, COMMENT OUT LATER

  va_pwm = pulseIn(chl1, HIGH, 25000);     //recording the pulse width from 1000 to 2000
  va_pwm = constrain(va_pwm, lowest, highest);
  
  //code to normalize the recorded pulse width between (0, 255)
  if ((va_pwm > va_low && va_pwm < va_high)){
    digitalWrite(enable,HIGH);
    va2_pwm = 0;
    va2_pwmB =0;
  } else{
    if (va_pwm >= va_high){
      digitalWrite(enable,LOW);
      va2_pwm  = map(va_pwm, va_high, highest, 50, 255);
      va2_pwm = constrain(va2_pwm, 50, 255);
      va2_pwmB = map(va_pwm, va_high, highest, 50, 255*down_shift);
      va2_pwmB = constrain(va2_pwmB, 50, 255*down_shift);
    } else if(va_pwm <= va_low){
      digitalWrite(enable,LOW);
      va2_pwm  = map(va_pwm, lowest, va_low, -255, -50);
      va2_pwm = constrain(va2_pwm, -255, -50);
      va2_pwmB = map(va_pwm, lowest, va_low, -255*down_shift, -50);
      va2_pwmB =constrain(va2_pwm, -255*down_shift, -50);
    }else{
      //disabling motors if signals are invalid
      digitalWrite(enable,HIGH);
      va2_pwm = 0;
      va2_pwmB =0;
  } 
 
  if (checkSwitch() != false){
    if (va2_pwm >= 0) {
      relaySwitch(true);
    } else {
      relaySwitch(false);
    }
  }
vectorControl(); 
}

