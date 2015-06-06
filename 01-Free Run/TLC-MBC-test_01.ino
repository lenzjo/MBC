/* TLC-SBC Development
 * Test 1: Monitor a free-running 6502
 * Note:   Tested only with a W65C02S
 */

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


//===  SETUP  ===================================================
void setup (){
  Serial.begin (115200);
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


//===  MAIN LOOP  ===============================================

void loop (){
  mode = digitalRead(MODE_pin);                   // Freerun or SingleStep?
  PULSE = analogRead(RANGE_pin);                  // Get val for clock pulse width
  
  if(mode == FREERUN){
    pulseClock();                                 // Just pulse the clock in Free Run mode 
  }
  
  else {                                          // We're in Single Step mode
    currButton = debounce(lastButton);            // Get button status
    if(lastButton == LOW && currButton == HIGH){  // Button has just been pressed!
      pulseClock();                               // So pulse the clock
    }
    lastButton = currButton;
  }
}
 

