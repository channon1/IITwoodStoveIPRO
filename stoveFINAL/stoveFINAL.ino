#include <SPI.h>

//required for thermos
#include <Adafruit_MAX31855.h>

//pins 3-11 for thermocouples
#define DO1   3
#define CS1   4
#define CLK1  5
#define DO2   6
#define CS2   7
#define CLK2  8
#define DO3   9
#define CS3   11
#define CLK3  12
#define O2    A0
#define COPIN A4
#define DUSTPIN A1
#define potPin A7


int pwmPin = 12;

//initialize thermocouples
Adafruit_MAX31855 thermocouple1(CLK1, CS1, DO1);
Adafruit_MAX31855 thermocouple2(CLK2, CS2, DO2);
Adafruit_MAX31855 thermocouple3(CLK3, CS3, DO3);
//OneWire  ds1(9);  // on pin 2 (a 4.7K resistor is necessary)

int measurePin = A1;
int ledPower = 1;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

int sensorValue;


void setup(void) {
  Serial.begin(9600);//start serial communication
  pinMode(ledPower,OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(measurePin,INPUT);
  pinMode(53,OUTPUT);
  digitalWrite(53,HIGH);
}

//every 4 seconds.. ish
void loop(void) {
  delay(2000);

  Serial.print("A"); //ABSerialProtocol

  getThermos();
  Serial.print(";");

  getO2();
  Serial.print(";");

  dustSensor();
  Serial.print(";");

  getCO();

  Serial.print(";");
  setFan();
  Serial.println("B"); 


  delay(1000);
}

// hard code fan to be at 100%

void setFan() {
  int val = analogRead(potPin);
  val = 100;
  //Serial.print(val); Serial.print(";");
  //val = val / 10.23 + 1; //1-100
  Serial.print(val); 
  //Serial.print(";");
  if(goFanGo(val)) {
    Serial.println("\nFan not Responding"); 
  }
}

// controlls percent 0-100

int goFanGo(int power) {
  DDRB |= (1<<DDB6);          //PB6 to output
  TCCR1A = 0b00110001;        //Setup Timer 1 for phase & frequency correct PWM (didn't feel like writing out all the
  TCCR1B = 0b00010001;            //effected bits, see data sheet, sorry), output on PB2
  OCR1A = 320;                //OCR1A is TOP value (turnaround), sets freq to 25kHz
  if((power <= 0) | (power > 100)) OCR1B = 320;
  else OCR1B = 320 - map(power, 0, 100, 200, 320);  //PB2 is set OCR1B value on up count and cleared on down count

  return 0;
}

//one of the thermos is broken

void getThermos(void) {
  //Serial.print(thermocouple1.readFarenheit());
  //Serial.print(";");
  //Serial.print(thermocouple1.readCelsius());
  //Serial.print(";");
  Serial.print(thermocouple2.readFarenheit());
  Serial.print(";");
  Serial.print(thermocouple2.readCelsius());
}

//doesnt work - thought to be a wire issue

void dustSensor() {
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024);

  dustDensity = 0.17 * calcVoltage - 0.1;

  Serial.print(dustDensity);
}

/*
 * CO sensor is not calibrated (needs CO meter) 
 * M7 sensor
 * It shows correlation but unknown scale
 */

void getCO() {
  sensorValue = analogRead(COPIN);       // read analog input pin 4
  Serial.print(sensorValue, DEC);  // prints the value read
}


/* ECONOX ECONOX ECONOX
 * 
 * O2 Does not work currently. 
 * The O2 sensor reports a mv value.
 * previous data was taken from manual readings
 */
 
void getO2(void) {
  delay(200); // delay because of reference change
  analogReference(INTERNAL1V1); // change reference voltage to measure 0 - 1.1v
  delay(200); // delay because of reference change
  double reading;
  reading = double(analogRead(O2)); //read and make double type
  reading *= (1.1/1023); // calc millivolts
  Serial.print(reading); 
  delay(100);  // delay because of reference change
  analogReference(DEFAULT);
  delay(200);  // delay because of reference change
}


