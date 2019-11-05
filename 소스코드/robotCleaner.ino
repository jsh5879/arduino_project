#define HC06 Serial3
int trigPin = 3;
int echoPin = 2;
long long time_previous, time_current;

int Enable1 = 38;
int Enable2 = 40;
int PWM1 = 9;
int PWM2 = 10;
int DIR1 = 39;
int DIR2 = 41;

boolean direction = true;


int measurePin = 0; //Connect dust sensor to Arduino A0 pin
int ledPower = 4;   //Connect 3 led driver pins of dust sensor to Arduino D2
int ledPin = 7;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void goStraight(void)
{
  Serial.println("go Straight");
  digitalWrite(Enable1, HIGH);
  digitalWrite(Enable2, HIGH);

  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, 255);
  delay(30);
  digitalWrite(DIR2, LOW);
  analogWrite(PWM2, 255);
  delay(30);

  delay(1000);
  digitalWrite(Enable1, LOW);
  digitalWrite(Enable2, LOW);
}

void rotate(void) 
{
  Serial.println("rotate");
  digitalWrite(Enable1, HIGH);
  digitalWrite(Enable2, HIGH);

  digitalWrite(DIR1, LOW);
  analogWrite(PWM1, 255);
  delay(30);
  digitalWrite(DIR2, HIGH);
  analogWrite(PWM2, 255);
  delay(30); 

  delay(1000);
  digitalWrite(Enable1, LOW);
  digitalWrite(Enable2, LOW);
}

float distance(void)
{
  digitalWrite(trigPin,HIGH);
  delay(10);
  digitalWrite(trigPin,LOW);
  float duration = pulseIn(echoPin,HIGH);
  float distance = duration * 340 / 10000 /2;
  Serial.println(distance);

  return distance;
}

void powerOn(void) {
  while(1) {
    Serial.println("Power On.");
    goStraight();
    
    if( distance() < 10 ) {
      Serial.println("Rotate.");
      rotate();
    }

    if( HC06.available()){
      char data1 = HC06.read();
      if( data1 == '1')
        return;
    }
  }
  return;
}

void timer(void) {
  while(true){
    time_current = millis();
    if(time_current - time_previous >= 10000)
      break;
    Serial.println("Timer on."); 
    goStraight();
    
    if( distance() < 10 ) {
      Serial.println("Rotate.");
      rotate();
    }
    if( HC06.available()){
      char data2 = HC06.read();
      if( data2 == '0')
        break;
    }
   }
  return;
}

void powerOff(void){
  
}
int DustSensor(){
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 200);

  dustDensity = 0.17 * calcVoltage - 0.1;

  Serial.print("Raw Signal Value (0-200): ");
  Serial.print(voMeasured);
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity); // unit: mg/m3

  if(dustDensity >= 0.5){
    delay(1000);
    return 3;
  }
  else
    return 0;
}



void setup()
{
  Serial.begin(9600);
  HC06.begin(9600);

  pinMode(Enable1, OUTPUT);
  pinMode(Enable2, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR2, OUTPUT);

  pinMode(ledPower,OUTPUT);
  pinMode(ledPin,OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(5, OUTPUT);

  time_previous = millis();
}

void loop()
{
  Serial.println("Normal Mode");
                           
  while(HC06.available())
  {
    char data = HC06.read();
    Serial.println(data);
    time_previous = millis();
    if( data == '1')
      powerOn();
    if( data == '2')
      timer();
  }
  if ( DustSensor() == 3){
    Serial.println("Dust mode");
    time_previous = millis();
    timer();
  }

}