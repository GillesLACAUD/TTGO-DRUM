
#define __ENCODER__
#include "Encoder.h"
#include "HardwareSerial.h"

#include "ihm.h"

// ----------------------------------------------------------------------------
// Button configuration (values for 1ms timer service calls)
//
#define ENC_BUTTONINTERVAL     4  	// check button every x milliseconds, also debouce time
#define ENC_DOUBLECLICKTIME 1500  	// second click within 600ms
#define ENC_HOLDTIME        2000  	// report held button after 2s
#define ENC_SINGLECLICKONLY 5
// ----------------------------------------------------------------------------
// Acceleration configuration (for 1000Hz calls to ::service())
//
//#define ENC_ACCEL_TOP        300   // max. acceleration: *12 (val >> 8)
//#define ENC_ACCEL_INC        80
//#define ENC_ACCEL_DEC        80

#define ENC_ACCEL_TOP        20   // max. acceleration: *12 (val >> 8)
#define ENC_ACCEL_INC        1
#define ENC_ACCEL_DEC        2

extern portMUX_TYPE timerMux_44100Hertz;

uint8_t senddebug = 0;

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
Button Enc_GetButton(Encoder* enc)
{

}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_SetDoubleClickEnabled(Encoder* enc, uint8_t b)
{

}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t Enc_GetDoubleClickEnabled(Encoder* enc)
{

}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_SetAccelerationEnabled(Encoder* enc, uint8_t a)
{
  enc->accelerationEnabled = a;
  if (enc->accelerationEnabled == 0)
  {
    enc->acceleration = 0;
  }
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t Enc_GetAccelerationEnabled(Encoder* enc)
{
  return enc->accelerationEnabled;
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_Init()
{
  //GPIO_InitTypeDef GPIO_InitStructure;
  Encoder* enc;

  uint8_t configType;
  configType = INPUT_PULLUP;
  pinMode(Enc_A.pinA, configType);
  pinMode(Enc_A.pinB, configType);
  pinMode(Enc_A.pinBTN, configType);

  pinMode(Enc_B.pinA, configType);
  pinMode(Enc_B.pinB, configType);
  pinMode(Enc_B.pinBTN, configType);

  pinMode(Enc_C.pinA, configType);
  pinMode(Enc_C.pinB, configType);
  pinMode(Enc_C.pinBTN, configType);

  enc = &Enc_A;
  if (digitalRead(Enc_A.pinA))
  {
    enc->last = 3;
  }
  if (digitalRead(Enc_A.pinB))
  {
    enc->last ^= 1;
  }
  //Enc_SetAccelerationEnabled(enc, 0);

  enc = &Enc_B;
  if (digitalRead(Enc_B.pinA))
  {
    enc->last = 3;
  }
  if (digitalRead(Enc_B.pinB))
  {
    enc->last ^= 1;
  }
  //Enc_SetAccelerationEnabled(enc, 0);

  enc = &Enc_C;
  if (digitalRead(Enc_C.pinA))
  {
    enc->last = 3;
  }
  if (digitalRead(Enc_C.pinB))
  {
    enc->last ^= 1;
  }
  //Enc_SetAccelerationEnabled(enc, 0);

  Tab_Enc_Param[0] = 2;
  Tab_Enc_Param[1] = 2;
  Tab_Enc_Param[2] = 2;
  Tab_Enc_Param[3] = 2;

  enc->delta = 0;

  Enc_A.ClkDirection = 1;
  Enc_B.ClkDirection = 1;
  Enc_C.ClkDirection = 1;

  //Serial.println("---------- Encoder init done");
}


//                           _______         _______
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

//	new	new	old	old
//	p2	p1	p2	p1	Result
//	----	----	----	----	------
//	0	0	0	0	no movement
//	0	0	0	1	+1
//	0	0	1	0	-1
//	0	0	1	1	+2  (assume pin1 edges only)
//	0	1	0	0	-1
//	0	1	0	1	no movement
//	0	1	1	0	-2  (assume pin1 edges only)
//	0	1	1	1	+1
//	1	0	0	0	+1
//	1	0	0	1	-2  (assume pin1 edges only)
//	1	0	1	0	no movement
//	1	0	1	1	-1
//	1	1	0	0	+2  (assume pin1 edges only)
//	1	1	0	1	-1
//	1	1	1	0	+1
//	1	1	1	1	no movement

int cptacc = 0;      // to test the accelearion

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
uint8_t Enc_Timer(Encoder* enc)
{
  static int cptmoved = 0;
  uint8_t param;
  uint8_t moved;
  int signe;


  enc->cptacc++;

  signe = 1;
  if (enc->ClkDirection == 0)
    signe = -1;

  switch (enc->mode)
  {
    // Mode select Setup Sound
    case 0:
      param = 0;
      break;
    // Mode select Parameter
    case 1:
      param = 1;
      break;
    // Mode Change parameter value
    case 2:
      param = Tab_Enc_Param[enc->id];
      break;
  }
  param = gui8_Ihm_CurrentScreen;

  // New state at 0
  uint8_t s = enc->state & 0x03;

  if (digitalRead(enc->pinA))
    s |= 4;
  if (digitalRead(enc->pinB))
    s |= 8;

  switch (s)
  {
    // No movement
    case 0: case 5: case 10: case 15:
      moved = 0;
      cptmoved--;
      break;

    case 1: case 7: case 8: case 14:
      enc->delta -= 1 * signe;
      moved = 1;
      cptmoved++;
      if (senddebug)
      {
        sprintf(messdebug, "---------- Encoder %d Turn Negatif", enc->id);
        Serial.println(messdebug);
      }
      //enc->val--;
      break;

    case 2: case 4: case 11: case 13:
      enc->delta += 1 * signe;
      moved = 1;
      cptmoved++;
      if (senddebug)
      {
        sprintf(messdebug, "---------- Encoder %d Turn Negatif val %d", enc->id, enc->val);
        Serial.println(messdebug);
      }
      //enc->val++;
      break;
  }
  // Set new state to old state
  enc->state = (s >> 2);

  //---------------------------------------------------------------------
  // SWITH ON/OFF/HELD/DOUBLE CLICK OK 04.02.21
  //---------------------------------------------------------------------

  if (!digitalRead(enc->pinBTN))
  {
    enc->keyDownTicks++;
    if (enc->keyDownTicks > (ENC_HOLDTIME / ENC_BUTTONINTERVAL))
    {
      if (enc->button != Held && senddebug)
        Serial.println("---------- Encoder Button Hold");
      enc->button = Held;
      enc->data[param].ptrfunctBPHold(enc->id);

    }
    if (senddebug)
      Serial.println("---------- Encoder ON");
  }
  // key is now up
  if (digitalRead(enc->pinBTN))
  {
    if (enc->keyDownTicks > ENC_BUTTONINTERVAL)
    {
      if (enc->button == Held)
      {
        enc->button = Released;
        enc->doubleClickTicks = 0;
        if (senddebug)
        {
          Serial.println("---------- Encoder Held");
        }
      }
      else
      {
        // prevent trigger in single click mode
        if (enc->doubleClickTicks > ENC_SINGLECLICKONLY)
        {
          if (enc->doubleClickTicks < (ENC_DOUBLECLICKTIME / ENC_BUTTONINTERVAL))
          {
            enc->button = DoubleClicked;
            enc->doubleClickTicks = 0;
            enc->data[param].ptrfunctBPDoubleClick(enc->id);
            if (senddebug)
            {
              sprintf(messdebug, "---------- Encoder Double Click");
              Serial.println(messdebug);
            }
          }
          else
          {
            if (senddebug)
            {
              Serial.println("---------- Encoder ICI");
            }
            enc->data[param].ptrfunctBPOn(enc->id);
          }
        }
        else
        {
          enc->doubleClickTicks = (enc->doubleClickEnabled) ? (ENC_DOUBLECLICKTIME / ENC_BUTTONINTERVAL) : ENC_SINGLECLICKONLY;
          enc->data[param].ptrfunctBPOn(enc->id);
          if (senddebug)
          {
            Serial.println("---------- Encoder Click");
          }
        }
      }
    }
    enc->keyDownTicks = 0;
  }
  if (enc->doubleClickTicks > 0)
  {
    enc->doubleClickTicks--;
    if (--enc->doubleClickTicks == 0)
    {
      enc->button = Clicked;
    }
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
int8_t Enc_TestClk(Encoder* enc)
{
  static int test;
  uint8_t param;
  uint8_t moved;

  // New state at 0
  uint8_t s = enc->state & 0x03;

  if (digitalRead(enc->pinA))
    s |= 4;
  if (digitalRead(enc->pinB))
    s |= 8;


  switch (s)
  {
    // No movement
    case 0: case 5: case 10: case 15:
      break;

    case 1: case 7: case 8: case 14:
      enc->delta--;
      return (0);
      moved = 1;
      //enc->val--;
      break;

    case 2: case 4: case 11: case 13:
      enc->delta++;
      return (1);
      moved = 1;
      break;
  }
  // Set new state to old state
  enc->state = (s >> 2);
  return (0x55);

}



/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
//***************************************************/
void Enc_GetValue(Encoder* enc)
{
  int16_t val;
  uint8_t param;
  int signe;

  portENTER_CRITICAL_ISR(&timerMux_44100Hertz);
  signe = 1;
  if (enc->ClkDirection == 0)
    signe = -1;


  switch (enc->mode)
  {
    // Mode select Setup Sound
    case 0:
      param = 0;
      break;
    // Mode select Parameter
    case 1:
      param = 1;
      break;
    // Mode Change parameter value
    case 2:
      param = Tab_Enc_Param[enc->id];
      break;
  }

  param = gui8_Ihm_CurrentScreen;

  val = enc->delta;
  enc->delta = val & 3;
  val >>= 2;

  int tmp;
  if (enc->mode == 2)
    tmp = enc->data[Tab_Enc_Param[enc->id]].Step;
  else
    tmp = enc->data[param].Step;

  if (enc->accelerationEnabled)
  {
    int delta = enc->data[param].MaxData - enc->data[param].MinData;
    delta /= 25;
    if (enc->cptacc > 20)
      enc->acceleration = 0;
    else if (enc->cptacc > 15)
      enc->acceleration = delta / 4;
    else if (enc->cptacc > 10)
      enc->acceleration = delta / 2;
    else
      enc->acceleration = delta;
  }
  else
    enc->acceleration = 0;

  if (signe == 1)
    enc->acceleration = 0 - enc->acceleration;

  if (val < 0)
  {
    enc->val += tmp * val + (enc->acceleration) * signe;
  }
  else if (val > 0)
  {
    enc->val += tmp * val - (enc->acceleration) * signe;
  }

  if (val != 0)
  {
    enc->cptacc = 0;

    //---------------------------------------
    // Min Max Value
    //---------------------------------------
    if (enc->val < enc->data[param].MinData)
    {
      enc->val = enc->data[param].MinData;
      enc->delta = enc->data[param].MinData * enc->steps;
      enc->delta = 0;
    }
    if (enc->val > enc->data[param].MaxData)
    {
      enc->val = enc->data[param].MaxData;
      enc->delta = enc->data[param].MaxData * enc->steps;
      enc->delta = 0;
    }
    if (enc->oldval != enc->val)
    {
      enc->oldval = enc->val;
      *enc->data[param].Data = (int)enc->val;
      enc->data[param].ptrfunctValueChange(enc->id);
    }
  }
  portEXIT_CRITICAL_ISR(&timerMux_44100Hertz);
}
