#ifndef CONFIG_h
#define CONFIG_h

/* ISO14443 support (for older Aime/Nesica/BANAPASSPORT cards... reader will pretend it was a FeliCa for maximum cardio compatibility) */
#define WITH_ISO14443 1

/* ISO15693 support (Older Banapassport). Set to 0 if not needed to reduce scan cycle time. */
#define WITH_ISO15693 1

/* Pinout for the PN5180 free pins */
#define PN5180_PIN_NSS  23 // PF0
#define PN5180_PIN_BUSY 22 // PF1
#define PN5180_PIN_RST  21 // PF4

/* Use a matrix keypad */
#define WITH_KEYPAD 1
  #define PIN_ROW1 5  // PC6
  #define PIN_ROW2 6  // PD7
  #define PIN_ROW3 4  // PD4
  #define PIN_ROW4 1  // PD3
  #define PIN_COL1 20 // PF5
  #define PIN_COL2 19 // PF6
  #define PIN_COL3 18 // PF7
  #define PIN_COL4 7  // PE6
  #define PIN_COL5 13 // PC7

/* LEDs */
#define WITH_LEDS 1
  #define PIN_LED1 8  // PB4
  #define PIN_LED2 9  // PB5
  #define PIN_LED3 10 // PB6
  #define PIN_LED4 11 // PB7
  #define PIN_LED5 12 // PD6

/* Player ID (1 or 2) */
#define CARDIO_ID 1
/* Enable custom VID/PID (will require to hold reset button to reflash firmware) */
#define CUSTOM_VIDPID 1

#endif
