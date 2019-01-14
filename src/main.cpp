#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <InfluxDb.h>
#include <MAX6675_Thermocouple.h>

#define DEVICE_NAME  "Huzzah-1"

#define INFLUXDB_HOST "home-nas.lan"
#define INFLUXDB_USER "xxx"
#define INFLUXDB_PASS "xxx"
#define INFLUXDB_DB "data_logger"

#define WIFI_SSID "Provost-Mesh"
#define WIFI_PASS "cra6ga12o4doph09"

#define HUZZA_SCK       14
#define HUZZAH_MOSI     13
#define HUZZAH_MISO     12

#define DO_PIN  HUZZAH_MISO
#define CS_PIN  HUZZAH_MOSI
#define SCK_PIN HUZZA_SCK

MAX6675_Thermocouple thermocouple(SCK_PIN, CS_PIN, DO_PIN);
Influxdb influx(INFLUXDB_HOST);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  Serial.println(" ### Hello ###");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  influx.setDb(INFLUXDB_DB);
//   influx.setDbAuth(INFLUXDB_DB, INFLUXDB_USER, INFLUXDB_PASS);

  Serial.println("Setup done");
}

// the loop function runs over and over again forever
void loop() {
  const double celsius = thermocouple.readCelsius();
  const double kelvin = thermocouple.readKelvin();
  const double fahrenheit = thermocouple.readFahrenheit();

  Serial.print("Temperature: ");
  Serial.print(String(celsius) + " C, ");
  Serial.print(String(kelvin) + " K, ");
  Serial.println(String(fahrenheit) + " F");

  InfluxData measurement("temperature");
  measurement.addTag("device", DEVICE_NAME);
  measurement.addValue("temp_C", celsius);
  measurement.addValue("temp_F", fahrenheit);
  measurement.addValue("temp_K", kelvin);
  influx.write(measurement);

  delay(500);
}