#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define EXT_MENU  0

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int state , n_state, optn;
bool btn_is_pressed;

void update_fsm();
void state_1();
void state_2();

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Start Screen");

  delay(1000);

  //attach interrupt to detect button press (pulled high)
  attachInterrupt(digitalPinToInterrupt(2),btn_press, FALLING);
  
  //Initialize state machine
  state = 0;
  n_state = 0;
  btn_is_pressed = 0;
  optn = 0;
}


void loop()
{
  lcd.clear();
  
  update_fsm(); //update state machine
  delay(500);
}


//function that takes care of actually updating the state machine
void update_fsm(){
  switch(state){
    case 0:         
      state_1();   //Display main screen
      
      //check btn state 
      if(btn_is_pressed) n_state = 1;  //transition to menu screen
      else n_state = 0;               //else stay in main screen

      
      state = n_state; //transition to next state
      break;
    
    case 1:
      state_2();

      if(optn == EXT_MENU && btn_is_pressed ) n_state = 0;   //transition back to main screen
      else n_state = 1;                                     //else stay in screen

      state = n_state;
      break;
  }

  //clear button
  btn_is_pressed = 0;
}


//function to check button
void btn_press(){
  btn_is_pressed = 1;
}
//main screen 
void state_1(){
  lcd.setCursor(1,0);
  lcd.print("Main screen");
}

//menu screen
void state_2(){
  
  optn = map(analogRead(A6), 0,1023,0,3);

  //test only
  lcd.setCursor(1,optn);
  lcd.print(optn);
}
