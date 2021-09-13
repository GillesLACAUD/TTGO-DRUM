#define __FCTENCODER__
#include "FctEncoder.h"
#include "HardwareSerial.h"
#include "global.h"
#include "ihm.h"
#include "encoder.h"
#include "simple_delay.h"

#include <EEPROM.h>

uint8_t debugfctencoder = 0;
extern TFT_eSPI tft;

char mess[30];

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_PBChangeScreen(int encid)
{
  char mlbl[30];
  Ihm_Pot* ptpot;

  if (encid != 0xFF)
    gui8_Ihm_CurrentScreen++;

  if (gui8_Ihm_CurrentScreen >= IHM_MAX_SCREEN)
    gui8_Ihm_CurrentScreen = 0;

  tft.fillScreen(IHM_BACKGROUND_COLOR);
  switch (gui8_Ihm_CurrentScreen)
  {
    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_PATTERN:
      Enc_A.val = gui8_PatSelect;
      Enc_A.oldval = Enc_A.val;
      Enc_B.val = gui8_Afftempo;
      Enc_B.oldval = Enc_B.val;
      Enc_C.val = gui16_GVolume;
      Enc_C.oldval = Enc_C.val;
      sprintf(mess, "%s", NamePattern[gui8_PatSelect]);
      Ihm_Set_Label(0, mess);
      sprintf(mess, "TEMPO %03d", gui8_Afftempo);
      Ihm_Set_Label(1, mess);
      sprintf(mess, "STEP %02d", gui8_stepcnt);
      Ihm_Set_Label(2, mess);
      Ihm_Set_Led(&Ihm_Led_StartStop);
      break;

    case IHM_SCREEN_DELAY:
      Enc_A.val = Delay_Time;
      Enc_A.oldval = Enc_A.val;
      Enc_B.val = Delay_Amp;
      Enc_B.oldval = Enc_B.val;
      Enc_C.val = Delay_Repeat;
      Enc_C.oldval = Enc_C.val;
      sprintf(mess, "DELAY TIME      %03d", Delay_Time);
      Ihm_Set_Label(0, mess);
      sprintf(mess, "DELAY AMP       %03d", Delay_Amp);
      Ihm_Set_Label(1, mess);
      sprintf(mess, "DELAY FEED      %03d", Delay_Repeat);
      Ihm_Set_Label(2, mess);
      break;

    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_VOLUME:
      // Restore the volume values
      for (uint8_t i = 0; i < 8; i++)
      {
        Ihm_Tab_Pot[i].ptui8_val = &gui8_Volume[i];
        Ihm_Draw_Pot(&Ihm_Tab_Pot[i]);

        sprintf(messdebug, "POT %d Vol %03d Pan %03d", i, gui8_Volume[i], gui8_Pan[i]);
        Serial.println(messdebug);
      }
      Ihm_PotVal = gui8_Volume[Ihm_PotSelect];
      Ihm_Select_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
      Enc_A.val = Ihm_PotSelect;
      Enc_A.oldval = Enc_A.val;
      Enc_B.val = gui8_Volume[Ihm_PotSelect];
      Enc_B.oldval = Enc_B.val;
      sprintf(mlbl, "VOL   %s   %03d   ", NameInstrument[Ihm_PotSelect], gui8_Volume[Ihm_PotSelect]);
      Ihm_Set_Label(2, mlbl);
      break;

    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_PAN:
      // Restore the pan values
      for (uint8_t i = 0; i < 8; i++)
      {
        Ihm_Tab_Pot[i].ptui8_val = &gui8_Pan[i];
        Ihm_Draw_Pot(&Ihm_Tab_Pot[i]);

        sprintf(messdebug, "POT %d Vol %03d Pan %d", i, gui8_Volume[i], gui8_Pan[i]);
        Serial.println(messdebug);
      }
      Ihm_PotVal = gui8_Pan[Ihm_PotSelect];
      Ihm_Select_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
      Enc_A.val = Ihm_PotSelect;
      Enc_A.oldval = Enc_A.val;
      Enc_B.val = gui8_Pan[Ihm_PotSelect];
      Enc_B.oldval = Enc_B.val;
      sprintf(mlbl, "PAN   %s   %03d   ", NameInstrument[Ihm_PotSelect], gui8_Pan[Ihm_PotSelect]);
      Ihm_Set_Label(2, mlbl);
      break;

    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_DECAY:
      // Restore the decay values
      for (uint8_t i = 0; i < 8; i++)
      {
        Ihm_Tab_Pot[i].ptui8_val = &gui8_Decay[i];
        Ihm_Draw_Pot(&Ihm_Tab_Pot[i]);
      }
      Ihm_PotVal = gui8_Decay[Ihm_PotSelect];
      Ihm_Select_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
      Enc_A.val = Ihm_PotSelect;
      Enc_A.oldval = Enc_A.val;
      Enc_B.val = gui8_Decay[Ihm_PotSelect];
      Enc_B.oldval = Enc_B.val;
      sprintf(mlbl, "DECAY  %s   %03d   ", NameInstrument[Ihm_PotSelect], gui8_Decay[Ihm_PotSelect]);
      Ihm_Set_Label(2, mlbl);
      break;

    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_REVERSE:
      Enc_A.val = gi16_SelReverse;
      Enc_A.oldval = Enc_A.val;
      sprintf(mess, "REV INST  %s  ", NameInstrument[gi16_SelReverse]);
      Ihm_Set_Label(2, mess);
      //Enc_B.val = gui16_SelPad;
      //Enc_B.oldval = Enc_B.val;
      Ihm_Draw_Reverse();
      oldsel = gi16_SelReverse;
      Ihm_Draw_PadSelect(gi16_SelReverse);

      for (uint8_t i = 0; i < 8; i++)
      {
        if (gui8_Reverse[i])
          Ihm_Draw_PadOn(i);
        else
          Ihm_Draw_PadOff(i);
      }
      break;

    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_PAD:
      Enc_A.val = gi16_SelInst;
      Enc_A.oldval = Enc_A.val;
      sprintf(mess, "INST  %s  ", NameInstrument[gi16_SelInst]);
      Ihm_Set_Label(2, mess);
      Enc_B.val = gui16_SelPad;
      Enc_B.oldval = Enc_B.val;
      Ihm_Draw_Pad();
      oldsel = gui16_SelPad;
      Fct_ChangeSelInst(gi16_SelInst);
      Ihm_Draw_PadSelect(gui16_SelPad);
      break;
      
    //------------------------------------------------
    //
    //------------------------------------------------
    case IHM_SCREEN_MISC:
      Enc_A.val = gui16_GPitch;
      Enc_A.oldval = Enc_A.val;
      sprintf(mess, "PITCH     %03d", gui16_GPitch);
      Ihm_Set_Label(0, mess);
      Enc_B.val = gui8_MidiRX;
      Enc_B.oldval = Enc_B.val;
      Enc_C.val = gui8_GuiColor;
      Enc_C.oldval = Enc_C.val;
      sprintf(mess, "RX CH     %03d",gui8_MidiRX);
      Ihm_Set_Label(1, mess);
      sprintf(mess, "GUI COLOR %02d",gui8_GuiColor);
      Ihm_Set_Label(2, mess);

      
      break;


  }

  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_PBChangeScreen %d", gui8_Ihm_CurrentScreen);
    Serial.println(messdebug);
  }
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeMidiRx(int)
{
  sprintf(mess, "RX CH     %03d",gui8_MidiRX);
  Ihm_Set_Label(1, mess);
  EEPROM.write(EEP_OFF_MIDIRX,gui8_MidiRX);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeGuiColor(int)
{
  sprintf(mess, "GUI COLOR %02d",gui8_GuiColor);
  Ihm_Set_Label(2, mess);
  EEPROM.write(EEP_OFF_GUICOLOR,gui8_GuiColor);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;

  switch(gui8_GuiColor)
  {
    case 0: gcolor=TFT_GREEN;break;
    case 1: gcolor=TFT_ORANGE;break;
    case 2: gcolor=TFT_PINK;break;
    case 3: gcolor=TFT_GOLD;break;
    case 4: gcolor=TFT_SKYBLUE;break;
  }
  tft.setTextColor(gcolor, IHM_BACKGROUND_COLOR);
  Fct_PBChangeScreen(0xFF);
  
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeSelVol(int encid)
{
  char mvol[30];

  Ihm_Select_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
  Ihm_PotVal = *Ihm_Tab_Pot[Ihm_PotSelect].ptui8_val;
  sprintf(mvol, "VOL   %s   %03d   ", NameInstrument[Ihm_PotSelect], Ihm_PotVal);
  Ihm_Set_Label(2, mvol);
  Enc_B.val = gui8_Volume[Ihm_PotSelect];
  Enc_B.oldval = Enc_B.val;
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeSelPot %d val %d", Ihm_PotSelect, Ihm_PotVal);
    Serial.println(messdebug);
  }

}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeValVol(int encid)
{
  char mvol[30];

  *Ihm_Tab_Pot[Ihm_PotSelect].ptui8_val = Ihm_PotVal;
  Ihm_Draw_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
  gui8_Volume[Ihm_PotSelect] = Ihm_PotVal;
  sprintf(mvol, "VOL   %s   %03d   ", NameInstrument[Ihm_PotSelect], gui8_Volume[Ihm_PotSelect]);
  Ihm_Set_Label(2, mvol);
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeValPot %d val %d", Ihm_PotSelect, gui8_Volume[Ihm_PotSelect]);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_VOLUME + Ihm_PotSelect, gui8_Volume[Ihm_PotSelect]);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}



/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeSelPan(int encid)
{
  char mvol[30];

  Ihm_Select_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
  Ihm_PotVal = *Ihm_Tab_Pot[Ihm_PotSelect].ptui8_val;
  sprintf(mvol, "PAN   %s   %03d   ", NameInstrument[Ihm_PotSelect], Ihm_PotVal);
  Ihm_Set_Label(2, mvol);
  Enc_B.val = gui8_Pan[Ihm_PotSelect];
  Enc_B.oldval = Enc_B.val;
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeSelPan %d val %d", Ihm_PotSelect, Ihm_PotVal);
    Serial.println(messdebug);
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeValPan(int encid)
{
  char mvol[30];

  *Ihm_Tab_Pot[Ihm_PotSelect].ptui8_val = Ihm_PotVal;
  Ihm_Draw_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
  gui8_Pan[Ihm_PotSelect] = Ihm_PotVal;
  sprintf(mvol, "PAN   %s   %03d   ", NameInstrument[Ihm_PotSelect], gui8_Pan[Ihm_PotSelect]);
  Ihm_Set_Label(2, mvol);
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeValPan %d val %d", Ihm_PotSelect, gui8_Pan[Ihm_PotSelect]);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_PAN + Ihm_PotSelect, gui8_Pan[Ihm_PotSelect]);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_PBSelPotAll(int encid)
{
  Ihm_ui8_PotSelectAll = !Ihm_ui8_PotSelectAll;
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_PBSelPotAll %d", Ihm_ui8_PotSelectAll);
    Serial.println(messdebug);
  }
}



/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangePattern(int encid)
{
  sprintf(mess, "%s", NamePattern[gui8_PatSelect]);
  Ihm_Set_Label(0, mess);
  gui8_patlength = patlen[gui8_PatSelect];
  if (gui8_stepcnt > gui8_patlength)
    gui8_stepcnt = 0;
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangePattern %d", gui8_PatSelect);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_LASTP, gui8_PatSelect);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;

  /*
    for (uint8_t i = 0; i < 8; i++)
    {
    gui8_Volume[i]  = EEPROM.read(EEP_OFF_VOLUME + 8 * gui8_PatSelect + i);
    gui8_Pan[i]     = EEPROM.read(EEP_OFF_PAN + 8 * gui8_PatSelect + i);
    }
    Delay_Time   = EEPROM.read(EEP_OFF_DELAY + 8 * gui8_PatSelect + 0);
    Delay_Amp    = EEPROM.read(EEP_OFF_DELAY + 8 * gui8_PatSelect + 1);
    Delay_Repeat = EEPROM.read(EEP_OFF_DELAY + 8 * gui8_PatSelect + 2);

    sprintf(messdebug, "Read Delay_Time %d",Delay_Time);
    Serial.println(messdebug);
  */

}

extern hw_timer_t * timer_44100Hertz;   // Timer for Audio DAC 8 bits
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangePitch(int encid)
{
  timerAlarmWrite(timer_44100Hertz, gui16_GPitch, true);
  sprintf(mess, "PITCH     %03d", gui16_GPitch);
  EEPROM.write(EEP_OFF_LASTPI,gui16_GPitch);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
  Ihm_Set_Label(0, mess);
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeVolume(int encid)
{
  sprintf(messdebug, "VOL %03d   STEP %02d", gui16_GVolume, gui8_stepcnt + 1);
  Ihm_Set_Label(2, messdebug);


  EEPROM.write(EEP_OFF_LASTV, gui16_GVolume);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_PBStartStop(int encid)
{
  if (!gui8_PB_startstop)
  {
    gui8_PB_startstop = 1;
    Ihm_Led_StartStop.ui8_state = LED_STATE_ON;
  }
  else
  {
    gui8_PB_startstop = 0;
    gui8_stepcnt = 0;
    gui32_cptit40K = 0;
    Ihm_Led_StartStop.ui8_state = LED_STATE_OFF;
    //Delay_Reset();
  }
  if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PATTERN)
    Ihm_Set_Led(&Ihm_Led_StartStop);
    
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_PBStartStop");
    Serial.println(messdebug);
  }
}
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeTempo(int)
{
  uint32_t temp;
  // BPM Battement per minute
  // 120 BPM = 120 Battement per minute
  // 120 BPM = 120/60 Battement per seconde = 2 = 2 temps => 500ms
  // for a better precision we choice the 44100 cpt
  // so 120 BPM -> 22050 cpt
  // cpt = (60/120)*44100 = 22050
  // cpt = (60/119)*44100 = 22235
  // cpt = (60/118)*44100 = 22423

  temp = 60 * 2000;
  temp /= (uint32_t)gui8_Afftempo;
  gui32_Maxcpttempo = (uint32_t)temp;

  sprintf(mess, "TEMPO %03d", gui8_Afftempo);
  Ihm_Set_Label(1, mess);
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeTempo %d", gui8_Afftempo);
    Serial.println(messdebug);
    sprintf(messdebug, "Fct_ChangeTempo MaxCpt %d", gui32_Maxcpttempo);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_LASTT, gui8_Afftempo);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_PBTestSound(int)
{
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_PBTestSound");
    Serial.println(messdebug);
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_DelayTime(int)
{
float tmp;
  sprintf(mess, "DELAY TIME      %03d", Delay_Time);
  tmp =(float)Delay_Time/100.0;
  Delay_SetLength(tmp);   // value between 0 to 1
  Ihm_Set_Label(0, mess);
   if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_DelayTime %d float %f", Delay_Time,tmp);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_DELAY + 0, Delay_Time);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
  

}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_DelayAmp(int)
{
  sprintf(mess, "DELAY AMP       %03d", Delay_Amp);
  Ihm_Set_Label(1, mess);
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_DelayAmp %d", Delay_Amp);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_DELAY + 1, Delay_Amp);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_DelayRepeat(int)
{
  sprintf(mess, "DELAY FEED      %03d", Delay_Repeat);
  Ihm_Set_Label(2, mess);
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_DelayRepeat %d", Delay_Repeat);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_DELAY + 2, Delay_Repeat);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeSelPad(int)
{
  Ihm_Draw_PadSelect(gui16_SelPad);
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_PBPadOnOff(int)
{
  uint8_t  mask;
  mask = 0x80 >> gi16_SelInst;

  if (pattern[gui8_PatSelect * 16 + gui16_SelPad] & mask)
  {
    pattern[gui8_PatSelect * 16 + gui16_SelPad] &= ~(0x80 >> gi16_SelInst);
  }
  else
  {
    pattern[gui8_PatSelect * 16 + gui16_SelPad] |= 0x80 >> gi16_SelInst;
  }
  Fct_ChangeSelInst(gi16_SelInst);

  // Save the new value in the eeprom
  if (gui8_PatSelect >= 16)
  {
    EEPROM.write(EEP_OFF_PATTERN + (gui8_PatSelect - 16) * 16 + gui16_SelPad, pattern[gui8_PatSelect * 16 + gui16_SelPad]);
    gui32_EEPCommit = 0;
    gui8_EEPAskCommit = 1;

    /*
      Serial.print("Eeprom write: ");
      Serial.print((gui8_PatSelect-16)*16+gui16_SelPad, DEC);
      Serial.print(" ");
      Serial.println(pattern[gui8_PatSelect*16+gui16_SelPad], HEX);
    */
  }


}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeSelInst(int)
{
  uint8_t  mask;

  sprintf(mess, "INST  %s  ", NameInstrument[gi16_SelInst]);
  Ihm_Set_Label(2, mess);
  // Draw the actif step
  mask = 0x80 >> gi16_SelInst;
  for (uint8_t s = 0; s < 16; s++)
  {
    if (pattern[gui8_PatSelect * 16 + s] & mask)
    {
      Ihm_Draw_PadOn(s);
    }
    else
    {
      Ihm_Draw_PadOff(s);
    }
  }
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeSelDecay(int)
{
  char mvol[30];

  Ihm_Select_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
  Ihm_PotVal = *Ihm_Tab_Pot[Ihm_PotSelect].ptui8_val;
  sprintf(mvol, "DECAY   %s   %03d   ", NameInstrument[Ihm_PotSelect], Ihm_PotVal);
  Ihm_Set_Label(2, mvol);
  Enc_B.val = gui8_Pan[Ihm_PotSelect];
  Enc_B.oldval = Enc_B.val;
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeSelDecay %d val %d", Ihm_PotSelect, Ihm_PotVal);
    Serial.println(messdebug);
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeValDecay(int)
{
  char mvol[30];

  *Ihm_Tab_Pot[Ihm_PotSelect].ptui8_val = Ihm_PotVal;
  Ihm_Draw_Pot(&Ihm_Tab_Pot[Ihm_PotSelect]);
  gui8_Decay[Ihm_PotSelect] = Ihm_PotVal;
  sprintf(mvol, "DECAY   %s   %03d   ", NameInstrument[Ihm_PotSelect], gui8_Decay[Ihm_PotSelect]);
  Ihm_Set_Label(2, mvol);
  if (debugfctencoder)
  {
    sprintf(messdebug, "Fct_ChangeValDecay %d val %d", Ihm_PotSelect, gui8_Volume[Ihm_PotSelect]);
    Serial.println(messdebug);
  }
  EEPROM.write(EEP_OFF_DECAY + Ihm_PotSelect, gui8_Decay[Ihm_PotSelect]);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;
}


/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_ChangeSelReverse(int)
{
  uint8_t  mask;

  sprintf(mess, "REV INST  %s  ", NameInstrument[gi16_SelReverse]);
  Ihm_Set_Label(2, mess);
  Ihm_Draw_PadSelect(gi16_SelReverse);
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
int Fct_PBReverse(int)
{
  gui8_Reverse[gi16_SelReverse] = !gui8_Reverse[gi16_SelReverse];
  if (gui8_Reverse[gi16_SelReverse])
  {
    Ihm_Draw_PadOn(gi16_SelReverse);
    gui16_Sample_Reverse |= 0x80 >> gi16_SelReverse;
  }
  else
  {
    Ihm_Draw_PadOff(gi16_SelReverse);
    gui16_Sample_Reverse &= ~(0x80 >> gi16_SelReverse);
  }
  EEPROM.write(EEP_OFF_REVERSE + gi16_SelReverse, gui8_Reverse[gi16_SelReverse]);
  //Serial.printf("I %d Rev %d",gi16_SelInst,gui8_Reverse[gi16_SelReverse]);
  gui32_EEPCommit = 0;
  gui8_EEPAskCommit = 1;

}
