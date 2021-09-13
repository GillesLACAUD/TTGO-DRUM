#ifdef __GLOBAL__
#define GLOBAL_EXTRN
#else
#define GLOBAL_EXTRN 	extern
#endif

// HARDWARE DEFINE

// PIste coupée qui menée a l'ampli op
#define GPIO_SYNC_IN    13

// OK EN COUPANT LA PISTE QUI MENE A L'AMPLI OP
#define GPIO_SYNC_OUT   12      // Normally 12   22= Midi TX pull up sur le TTGo :-(  17 Pas de Click ok ???

#define GPIO_DAC_1   25
#define GPIO_DAC_2   26

//--------- Ringbuf parameters ----------
#define MAX_BUFFER_SAMPLE	1024
#define MAX_INST            8

GLOBAL_EXTRN    uint16_t samplepnt[MAX_INST];
GLOBAL_EXTRN    uint16_t samplecnt[MAX_INST];

GLOBAL_EXTRN int     gi16_trash;                   // 


GLOBAL_EXTRN  unsigned char gui8_RingbufferLeft[MAX_BUFFER_SAMPLE];
GLOBAL_EXTRN  unsigned char gui8_RingbufferRight[MAX_BUFFER_SAMPLE];
GLOBAL_EXTRN uint16_t gui16_RingWrite;
GLOBAL_EXTRN uint16_t gui16_RingRead;

GLOBAL_EXTRN int     gui16_GPitch;                // General Pitch
GLOBAL_EXTRN int     gui16_GVolume;               // General Volume
GLOBAL_EXTRN int     gi16_SelInst;                // Select instrument
GLOBAL_EXTRN int     gi16_SelReverse;             // Select reverse instrument 

GLOBAL_EXTRN uint8_t gui8_Volume[MAX_INST];	    // Volume for each instrument
GLOBAL_EXTRN	uint8_t gui8_Pan[MAX_INST];		    // 0 LEFT 50 CENTER 100 RIGHT
GLOBAL_EXTRN	uint8_t gui8_Decay[MAX_INST];	    //
GLOBAL_EXTRN  uint8_t gui8_Reverse[MAX_INST];   //

GLOBAL_EXTRN int     gui8_MidiRX;       //
GLOBAL_EXTRN int     gui8_GuiColor;     //

GLOBAL_EXTRN  volatile uint32_t  reg_dac;

#define INST_GU     0
#define INST_BG2	1
#define INST_BD		2
#define INST_CL		3
#define INST_CW		4
#define INST_MA		5
#define INST_CY		6
#define INST_QU     7

#define SIZE_TAB_DELAY  44100
#ifdef __GLOBAL__
int Delay_Time = 40;    // 0 to 100 100 = 1s = 44100 Ech
int Delay_Repeat = 1;  // 1 to 8
int Delay_Amp = 10;     // 1 to 100 100 = 1/x
#else
GLOBAL_EXTRN  int Delay_Time;      // 0 to 100 100 = 1s = 44100 Ech
GLOBAL_EXTRN  int Delay_Repeat;    // 1 to 8
GLOBAL_EXTRN  int Delay_Amp;       // 1 to 100 100 = 1/x
#endif

GLOBAL_EXTRN uint8_t syncinstate;
GLOBAL_EXTRN uint8_t oldsyncinstate;
GLOBAL_EXTRN uint8_t cptedge;

GLOBAL_EXTRN  int Delay_TimeEch;   // 0 to 100 100 = 1s = 44100 Ech

GLOBAL_EXTRN  uint16_t CptDelay;
GLOBAL_EXTRN  uint8_t Tab_DelayLeft[SIZE_TAB_DELAY];
GLOBAL_EXTRN  uint8_t Tab_DelayRight[SIZE_TAB_DELAY];

/*---------------------------------------------*/

#ifdef __GLOBAL__
volatile uint16_t gui16_RingCount = MAX_BUFFER_SAMPLE;
uint8_t gui8_MuteMask = 0xFF;
#else
GLOBAL_EXTRN	volatile uint16_t gui16_RingCount;
GLOBAL_EXTRN	uint8_t gui8_MuteMask;
#endif

GLOBAL_EXTRN char messdebug[100];

GLOBAL_EXTRN	uint16_t  gui16_Sample_Reverse;
GLOBAL_EXTRN	uint16_t  gui8_Sample_inc;

