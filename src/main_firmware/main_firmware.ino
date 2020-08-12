#include <EEPROM.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* -------------Pin assignments------------
 * ----HX711----
 * A0 - Dt pin
 * A1 - SCK pin
 * 
 * ---I2C LCD---
 * A4 - SDA
 * A5 - SCL
 * 
 */


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

HX711 scale;


/*
//weight tresholds
//Jumbo 81.0 onwards
#define JUMBO_MIN 81.0

//2X 80.0-74.0
#define XXL_MAX 79.0
#define XXL_MIN 74.0

//XL 69.0-64.6
#define XL_MAX 69.0
#define XL_MIN 64.6

//L 64.5-58.0
#define L_MAX 64.5
#define L_MIN 58.0

//M 57.8-54.5
#define M_MAX 57.8
#define M_MIN 54.5

//S 54.0-49.0
#define S_MAX 54.0
#define S_MIN 49.0

//XS 48.0-10.0
#define XS_MAX 48.0
#define XS_MIN 10.0

*/


//Finite state machine states
#define MAIN_STATE 0
#define MENU_STATE 1
#define CONFIG_STATE 2
#define CONFIG_CHANGE_MIN 3
#define CONFIG_CHANGE_MAX 4


//Btn pins
#define MENU_BTN_PIN 2
#define LEFT_BTN_PIN 3
#define RIGHT_BTN_PIN 4

//Btn debounce time (in ms)
#define DEBOUNCE_DELAY 50

//State transition delay (in ms)
#define TRANSITION_DELAY 500
//Menu options
#define TARE_OPTN 0
#define CONFIG_OPTN 1
#define EXT_OPTN 2


//Config options
#define XS_OPTN 0
#define S_OPTN 1
#define M_OPTN 2
#define L_OPTN 3
#define XL_OPTN 4
#define XXL_OPTN 5
#define JB_OPTN 6
#define CEXT_OPTN 7


float JUMBO_MIN = 81.0;

//2X 80.0-74.0
float XXL_MAX = 79.0;
float XXL_MIN = 74.0;

//XL 69.0-64.6
float XL_MAX = 69.0;
float XL_MIN = 64.6;

//L 64.5-58.0
float L_MAX = 64.5;
float L_MIN = 58.0;

//M 57.8-54.5
float M_MAX = 57.8;
float M_MIN = 54.5;

//S 54.0-49.0
float S_MAX = 54.0;
float S_MIN = 49.0;

//XS 48.0-10.0
float XS_MAX = 48.0;
float XS_MIN = 10.0;


int c_state , n_state;

//flag
boolean menu_press, left_press, right_press;

//for debouncing
int left_btn_state, last_left_btn_state;
int right_btn_state, last_right_btn_state;
int menu_btn_state, last_menu_btn_state;

//for menu/config options
int optn;


void setup() {
  Serial.begin(38400);

  //send info to terminal if available
  Serial.println("HX711 Calibration");

  //Initialize constants and btns
  init_constants_btns();
  //Initialize egg scale
  setup_eggscale();
  //setup main screen
  setup_main_screen();

  read_weights_from_eeprom();
}


void loop() {

   fsm();
    

}


