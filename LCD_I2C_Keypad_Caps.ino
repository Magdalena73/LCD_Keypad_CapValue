/*********************

Initial code: Example code for the Adafruit RGB Character LCD Shield and Library

**********************/

// include the library code:
#include <Adafruit_RGBLCDShield.h>
#include <Wire.h>
//#include <utility/Adafruit_MCP23017.h>
#define ON 0x7
#define OFF 0x0
#define num_Screens 4

float C_bank[8] = {1.0, 2.2, 4.7, 10.0, 22.0, 47.0, 100.0, 200.0};
float C_array[256];

String screens[num_Screens][2] = {{"Inductance","uH"}, {"Frequency", "kHz"},{"Capacitance is","nF"},{"Implemented C = ","nF"}};
float parameters[num_Screens];

int currentScreen,newScreen;
bool flag_Screen;

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
//******************* generate C_bank possible values *********************
void generate_array(float C_array[256]){
 int i, j, counter, bit_counter;  float C_value;
  counter = 1;
  for(j = 0; j < 256; j++){
    C_array[j] = 0.0; 
    for (i = 0; i < 8; i++){
      bit_counter = (counter >> i) &1;
      if(bit_counter == 1){
        C_array[j] = C_array[j] + C_bank[i];
      }
     }
    counter++;
  }
}

void setup() {
  generate_array(C_array);
  //initialize the ports as outputs, for the relais:
  for(int i= 2; i< 10; i++){
    pinMode(i,OUTPUT);
  }
  //initialize all relais OFF
  
  for(int i= 2; i< 10; i++){
    digitalWrite(i,LOW);
  }
  //Serial.begin(9600);
  //initialize LCD:
  lcd.begin(16, 2);
  lcd.setCursor(0,0); 
  parameters[0] = 100.0;
  parameters[1] = 100.0;
  parameters[2]= 0.0;
  parameters[3] = 0.0;
  currentScreen = 0;
  printScreen();
  lcd.setBacklight(ON);
  flag_Screen = true;
}

// ***************** MAIN ******************************
void loop() {
  uint8_t buttons = lcd.readButtons();
  
 // if (flag_Screen == true)){
  if (buttons) {
    if (buttons & BUTTON_UP) {
      screenChange(0);      
    }
    if (buttons & BUTTON_DOWN) {
      screenChange(1);
    }
    if (buttons & BUTTON_LEFT) {
      parameterChange(1);
    }
    if (buttons & BUTTON_RIGHT) {
      parameterChange(0);
    }
    if (buttons & BUTTON_SELECT) {
      storeValues();
    }
   /* printScreen();
    delay(100);*/
  }
 if (newScreen != currentScreen){
    printScreen();
    delay(100);
    newScreen = currentScreen;
 }
  
}
//****************** functions ******************************************
void screenChange(int input){
  if(input == 0){              // 0 = bttn UP, last menu
    if (currentScreen == 0){
      currentScreen = num_Screens - 1;
    }
    else{
      currentScreen--;
    }
  }
  else if (input == 1){        // 1 = btnn DOWN, next menu
    if(currentScreen == num_Screens - 1){
      currentScreen = 0;
    }
    else{
      currentScreen++;
    }
  }
  
}
//********************************************************
void parameterChange(int key){
 if ((currentScreen == 0)||(currentScreen == 1)){
  if(key == 0){     // 0 = bttn RIGHT, increase the parameter
    parameters[currentScreen]= parameters[currentScreen] + 5.0;
  }
  else if(key == 1){ // 1 = bttn LEFT, decrease the parameter
     parameters[currentScreen]= parameters[currentScreen] - 5.0;
  }
 } 
 printScreen();
 delay(100);
}
//***************************************************
void storeValues()
{  
  float L_value,freq, w, C_value,C_implemented;
  int C_index;
  
  switch (currentScreen){
  case 0: {
      for(int i= 2; i< 10; i++){
         digitalWrite(i,LOW);
      }
      currentScreen = 1;
      break;
   }
  case 1: {      
      L_value = parameters[0];
      freq = parameters[1];
      w = 2.0*PI*freq;
      C_value = 1000000000.0/(w*w*L_value);
      parameters[2] = C_value;
      currentScreen = 2;
      break;
  }
  case 2: { 
      L_value = parameters[0];
      freq = parameters[1];
      w = 2.0*PI*freq;
      C_value = 1000000000.0/(w*w*L_value);
      parameters[2] = C_value;
      C_index = found_value(C_value);
      C_implemented = C_array[C_index];
      parameters[3] = C_implemented;
      C_index = C_index+1;
      C_turn_on(C_index);
      /*printScreen();
      delay(100);*/
      currentScreen = 3;
      break;
   }
   case 3:{
    C_value = parameters[2];
    C_index = found_value(C_value);
    C_implemented = C_array[C_index];
    parameters[3] = C_implemented;
    C_index = C_index+1;
    C_turn_on(C_index);
    currentScreen = 0;
    break;
   }
  }
}
//********************************************
void printScreen() {
  lcd.clear();
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(parameters[currentScreen]);
  lcd.setCursor(7,1);
  lcd.print(screens[currentScreen][1]);
}

//*********************************************
int found_value (float value){
  int i, index;
  for(i = 0; i < 256; i++){
    if(value >= C_array[i]){
      index = i;
    }
    else
    i = 256;
  }
  return(index);
}
//*******************************
void C_turn_on(int counter){
 int bit_counter, i, pin_nr;
    for (i = 0; i < 8; i++){
      pin_nr = i+2;
      bit_counter = (counter >> i) &1;
      if(bit_counter == 1){
        digitalWrite(pin_nr,HIGH);
      }
      else{
       digitalWrite(pin_nr,LOW);
      }
    }
}
