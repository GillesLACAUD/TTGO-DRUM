/*
 * a simple implementation to use midi
 *
 * Author: Marcel Licence
 */

#include <Arduino.h>

#define __MIDI__
#include "midi_interface.h"

#include <EEPROM.h>
#include "global.h"
#include "ihm.h"
#include "Pattern_and_samples.h"
#include "simple_delay.h"

extern hw_timer_t * timer_44100Hertz;   // Timer for Audio DAC 8 bits

/* use define to dump midi data */
//#define DUMP_SERIAL2_TO_SERIAL

/* constant to normalize midi value to 0.0 - 1.0f */
#define NORM127MUL	0.007874f

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
inline void Midi_NoteOn(uint8_t note,uint8_t vel)
{
  if(vel !=0)
  {
    switch(note)
    {
      case 60:
      samplecnt[INST_QU]=Tab_SampleSize[INST_QU];
      samplepnt[INST_QU]=0; if(gui16_Sample_Reverse & 1)samplepnt[INST_QU]=samplecnt[INST_QU];
      gui8_Volume[7] = vel;
      break;

      case 61:
      samplecnt[INST_CY]=Tab_SampleSize[INST_CY];
      samplepnt[INST_CY]=0; if(gui16_Sample_Reverse & 2)samplepnt[INST_CY]=samplecnt[INST_CY];
      gui8_Volume[6] = vel;
      break;

      case 62:
      samplecnt[INST_MA]=Tab_SampleSize[INST_MA];
      samplepnt[INST_MA]=0; if(gui16_Sample_Reverse & 4)samplepnt[INST_MA]=samplecnt[INST_MA];
      gui8_Volume[5] = vel;
      break;

      case 63:
      samplecnt[INST_CW]=Tab_SampleSize[INST_CW];
      samplepnt[INST_CW]=0; if(gui16_Sample_Reverse & 8)samplepnt[INST_CW]=samplecnt[INST_CW];
      gui8_Volume[4] = vel;
      break;

      case 64:
      samplecnt[INST_CL]=Tab_SampleSize[INST_CL];
      samplepnt[INST_CL]=0; if(gui16_Sample_Reverse & 16)samplepnt[INST_CL]=samplecnt[INST_CL];
      gui8_Volume[3] = vel;
      break;

      case 65:
      samplecnt[INST_BD]=Tab_SampleSize[INST_BD];
      samplepnt[INST_BD]=0; if(gui16_Sample_Reverse & 32)samplepnt[INST_BD]=samplecnt[INST_BD];
      gui8_Volume[2] = vel;
      break;

      case 66:
      samplecnt[INST_BG2]=Tab_SampleSize[INST_BG2];
      samplepnt[INST_BG2]=0; if(gui16_Sample_Reverse & 64)samplepnt[INST_BG2]=samplecnt[INST_BG2];
      gui8_Volume[1] = vel;
      break;

      case 67:
      samplecnt[INST_GU]=Tab_SampleSize[INST_GU];
      samplepnt[INST_GU]=0; if(gui16_Sample_Reverse & 128)samplepnt[INST_GU]=samplecnt[INST_GU];
      gui8_Volume[0] = vel;
      break;
    }
  }
  
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
inline void Midi_NoteOff(uint8_t note,uint8_t vel)
{

}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
inline void Midi_ProgramChange(uint8_t channel, uint8_t data1)
{
char mtext[30]; 

  
  if(data1 < 32)
  {
    gui8_PatSelect = data1;
    if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PATTERN)
    {
      sprintf(mtext, "%s", NamePattern[data1]);
      Ihm_Set_Label(0, mtext);
    }
    gui8_patlength = patlen[data1];
    if (gui8_stepcnt > gui8_patlength)
      gui8_stepcnt = 0;
    EEPROM.write(EEP_OFF_LASTP, gui8_PatSelect);
    gui32_EEPCommit = 0;
    gui8_EEPAskCommit = 1;  
  }
    
}


