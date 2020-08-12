#include <HX711.h>
HX711 scale;
float b = 0;
float m = 0;

//Equation used to get zero_factor
  //y = mx + b
  //where
  //y => desired/actual weight
  //m => multiplier
  //x => raw value from hx711 (use scale.read_average())
  //b => baseline raw value/offset (scale is already tared using scale.tare())

  //Instructions:
  //get b as raw value offset
  //use m = (y-b)/x

  
void setup() {  
  
  Serial.begin(9600);
  Serial.println("Begin Operation");
  Serial.println("Getting base offset");
  
  scale.begin(A0,A1);

  //Getting base offset
  //scale.tare();
  
  b = scale.read_average();
  /*
  Serial.print("base offset: ");
  Serial.print(b);
  scale.set_offset(b);
  
  m = 1;
  scale.set_scale(m);
  scale.tare();
  */
  scale.tare();
}

void loop() {
  
  float raw = scale.read_average();    
  //double m = (0 - b)/ raw;
  float units = -1 * raw + b;
  
  Serial.print("Unit val: ");
  Serial.print(units);
  Serial.print(" Raw val: ");
  Serial.print(raw);
  Serial.print(" offset: ");
  Serial.print(b);
  Serial.print(" multiplier: ");
  Serial.print(m);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      m += 1;
    else if(temp == '-' || temp == 'z')
      m -= 1;
  }
}
