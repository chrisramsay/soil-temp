#include <OneWire.h>
#include <DallasTemperature.h>
#include <UIPEthernet.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

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

  // Start up the library
  sensors.begin();
}
 
 
void loop(void)
{
  size_t size;

  // call sensors.requestTemperatures() to issue a global temperature
  sensors.requestTemperatures();

  if (EthernetClient client = server.available())
    {
      while((size = client.available()) > 0)
        {
          uint8_t* msg = (uint8_t*)malloc(size);
          size = client.read(msg,size);
          Serial.write(msg,size);
          free(msg);
        }
      client.print(sensors.getTempCByIndex(0));
      client.print(", ");
      client.print(sensors.getTempCByIndex(1));
      client.print(", ");
      client.print(sensors.getTempCByIndex(2));
      client.print(", ");
      client.println(sensors.getTempCByIndex(3));
      client.stop();
    } 
}