//--------------------Function for statemachine----------------------------
void fsm(){

  
  //set next state transition
  switch(c_state){
    case MAIN_STATE:
      //on main screen
      main_screen();
      //check buttons first
      check_btns();
  
      if(menu_press){
        setup_menu_screen();     //call transition function
        n_state = MENU_STATE;
        menu_press = false;
      }else{ 
        n_state = MAIN_STATE;
      }
      break;
      
    case MENU_STATE:

      //display menu items and check buttons 
      menu_screen();

      if(menu_press){
        switch(optn){
          case EXT_OPTN:
            setup_main_screen();   //call transition function to setup main screen
            n_state = MAIN_STATE; //back to main screen
            break;
          case TARE_OPTN:
            menu_tare();              //call tare routine
            setup_menu_screen();           //call transition function to setup menu items again
            n_state = MENU_STATE;     //stay in menu
            break;
          case CONFIG_OPTN:
            setup_config_screen();         //setup config items
            n_state = CONFIG_STATE;   //go to config screen
            break; 
        }

        //reset flag
        menu_press = false;
      }else
        n_state = MENU_STATE;
       break;
      
    case CONFIG_STATE:
      config_screen();

      if(menu_press){

        if(optn == CEXT_OPTN){
          setup_menu_screen();  //go back to menu screen
          n_state = MENU_STATE; //return to main menu
        }
        else{
          n_state = CONFIG_CHANGE_MIN;           //proceed into config change 
        }
        //clear flag
        menu_press = false;
      }
      break;

      
    case CONFIG_CHANGE_MIN:
      //take user info
      config_change_min();
      //display info on screen
      switch(optn){
          case XS_OPTN:
            setup_config_change(XS_OPTN);
            break;
          case S_OPTN:
            setup_config_change(S_OPTN);
            break;
          case M_OPTN:
            setup_config_change(M_OPTN);
            break;
          case L_OPTN:
            setup_config_change(L_OPTN);
            break;
          case XL_OPTN:
            setup_config_change(XL_OPTN);
            break;
          case XXL_OPTN:
            setup_config_change(XXL_OPTN);
            break;
          case JB_OPTN:
            setup_config_change(JB_OPTN);
            break;
        }
        
    
      check_btns();  //check buttons again

      //state transition
      if(menu_press){
        n_state = CONFIG_CHANGE_MAX;   //temporarily proceed back to menu screen
        menu_press = false;
      }else{
        n_state = CONFIG_CHANGE_MIN;    //loop back till menu press
      }
      delay(50);
      break;
      
    case CONFIG_CHANGE_MAX:
      //take user info
      config_change_max();
      //display info on screen
      switch(optn){
          case XS_OPTN:
            setup_config_change(XS_OPTN);
            break;
          case S_OPTN:
            setup_config_change(S_OPTN);
            break;
          case M_OPTN:
            setup_config_change(M_OPTN);
            break;
          case L_OPTN:
            setup_config_change(L_OPTN);
            break;
          case XL_OPTN:
            setup_config_change(XL_OPTN);
            break;
          case XXL_OPTN:
            setup_config_change(XXL_OPTN);
            break;
          case JB_OPTN:
            setup_config_change(JB_OPTN);
            break;
        }
        
    
      check_btns();  //check buttons again

      //state transition
      if(menu_press){
        setup_save_screen();            //save changes to eeprom
        setup_config_screen();            //setup menu screen
        n_state = CONFIG_STATE;         //proceed back to config screen
        menu_press = false;
      }else{
        n_state = CONFIG_CHANGE_MAX;    //loop back till menu press
      }
      delay(50);
      break;
    
  }

  //actual transition
  c_state = n_state;
}


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




