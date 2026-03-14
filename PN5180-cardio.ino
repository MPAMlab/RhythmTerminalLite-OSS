#include "Config.h"
#include "src/PN5180/PN5180.h"
#include "src/PN5180/PN5180FeliCa.h"
#include "src/PN5180/PN5180ISO15693.h"
#if WITH_ISO14443 == 1
  #include "src/PN5180/PN5180ISO14443.h"
#endif
#include "CARDIOHID.h"
#include "Joystick.h"

PN5180FeliCa nfcFeliCa(PN5180_PIN_NSS, PN5180_PIN_BUSY, PN5180_PIN_RST);
PN5180ISO15693 nfc15693(PN5180_PIN_NSS, PN5180_PIN_BUSY, PN5180_PIN_RST);
#if WITH_ISO14443 == 1
  PN5180ISO14443 nfc14443(PN5180_PIN_NSS, PN5180_PIN_BUSY, PN5180_PIN_RST);
#endif
CARDIOHID_ Cardio;

#if WITH_KEYPAD == 1
  // Matrix dimensions from Config.h defines
  const byte ROWS = 4;
  const byte COLS = 5;
  const byte rowPins[ROWS] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};
  const byte colPins[COLS] = {PIN_COL1, PIN_COL2, PIN_COL3, PIN_COL4, PIN_COL5};

  // State & Debounce Tracking
  bool lastState[ROWS][COLS]; 
  byte debounceCounter[ROWS][COLS];
  const byte DEBOUNCE_THRESHOLD = 5; 

  // Joystick Configuration
  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
    JOYSTICK_TYPE_GAMEPAD,
    20, 0,                  
    false, false, false,     
    false, false, false,     
    false, false,            
    false, false, false);    
#endif

#if WITH_LEDS == 1
  byte ledPins[5] = {PIN_LED1, PIN_LED2, PIN_LED3, PIN_LED4, PIN_LED5};
#endif

void printHex(uint8_t *data, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    if (i < length - 1) Serial.print(" ");
  }
  Serial.println();
}
 
void setup() {
  Serial.begin(115200);

#if WITH_LEDS == 1
  for(int i=0; i<5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Turn ON (sink to MCU)
  }
  delay(500);
  for(int i=0; i<5; i++) {
    digitalWrite(ledPins[i], HIGH); // Turn OFF
  }
#endif

#if WITH_KEYPAD == 1
  for (int i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT);
    for (int j = 0; j < COLS; j++) {
      lastState[i][j] = false;
      debounceCounter[i][j] = 0;
    }
  }
  Joystick.begin();
  Serial.println("Joystick Initialized");
#endif

/* NFC */
  nfcFeliCa.begin();
  nfcFeliCa.reset();

  uint8_t productVersion[2];
  nfcFeliCa.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  if (0xff == productVersion[1]) { 
    Serial.println("NFC PN5180 Initialization FAILED!");
#if WITH_LEDS == 1
    for(int i=0; i<5; i++) digitalWrite(ledPins[i], LOW);
#endif
    exit(-1); 
  }
  
  uint8_t firmwareVersion[2];
  nfcFeliCa.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  uint8_t eepromVersion[2];
  nfcFeliCa.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  nfcFeliCa.setupRF();
  Serial.println("NFC PN5180 Initialized");
}

unsigned long lastReport = 0;
int cardBusy = 0;

void loop() {
#if WITH_KEYPAD == 1
  keypadCheck();
#endif
  
  /* NFC */
  if (millis()-lastReport < cardBusy) return;
  
  cardBusy = 0;
  uint8_t uid[8] = {0,0,0,0,0,0,0,0};
  uint8_t hid_data[8] = {0,0,0,0,0,0,0,0};
  
#if WITH_ISO14443 == 1
  nfc14443.reset();
  nfc14443.setupRF();
  uint8_t uidLengthMF = nfc14443.readCardSerial(uid);
  if (uidLengthMF > 0) 
  {
      // Translate to Spice2x / Aime E004 format
      hid_data[0] = 0xE0;
      hid_data[1] = 0x04;
      hid_data[2] = uid[0];
      hid_data[3] = uid[1];
      hid_data[4] = uid[2];
      hid_data[5] = uid[3];
      hid_data[6] = uid[0];
      hid_data[7] = uid[1];
      
      Cardio.sendState(2, hid_data);
      Serial.print("ISO14443 Detected. Translated to: ");
      printHex(hid_data, 8);
      lastReport = millis();
      cardBusy = 3000;
      return;
    }
#endif 

  nfcFeliCa.reset();
  nfcFeliCa.setupRF();
  uint8_t uidLength = nfcFeliCa.readCardSerial(uid);
    if (uidLength > 0) {
      Cardio.sendState(2, uid);      
      Serial.print("FeliCa Detected. IDm: ");
      printHex(uid, 8);
      lastReport = millis();
      cardBusy = 3000;
      return;
    }

  nfc15693.reset();
  nfc15693.setupRF();
  ISO15693ErrorCode rc = nfc15693.getInventory(uid);
  if (rc == ISO15693_EC_OK ) {
    for (int i=0; i<8; i++) {
      hid_data[i] = uid[7-i];
    }
    Cardio.sendState(1, hid_data);
    Serial.print("ISO15693 Detected. UID: ");
    printHex(hid_data, 8);
    lastReport = millis();
    cardBusy = 3000;
    return;
  }
  lastReport = millis();
  cardBusy = 200;
}

#if WITH_KEYPAD == 1
void keypadCheck(){
  for (int r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(50); 

    for (int c = 0; c < COLS; c++) {
      bool rawRead = (digitalRead(colPins[c]) == LOW); 
      
      if (rawRead != lastState[r][c]) {
        debounceCounter[r][c]++;
        if (debounceCounter[r][c] >= DEBOUNCE_THRESHOLD) {
          lastState[r][c] = rawRead;
          debounceCounter[r][c] = 0;
          
          int buttonIdx = (r * COLS) + c;
          if (lastState[r][c]) {
            Joystick.setButton(buttonIdx, 1);
            Serial.print("Button "); Serial.print(buttonIdx + 1); Serial.println(" Pressed");
#if WITH_LEDS == 1
            if (r == 0) digitalWrite(ledPins[c], LOW); 
#endif
          } else {
            Joystick.setButton(buttonIdx, 0);
            Serial.print("Button "); Serial.print(buttonIdx + 1); Serial.println(" Released");
#if WITH_LEDS == 1
            if (r == 0) digitalWrite(ledPins[c], HIGH); 
#endif
          }
        }
      } else {
        debounceCounter[r][c] = 0;
      }
    }
    pinMode(rowPins[r], INPUT);
  }
}
#endif
