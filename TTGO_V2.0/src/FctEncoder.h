#ifdef __FCTENCODER__
#define FCTENCODER_EXTRN
#else
#define FCTENCODER_EXTRN 	extern
#endif


FCTENCODER_EXTRN int Fct_ChangeMidiRx(int);
FCTENCODER_EXTRN int Fct_ChangeGuiColor(int);

FCTENCODER_EXTRN int Fct_ChangePattern(int);
FCTENCODER_EXTRN int Fct_ChangeTempo(int);
FCTENCODER_EXTRN int Fct_ChangeVolume(int);
FCTENCODER_EXTRN int Fct_PBStartStop(int);

FCTENCODER_EXTRN int Fct_PBTestSound(int);

FCTENCODER_EXTRN int Fct_PBChangeScreen(int);

FCTENCODER_EXTRN int Fct_PBSelPotAll(int);

FCTENCODER_EXTRN int Fct_ChangeSelVol(int);
FCTENCODER_EXTRN int Fct_ChangeValVol(int);

FCTENCODER_EXTRN int Fct_ChangeSelPan(int);
FCTENCODER_EXTRN int Fct_ChangeValPan(int);

FCTENCODER_EXTRN int Fct_DelayTime(int);
FCTENCODER_EXTRN int Fct_DelayAmp(int);
FCTENCODER_EXTRN int Fct_DelayRepeat(int);


FCTENCODER_EXTRN int Fct_ChangeSelPad(int);
FCTENCODER_EXTRN int Fct_PBPadOnOff(int);

FCTENCODER_EXTRN int Fct_ChangeSelInst(int);

FCTENCODER_EXTRN int Fct_ChangeSelDecay(int);
FCTENCODER_EXTRN int Fct_ChangeSelReverse(int);
FCTENCODER_EXTRN int Fct_ChangeValDecay(int);
FCTENCODER_EXTRN int Fct_PBReverse(int);

FCTENCODER_EXTRN int Fct_ChangePitch(int encid);