/*
 * this function will be called when a control change message has been received
 */
 /****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
inline void Midi_ControlChange(uint8_t channel, uint8_t data1, uint8_t data2)
{
uint8_t ch;
char mtext[30];
uint32_t temp;

    switch(data1)
    {
        case MIDI_CC_TEMPO:
        gui8_Afftempo = (uint8_t)(((uint16_t)data2*240)/127);
        temp = 60 * 2000;
        temp /= (uint32_t)gui8_Afftempo;
        gui32_Maxcpttempo = (uint32_t)temp;
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PATTERN)
        {
          sprintf(mtext, "TEMPO %03d", gui8_Afftempo);
          Ihm_Set_Label(1, mtext);        
        }
        EEPROM.write(EEP_OFF_LASTT, gui8_Afftempo);
        gui32_EEPCommit = 0;
        gui8_EEPAskCommit = 1;
        break;
      
        case MIDI_CC_VOL1:
        case MIDI_CC_VOL2:
        case MIDI_CC_VOL3:
        case MIDI_CC_VOL4:
        case MIDI_CC_VOL5:
        case MIDI_CC_VOL6:
        case MIDI_CC_VOL7:
        case MIDI_CC_VOL8:
        ch =  data1 - MIDI_CC_VOL1;
        gui8_Volume[ch] = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_VOLUME + ch, gui8_Volume[ch]);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_VOLUME)
        {
          Ihm_Tab_Pot[ch].ptui8_val = &gui8_Volume[ch];
          Ihm_Draw_Pot(&Ihm_Tab_Pot[ch]);

          sprintf(mtext, "VOL   %s   %03d   ", NameInstrument[ch],gui8_Volume[ch]);
          Ihm_Set_Label(2, mtext);
        }
        break;
        
        // Pan for each instrument
        case MIDI_CC_PAN1:
        case MIDI_CC_PAN2:
        case MIDI_CC_PAN3:
        case MIDI_CC_PAN4:
        case MIDI_CC_PAN5:
        case MIDI_CC_PAN6:
        case MIDI_CC_PAN7:
        case MIDI_CC_PAN8:
        ch =  data1 - MIDI_CC_PAN1;
        gui8_Pan[ch] = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_PAN + ch, gui8_Pan[ch]);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PAN)
        {
          Ihm_Tab_Pot[ch].ptui8_val = &gui8_Pan[ch];
          Ihm_Draw_Pot(&Ihm_Tab_Pot[ch]);
          sprintf(mtext, "PAN   %s   %03d   ", NameInstrument[ch], gui8_Pan[ch]);
          Ihm_Set_Label(2, mtext);
        }
        break;
        
        // Size for each instrument
        case MIDI_CC_SIZE1:
        case MIDI_CC_SIZE2:
        case MIDI_CC_SIZE3:
        case MIDI_CC_SIZE4:
        case MIDI_CC_SIZE5:
        case MIDI_CC_SIZE6:
        case MIDI_CC_SIZE7:
        case MIDI_CC_SIZE8:
        ch =  data1 - MIDI_CC_SIZE1;
        gui8_Decay[ch] = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_DECAY + ch, gui8_Decay[ch]);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_DECAY)
        {
          Ihm_Tab_Pot[ch].ptui8_val = &gui8_Decay[ch];
          Ihm_Draw_Pot(&Ihm_Tab_Pot[ch]);
          sprintf(mtext, "DECAY  %s   %03d   ", NameInstrument[ch], gui8_Decay[ch]);
          Ihm_Set_Label(2, mtext);
        }
        break;
        
        // Reverse for each instrument
        case MIDI_CC_REV1:
        case MIDI_CC_REV2:
        case MIDI_CC_REV3:
        case MIDI_CC_REV4:
        case MIDI_CC_REV5:
        case MIDI_CC_REV6:
        case MIDI_CC_REV7:
        case MIDI_CC_REV8:
        ch =  data1 - MIDI_CC_REV1;
        if(data2 > 64)
          gui8_Reverse[ch] = 1;
        else
          gui8_Reverse[ch] = 0;
        EEPROM.write(EEP_OFF_REVERSE + ch, gui8_Reverse[ch]);          
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_REVERSE)
        {          
          sprintf(mtext, "REV INST  %s  ", NameInstrument[ch]);
          Ihm_Set_Label(2,mtext);
          if (gui8_Reverse[ch])
          {
            Ihm_Draw_PadOn(ch);
            gui16_Sample_Reverse |= 0x80 >> ch;
          }
          else
          {
            Ihm_Draw_PadOff(ch);
            gui16_Sample_Reverse &= ~(0x80 >> ch);
          }
        }
        break;
        
        // Delay parameters
        case MIDI_CC_DELAY_IIME:
        Delay_Time = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_DELAY + 0, Delay_Time);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_DELAY)
        {
          sprintf(mtext, "DELAY TIME      %03d", Delay_Time);
          Ihm_Set_Label(0, mtext);
        }
        break;
        case MIDI_CC_DELAY_VOL:
        Delay_Amp = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_DELAY + 1, Delay_Amp);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_DELAY)
        {
          sprintf(mtext, "DELAY AMP       %03d", Delay_Amp);
          Ihm_Set_Label(1, mtext);
        }
        break;
        case MIDI_CC_DELAY_FEED:
        Delay_Repeat = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_DELAY + 2, Delay_Repeat);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_DELAY)
        {
          sprintf(mtext, "DELAY FEED      %03d", Delay_Repeat);
          Ihm_Set_Label(2, mtext);
        }
        break;
        
        case MIDI_CC_GVOL:
        gui16_GVolume = (uint8_t)(((uint16_t)data2*100)/127);
        EEPROM.write(EEP_OFF_LASTV, gui16_GVolume);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PATTERN)
        {
          sprintf(mtext, "VOL %03d   STEP %02d", gui16_GVolume, gui8_stepcnt + 1);
          Ihm_Set_Label(2, mtext);
        }
        break;
        case MIDI_CC_PITCH:
        gui16_GPitch = 10 + (uint8_t)(((uint16_t)data2*240)/127);
        timerAlarmWrite(timer_44100Hertz, gui16_GPitch, true);
        EEPROM.write(EEP_OFF_LASTPI,gui16_GPitch);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_MISC)
        {
          sprintf(mtext, "PITCH     %03d", gui16_GPitch);
          Ihm_Set_Label(0, mtext);  
        }
        break;
        case MIDI_CC_COLOR:
        gui8_GuiColor = data2/12;
        EEPROM.write(EEP_OFF_GUICOLOR,gui8_GuiColor);
        if(gui8_Ihm_CurrentScreen==IHM_SCREEN_MISC)
        {
          sprintf(mtext, "GUI COLOR %02d",gui8_GuiColor);
          Ihm_Set_Label(2, mtext);
        }
        break;
    }
    gui32_EEPCommit = 0;
    gui8_EEPAskCommit = 1;
}


// Sysex Midi clock and Real time
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
inline void HandleRealTimeMsg(uint8_t realtime)
{
uint8_t incomingByte;

  switch(realtime)
  {
    case MIDI_CLOCK:
    /*
    if(!gu8_MidiClock)
    {
      gu8_MidiClock=1;
      gu8_MidiCptClock=0;
    }
    gu16_TimerMidiClock=0;
    */
    break;
    
    case MIDI_SYSTEM_EXCLUSIVE:
    incomingByte=0x00;
    #ifdef DUMP_SERIAL2_TO_SERIAL
    Serial.printf("SYSEX 0xF0");
    #endif
    while(incomingByte !=0xF7)      // Wait end sysex
    {
      if (Serial2.available())
      {
        incomingByte = Serial2.read();
        #ifdef DUMP_SERIAL2_TO_SERIAL
        Serial.printf("%02x ",incomingByte);
        #endif
      }
    }
    #ifdef DUMP_SERIAL2_TO_SERIAL
    Serial.printf("SYSEX 0xF7\n");
    #endif
    break;
    
    case MIDI_START:
    gui8_PB_startstop = 1;
    //Ihm_Led_StartStop.ui8_state = LED_STATE_ON;
    //if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PATTERN)
    //  Ihm_Set_Led(&Ihm_Led_StartStop);
    break;
    
    case MIDI_STOP:
    gui8_PB_startstop = 0;
    gui8_stepcnt = 0;
    gui32_cptit40K = 0;
    Ihm_Led_StartStop.ui8_state = LED_STATE_OFF;
    //if(gui8_Ihm_CurrentScreen==IHM_SCREEN_PATTERN)
    //  Ihm_Set_Led(&Ihm_Led_StartStop);
    //Delay_Reset();
    break;
    
  }
}


