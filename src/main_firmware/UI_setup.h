
//This file contains EEPROM and screen setup functions to be used in the main file

//--------------------------------------------EEPROM functions--------------------------------------
//-------------------Function to write constants------------------
//--> use function to write constants into eeprom
void write_weights_to_eeprom(){
  
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

//-------------------Function to read constants in eeprom--------
//-->use function to read all weights from eeprom
void read_weights_from_eeprom(){

  //iterate through first 13 (float sized) addresses
  for(int i = 0 ,addr = 0; i < 13; i++ , addr += sizeof(float)){
    
    switch(i){
      case 0:
        EEPROM.get(addr,XS_MIN);
        break;
      case 1:
        EEPROM.get(addr,XS_MAX);
        break;
      case 2:
        EEPROM.get(addr,S_MIN);
        break;
      case 3:
        EEPROM.get(addr,S_MAX);
        break;
      case 4:
        EEPROM.get(addr,M_MIN);
        break;
      case 5:
        EEPROM.get(addr,M_MAX);
        break;
      case 6:
        EEPROM.get(addr,L_MIN);
        break;
      case 7:
        EEPROM.get(addr,L_MAX);
        break;
      case 8:
        EEPROM.get(addr,XL_MIN);
        break;
      case 9:
        EEPROM.get(addr,XL_MAX);
        break;
      case 10:
        EEPROM.get(addr,XXL_MIN);
        break;
      case 11:
        EEPROM.get(addr,XXL_MAX);
        break;
      case 12:
        EEPROM.get(addr,JUMBO_MIN);
        break;
    }
  }
}


//-------------------Function to read callibration factor from eeprom----------------
//-> use function to read pre-saved callibration factor in eeprom
void read_callibration_factor_from_eeprom(){
  int factor_addr = 13 * sizeof(float);
  EEPROM.get(factor_addr,callibration_factor);
}

//-------------------Functions to write callibration factor to eeprom--------------
//-> use function to write updated callibration factor to eeprom
void write_callibration_factor_to_eeprom(){
  int factor_addr = 13* sizeof(float);
  EEPROM.put(factor_addr,callibration_factor);
}

//-------------------Function to initialize eeprom with constants-----------------
//--> use function to initialize eeprom with hard coded tresholds whenever thresholds are changes
// also added calibration factor to eeprom
void initialize_eeprom(){

  
  for(int i = 0 ,addr = 0; i < 14; i++ , addr += sizeof(float)){
    
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
      case 13:
        EEPROM.put(addr,callibration_factor);
        break;
    }
  }
  
}



//-------------------------------------------SETUP FUNCTIONS----------------------------------------
//-------------------Function to setup scale------------------
//-> use function setup open egg scale
void setup_eggscale(){
   //Initialize lcd display
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Calibration..");
  
  scale.begin(A0 , A1);
  

  //Get and set base offset
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Setting offset..");

  //Load settings from eeprom
  // initialize_eeprom();
  read_callibration_factor_from_eeprom();
  read_weights_from_eeprom();


  //set hardcoded calibration multiplier
  scale.set_scale(callibration_factor);
  scale.tare();

  //Finished callibration
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Scale Ready!");
}

//-------------------Function to transition from MENU to MAIN---------------
//-> use function in fsm for transition states
void setup_main_screen(){
  //temporary state only
  lcd.clear();     //clear lcd
  //print data to lcd
  lcd.setCursor(0,0);
  lcd.print("Weight: ");
  lcd.setCursor(0,1);
  lcd.print("Size: ");
  lcd.noBlink();

      
  //delay to avoid sampling another btn press too quickly
  delay(TRANSITION_DELAY);
}

//-------------------Function to transition from MAIN to MENU---------------
//-> use function in fsm for transition states
void setup_menu_screen(){
  //temporary state only
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tare");
  lcd.setCursor(0,1);
  lcd.print("Config");
  lcd.setCursor(8,0);
  lcd.print("Callibr");
  lcd.setCursor(8,1);
  lcd.print("Exit");

  //delay to avoid sampling another btn press too quickly
  delay(TRANSITION_DELAY);
}

//----------------Function to transition to config from menu screen--------
//-> To do: write existing test into screen for scrolling
void setup_config_screen(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("XS");
  lcd.setCursor(3,0);
  lcd.print(" S");
  lcd.setCursor(6,0);
  lcd.print(" M");
  lcd.setCursor(9,0);
  lcd.print(" L");
  lcd.setCursor(13,0);
  lcd.print("XL");
  lcd.setCursor(0,1);
  lcd.print("2X");
  lcd.setCursor(4,1);
  lcd.print("JB");
  lcd.setCursor(11,1);
  lcd.print("EXIT");

  //initialize option to xs
  optn = XS_OPTN;
  
}


//-------------------Function to print out settings save on eeprom--------
//-> use this to display weight min and max based on config option 
void setup_config_change(int c_optn){

  float optn_min = 0 , optn_max = 0;    //variables for printing information
  String buff = " ";                  //wow a string primitive wow

  switch(c_optn){
    case XS_OPTN:
      buff = "XS";
      optn_min = XS_MIN;
      optn_max = XS_MAX;
      break;
    case S_OPTN:
      buff = "S";
      optn_min = S_MIN;
      optn_max = S_MAX;
      break;
    case M_OPTN:
      buff = "M";
      optn_min = M_MIN;
      optn_max = M_MAX;
      break;
    case L_OPTN:
      buff = "L";
      optn_min = L_MIN;
      optn_max = L_MAX;
      break;
    case XL_OPTN:
      buff = "XL";
      optn_min = XL_MIN;
      optn_max = XL_MAX;
      break;
    case XXL_OPTN:
      buff = "2X";
      optn_min = XXL_MIN;
      optn_max = XXL_MAX;
      break;
    case JB_OPTN:
      buff = "JUMBO";
      optn_min = JUMBO_MIN;
      break;  
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(buff);
  lcd.print("_MIN  ");
  lcd.print(optn_min,1);
  lcd.setCursor(0,1);
  lcd.print(buff);
  lcd.print("_MAX  ");
  lcd.print(optn_max,1);

  //blink cursor on proper side
  if(c_state == CONFIG_CHANGE_MIN)  lcd.setCursor(12,0);
  else if(c_state == CONFIG_CHANGE_MAX) lcd.setCursor(12,1);
}

//-------------------Function to save modified weights into eeprom--------
//-> use this after chaning max threshold in any weight category
void setup_save_screen(){
  lcd.clear();
  lcd.print("Saving settings...");
  write_weights_to_eeprom();
  write_callibration_factor_to_eeprom();
  delay(2000);
  
}


//------------------Function to print out callibration edit screen--------------
//-> use this to print out callibration edit screen
void setup_callibrate_screen(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Factor: ");
  lcd.print(callibration_factor,1);
  lcd.setCursor(0,1);
  lcd.print("Weight: ");
  lcd.print(scale.get_units(),1);
  lcd.setCursor(15,0);
  delay(100);
}
