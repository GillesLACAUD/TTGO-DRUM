

#ifdef __ENCODER__
#define ENCODER_EXTRN
#else
#define ENCODER_EXTRN 	extern
#endif

#include "stdint.h"
#include "global.h"
#include "pattern_and_samples.h"
#include "FctEncoder.h"

extern int Ihm_PotSelect;
extern int Ihm_PotVal;


typedef enum Button_e {
  Open = 0,
  Closed,

  Pressed,
  Held,
  Released,

  Clicked,
  DoubleClicked

} Button;

#define TYPE_DATA	0
#define TYPE_LIST	1

typedef struct
{
  char 		    Name[16];
  uint8_t		  Type;
  const char	*ptTabList;	// pt to the list
  char		    Index;			// Nb item in list
  int*		    Data;
  uint8_t		  SizeData;
  int 		    MinData;
  int 		    MaxData;
  int 		    Step;
  int 		    (*ptrfunctValueChange) (int);
  int 		    (*ptrfunctBPOn) (int);
  int 		    (*ptrfunctBPOff) (int);
  int 		    (*ptrfunctBPHold) (int);
  int 		    (*ptrfunctBPDoubleClick) (int);

} Encoder_Data;

ENCODER_EXTRN  Encoder_Data	DataNull;
ENCODER_EXTRN  int           IntNull;

typedef struct
{
  uint8_t 			id;
  // Define the GPIO pin
  const uint8_t pinA;
  const uint8_t pinB;
  const uint8_t pinBTN;

  uint8_t 			mode;
  int16_t			 	val;
  int16_t 			oldval;
  uint8_t 			state;

  volatile int16_t 	delta;
  volatile int16_t 	last;
  uint8_t 			    steps;
  volatile int16_t 	acceleration;
  int16_t           cptacc;

  uint8_t				    doubleClickEnabled;
  uint8_t				    accelerationEnabled;
  //Button 				  button;
  uint8_t           button;
  Encoder_Data*		  data;
  uint16_t 			    keyDownTicks;
  uint16_t 			    doubleClickTicks;
  uint8_t				    ClkDirection;
} Encoder;



ENCODER_EXTRN  int test;
ENCODER_EXTRN  char TabListNull[0][0];

#define	A_MAX_ENCODER_DATA	5+2+1
#define	B_MAX_ENCODER_DATA	5+2+1
#define	C_MAX_ENCODER_DATA	5+2+1

#define	END_A_PARAM			9
#define	END_B_PARAM			16

ENCODER_EXTRN	uint8_t	Tab_Enc_Param[4];

#define MAX_DEST		5
#define MAX_CHARLIST	10

#ifdef __ENCODER__


int FctNull(int val)
{
  return val;
}


Encoder_Data	A_TabEncoder_Data[A_MAX_ENCODER_DATA] =
{
  "PATTERN",  TYPE_DATA, &TabListNull[0][0], 0, &gui8_PatSelect     , 1, 0,PATTERN_MAX, 1, Fct_ChangePattern, Fct_PBStartStop, FctNull, FctNull,Fct_PBStartStop,
                        "TIME",     TYPE_DATA, &TabListNull[0][0], 0, &Delay_Time         , 1, 1,95         , 1, Fct_DelayTime, Fct_PBStartStop, FctNull, FctNull, FctNull,
  "SELECT",   TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotSelect      , 1, 0, 7         , 1, Fct_ChangeSelVol, Fct_PBStartStop, FctNull, FctNull, Fct_PBSelPotAll,
  "SELECT",   TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotSelect      , 1, 0, 7         , 1, Fct_ChangeSelPan, Fct_PBStartStop, FctNull, FctNull, Fct_PBSelPotAll,
  "SELECT",   TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotSelect      , 1, 0, 7         , 1, Fct_ChangeSelDecay, Fct_PBStartStop, FctNull, FctNull, Fct_PBSelPotAll,
  "SELECT",   TYPE_DATA, &TabListNull[0][0], 0, &gi16_SelReverse    , 1, 0, 7         , 1, Fct_ChangeSelReverse, Fct_PBStartStop, FctNull, FctNull, Fct_PBSelPotAll,
  "INST",     TYPE_DATA, &TabListNull[0][0], 0, &gi16_SelInst       , 1, 0, 7         , 1, Fct_ChangeSelInst, Fct_PBStartStop, FctNull, FctNull,Fct_PBChangeScreen,
  "PITCH",    TYPE_DATA, &TabListNull[0][0], 0, &gui16_GPitch       , 1, 10,250       , 1, Fct_ChangePitch, Fct_PBStartStop, FctNull, FctNull,Fct_PBChangeScreen,
 
};