/*
 * function will be called when a short message has been received over midi
 */
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
inline void HandleShortMsg(uint8_t *data)
{
    uint8_t ch = data[0] & 0x0F;
    
    //Serial.printf("MIDI CH %d MidiRX %d\n",ch,gui8_MidiRX-1);
    if(ch != (gui8_MidiRX-1))
      return;
    
    switch (data[0] & 0xF0)
    {
    /* note on */
    case 0x90:
        Midi_NoteOn(data[1],data[2]);
        #ifdef DUMP_SERIAL2_TO_SERIAL
        Serial.printf("NOTE ON\n");
        #endif
        break;
    /* note off */
    case 0x80:
        Midi_NoteOff(data[1],data[2]);
        #ifdef DUMP_SERIAL2_TO_SERIAL
        Serial.printf("NOTE OFF\n");
        #endif
        break;
    case 0xb0:
        Midi_ControlChange(ch, data[1], data[2]);
        #ifdef DUMP_SERIAL2_TO_SERIAL
        Serial.printf("CC MIDI\n");
        #endif
        break;

    case MIDI_PROGRAM_CHANGE:
        Midi_ProgramChange(ch, data[1]);
        #ifdef DUMP_SERIAL2_TO_SERIAL
        Serial.printf("PC MIDI\n");
        #endif
        break;
        
    }
}

