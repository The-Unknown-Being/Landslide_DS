#include <Wire.h> 
#include <string.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int soilMoistureSensor_pin = A0;
int trans_pin_0 = 7;
int soilMoistureValue = 0;
int lowerThreshold = 0;
int upperThreshold = 1023;
int turn = 1;
int curr = 0;

int alertPin_buzzer = 3;
int alertPin_led = 4;
int alertThreshold = 60;
int pressThreshold = 10;

void pwd(char* txt, int time) // prints txt and delays by time seconds
{
  lcd.clear();
  lcd.print(txt);
  delay(time*1000);
  lcd.clear();
}

void lcdInit()
{
  lcd.begin();
  lcd.backlight();
  lcd.setBacklight(50);
}

void caliberate()
{
  pwd("Initializing...", 1);

  // when sensor is in air
  turn = 1;
  pwd("In dry air...", 2);
  lcd.print("Now caliberating...");
  int caliberatingValue = lowerThreshold;
  int temp = 0;
  for(int i = 0; i < 30; i++)
  {
    delay(100);
    temp = analogRead(soilMoistureSensor_pin);
    if( temp > caliberatingValue)
    {
      caliberatingValue = temp;
    }
  }
  upperThreshold = caliberatingValue;
  pwd("Caliberated!!", 1);

  // when sensor is in water
  pwd("In water...", 2);
  lcd.print("Now caliberating...");
  caliberatingValue = upperThreshold;
  for(int i = 0; i < 30; i++)
  {
    delay(100);
    temp = analogRead(soilMoistureSensor_pin);
    if(temp < caliberatingValue)
    {
      caliberatingValue = temp;
    }
  }

  lowerThreshold = caliberatingValue;
  pwd("Caliberated!!", 1);
}

void checkAlert(int soilMoisturePercent)
{
  if(soilMoisturePercent >= alertThreshold)
  {
    digitalWrite(alertPin_buzzer, HIGH);
    digitalWrite(alertPin_led, HIGH);
  }
}

void resetAlert(int soilMoisturePercent)
{
  if(soilMoisturePercent < alertThreshold)
  {
    digitalWrite(alertPin_buzzer, LOW);
    digitalWrite(alertPin_led, LOW);
  }
}


void setup()
{
  pinMode(soilMoistureSensor_pin, INPUT_PULLUP);
  pinMode(trans_pin_0, OUTPUT);
  digitalWrite(trans_pin_0, HIGH );

  Serial.begin(9600);
	lcdInit();
  caliberate();

  Serial.println(upperThreshold);
  Serial.println(lowerThreshold);
}

void loop()
{
  soilMoistureValue = analogRead(soilMoistureSensor_pin);
  curr = abs(float(upperThreshold-soilMoistureValue) / float(upperThreshold-lowerThreshold) * 100);
  String lcdTxt = "Soil: " + String(curr) + " %";
  lcdTxt = lcdTxt.c_str();
  lcd.print(lcdTxt);
  delay(1000);
  lcd.clear();

  checkAlert(curr);
  resetAlert(curr);
}

 

