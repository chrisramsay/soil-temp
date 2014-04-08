#include <OneWire.h>
#include <dht.h>
#include <DallasTemperature.h>
#include <UIPEthernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 3
// Uncomment for verbose output
// #define DEBUG
// DHT 22
#define DHT22_PIN 2
dht DHT;

OneWire  oneWire(ONE_WIRE_BUS);

// Probes
DeviceAddress add1 = { 0x28, 0xC8, 0xCC, 0xBF, 0x04, 0x00, 0x00, 0xB6 };
DeviceAddress add2 = { 0x28, 0xBA, 0x17, 0xD0, 0x04, 0x00, 0x00, 0x64 };
DeviceAddress add3 = { 0x28, 0xE3, 0x67, 0xCE, 0x04, 0x00, 0x00, 0x9C };

// Indicator LEDs
int led_15 = 4;
int led_30 = 5;
int led_100 = 6;

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Indicators
  pinMode(led_15, OUTPUT);
  pinMode(led_30, OUTPUT); 
  pinMode(led_100, OUTPUT);

  // Server initialisation
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  IPAddress myIP(192,168,100,6);
  Ethernet.begin(mac,myIP);

  // Start Server
  server.begin();

  // Start up the library, resolution 12 bit
  sensors.begin();
  sensors.setResolution(add1, 12);
  sensors.setResolution(add2, 12);
  sensors.setResolution(add3, 12);

}
 
 
void loop(void)
{
  size_t size;
  float t_15;
  float t_30;
  float t_100;
  float dewpt;
  EthernetClient client = server.available();

  // Get temperature outside the ethernet part
  sensors.requestTemperatures();
  t_15 = sensors.getTempC(add1);
  t_30 = sensors.getTempC(add2);
  t_100 = sensors.getTempC(add3);
  DHT.read22(DHT22_PIN);
  dewpt = dewPoint(DHT.temperature, DHT.humidity);

  // Check lights
  device_check(t_15, led_15);
  device_check(t_30, led_30);
  device_check(t_100, led_100);

  if (EthernetClient client = server.available()) {
    while((size = client.available()) > 0) {
      uint8_t* msg = (uint8_t*)malloc(size);
      size = client.read(msg,size);
      Serial.write(msg,size);
      free(msg);
    }
    client.print(t_15);
    client.print(", ");
    client.print(t_30);
    client.print(", ");
    client.print(t_100);
    client.print(", ");
    client.stop();
  }

}

void device_check(float t, int led) {
#ifdef DEBUG
  delay(1000);
#endif
  if (t >= 85.0 || t <= -127.0) {
    alert(led);
  } else {
    ok(led);
  }
}

void alert(int pin)
{
  // Makes RED
  digitalWrite(pin, HIGH);
}

void ok(int pin)
{
  // Makes GREEN
  digitalWrite(pin, LOW);
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
    RHS += log10(1013.246);

    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;

    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558 - T);
}