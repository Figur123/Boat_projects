#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Set the I2C address of your LCD (usually 0x27 or 0x3F)
#define LCD_ADDRESS 0x27


float calcTemp(int tempValue);
void initialDisplay();
void wantedTempUpdate();
void tempUpdate();
int tempReadPin = A3;
int tempValue = 0;
float temp = calcTemp(tempValue); // ((5V / 1024int) / 10mV/C )*tempValue
float prevTemp = temp;
int wantedTemp = 3;
float tempTresh = 1;

unsigned long lastTime = 0;
const unsigned long interval = 500;
const unsigned long shortInterval = 300;
unsigned long now;


//Joystick
const int VRXPin = A1;
const int VRYPin = A2;
int VRX;
int VRY;
int thresholdValue = 250;
const int isPressedPin = 15;
int isPressed = 0;
bool restart = 1.2;

//Servo
const int servoPin = 4;
int pos = 0;
Servo switcher;
void changeFreezerStatus(bool state);
bool freezerState = 0;

// Create the LCD object (16 columns, 2 rows)
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

void setup() {
  // Initialize I2C with custom pins (SDA = 2, SCL = 3)
  Wire.begin();
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();  // Turn on the backlight

  // Servo stuff
  switcher.attach(servoPin);


  // Display "Hello World!" on the first row
  lcd.setCursor(0, 0);  // column 0, row 0
  lcd.print("Bra Rumpe (-;");
  delay(2000);
  initialDisplay();
}

void loop() {
  //Temp shit
  tempValue = analogRead(tempReadPin);
  temp = calcTemp(tempValue);
  temp = (int)(temp * 10.0) / 10.0;


  //joystick shit
  VRX = analogRead(VRXPin);
  VRY = analogRead(VRYPin);
  isPressed = digitalRead(isPressedPin);

  now = millis();
  
  if (now - lastTime >= interval){
    if (VRY - 512 < -thresholdValue){
      wantedTemp -= 1;
      wantedTempUpdate();
      lastTime = now;
    }
    else if (VRY - 512 > thresholdValue){
      wantedTemp += 1;
      wantedTempUpdate();
      lastTime = now;
    }
    if (abs(temp - prevTemp) > 0.5){
      tempUpdate();
    }
    if (temp < wantedTemp-tempTresh && !freezerState){
      freezerState = 1;
      changeFreezerStatus(freezerState);
    }
    if (temp > wantedTemp+tempTresh && freezerState){
      freezerState = 0;
      changeFreezerStatus(freezerState);
    }


  }
  delay(10);
}

void initialDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CurrTemp = " + String(temp));
  lcd.setCursor(0,1);
  lcd.print("WantedTemp = " + String(wantedTemp));
}

float calcTemp(int tempValue){
  return ((5. / 1024.) / 0.010) * tempValue - 360.;
}

void tempUpdate(){
  lcd.setCursor(11,0);
  lcd.print("    ");
  lcd.setCursor(11,0);
  lcd.print(temp);
  prevTemp = temp;
}

void wantedTempUpdate(){
  lcd.setCursor(13,1);
  lcd.print("    ");
  lcd.setCursor(13,1);
  lcd.print(wantedTemp);
  Serial.println(wantedTemp);
}


//functions

void changeFreezerStatus(bool state){
  const int onAngle = 135;
  const int offAngle = 45;
  if (state){
    switcher.write(onAngle);
  }
  else{
    switcher.write(offAngle);
  }
}


