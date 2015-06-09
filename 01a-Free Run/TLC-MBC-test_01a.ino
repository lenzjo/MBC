/* TLC-SBC Development
 * Test 1: Monitor a free-running 6502
 * Note:   Tested only with a W65C02S
 * This is to be used with a MCP23017
 * Port A is connected to A8-A15
 * Port B is connected to A0-A7
 */

#include "Wire.h"
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 addrbus;  // Instantiate the 23017 object

const int PH2_pin = 3;      // Clock output

const int MODE_pin = A0;    // Freerun/SingleStep switch
const int BUTTON_pin = A1;  // Single Step Button
const int RANGE_pin = A2;   // Connected to a pot to vary clock freq.
const int LED_pin = 13;     // Just for testing

const int FREERUN = 1;

int mode = LOW;             // HIGH = freerun, LOW = Single-Step
int PULSE = 500;            // Change the length of the clock pulse here

boolean lastButton = LOW;
boolean currButton = LOW;

boolean TESTING = true;     // If in testing mode flash the led with each clock pulse. 

int addrH;                  // Value of A8-A15
int addrL;                  // Value of A0-A7
long curr_addr = 0;
long prev_addr = 0;


//===  SETUP  ===================================================
void setup (){
  Serial.begin (115200);
  Wire.begin();                  // Setup I2C
  addrBus.begin();               // Iniz the MCP23017
  pinMode(PH2_pin, OUTPUT);      // Clock pin is output
  digitalWrite (PH2_pin, HIGH);  // Set PH2 HIGH
  pinMode(MODE_pin, INPUT);      // Mode "switch"
  pinMode(BUTTON_pin, INPUT);    // Normally LOW, HIGH = Single Clock Cycle
  pinMode(RANGE_pin, INPUT);     // To read val of pot.
  pinMode(LED_pin, OUTPUT);      // Enable the onboard led
}


//===  MY FUNCTIONS  =============================================

//=== Send one clock cycle ===
void pulseClock(){
  // Pulse the PH2 clock 
  digitalWrite (PH2_pin, LOW);
  if(TESTING) digitalWrite(LED_pin, LOW);
  delay (PULSE);
  
  digitalWrite (PH2_pin, HIGH);
  if(TESTING) digitalWrite(LED_pin, HIGH);
  delay (PULSE);
}


//=== Debounce a button ===
boolean debounce(boolean last){
  boolean current = digitalRead(BUTTON_pin);  // Read the button
  if(last != current){                        // Button has changed
    delay(5);                                 // Wait fo 5ms.
    current = digitalRead(BUTTON_pin);        // and read it again
  }
  return current;
}

void printHexByte( int val){
  if(val < 16) { Serial.print("0") }
  Serial.print(val, HEX);
}


//=== read and Display state of Address Bus ===
void readAddressBus(){
  // Read the address lines
  addrH = addrBus.readGPIO(0);      // Get hi byte of addr
  addrL = addrBus.readGPIO(1);      // Get lo byte of addr
  curr_addr = (addrH * 16) + addrL; // Combine to get current addr

  // Update display if address changes
  if (curr_addr != prev_addr){      // If addr has changed
    Serial.print("$");              // Display it in $xxxx format
    printHexByte(addrH);
    printHexByte(addrL);
    Serial.println(": ");
  }
  prev_addr = curr_addr;            // Remember curr addr for next time round
}


//===  MAIN LOOP  ===============================================

void loop (){
  mode = digitalRead(MODE_pin);                   // Freerun or SingleStep?
  PULSE = analogRead(RANGE_pin);                  // Get val for clock pulse width
  
  if(mode == FREERUN){
    pulseClock();                                 // Just pulse the clock in Free Run mode 
    readAddressBus();                             // and show the current address
  }
  
  else {                                          // We're in Single Step mode
    currButton = debounce(lastButton);            // Get button status
    if(lastButton == LOW && currButton == HIGH){  // Button has just been pressed!
      pulseClock();                               // So pulse the clock
      readAddressBus();                             // and show the current address
    }
    lastButton = currButton;
  }
}
 

