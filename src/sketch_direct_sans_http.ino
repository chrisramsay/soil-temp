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


// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

void setup(void)
{
  // start serial port
  Serial.begin(9600);
}
 
 
void loop(void)
{
  float t_15;
  float t_30;
  float t_100;

  // Get temperature outside the ethernet part
  t_15 = get_temperature(add1);
  t_30 = get_temperature(add2);
  t_100 = get_temperature(add3);

  Serial.println(t_15);
  Serial.println(t_30);
  Serial.println(t_100);
  delay(3000);
}

float get_temperature(uint8_t *address) {

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

  // SignBit == -32768 when negative, 0 otherwise.
  if (SignBit) {
    TReading = 0 - ((TReading ^ 0xffff) + 1); // 2's comp
  }

  return (float) TReading * (float) 0.0625; 
}