//-------------------Function to initialize eeprom with constants-----------------
//--> use function to initialize eeprom with hard coded  constants
void initialize_eeprom(){

  
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
  
  //scale.tare();
  //scale.set_offset(scale.read_average());

  //set hardcoded calibration multiplier
  scale.set_scale(642.3076923);
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
  lcd.setCursor(9,0);
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

//-------------------Function to display tare screen-----------------------
//-> use function to scale.tare() with some delays
void menu_tare(){
  //display text  
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.print("Setting offset..");
  delay(1000);
  scale.tare();
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tare done");
  delay(1000);
  lcd.clear();
}



//--------------------Function to display menu screen----------------------
//-> use function to display menu screen for options
// actual transition to other state happens in fsm 
void menu_screen(){


  //check buttons first
  check_btns();

  //menu item transition based on input
  if(left_press) {
    optn -= 1;
    //clear button flag
    left_press = false;
  }else if(right_press){
    optn++;
    //clear button flag
    right_press = false;
  }
  
  
 
  if(optn < TARE_OPTN) optn = TARE_OPTN;
  else if(optn > EXT_OPTN) optn = EXT_OPTN;

  switch(optn){
    case TARE_OPTN:
      lcd.setCursor(7,0);
      lcd.blink();
      break;
    case CONFIG_OPTN:
      lcd.setCursor(7,1);
      lcd.blink();
      break;
    case EXT_OPTN:
      lcd.setCursor(14,0);
      lcd.blink();
      break;     
  }



}


//--------------------Function to display config screen---------------------
//-> use this to display configuration menu to change saved weights
// note: this uses eeprom so be careful
void config_screen(){

  //check buttons first
  check_btns();

  //menu item transition based on input
  if(left_press) {
    optn -= 1;
    //clear button flag
    left_press = false;
  }else if(right_press){
    optn++;
    //clear button flag
    right_press = false;
  }
  
  
 
  if(optn < XS_OPTN) optn = XS_OPTN;
  else if(optn > CEXT_OPTN) optn = CEXT_OPTN;

  switch(optn){
    case XS_OPTN:
      lcd.setCursor(2,0);
      lcd.blink();
      break;
    case S_OPTN:
      lcd.setCursor(5,0);
      lcd.blink();
      break;
    case M_OPTN:
      lcd.setCursor(8,0);
      lcd.blink();
      break;
    case L_OPTN:
      lcd.setCursor(11,0);
      lcd.blink();
      break;
    case XL_OPTN:
      lcd.setCursor(15,0);
      lcd.blink();
      break;
    case XXL_OPTN:
      lcd.setCursor(2,1);
      lcd.blink();
      break;
    case JB_OPTN:
      lcd.setCursor(6,1);
      lcd.blink();
      break;
    case CEXT_OPTN:
      lcd.setCursor(15,1);
      lcd.blink();
      break;     
  }
  
}



//-------------------Function to print out settings save on eeprom--------
//-> use this to display weight min and max based on config option 
void setup_config_change(int c_optn){

  int optn_min = 0 , optn_max = 0;    //variables for printing information
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
  lcd.print(optn_min);
  lcd.setCursor(0,1);
  lcd.print(buff);
  lcd.print("_MAX  ");
  lcd.print(optn_max);

  //blink cursor on proper side
  if(c_state == CONFIG_CHANGE_MIN)  lcd.setCursor(11,0);
  else if(c_state == CONFIG_CHANGE_MAX) lcd.setCursor(11,1);
}

//-------------------Function to save modified weights into eeprom--------
//-> use this after chaning max threshold in any weight category
void setup_save_screen(){
  lcd.clear();
  lcd.print("Saving settings...");
  delay(2000);
  write_weights_to_eeprom();
  lcd.print("done saving!");
  delay(2000);
  
}
//---------------Function to modify weight constants' minimum--------------
//-> use function to update constants during config change screen
// this function is for changing minimum part of the constant
void config_change_min(){

  int optn_increment = 0;
  
  check_btns();

  //set increments/decrement based on flags
  if(left_press){
    optn_increment = 1;
    left_press = false;
  }else if(right_press){
    optn_increment = -1;
    right_press = false;
  }

  //note optn should contain existing weight option
  //use this to increment or decrement weights 
  switch(optn){
        case XS_OPTN:
          if((XS_MIN + optn_increment) < XS_MAX)
            XS_MIN += optn_increment;
          break;
        case S_OPTN:

          //avoid overlaps 
          if(((S_MIN + optn_increment) > XS_MAX) && ((S_MIN + optn_increment )< S_MAX))
            S_MIN += optn_increment;
          
          break;
        case M_OPTN:

          if(((M_MIN + optn_increment) > S_MAX) && ((M_MIN + optn_increment) < M_MAX))
            M_MIN += optn_increment;
          break;
        case L_OPTN:

          if(((L_MIN + optn_increment) > M_MAX) && ((L_MIN + optn_increment) < L_MAX ))
            L_MIN += optn_increment;
          break;
        case XL_OPTN:

          if(((XL_MIN + optn_increment) > L_MAX) && ((XL_MIN + optn_increment) < XL_MAX))
            XL_MIN += optn_increment;
          break;
        case XXL_OPTN:

          if(((XXL_MIN + optn_increment) > XL_MAX) && ((XXL_MIN + optn_increment) < XXL_MAX))
            XXL_MIN += optn_increment;
          break;
        case JB_OPTN:

          if((JUMBO_MIN + optn_increment) > XXL_MAX)
            JUMBO_MIN += optn_increment;
          break; 
    }
}


//----------------Function to modify weight constants' maximum--------------
//-> use function to update constants during config change screen
//
void config_change_max(){

  int optn_increment = 0;
  
  check_btns();

  //set increments/decrement based on flags
  if(left_press){
    optn_increment = 1;
    left_press = false;
  }else if(right_press){
    optn_increment = -1;
    right_press = false;
  }

  //note optn should contain existing weight option
  //use this to increment or decrement weights 
  switch(optn){
        case XS_OPTN:
          if((XS_MAX + optn_increment) < S_MIN)
            XS_MAX += optn_increment;
          break;
        case S_OPTN:
          //avoid overlaps 
          if(((S_MAX + optn_increment) > S_MIN) && ((S_MAX + optn_increment )< M_MIN))
            S_MAX += optn_increment;
          
          break;
        case M_OPTN:

          if(((M_MAX + optn_increment) > M_MIN) && ((M_MAX + optn_increment) < L_MIN))
            M_MAX += optn_increment;
          break;
        case L_OPTN:

          if(((L_MAX + optn_increment) > L_MIN) && ((L_MAX + optn_increment) < XL_MIN ))
            L_MAX += optn_increment;
          break;
        case XL_OPTN:

          if(((XL_MAX + optn_increment) > XL_MIN) && ((XL_MAX + optn_increment) < XXL_MIN))
            XL_MAX += optn_increment;
          break;
        case XXL_OPTN:

          if(((XXL_MAX + optn_increment) > XXL_MIN) && ((XXL_MAX + optn_increment) < JUMBO_MIN))
            XXL_MAX += optn_increment;
          break;
        case JB_OPTN:
          ;
          break; 
    }
}




//--------------------Function to display main screen----------------------
//-> use function to display normal segregator function
void main_screen(){
    
    float units = scale.get_units();

   

    //if serial is available send bits
    if(Serial.available())
      Serial.println(units); // print out ave measurements
    
    lcd.setCursor(8,0);
    lcd.print(units);
    lcd.setCursor(6,1);
    
    if(units <= XS_MAX && units >= XS_MIN){
      lcd.print("XS   ");     //XS 
    }else if( units > XS_MAX && units < S_MIN){
      lcd.print("XS-S ");         //in between XS and S
    }else if( units >= S_MIN && units <= S_MAX){
      lcd.print("S    ");     //S  
    }else if((units > S_MAX) && (units < M_MIN)){ 
      lcd.print("S-M  ");     //in between S and M
    }else if ((units >= M_MIN) &&( units <= M_MAX)){
      lcd.print("M    ");     //M 
    }else if((units > M_MAX) && (units < L_MIN)){
      lcd.print("M-L  ");     // in between M and L
    }else if ((units >= L_MIN)&&(units <= L_MAX)){ 
      lcd.print("L    ");      //L
    }else if(units > L_MAX && units < XL_MIN){
      lcd.print("L-XL ");     // in between L and XL
    }else if( units >= XL_MIN && units <= XL_MAX){ 
      lcd.print("XL   ");       //XL
    }else if( units > XL_MAX && units < XXL_MIN){
             
      lcd.print("XL-2X");       // in between XL and XXL
    }else if(units >= XXL_MIN && units <= XXL_MAX){
      lcd.print("2X   ");       //XXL
    }else if(units > XXL_MAX && units < JUMBO_MIN){
      lcd.print("2X-JB");
    }else if(units >= JUMBO_MIN){
      lcd.print("JUMBO");
    }else if( units < XS_MIN ){
      lcd.print("     ");
    }
}


//--------------------Function to get button input---------------------
//-> use function to check for button presses w/ debouncing
// note that BUTTONS ARE PULLED TO HIGH thus presses are registered as LOW
void check_btns(){

  unsigned long last_debounce_time = 0;
  int l_reading = digitalRead(LEFT_BTN_PIN);
  int r_reading = digitalRead(RIGHT_BTN_PIN);
  int m_reading = digitalRead(MENU_BTN_PIN);

  //---------------Menu button---------------------
  //edge detection either from noise or presss
  if(last_menu_btn_state != m_reading){

    //start debouncing
    last_debounce_time = millis();
  
    //accurate debounce delay to ignore noise
    while(!((millis() - last_debounce_time) > DEBOUNCE_DELAY));

    //sample again
    m_reading = digitalRead(MENU_BTN_PIN);
    if(last_menu_btn_state!= m_reading){
      menu_btn_state = m_reading;
  
      //only do something on rising edge 
      if(last_menu_btn_state == HIGH && menu_btn_state == LOW)
        menu_press = true;
      else
        menu_press = false;  
    }
    last_menu_btn_state = menu_btn_state;
  }

  //---------------Left button---------------------
  //edge detection either from noise or press
  if(last_left_btn_state != l_reading){

    //start debouncing
    last_debounce_time = millis();
  
    //accurate debounce delay to ignore noise
    while(!((millis() - last_debounce_time) > DEBOUNCE_DELAY));

    //sample again
    l_reading = digitalRead(LEFT_BTN_PIN);
    if(last_left_btn_state!= l_reading){
      left_btn_state = l_reading;
  
      //only do something on rising edge 
      if(last_left_btn_state == HIGH && left_btn_state == LOW)
        left_press = true;
      else
        left_press = false;  
    }
    last_left_btn_state = left_btn_state;
  }

  //------------------Right button--------------
   //edge detection either from noise or press
  if(last_right_btn_state != r_reading){

    //start debouncing
    last_debounce_time = millis();
  
    //accurate debounce delay to ignore noise
    while(!((millis() - last_debounce_time) > DEBOUNCE_DELAY ));

    //sample again
    r_reading = digitalRead(RIGHT_BTN_PIN);
    if(last_right_btn_state!= r_reading){
      right_btn_state = r_reading;
  
      //only do something on rising edge 
      if(last_right_btn_state == HIGH && right_btn_state == LOW)
        right_press = true;
      else
        right_press = false;  
    }
    last_right_btn_state = right_btn_state;
  }
 
}


//--------------------Function to reset flags--------------------------
//-> use function to reset/init stat variables and button press flags
void init_constants_btns(){
  
  pinMode(MENU_BTN_PIN, INPUT);
  pinMode(LEFT_BTN_PIN, INPUT);
  pinMode(RIGHT_BTN_PIN, INPUT);

  
  
  //set states to 
  c_state = MAIN_STATE;
  n_state = MAIN_STATE;
  
  menu_press = false;
  left_press = false;
  right_press = false;

}
