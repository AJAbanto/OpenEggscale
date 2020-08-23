#include <EEPROM.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "constants.h"
#include "UI_setup.h"





void setup() {
  Serial.begin(38400);

  //Initialize btns pins and flags
  init_constants_btns();
  //Initialize egg scale
  setup_eggscale();
  //setup main screen
  setup_main_screen();

}


void loop() {

   fsm();
    

}


//--------------------Function for state machine----------------------------
//-> finite state machine that contains behavior of the program
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
          case CALLIBRATE_OPTN:
            setup_callibrate_screen();    //setup callibration items
            n_state = CALLIBRATE_STATE;   //go to callibrate screen
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
      
   case CALLIBRATE_STATE:

      setup_callibrate_screen();
      callibration_screen();
      check_btns();
      if(menu_press){
        setup_save_screen();                      //save changes to eeprom
        setup_menu_screen();                      //re-setup menu screen
        optn = EXT_OPTN;                // assume exit back to main screen
        n_state = MENU_STATE;                     //proceed to the menu
        menu_press = false;
      }else
        n_state = CALLIBRATE_STATE;             //loop back till menu press

      break;
  }

  //actual transition
  c_state = n_state;
}






//-------------------------------------------MENU/CONFIG/CALLIBRATION FUNCTIONS-------------------------------

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
    case CALLIBRATE_OPTN:
      lcd.setCursor(15,0);
      lcd.blink();
      break;
    case EXT_OPTN:
      lcd.setCursor(15,1);
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


//---------------Function to modify weight constants' minimum--------------
//-> use function to update constants during config change screen
// this function is for changing minimum part of the constant
void config_change_min(){

  float optn_increment = 0;
  
  check_btns();

  //set increments/decrement based on flags
  if(left_press){
    optn_increment = -0.1f;
    left_press = false;
  }else if(right_press){
    optn_increment = 0.1f;
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

  float optn_increment = 0.0f;
  
  check_btns();

  //set increments/decrement based on flags
  if(left_press){
    optn_increment = -0.1f;
    left_press = false;
  }else if(right_press){
    optn_increment = 0.1f;
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

//----------------Function to modify callibration factor-------------------
//-> use function to update pre-set callibration factor
void callibration_screen(){

  float factor_increment = 0.0f;
  //check buttons first
  check_btns();

  //set if increase or decrease
  if(left_press) {
    factor_increment = -1.0f;
    //clear button flag
    left_press = false;
  }else if(right_press){
    factor_increment = 1.0f;
    //clear button flag
    right_press = false;
  }

  //update callibration factor
  callibration_factor += factor_increment;

  scale.set_scale(callibration_factor);
}

//--------------------Function to display main screen----------------------
//-> use function to display normal segregator function
void main_screen(){
    
    float units = scale.get_units();

   

    //if serial is available send bits
    if(Serial.available())
      Serial.println(units); // print out ave measurements
    
    lcd.setCursor(8,0);

    //set measurement threshold at >= 1g
    if(units >= 1) lcd.print(units,2);
    else{
      lcd.print(0);
      lcd.print("     ");
    }
    
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
