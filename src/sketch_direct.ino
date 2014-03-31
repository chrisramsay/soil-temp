#include <OneWire.h>
#include <UIPEthernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 3
// Uncomment for verbose output
// #define DEBUG

OneWire  sensor_bus(ONE_WIRE_BUS);

// Probes
uint8_t add1[8] = { 0x28, 0xC8, 0xCC, 0xBF, 0x04, 0x00, 0x00, 0xB6 };
uint8_t add2[8] = { 0x28, 0xBA, 0x17, 0xD0, 0x04, 0x00, 0x00, 0x64 };
uint8_t add3[8] = { 0x28, 0xE3, 0x67, 0xCE, 0x04, 0x00, 0x00, 0x9C };

// Indicator LEDs
int led_15 = 5;
int led_30 = 6;
int led_100 = 7;

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

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

  Serial.println("Setup 3.");
  // Start Server
  server.begin();
}
 
 
void loop(void)
{
  size_t size;
  float t_15;
  float t_30;
  float t_100;

  // Get temperature outside the ethernet part
  t_15 = get_temperature(add1);
  t_30 = get_temperature(add2);
  t_100 = get_temperature(add3);

  // Check lights
  is_ok(t_15, led_15);
  is_ok(t_30, led_30);
  is_ok(t_100, led_100);

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

void is_ok(float t, int led) {
#ifdef DEBUG
  delay(1000);
#endif
  if (t == 85.0) {
    digitalWrite(led, LOW);
  } else {
    digitalWrite(led, HIGH);
  }
}

int get_temperature(uint8_t *address) {

  byte data[12];
  int HighByte, LowByte, TReading, SignBit, x;

  sensor_bus.reset();
  sensor_bus.select(address);
  sensor_bus.write(0x44,1);
 
  sensor_bus.reset();
  sensor_bus.select(address);
  sensor_bus.write(0xBE,1);
 
  // Read in 9 bytes
  for(x=0; x<9; x++){
    data[x] = sensor_bus.read();
  }
  
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit

  // Test for negative
  if (SignBit) {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }

  return TReading;
  // return (float) TReading * (float) 0.0625; 
}