/*
  ModbusRTUMasterExample_ESP32
  
  This example demonstrates how to setup and use the ModbusRTUMaster library on an Arduino Nano ESP32.
  It is intended to be used with a second board running ModbusRTUSlaveExample from the ModbusRTUSlave library.  
  
  Circuit:
  - A pushbutton switch from pin 2 to GND
  - A pushbutton switch from pin 3 to GND
  - A LED from pin 5 to GND with a 330 ohm series resistor
  - A LED from pin 6 to GND with a 330 ohm series resistor
  - A LED from pin 7 to GND with a 330 ohm series resistor
  - A LED from pin 8 to GND with a 330 ohm series resistor
  - The center pin of a potentiometer to pin A0, and the outside pins of the potentiometer to 3.3V and GND
  - The center pin of a potentiometer to pin A1, and the outside pins of the potentiometer to 3.3V and GND 
  - pin 10 to pin 11 of the slave/server board
  - pin 11 to pin 10 of the slave/server board
  - GND to GND of the slave/server board
  - Pin 13 is set up as the driver enable pin. This pin will be HIGH whenever the board is transmitting.
  
  Created: 2023-11-11
  By: C. M. Bulliner
  
*/

#include <ModbusRTUMaster.h>

const uint8_t ledPins[4] = {D8, D7, D6, D5};
const uint8_t buttonPins[2] = {D3, D2};
const uint8_t potPins[2] = {A0, A1};

const uint8_t rxPin = D10;
const uint8_t txPin = D11;
const uint8_t dePin = D13;

ModbusRTUMaster modbus(Serial1, dePin); // serial port, driver enable pin for rs-485 (optional)

bool coils[2];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

void setup() {
  analogReadResolution(10);

  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(potPins[0], INPUT);
  pinMode(potPins[1], INPUT);
  
  modbus.begin(38400, SERIAL_8N1, rxPin, txPin);
}

void loop() {
  coils[0] = !digitalRead(buttonPins[0]);
  coils[1] = !digitalRead(buttonPins[1]);
  holdingRegisters[0] = map(analogRead(potPins[0]), 0, 1023, 0, 255);
  holdingRegisters[1] = map(analogRead(potPins[1]), 0, 1023, 0, 255);
  
  modbus.writeMultipleCoils(1, 0, coils, 2);                       // slave id, starting data address, bool array of coil values, number of coils to write
  modbus.writeMultipleHoldingRegisters(1, 0, holdingRegisters, 2); // slave id, starting data address, unsigned 16 bit integer array of holding register values, number of holding registers to write
  modbus.readDiscreteInputs(1, 0, discreteInputs, 2);              // slave id, starting data address, bool array to place discrete input values, number of discrete inputs to read
  modbus.readInputRegisters(1, 0, inputRegisters, 2);              // slave id, starting data address, unsigned 16 bit integer array to place input register values, number of input registers to read
  
  digitalWrite(ledPins[0], discreteInputs[0]);
  digitalWrite(ledPins[1], discreteInputs[1]);
  analogWrite(ledPins[2], inputRegisters[0]);
  analogWrite(ledPins[3], inputRegisters[1]);
}
