#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <InfluxDb.h>
#include <MAX6675_Thermocouple.h>

#define DEVICE_NAME  "Huzzah-1"

#define READS_PER_LOOP 3

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

  // Jumper across 16-0 to enable sending to Influx
  pinMode(0, OUTPUT);
  pinMode(16, INPUT_PULLDOWN_16);
  digitalWrite(0, HIGH);

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
  // Take a few quick measurements and average them
  double temp = 0.0;
  for (int i=0; i<READS_PER_LOOP; ++i)
  {
    temp += thermocouple.readCelsius();
    delay(5);
  }
  const double celsius = temp / READS_PER_LOOP;
  const double fahrenheit = (celsius * 1.8) + 32.0;

  Serial.print("Temperature: ");
  Serial.print(String(celsius) + " C, ");
  Serial.println(String(fahrenheit) + " F");

  if( digitalRead(16) == HIGH )
  {
    InfluxData measurement("temperature");
    measurement.addTag("device", DEVICE_NAME);
    measurement.addValue("temp_C", celsius);
    measurement.addValue("temp_F", fahrenheit);
    influx.write(measurement);
  }

  delay(250);
}