Encoder_Data	B_TabEncoder_Data[B_MAX_ENCODER_DATA] =
{
  "TEMPO",    TYPE_DATA, &TabListNull[0][0], 0, (int*)&gui8_Afftempo, 1, 10, 240      , 1, Fct_ChangeTempo, Fct_PBTestSound, FctNull, FctNull, FctNull,
  "AMP",      TYPE_DATA, &TabListNull[0][0], 0, &Delay_Amp          , 1, 0, 100       , 1, Fct_DelayAmp, FctNull, FctNull, FctNull, FctNull,
  "VALUE",		TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotVal         , 1, 0, 100       , 1, Fct_ChangeValVol, FctNull, FctNull, FctNull, FctNull,
  "VALUE",    TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotVal         , 1, 0, 100       , 1, Fct_ChangeValPan, FctNull, FctNull, FctNull, FctNull,
  "VALUE",    TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotVal         , 1, 0, 100       , 1, Fct_ChangeValDecay, FctNull, FctNull, FctNull, FctNull,
  "VALUE",    TYPE_DATA, &TabListNull[0][0], 0, &Ihm_PotVal         , 1, 0, 100       , 1, FctNull, Fct_PBReverse, FctNull, FctNull, FctNull,
  "SELECT",   TYPE_DATA, &TabListNull[0][0], 0, &gui16_SelPad       , 1, 0, 15        , 1, Fct_ChangeSelPad, Fct_PBPadOnOff, FctNull, FctNull, FctNull,
  "MIDIRX",   TYPE_DATA, &TabListNull[0][0], 0, &gui8_MidiRX        , 1, 1, 16        , 1, Fct_ChangeMidiRx, FctNull, FctNull, FctNull, FctNull,

   
};

Encoder_Data	C_TabEncoder_Data[C_MAX_ENCODER_DATA] =
{
  "VOLUME",   TYPE_DATA, &TabListNull[0][0], 0, &gui16_GVolume      , 1, 0, 100, 1      , Fct_ChangeVolume, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "FEED",     TYPE_DATA, &TabListNull[0][0], 0, &Delay_Repeat       , 1, 0, 100, 1      , Fct_DelayRepeat, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "",         TYPE_DATA, &TabListNull[0][0], 0, &IntNull            , 1, 0, 100, 1      , FctNull, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "",         TYPE_DATA, &TabListNull[0][0], 0, &IntNull            , 1, 0, 100, 1      , FctNull, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "",         TYPE_DATA, &TabListNull[0][0], 0, &IntNull            , 1, 0, 100, 1      , FctNull, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "",         TYPE_DATA, &TabListNull[0][0], 0, &IntNull            , 1, 0, 100, 1      , FctNull, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "",         TYPE_DATA, &TabListNull[0][0], 0, &IntNull            , 1, 0, 100, 1      , FctNull, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  "COLOR",    TYPE_DATA, &TabListNull[0][0], 0, &gui8_GuiColor      , 1, 0,  4 , 1      , Fct_ChangeGuiColor, Fct_PBChangeScreen, FctNull, FctNull,Fct_PBChangeScreen,
  
};


#else
extern int FctNull(int val);
extern Encoder_Data	A_TabEncoder_Data[A_MAX_ENCODER_DATA];
extern Encoder_Data	B_TabEncoder_Data[B_MAX_ENCODER_DATA];
extern Encoder_Data	C_TabEncoder_Data[C_MAX_ENCODER_DATA];

extern char TabListDest[MAX_DEST][MAX_CHARLIST];

#endif

#ifdef __ENCODER__
//                ID  PA    PB    PP    M   V   OV  S   D   L   S   A   CA  DC    AE    B
Encoder Enc_A = { 1,  36,   37,   38,   0,  0,  0,  0,  4,  0,  1,  1,  0,  1,    1,    0, &A_TabEncoder_Data[0], 0, 0,0};
Encoder Enc_B = { 2,  39,   32,   2,    0,  0,  0,  0,  4,  0,  1,  1,  0,  1,    1,    0, &B_TabEncoder_Data[0], 0, 0,0};
Encoder Enc_C = { 3,  33,   27,   15,   0,  0,  0,  0,  4,  0,  1,  1,  0,  1,    1,    0, &C_TabEncoder_Data[0], 0, 0,0};
#else
extern Encoder Enc_A;
extern Encoder Enc_B;
extern Encoder Enc_C;
#endif

ENCODER_EXTRN	void	 	  Enc_Init();
ENCODER_EXTRN uint8_t   Enc_Timer(Encoder* enc);
ENCODER_EXTRN void      Enc_GetValue(Encoder* enc);
ENCODER_EXTRN	Button 		Enc_GetButton(Encoder* enc);
ENCODER_EXTRN	void 		  Enc_SetDoubleClickEnabled(Encoder* enc, uint8_t b);
ENCODER_EXTRN	uint8_t 	Enc_GetDoubleClickEnabled(Encoder* enc);
ENCODER_EXTRN	void 		  Enc_SetAccelerationEnabled(Encoder* enc, uint8_t a);
ENCODER_EXTRN	uint8_t 	Enc_GetAccelerationEnabled(Encoder* enc);
ENCODER_EXTRN	int8_t 		Enc_TestClk(Encoder* enc);
