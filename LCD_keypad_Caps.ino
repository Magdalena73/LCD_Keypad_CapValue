#include <LiquidCrystal.h>
/***** Program for the blog: *****/
/**** https://mstronik.blog/2019/09/17/lcd-keypad_menu-to-obtain-capacitance-value/ ****/
/**** Menu asks for required frequency and used inductance to calculate the required capacitance ****/
/**** aproximating to the implemented values of the capacitor bank ****/
/***** Magdalena Vaclavek *****/

// select the pins used on the LCD panel
const int rs = 9, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const float pi = 3.14159;
int lcd_key= 1023;
int lcd_key_in = 1023;
int key_state = 1023;
bool key_flag = LOW;
int last_reading = 1023;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

#define bttnRIGHT  0
#define bttnUP     1
#define bttnDOWN   2
#define bttnLEFT   3
#define bttnSELECT 4
#define bttnNONE   5

#define num_Screens 3
#define Arr_size 50
int16_t keypad_read[Arr_size];

String screens[num_Screens][2] = {{"Inductance","uH"}, {"Frequency", "kHz"},{"Capacitance is","nF"}};
int parameters[num_Screens];
int currentScreen = 0;
int newScreen;

//**************** average function ***************************
int16_t read_average(int16_t Readings[])
{ int16_t min_value,max_value, av;
  uint8_t i;  
  max_value = Readings[0];
  min_value = Readings[0];
  for (i=1; i<Arr_size; i++){
    if (Readings[i]> max_value)
      max_value = Readings[i];
    if (Readings[i]< min_value)
      min_value = Readings[i];          
  }
  
 av = (max_value + min_value)/2;
 return (av);
}

// ************ Function to read the buttons *******************
int read_LCD_buttons()
{
 /*for (int i=0; i < Arr_size; i++){
  keypad_read[i] = analogRead(0);   
 }
 lcd_key_in = read_average(keypad_read);*/
 
 lcd_key_in = analogRead(0);   
 //if the push-button changed, reset the debouncing timer
if (lcd_key_in != last_reading){
  lastDebounceTime = millis();
 }
 // the reading has been there long enough, so take it
 if ((millis() - lastDebounceTime) > debounceDelay){
  if (lcd_key_in != key_state){
    key_state = lcd_key_in;
    if (key_state > 1000){
      key_flag = HIGH;
    }
  }
 }
 last_reading = lcd_key_in;
 
 if (key_flag == HIGH){
 
  if (lcd_key_in > 1000) 
    return bttnNONE;
  if ((lcd_key_in > 0)&&(lcd_key_in < 20))   
    return bttnRIGHT;  //6
  if ((lcd_key_in > 100)&&(lcd_key_in < 200))  
    return bttnUP; //154
  if ((lcd_key_in > 330)&&(lcd_key_in < 430))
    return bttnDOWN; //381
  if ((lcd_key_in > 500)&&(lcd_key_in < 650))
    return bttnLEFT; //602
  if ((lcd_key_in > 800)&&(lcd_key_in < 980))
    return bttnSELECT;//933
  return bttnNONE;  // when all others fail, return NONE
  key_flag = LOW;
  }
}

void setup()
{
 //Serial.begin(9600);
 lcd.begin(16, 2);              
 lcd.setCursor(0,0); 
 parameters[0] = 100;
 parameters[1] = 100;
 parameters[2]= 0;
 currentScreen = 0;
 printScreen();
 delay(100);
}
 
void loop()
{        
 lcd_key = read_LCD_buttons();  

 switch (lcd_key)               
 {
   case bttnRIGHT:
     {
     parameterChange(0);
     printScreen();
     delay(100);
     break;
     }
   case bttnLEFT:
     {
     parameterChange(1);
     printScreen();
     delay(100);
     break;
     }
   case bttnUP:
     {
     screenChange(0);
     break;
     }
   case bttnDOWN:
     {
     screenChange(1);
     break;
     }
   case bttnSELECT:
     {
     storeValues();
     break;
     }
     case bttnNONE:
     {    
     break;
     }
  }
  if (newScreen != currentScreen){
   printScreen();
   delay(100);
  }
  newScreen = currentScreen;
}

void screenChange(int input){
  if(input == 0){              // 0 = bttn UP, next menu
    if (currentScreen == 0){
      currentScreen = num_Screens - 1;
    }
    else{
      currentScreen--;
    }
  }
  else if (input == 1){        // 1 = btnn DOWN, go back to last menu
    if(currentScreen == num_Screens - 1){
      currentScreen = 0;
    }
    else{
      currentScreen++;
    }
  }
  
}

void parameterChange(int key){
 if ((currentScreen == 0)||(currentScreen == 1)){
  if(key == 0){     // 0 = bttn RIGHT, increase the parameter
    parameters[currentScreen]= parameters[currentScreen] + 5;
  }
  else if(key == 1){ // 1 = bttn LEFT, decrease the parameter
     parameters[currentScreen]= parameters[currentScreen] - 5;
  }
 } 
}

void storeValues()
{ int L_value,freq; 
  float w, C_value;
  switch (currentScreen){
  case 0: {
      currentScreen=1;
      delay(100);
      break;
   }
  case 1: {      
      currentScreen=2;
      L_value = parameters[0];
      freq = parameters[1];
      w = 2*pi*freq;
      C_value = 1000000000/(w*w*L_value);
      parameters[2] = int(C_value);
      delay(100);
      break;
  }
  case 2: { 
      L_value = parameters[0];
      freq = parameters[1];
      w = 2.0*pi*freq;
      C_value = 1000000000.0/(w*w*L_value);
      parameters[2] = int(C_value);
     // printScreen();
      delay(100);
      break;
   }
  }
}
void printScreen() {
  lcd.clear();
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(parameters[currentScreen]);
  lcd.setCursor(5,1);
  lcd.print(screens[currentScreen][1]);
}
