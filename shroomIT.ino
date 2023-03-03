#include "DHT.h"                    // library for the humidity-temperature sensor
#include <LiquidCrystal_I2C.h>      // library for the LCD Display
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

const float minimum_humidity = 65.00; // (%)
const float minimum_temp = 25;        // (°C)

#define DHTPIN 2                   // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11
#define CH1 3
#define CH2 4
#define CH3 5
#define CH4 6
#define H_12 43200000
#define H_24 86400000

DHT dht(DHTPIN, DHTTYPE);

unsigned long myTime; // for the time management
 extern volatile unsigned long timer0_millis;
//volatile unsigned long timer0_millis;


byte smileyFace[] = {
    B00000,
    B11011,
    B11011,
    B00000,
    B10001,
    B10001,
    B11111,
    B00000};

void setup()
{
  Serial.begin(9600);
  Wire.setClock(10000);

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  dht.begin();

  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, smileyFace);
  lcd.setCursor(0, 2);
  lcd.print("ShroomIT");
  lcd.setCursor(13, 2);
  lcd.write(0);
}


void loop()
{
  // Wait a few seconds between measurements
  delay(3000);

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Check if any reading failed and exit early
  if (isnan(humidity) || isnan(temp) || isnan(f))
  {
    Serial.println(F("Failed to read DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temp, humidity, false);

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  // Print measured temperature and humidity on the LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(10, 0);
  lcd.print(temp);
  lcd.setCursor(15, 0);
  lcd.print(F(" \xDF""C"));
  lcd.setCursor(0, 1);
  lcd.print("Hum:");
  lcd.setCursor(10, 1);
  lcd.print(humidity);
  lcd.setCursor(15, 1);
  lcd.print(" %");

  // Set the lights ON for 12 hours, then turn them off
  Serial.print("Time: ");
  myTime = millis();

  Serial.println(myTime); // prints time since program started

  digitalWrite(CH1, LOW);

  if (myTime > H_12)
  {
    digitalWrite(CH1, HIGH);
  }

  // when 24h passed, impose the end of the loop
  if (myTime > H_24)
  {
    noInterrupts();
    timer0_millis = 0;
    interrupts();
  }

  // setting the atomizer and the fan ON if the humidity is under the minimum humidity
  if (humidity < minimum_humidity)
  {
    digitalWrite(CH3, HIGH);
    digitalWrite(CH4, HIGH);
    lcd.setCursor(0, 2);
    lcd.print("Umidify:");
    lcd.setCursor(10, 2);
    lcd.print("ON");
  }
  else
  {
    digitalWrite(CH3, LOW);
    digitalWrite(CH4, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Umidify:");
    lcd.setCursor(10, 2);
    lcd.print("OFF");
  }

  // setting the hot resistance ON if the temperature is under the minimum temperature
  if (temp < minimum_temp)
  {
    digitalWrite(CH2, HIGH);
    lcd.setCursor(0, 3);
    lcd.print("Heater:");
    lcd.setCursor(10, 3);
    lcd.print("ON");
  }
  else
  {
    digitalWrite(CH2, LOW);
    lcd.setCursor(0, 3);
    lcd.print("Heater:");
    lcd.setCursor(10, 3);
    lcd.print("OFF");
  }
}
