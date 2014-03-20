
    #include <OneWire.h>

    int sensors_pin = 2; //the pin where the bus its connected to arduino

    OneWire  sensor_bus(sensors_pin);

    float get_temperature (uint8_t *address);

    void setup(void) {
      Serial.begin(9600);
    }

    void loop(void) {
      uint8_t add1[8] = { 0x28, 0xC8, 0xCC, 0xBF, 0x04, 0x00, 0x00, 0xB6 };
      uint8_t add2[8] = { 0x28, 0xBA, 0x17, 0xD0, 0x04, 0x00, 0x00, 0x64 };
      uint8_t add3[8] = { 0x28, 0x81, 0xA0, 0x8C, 0x05, 0x00, 0x00, 0x0E };
      uint8_t add4[8] = { 0x28, 0xE3, 0x67, 0xCE, 0x04, 0x00, 0x00, 0x9C };

      Serial.print(get_temperature(add1));
      Serial.println(" C");
      Serial.print(get_temperature(add2));
      Serial.println(" C");
      Serial.print(get_temperature(add3));
      Serial.println(" C");
      Serial.print(get_temperature(add4));
      Serial.println(" C");

      delay(2000);
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
