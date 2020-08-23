LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

HX711 scale;


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


/*
//Old weight tresholds from 24/08/2020

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
#define CALLIBRATE_STATE 5


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
#define CALLIBRATE_OPTN 2
#define EXT_OPTN 3


//Config options
#define XS_OPTN 0
#define S_OPTN 1
#define M_OPTN 2
#define L_OPTN 3
#define XL_OPTN 4
#define XXL_OPTN 5
#define JB_OPTN 6
#define CEXT_OPTN 7




float JUMBO_MIN = 79.0;

//2X 80.0-74.0
float XXL_MAX = 77.0;
float XXL_MIN = 72.0;

//XL 69.0-64.6
float XL_MAX = 67.0;
float XL_MIN = 62.6;

//L 64.5-58.0
float L_MAX = 62.5;
float L_MIN = 56.0;

//M 57.8-54.5
float M_MAX = 55.8;
float M_MIN = 52.5;

//S 54.0-49.0
float S_MAX = 52.0;
float S_MIN = 47.0;

//XS 48.0-10.0
float XS_MAX = 46.0;
float XS_MIN = 8.0;

//callibration factor
float callibration_factor = 615.31;

//state transition variables
int c_state , n_state;

//flag
boolean menu_press, left_press, right_press;

//for debouncing
int left_btn_state, last_left_btn_state;
int right_btn_state, last_right_btn_state;
int menu_btn_state, last_menu_btn_state;

//for menu/config options
int optn;
