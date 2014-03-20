#include <OneWire.h>
#include <UIPEthernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

OneWire  sensor_bus(ONE_WIRE_BUS);

// Temperature
float get_temperature (uint8_t *address);
// Probes
uint8_t add1[8] = { 0x28, 0xC8, 0xCC, 0xBF, 0x04, 0x00, 0x00, 0xB6 };
uint8_t add2[8] = { 0x28, 0xBA, 0x17, 0xD0, 0x04, 0x00, 0x00, 0x64 };
uint8_t add3[8] = { 0x28, 0x81, 0xA0, 0x8C, 0x05, 0x00, 0x00, 0x0E };
uint8_t add4[8] = { 0x28, 0xE3, 0x67, 0xCE, 0x04, 0x00, 0x00, 0x9C };

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Server initialisation
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  IPAddress myIP(192,168,100,6);
  Ethernet.begin(mac,myIP);

  // Start Server
  server.begin();
}
 
 
void loop(void)
{
  size_t size;

  if (EthernetClient client = server.available()) {
    while((size = client.available()) > 0) {
      uint8_t* msg = (uint8_t*)malloc(size);
      size = client.read(msg,size);
      Serial.write(msg,size);
      free(msg);
    }
    client.print(get_temperature(add1));
    client.print(", ");
    client.print(get_temperature(add2));
    client.print(", ");
    client.print(get_temperature(add3));
    client.print(", ");
    client.print(get_temperature(add4));
    client.print(", ");
    client.stop();
  } 
}

float get_temperature(uint8_t *address) {
  byte data[12];
  int x;
  sensor_bus.reset();
  sensor_bus.select(address);
  sensor_bus.write(0x44,1);
 
  sensor_bus.reset();
  sensor_bus.select(address);
  sensor_bus.write(0xBE,1);
 
  for(x=0;x<9;x++){
    data[x] = sensor_bus.read();
  }
   
  unsigned short tr = data[0];
  tr = tr + data[1]*256;
 
  if(tr > 0xF000){
    tr = 0 - tr;
  }
 
 
  float temperature = (float)tr*(float)0.0625 ;

  return temperature;
}