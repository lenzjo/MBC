/* TLC-SBC Development
 * Test 1: Monitor a free-running 6502
 * Note:   Tested only with a W65C02S
 * This is to be used with a MCP23017
 */
/************************************************************************
 *
 * Purpose: Initially to monitor the address and data bus of a 6502, but 
 * the hardware could be used as a Static Processor Emulator to test the
 * circuitry surrounding the processor.
 *
 * NOTE: This program does NOT read the Control Lines as it is not within the
 * requirements of this program. If you want to read the Control Bus there is
 * sufficient documentation below to create the needed functionality.
 *
 * Requirements: To use this program you will need an arduino of some kind, 
 * I used a Nano, 2x MCP23017 and a couple of switches or jumpers.
 *
 * Pin Usage:
 * 23017 @ addr 32:
 *   Port A: address lines A8-A15
 *   Port B: address lines A0-A7
 * 23017 @ addr 33:
 *   Port A: data bus d0-d7
 *   Port B:
 *     Bit    :  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7   |
 *     Purpose: R/W | RES | BE  | ML  | RDY | IRQ | NMI | SYNC |
 *
 * Some of these pins are inputs to the CPU and can cause program malfunction
 * if not handled correctly. For the purpose of this program the whole of
 * Port B has benn made an input so that you can read the status of the pins.
 *
 * Nano Usage:
 *  A0 : Mode switch - can be a toggle SPST switch or a fly lead that can be
 *       connected to either +5v or GND.
 *  A1 : Single Step - A momentary push button
 *  A4 : SDA
 *  A5 : SCL
 *  PD3: Used to produce clock pulse for the 6502
 *
 * If A0 is high, we are in free run mode and will produce continuous clock
 * pulse and display the current addr and data in the Serial Monitor.
 *
 * If A0 is low, we are in Single Step mode, a single clock pulse will be 
 * emitted each time the A1 push button is pressed.
 *
 *     
 ***********************************************************************/

#include "Wire.h"

//MCP23017 Addresses
const int addrLines = 0b00100000;   // @ addr $20
const int dataLines = 0b00100001;   // @ addr $21

// MCP23017 Registers
const int IORA = 0x00;              // I/O reg port A
const int IORB = 0x01;              // I/O Reg port B
const int DARA = 0x12;              // Data Reg port A
const int DARB = 0x13;              // Data Reg port B

// Nano pin usage
const int PH2_pin = 3;              // Clock output
const int MODE_pin = A0;            // Freerun/SingleStep switch
const int BUTTON_pin = A1;          // Single Step Button
const int RANGE_pin = A2;           // Connected to a pot to vary clock freq.
const int LED_pin = 13;             // Just for testing

const int FREERUN = 1;

int mode = LOW;                     // HIGH = freerun, LOW = Single-Step
int PULSE = 500;                    // Change the length of the clock pulse here

boolean lastButton = LOW;
boolean currButton = LOW;

boolean TESTING = true;             // If in testing mode flash the led with each clock pulse. 

int addrH;                          // Value of A8-A15
int addrL;                          // Value of A0-A7
long curr_addr = 0;
long prev_addr = 0;


//===  SETUP  ===================================================
void setup (){
  Wire.begin();
  
  // Setup the MCP23017 ports as inputs, this is the default status
  // but I don't always trust a data sheet ;)
  
  // Setup to read Address Lines
  Wire.beginTransmission(addrLines);
  Wire.write(IORA);                 // Select Port A
  Wire.write(0xff);                 // Make it all inputs
  Wire.endTransmission();
  Wire.beginTransmission(addrLines);
  Wire.write(IORB);                 // Select Port B
  Wire.write(0xff);                 // Make it all inputs
  Wire.endTransmission();
  
  // Setup Data and Ctrl Lines
  Wire.beginTransmission(dataLines);
  Wire.write(IORA);                 // Select Port A - Data bus
  Wire.write(0xff);                 // Make it all inputs
  Wire.endTransmission();
  Wire.beginTransmission(dataLines);
  Wire.write(IORB);                 // Select Port A - Ctrl Lines
  Wire.write(0xff);                 // Make it all inputs
  Wire.endTransmission();
  
  Serial.begin (115200);            // Setup for Serial Monitor
  
  pinMode(PH2_pin, OUTPUT);         // Clock pin is output
  digitalWrite (PH2_pin, HIGH);     // Set PH2 HIGH
  
  pinMode(MODE_pin, INPUT);         // Mode "switch"
  pinMode(BUTTON_pin, INPUT);       // Normally LOW, HIGH = Single Clock Cycle
  pinMode(RANGE_pin, INPUT);        // To read val of pot.
  pinMode(LED_pin, OUTPUT);         // Enable the onboard led
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
  if(val < 16) { Serial.print("0"); }
  Serial.print(val, HEX);
}

//=== Read A8-A15 from port A ===
int getAddrH(){
  Wire.beginTransmission(addrLines);
  Wire.write(DARA);                           // Select Port A
  Wire.endTransmission();
  Wire.requestFrom(addrLines, 1);             // Request 1 byte
  int ah = Wire.read();                       // Try to read that byte
  Wire.endTransmission();
  return ah;
}

//=== read A0-A7 from port B ===
int getAddrL(){
  Wire.beginTransmission(addrLines);
  Wire.write(DARB);                           // Select Port B
  Wire.endTransmission();
  Wire.requestFrom(addrLines, 1);
  int al = Wire.read();                       // Try to read that byte
  Wire.endTransmission();
  return al;
}

//=== Read D0-D7 from port A ===
int getDataBus(){
  Wire.beginTransmission(dataLines);
  Wire.write(DARA);                           // Select Port A
  Wire.endTransmission();
  Wire.requestFrom(dataLines, 1);
  int da = Wire.read();                       // Try to read that byte
  Wire.endTransmission();
  return da;
}


//=== read and Display state of Address Bus ===
void readAddressBus(){
  // Read the address lines
  addrH = getAddrH();                         // Get HI byte of addr
  addrL = getAddrL();                         // Get LO byte of addr
  curr_addr = (addrH * 16) + addrL;           // Combine to get current addr

  // Update display if address changes
  if (curr_addr != prev_addr){                // If addr has changed
    Serial.print("$");                        // Display it in $xxxx format
    printHexByte(addrH);
    printHexByte(addrL);
  }
  prev_addr = curr_addr;                      // Remember curr addr for next time round
}

//=== Read and print the data bus ================
void readDataBus(){
  int dataBus = getDataBus();
  printHexByte(dataBus);
}


//===  MAIN LOOP  ===============================================

void loop (){
  mode = digitalRead(MODE_pin);                   // Freerun or SingleStep?
  PULSE = analogRead(RANGE_pin);                  // Get val for clock pulse width
  
  if(mode == FREERUN){
    pulseClock();                                 // Just pulse the clock in Free Run mode 
    readAddressBus();                             // and show the current address
    Serial.println(": ");
    readDataBus();                                // then show what's on the Data Bus
  }
  
  else {                                          // We're in Single Step mode
    currButton = debounce(lastButton);            // Get button status
    if(lastButton == LOW && currButton == HIGH){  // Button has just been pressed!
      pulseClock();                               // So pulse the clock
      readAddressBus();                           // and show the current address
      Serial.println(": ");
      readDataBus();                              // then show what's on the Data Bus
    }
    lastButton = currButton;
  }
}
 

