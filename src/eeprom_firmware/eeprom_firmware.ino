#include <EEPROM.h>

float JUMBO_MIN = 81.0f;

//2X 80.0-74.0
float XXL_MAX = 79.0f;
float XXL_MIN = 74.0f;

//XL 69.0-64.6
float XL_MAX = 69.0f;
float XL_MIN = 64.6f;

//L 64.5-58.0
float L_MAX = 64.5f;
float L_MIN = 58.0f;

//M 57.8-54.5
float M_MAX = 57.8f;
float M_MIN = 54.5f;

//S 54.0-49.0
float S_MAX = 54.0f;
float S_MIN = 49.0f;

//XS 48.0-10.0
float XS_MAX = 48.0f;
float XS_MIN = 10.0f;

void setup() {
  Serial.begin(9600);
  test_eeprom();
}

void loop() {
  // put your main code here, to run repeatedly:

}


void test_eeprom(){
  
  float f = 0.00f;
  for(int i = 0 ,addr = 0 ;i<14; i++ , addr += sizeof(float)){
    EEPROM.get(addr,f);
    Serial.println(f);
  }
  
}

//-------------------Function to initialize eeprom with constants-----------------
//--> use function to initialize eeprom with hard coded  constants
void initialize_eeprom(){

  EEPROM.put(0.12,0);
  for(int i = 0 ,addr = 0; i < 13; i++ , addr += sizeof(float)){
    
    switch(i){
      case 0:
        EEPROM.put(addr,XS_MIN);
        break;
      case 1:
        EEPROM.put(addr,XS_MAX);
        break;
      case 2:
        EEPROM.put(addr,S_MIN);
        break;
      case 3:
        EEPROM.put(addr,S_MAX);
        break;
      case 4:
        EEPROM.put(addr,M_MIN);
        break;
      case 5:
        EEPROM.put(addr,M_MAX);
        break;
      case 6:
        EEPROM.put(addr,L_MIN);
        break;
      case 7:
        EEPROM.put(addr,L_MAX);
        break;
      case 8:
        EEPROM.put(addr,XL_MIN);
        break;
      case 9:
        EEPROM.put(addr,XL_MAX);
        break;
      case 10:
        EEPROM.put(addr,XXL_MIN);
        break;
      case 11:
        EEPROM.put(addr,XXL_MAX);
        break;
      case 12:
        EEPROM.put(addr,JUMBO_MIN);
        break;
    }
  }
  
}
