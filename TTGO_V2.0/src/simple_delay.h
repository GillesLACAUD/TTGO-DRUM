/* max delay can be changed but changes also the memory consumption */
#define MAX_DELAY	6000

#include <Arduino.h>

#ifdef __DELAY__
#define DELAY_EXTRN
#else
#define DELAY_EXTRN extern
#endif

/*
 * module variables
 */

#ifdef __DELAY__
float delayToMix = 0.0;
float delayFeedback = 0.0;
uint32_t delayLen = 11098;
uint32_t delayIn = 0;
uint32_t delayOut = 0;
#else
extern float delayToMix;
extern float delayFeedback;
extern uint32_t delayLen;
extern uint32_t delayIn;
extern uint32_t delayOut;
#endif


DELAY_EXTRN uint8_t delayLine_l[MAX_DELAY];
DELAY_EXTRN uint8_t delayLine_r[MAX_DELAY];

DELAY_EXTRN void Delay_Init();
DELAY_EXTRN void Delay_Reset();
DELAY_EXTRN void Delay_Process(float *signal_l, float *signal_r);
DELAY_EXTRN void Delay_SetFeedback(float value);
DELAY_EXTRN void Delay_SetLevel(float value);
DELAY_EXTRN void Delay_SetLength(float value);
