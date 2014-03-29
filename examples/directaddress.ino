
    #include <OneWire.h>

    const int sensors = 4; //number of temperature sensors on the bus
    int sensors_pin = 2; //the pin where the bus its connected to arduino
    uint8_t sensors_address[sensors][8]; //here will store the sensors addresses for later use

    OneWire  sensor_bus(sensors_pin);

    float get_temperature (uint8_t *address);

    void setup(void) {
      Serial.begin(9600);
      int x,y,c=0;
      Serial.println("Starting to look for sensors...");
      for(x=0;x<sensors;x++){
        if(sensor_bus.search(sensors_address[x]))
          c++;
      }
      if(c > 0) {
        Serial.println("Found this sensors : ");
        for(x=0;x<sensors;x++) {
          Serial.print("\tSensor ");
          Serial.print(x+1);
          Serial.print(" at address : ");
          for(y=0;y<8;y++){
            Serial.print(sensors_address[x][y]);
            Serial.print(" ");
          }
          Serial.println();
        }
      } else
        Serial.println("Didn't find any sensors");
    }

    void loop(void) {
       
        for(int x=0;x<sensors;x++) {
          Serial.print("Sensor ");
          Serial.print(x+1);
          Serial.print(" ");
          Serial.print(get_temperature(sensors_address[x]));
          Serial.println(" C");
        }
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
     
        //For testing purpuses only
      
      for(x=0;x<9;x++){
        Serial.print(data[x],DEC );
        Serial.print(" ");
      }
     
      Serial.print(" tr=");
      Serial.print(tr, DEC);
      Serial.print("; ");
      
     
      if(tr > 0xF000){
        tr = 0 - tr;
      }
     
     
      float temperature = (float)tr*(float)0.0625 ;

      return temperature;
    }
