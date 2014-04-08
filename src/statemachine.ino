#include <OneWire.h>
#include <DallasTemperature.h>
#include <UIPEthernet.h>

#define ONE_WIRE_BUS 3
OneWire  oneWire(ONE_WIRE_BUS);

// Probes
DeviceAddress add1 = { 0x28, 0xC8, 0xCC, 0xBF, 0x04, 0x00, 0x00, 0xB6 };
DeviceAddress add2 = { 0x28, 0xBA, 0x17, 0xD0, 0x04, 0x00, 0x00, 0x64 };
DeviceAddress add3 = { 0x28, 0xE3, 0x67, 0xCE, 0x04, 0x00, 0x00, 0x9C };

int led_15 = 4;
int led_30 = 5;
int led_100 = 6;

// Set up server on port 1000
EthernetServer server = EthernetServer(1000);

// oneWire reference to Dallas Temperature.
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

void loop() {

	// initial state is 1, the "idle" state.
  	static int state = 1;

  	// To store the "current" time in for delays.
  	static unsigned long ts;

  	// Set up client
  	EthernetClient client = server.available();
  	size_t size = client.available();
  	uint8_t* msg = 0;

  	// Temperature measurements
  	float t_15;
  	float t_30;
  	float t_100;

  	sensors.requestTemperatures();
	t_15 = sensors.getTempC(add1);
	t_30 = sensors.getTempC(add2);
	t_100 = sensors.getTempC(add3);

	if (size > 0) state = 5;
  	
  	switch(state)
  	{
  		case 1:
  			// Waiting for forced state change
  			Serial.println("state: 1 - WAITING...  ------------------------------------+++++++");
  			check(t_15, led_15);
  			check(t_30, led_30);
  			check(t_100, led_100);
			break;

  		case 5:
  			Serial.println("STATE 5!!!!!!!!!!!!!ħŋđðđđe¶e¶eŧ←ŧ¶ŧ←¶←→↓←↓→→→øø→øþ!!!!!!!!!!!!!!!");
 			msg = (uint8_t*)malloc(size);
			size = client.read(msg,size);
			Serial.write(msg,size);
			free(msg);
    		client.print(t_15);
    		client.print(", ");
    		client.print(t_30);
    		client.print(", ");
    		client.print(t_100);
    		client.print(", ");
    		client.stop();
			client.stop();
			state = 1;
			break;
  		default:
  			state = 1;
  			break;
  	}
}

void check(float t, int pin)
{
	if (t >= 85.0 || t <= -127.0)
	{
		alert(pin);
	} else {
		ok(pin);
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