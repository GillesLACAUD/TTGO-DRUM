#include <Arduino.h>

#ifdef __MIDI__
#define MIDI_EXTRN
#else
#define MIDI_EXTRN 
#endif

/*
 * look for midi interface using 1N136
 * to convert the MIDI din signal to
 * a uart compatible signal
 */
#define RXD2 21 /* U2RRXD */
#define TXD2 22

#define MIDI_CLOCK               0xF8
#define MIDI_START               0xFA
#define MIDI_CONTINUE            0xFB
#define MIDI_STOP                0xFC
#define MIDI_SYSTEM_EXCLUSIVE    0xF0
#define MIDI_PROGRAM_CHANGE      0xC0


// Section                  OSC/FILTER/LFO....
// Pot Number               0 to 11 or less or more
// Text Label               Label for the Pot OSC/DETUNE/SUB....
// Value Type               Value type text or Numeric
// Text Value               Text for a specific pot io Wave OSC SIN/TRI/SAW... 
// MIDI CC                  Incoming Midi CC 


#define MAX_CHAR_LABEL  5
#define MAX_POT         12

typedef struct Def_Pot
{
    uint8_t     nb;
    char        label[MAX_CHAR_LABEL];
    char*       ptlist;                     // if null print the data
    uint8_t     midicc;
};

typedef struct Def_Section
{
    uint8_t     section;
    Def_Pot     pot[MAX_POT];
};


enum midi_cc
{

  MIDI_CC_TEMPO=5,
  
  // Volume for each instrument
  MIDI_CC_VOL1=10,
  MIDI_CC_VOL2,
  MIDI_CC_VOL3,
  MIDI_CC_VOL4,
  MIDI_CC_VOL5,
  MIDI_CC_VOL6,
  MIDI_CC_VOL7,
  MIDI_CC_VOL8,

  // Pan for each instrument
  MIDI_CC_PAN1=20,
  MIDI_CC_PAN2,
  MIDI_CC_PAN3,
  MIDI_CC_PAN4,
  MIDI_CC_PAN5,
  MIDI_CC_PAN6,
  MIDI_CC_PAN7,
  MIDI_CC_PAN8,

  // Size for each instrument
  MIDI_CC_SIZE1=30,
  MIDI_CC_SIZE2,
  MIDI_CC_SIZE3,
  MIDI_CC_SIZE4,
  MIDI_CC_SIZE5,
  MIDI_CC_SIZE6,
  MIDI_CC_SIZE7,
  MIDI_CC_SIZE8,

  // Reverse for each instrument
  MIDI_CC_REV1=40,
  MIDI_CC_REV2,
  MIDI_CC_REV3,
  MIDI_CC_REV4,
  MIDI_CC_REV5,
  MIDI_CC_REV6,
  MIDI_CC_REV7,
  MIDI_CC_REV8,

  // Delay parameters
  MIDI_CC_DELAY_IIME=70,
  MIDI_CC_DELAY_VOL=71,
  MIDI_CC_DELAY_FEED=72,

  MIDI_CC_GVOL=60,      // Global Volume
  MIDI_CC_PITCH=61,     // Global pitch
  MIDI_CC_COLOR=62,     // Screen color

};


/* use define to dump midi data */
#define DUMP_SERIAL2_TO_SERIAL

/* constant to normalize midi value to 0.0 - 1.0f */
#define NORM127MUL	0.007874f

MIDI_EXTRN inline void Midi_NoteOn(uint8_t note,uint8_t vel);
MIDI_EXTRN inline void Midi_NoteOff(uint8_t note,uint8_t vel);
MIDI_EXTRN inline void Midi_ControlChange(uint8_t channel, uint8_t data1, uint8_t data2);
MIDI_EXTRN inline void Midi_ProgramChange(uint8_t channel, uint8_t data1);
MIDI_EXTRN inline void HandleShortMsg(uint8_t *data);
MIDI_EXTRN void Midi_Setup();
MIDI_EXTRN void Midi_Process();