/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void Midi_Setup()
{
    Serial2.begin(31250, SERIAL_8N1, RXD2, TXD2);
    pinMode(RXD2, INPUT_PULLUP);  /* 25: GPIO 16, u2_RXD */
}
/*
 * this function should be called continuously to ensure that incoming messages can be processed
 */
/****************************************************/
/*                                                  */
/*                                                  */
/*                                                  */
/*                                                  */
/****************************************************/
void Midi_Process()
{
    /*
     * watchdog to avoid getting stuck by receiving incomplete or wrong data
     */
    static uint32_t inMsgWd = 0;
    static uint8_t inMsg[3];
    static uint8_t inMsgIndex = 0;

    //Choose Serial1 or Serial2 as required

    if (Serial2.available())
    {
        uint8_t incomingByte = Serial2.read();

        //Serial2.write(incomingByte);    // No Midi Out

        #ifdef DUMP_SERIAL2_TO_SERIAL
        Serial.printf("%02x\n", incomingByte);
        #endif
        
        /* System or real time messages */
        if ((incomingByte >= 0xF0))
        {
            HandleRealTimeMsg(incomingByte);
            return;
        }
        
        if (inMsgIndex == 0)
        {
            if ((incomingByte & 0x80) != 0x80)
            {
                inMsgIndex = 1;
            }
        }
        inMsg[inMsgIndex] = incomingByte;
        inMsgIndex += 1;

        if(inMsg[0] == MIDI_PROGRAM_CHANGE && inMsgIndex==2)
        {
          #ifdef DUMP_SERIAL2_TO_SERIAL
          Serial.printf("PROGRAM CHANGE %d\n",inMsg[1]);
          #endif          
          HandleShortMsg(inMsg);
          inMsgIndex = 0;
        }

        if (inMsgIndex >= 3)
        {
            #ifdef DUMP_SERIAL2_TO_SERIAL
            Serial.printf(">%02x %02x %02x\n", inMsg[0], inMsg[1], inMsg[2]);
            #endif
            HandleShortMsg(inMsg);
            inMsgIndex = 0;
        }
        //reset watchdog to allow new bytes to be received
        inMsgWd = 0;
    }
    else
    {
        if (inMsgIndex > 0)
        {
            inMsgWd++;
            if (inMsgWd == 0xFFF)
            {
                inMsgIndex = 0;
            }
        }
    }

}
