#include <Wire.h>
#include <Adafruit_INA219.h>
#include <DHT.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// =====================================================
// PIN DEFINITIONS
// =====================================================

#define LED_PIN   2
#define RELAY_PIN 4

#define DHTPIN    15
#define DHTTYPE   DHT11

// ST7735 TFT Display
#define TFT_CS    5
#define TFT_DC    27
#define TFT_RST   26


// =====================================================
// OBJECTS
// =====================================================

Adafruit_ST7735 tft = Adafruit_ST7735(
  TFT_CS,
  TFT_DC,
  TFT_RST
);

Adafruit_INA219 ina219;

DHT dht(DHTPIN, DHTTYPE);


// =====================================================
// SETUP
// =====================================================

void setup()
{
  // Serial communication
  Serial.begin(9600);

  // Configure output pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Initially turn OFF
  digitalWrite(LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  // Initialize I2C
  Wire.begin(21, 22);

  // Initialize INA219
  if (!ina219.begin())
  {
    Serial.println("INA219 not detected!");
  }
  else
  {
    Serial.println("INA219 initialized.");
  }

  // Initialize DHT11
  dht.begin();

  // Initialize ST7735 TFT
  tft.initR(INITR_BLACKTAB);

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  // Startup message
  tft.setCursor(10, 20);
  tft.println("HEALTH MONITOR");

  tft.setCursor(10, 40);
  tft.println("SYSTEM STARTING...");

  delay(2000);

  tft.fillScreen(ST77XX_BLACK);
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // ===================================================
  // READ SENSOR VALUES
  // ===================================================

  float voltage = ina219.getBusVoltage_V();

  float current = ina219.getCurrent_mA();

  float temperature = dht.readTemperature();


  // ===================================================
  // CHECK TEMPERATURE SENSOR
  // ===================================================

  if (isnan(temperature))
  {
    Serial.println("DHT11 sensor error!");
  }


  // ===================================================
  // RECEIVE COMMANDS FROM PYTHON
  // ===================================================

  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');

    command.trim();

    command.toUpperCase();


    // -------------------------------------------------
    // LED ON
    // -------------------------------------------------

    if (command == "LED_ON")
    {
      digitalWrite(LED_PIN, HIGH);

      Serial.println("LED ON");
    }


    // -------------------------------------------------
    // LED OFF
    // -------------------------------------------------

    else if (command == "LED_OFF")
    {
      digitalWrite(LED_PIN, LOW);

      Serial.println("LED OFF");
    }


    // -------------------------------------------------
    // MOTOR ON
    // -------------------------------------------------

    else if (command == "MOTOR_ON")
    {
      digitalWrite(RELAY_PIN, HIGH);

      Serial.println("MOTOR ON");
    }


    // -------------------------------------------------
    // MOTOR OFF
    // -------------------------------------------------

    else if (command == "MOTOR_OFF")
    {
      digitalWrite(RELAY_PIN, LOW);

      Serial.println("MOTOR OFF");
    }
  }


  // ===================================================
  // DISPLAY HEALTH MONITORING DATA
  // ===================================================

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);

  tft.setTextSize(1);


  // -------------------------------------------------
  // TITLE
  // -------------------------------------------------

  tft.setCursor(20, 5);
  tft.println("HEALTH MONITOR");


  // -------------------------------------------------
  // TEMPERATURE
  // -------------------------------------------------

  tft.setCursor(0, 30);

  tft.print("Temp: ");

  if (isnan(temperature))
  {
    tft.println("ERROR");
  }
  else
  {
    tft.print(temperature, 1);
    tft.println(" C");
  }


  // -------------------------------------------------
  // VOLTAGE
  // -------------------------------------------------

  tft.setCursor(0, 55);

  tft.print("Voltage: ");

  tft.print(voltage, 2);

  tft.println(" V");


  // -------------------------------------------------
  // CURRENT
  // -------------------------------------------------

  tft.setCursor(0, 80);

  tft.print("Current: ");

  tft.print(current, 1);

  tft.println(" mA");


  // -------------------------------------------------
  // POWER
  // -------------------------------------------------

  float power = voltage * current / 1000.0;

  tft.setCursor(0, 105);

  tft.print("Power: ");

  tft.print(power, 2);

  tft.println(" W");


  // ===================================================
  // SEND DATA TO PYTHON
  // ===================================================

  Serial.print("TEMP:");
  Serial.print(temperature, 1);

  Serial.print(",VOLT:");
  Serial.print(voltage, 2);

  Serial.print(",CURRENT:");
  Serial.print(current, 1);

  Serial.print(",POWER:");
  Serial.println(power, 2);


  // ===================================================
  // UPDATE EVERY 2 SECONDS
  // ===================================================

  delay(2000);
}