GLOBAL_EXTRN	uint16_t gui16_SFREQ;

GLOBAL_EXTRN  uint8_t   gui8_Afftempo;                // From 10 to 240 BPM just for the UI
GLOBAL_EXTRN  volatile  uint32_t  gui32_Maxcpttempo;  // Trigger for the tempo = (gui8_Afftempo*44100)/60
GLOBAL_EXTRN  volatile  uint32_t  gui32_Cpttempo;     // Tempo cpt
GLOBAL_EXTRN  volatile  uint8_t   gui8_TrigBpm;       // New bpm
GLOBAL_EXTRN  volatile  uint8_t   gui8_SyncAnaExt;    // Detect externe sync analog

GLOBAL_EXTRN  uint8_t  gui8_CptSound;
GLOBAL_EXTRN	uint32_t gui32_cptit40K;
GLOBAL_EXTRN	uint32_t gui32_tempo;
GLOBAL_EXTRN	uint32_t gui32_IHM;
GLOBAL_EXTRN  uint32_t gui32_EEPCommit;
GLOBAL_EXTRN  uint32_t gui32_Cpt1ms;

GLOBAL_EXTRN  uint8_t gui8_EEPAskCommit;



#define	TIMER_MIDI_AFF	10000
GLOBAL_EXTRN	uint16_t gui16_TimerMidiIn;
GLOBAL_EXTRN	uint8_t  gui8_MidiIn;

#define	TIMER_MIDI_CLOCK	30000
GLOBAL_EXTRN	uint16_t gui16_TimerMidiClock;
GLOBAL_EXTRN	uint8_t  gui8_MidiClock;
GLOBAL_EXTRN	uint8_t  gui8_MidiCptClock;

GLOBAL_EXTRN	uint8_t gui8_Tempo;
GLOBAL_EXTRN	int gui8_PatSelect;
GLOBAL_EXTRN	uint8_t gui8_PotentiometerSelect;
GLOBAL_EXTRN	uint8_t gui8_PotentiometerVal;
GLOBAL_EXTRN	uint8_t gui8_InstSelect;

GLOBAL_EXTRN	uint8_t	gui8_PB_startstop;
GLOBAL_EXTRN	uint8_t gui8_patlength;
GLOBAL_EXTRN	uint8_t gui8_stepcnt;
GLOBAL_EXTRN  uint8_t gui8_Trigstep;
GLOBAL_EXTRN	uint8_t gui8_SelInstrument;

GLOBAL_EXTRN  int gui16_SelPad;

GLOBAL_EXTRN uint8_t st;


//------------------ EEPROM --------------------------------
// 2    char Ident 0xdead
// 8*16 char 8 Patterns 16 step
// 8*16 char 8 Patterns 12 step
// 32*8 char 32 Pattern 8 Volume
// 32*8 char 32 Pattern 8 Pan
// 32*4 char 32 Pattern 4 char Time/Amp/Repeat/Free

#define EEP_OFF_IDENT       0x00                        // Ident eep programmed with a 0xDEAD code - else write 0x00
#define EEP_OFF_ENCLCK      EEP_OFF_IDENT+2             // Clock direction for the encoder
#define EEP_OFF_LASTP       EEP_OFF_ENCLCK+3            // Keep the last pattern selected
#define EEP_OFF_LASTT       EEP_OFF_LASTP+1             // Keep the last tempo
#define EEP_OFF_LASTV       EEP_OFF_LASTT+1             // Keep the last general volume
#define EEP_OFF_LASTPI      EEP_OFF_LASTV+1             // Keep the last pitch

#define EEP_OFF_PATTERN     EEP_OFF_LASTPI+1             // User patterns
#define EEP_OFF_VOLUME      EEP_OFF_PATTERN+8*32        // Volume Preset and User pattern
#define EEP_OFF_PAN         EEP_OFF_VOLUME+8            // Pan Preset and User pattern
#define EEP_OFF_DECAY       EEP_OFF_PAN+8               // Size Preset and User pattern
#define EEP_OFF_DELAY       EEP_OFF_DECAY+8              // Delay Preset and User pattern
#define EEP_OFF_REVERSE     EEP_OFF_DELAY+4

#define EEP_OFF_MIDIRX      EEP_OFF_REVERSE+8
#define EEP_OFF_GUICOLOR    EEP_OFF_MIDIRX+1

#define EEP_OFF_FREE        EEP_OFF_GUICOLOR+1
