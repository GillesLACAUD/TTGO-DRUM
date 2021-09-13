/*
  Test the tft.print() viz embedded tft.write() function

  This sketch used font 2, 4, 7

  Make sure all the display driver and pin comnenctions are correct by
  editting the User_Setup.h file in the TFT_eSPI library folder.

  Note that yield() or delay(0) must be called in long duration for/while
  loops to stop the ESP8266 watchdog triggering.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################

  https://github.com/hexagon5un/jan_ostmans_synths

*/

//------------------------------------------------------------------------
// VALID __HARDWARE_TEST__  for a Hardware test -> Encoder + SYNC + MIDI
// VALID __IHM_TEST__       for an IHM demo
// VALID __BLUETOOTH__      to enable Bluetooth
//------------------------------------------------------------------------



//16 bits PWM
//https://www.reddit.com/r/esp32/comments/ee3rtm/esp32_16bit_pwm/


#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <EEPROM.h>
#include "simple_delay.h"
#include "midi_interface.h"


#include "WiFi.h"

const char* ssid = "TTGO DRUM";
const char* password = "";

#ifdef __BLUETOOTH__
#include "BluetoothSerial.h"
#endif
#include "HardwareSerial.h"
#include "IHM.h"
#include "Encoder.h"

#define __GLOBAL__
#include "Global.h"

//#define __HARDWARE_TEST__
//#define __IHM_TEST__
//#define __BLUETOOTH__     //WARNING TAKE 50% OF THE PROGRAM MEMORY


#define __PATTERN__
#include "Pattern_and_samples.h"



#ifdef __BLUETOOTH__
BluetoothSerial SerialBT;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#endif


TFT_eSPI tft = TFT_eSPI();                // Invoke library, pins defined in User_Setup.h

#define TFT_GREY 0x5AEB                   // New colour for test purpose
#define TFT_DEFAULT_COLOR TFT_GREEN

// RECTANGLE FOR TEST
#define RECT_Sx 1
#define RECT_Sy 1
#define RECT_W  239
#define RECT_H  40
#define RECT_OFF 45

char  Tab_Aff[3][20];

hw_timer_t * timer_xms = NULL;          // Timer for IHM Update and encoders get status
hw_timer_t * timer_44100Hertz = NULL;   // Timer for Audio DAC 8 bits

portMUX_TYPE timerMux_xms = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux_44100Hertz = portMUX_INITIALIZER_UNLOCKED;
volatile int interruptCounter;



void IRAM_ATTR SendSound()
{
  uint16_t   valleft = 0, valright = 0;
  
      
  //-------------------  Ringbuffer handler -------------------------
  //If entry in FIFO..
  if (gui16_RingCount)
  {

    valleft = gui8_RingbufferLeft[gui16_RingRead];
    valright = gui8_RingbufferRight[gui16_RingRead];
    if (1)
    {
      delayLine_l[delayIn] = valleft;
      delayLine_r[delayIn] = valright;

      delayOut = delayIn + (1 + MAX_DELAY - delayLen);
      if (delayOut >= MAX_DELAY)
        delayOut -= MAX_DELAY;

      valleft   += (delayLine_l[delayOut] * Delay_Amp) / 100;
      valright  += (delayLine_r[delayOut] * Delay_Amp) / 100;

      delayLine_l[delayIn] += (delayLine_l[delayOut] * Delay_Repeat / 2) / 100;
      delayLine_r[delayIn] += (delayLine_r[delayOut] * Delay_Repeat / 2) / 100;

      delayIn ++;

      if (delayIn >= MAX_DELAY)
        delayIn = 0;
    }

    //valleft = valright; issue on val left ????
    
    reg_dac = REG_READ(0x3FF48484);
    reg_dac &= ~0x07F80000;
    reg_dac |= valright << 19;
    REG_WRITE(0x3FF48484, reg_dac);

    reg_dac = REG_READ(0x3FF48488);
    reg_dac &= ~0x07F80000;
    reg_dac |= valleft << 19;
    REG_WRITE(0x3FF48488, reg_dac);

    gui16_RingRead++;
    gui16_RingCount--;
    if (gui16_RingRead > MAX_BUFFER_SAMPLE)
      gui16_RingRead = 0;
    if (gui16_RingCount == 0)
      gui16_RingCount = MAX_BUFFER_SAMPLE;

  }
}



/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void IRAM_ATTR onTimer_xms()
{
  portENTER_CRITICAL_ISR(&timerMux_xms);
  interruptCounter++;
  gui32_Cpt1ms++;
  gui32_Cpttempo++;
  portEXIT_CRITICAL_ISR(&timerMux_xms);
}

/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void IRAM_ATTR onTimer_44100Hertz()
{

  portENTER_CRITICAL_ISR(&timerMux_44100Hertz);
  // your code here
  gui32_cptit40K++;
  gui32_IHM++;
  gui32_EEPCommit++;

  gui8_CptSound++;

  if (gui8_MidiIn)
    gui16_TimerMidiIn++;

  if (gui8_MidiClock)
    gui16_TimerMidiClock++;

  //gui32_Cpttempo++;
  if (gui32_Cpttempo > gui32_Maxcpttempo / 4) // DIV4 for 22050 Fs
  {
    gui32_Cpttempo = 0;
    if (!gui8_SyncAnaExt)
      gui8_TrigBpm = 1;
  }
  SendSound();
  portEXIT_CRITICAL_ISR(&timerMux_44100Hertz);
}

/*****************************************/
/* GENERATE A TRIANGLE 440 HERTZ         */
/*                                       */
/*****************************************/
void IRAM_ATTR sawonTimer_44100Hertz()
{
  static uint8_t   val;
  static uint8_t   cptval;

  portENTER_CRITICAL_ISR(&timerMux_44100Hertz);
  // your code here
  gui32_cptit40K++;
  gui32_IHM++;

  gui32_Cpttempo++;
  if (gui32_Cpttempo > gui32_Maxcpttempo)
  {
    gui32_Cpttempo = 0;
    gui8_TrigBpm = 1;
  }

  cptval++;
  if (cptval < 100)
  {
    val += 2;
  }
  else
  {
    val -= 2;
  }
  if (val == 0)
  {
    cptval = 0;
  }

  reg_dac = REG_READ(0x3FF48484);
  reg_dac &= ~0x07F80000;
  reg_dac |= val << 19;
  REG_WRITE(0x3FF48484, reg_dac);

  reg_dac = REG_READ(0x3FF48488);
  reg_dac &= ~0x07F80000;
  reg_dac |= val << 19;
  REG_WRITE(0x3FF48488, reg_dac);

  portEXIT_CRITICAL_ISR(&timerMux_44100Hertz);
}

TaskHandle_t  Core0TaskHnd;
/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void Task0UpdateIHM(void *parameter)
{
  while (true)
  {

    if (gui8_CptSound)
    {
      gui8_CptSound = 0;
    }
    if (gui32_EEPCommit > 10000 && gui8_EEPAskCommit)
    {
      gui32_EEPCommit = 0;
      gui8_EEPAskCommit = 0;
      EEPROM.commit();
    }
    //--------------------------------
    //From old project pop7
    if (gui32_IHM > 800)  // 200ms
    {
      gui32_IHM = 0;
      #ifdef __BLUETOOTH__
      SerialBT.println("CECI EST UN TEST");
      #endif
    }
    
    syncinstate = digitalRead(GPIO_SYNC_IN);
    if (syncinstate != oldsyncinstate)
    {
      if (!gui8_SyncAnaExt)
      {
        gui8_SyncAnaExt = 1;
        Serial.println("SYNC EXTERN...............");
      }
      cptedge++;
      if (cptedge == 2)
      {
        gui8_TrigBpm = 1;
        cptedge = 0;
      }
      oldsyncinstate = syncinstate;
    }
    else
    {
    }
    Midi_Process();  

    //AudioLoop();

    if (interruptCounter > 1)
    {
      Enc_Timer(&Enc_A);
      Enc_Timer(&Enc_B);
      Enc_Timer(&Enc_C);

      Enc_GetValue(&Enc_A);
      Enc_GetValue(&Enc_B);
      Enc_GetValue(&Enc_C);

      //portENTER_CRITICAL(&timerMux_xms);
      interruptCounter = 0;
      //portEXIT_CRITICAL(&timerMux_xms);

      if (gui32_Cpt1ms > 1000)
      {
        gui32_Cpt1ms = 0;
        //sprintf(messdebug, "TOP 1s %d",gui32_Cpt1ms);
        //Serial.println(messdebug);
      }
      if (gui32_cptit40K > 44100)
      {
        //Serial.println("SAMPLE 1s");
        gui32_cptit40K = 0;
        Serial.printf("TR %04X\n",gi16_trash);
      }
    }


    //Serial.printf("T0\n");
    delay(1);
    yield();
  }
}



/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void setup(void) {


#ifdef __BLUETOOTH__
  SerialBT.begin("HANSY2", 0); //Bluetooth device name
  SerialBT.println("CECI EST UN TEST");
#endif

  tft.init();
  tft.setRotation(1);

  timer_xms = timerBegin(0, 1600, true);                 // Prescaler for f processor 80MHertz - Strange the ESP is at 240MHertz
  timerAttachInterrupt(timer_xms, &onTimer_xms, true);  // Attach interrupt
  timerAlarmWrite(timer_xms, 50, true);                 // Set cpt timer for xms -> 1000 = 1s
  timerAlarmEnable(timer_xms);

  gui16_GPitch = 50; // 20Khertz

  // Timer at 44100 KHertz
  timer_44100Hertz = timerBegin(1, 80, true);                           // Prescaler for f processor 80MHertz - Strange the ESP is at 240MHertz (No 240 for WIFI and BT 80 for CPU)
  timerAttachInterrupt(timer_44100Hertz, &onTimer_44100Hertz, true);    // Attach interrupt
  //timerAlarmWrite(timer_44100Hertz, 25, true);                        // Set cpt timer for 40KHertz
  timerAlarmWrite(timer_44100Hertz, gui16_GPitch, true);                // Set cpt timer for 20KHertz
  //timerAlarmWrite(timer_44100Hertz, 100, true);                       // Set cpt timer for 10KHertz
  timerAlarmEnable(timer_44100Hertz);

  EEPROM.begin(1023);
  Serial.begin(115200);

  Midi_Setup();
  
  //WiFi.softAP(ssid, password);

  //Serial.print("Adresse IP: ");
  //Serial.println(WiFi.softAPIP());

}
/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void espDelay(int ms)
{
  esp_sleep_enable_timer_wakeup(ms * 1000);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  esp_light_sleep_start();
}

/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void AddSample(int32_t* left, int32_t* right, uint8_t* sample, uint8_t index)
{
  /*
  *left +=((int32_t)(*sample-128));
  *right  +=((int32_t)(*sample-128));
  return;
  */
 
  if (gui8_Pan[index] < 50)
  {
    *right  += ((int32_t)(*sample - 128) * gui8_Volume[index] * gui8_Pan[index] * gui16_GVolume) / (100 * 100 * 50);
    *left += ((int32_t)(*sample - 128) * gui8_Volume[index] * gui16_GVolume) / (100 * 100);
  }
  else
  {
    *right  += ((int32_t)(*sample - 128) * gui8_Volume[index] * gui16_GVolume) / (100 * 100);
    *left += ((int32_t)(*sample - 128) * gui8_Volume[index] * (100 - gui8_Pan[index]) * gui16_GVolume) / (100 * 100 * 50);
  }

}

/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void AudioLoop()
{
  int32_t totalleft;
  int32_t totalright;
  
  if (gui16_RingCount < MAX_BUFFER_SAMPLE)
  {
    totalleft = 0;
    totalright = 0;
    
    if (samplecnt[INST_GU])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&GU[samplepnt[INST_GU]], INST_GU);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 128)gui8_Sample_inc = -1;
      samplepnt[INST_GU] += gui8_Sample_inc;
      samplecnt[INST_GU]--;
    }
    if (samplecnt[INST_BG2])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&BG2[samplepnt[INST_BG2]], INST_BG2);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 64)gui8_Sample_inc = -1;
      samplepnt[INST_BG2] += gui8_Sample_inc;
      samplecnt[INST_BG2]--;
    }
    if (samplecnt[INST_BD])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&BD[samplepnt[INST_BD]], INST_BD);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 32)gui8_Sample_inc = -1;
      samplepnt[INST_BD] += gui8_Sample_inc;
      samplecnt[INST_BD]--;
    }
    if (samplecnt[INST_CL])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&CL[samplepnt[INST_CL]], INST_CL);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 16)gui8_Sample_inc = -1;
      samplepnt[INST_CL] += gui8_Sample_inc;
      samplecnt[INST_CL]--;
    }
    if (samplecnt[INST_CW])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&CW[samplepnt[INST_CW]], INST_CW);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 8)gui8_Sample_inc = -1;
      samplepnt[INST_CW] += gui8_Sample_inc;
      samplecnt[INST_CW]--;
    }
    if (samplecnt[INST_MA])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&MA[samplepnt[INST_MA]], INST_MA);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 4)gui8_Sample_inc = -1;
      samplepnt[INST_MA] += gui8_Sample_inc;
      samplecnt[INST_MA]--;
    }
    if (samplecnt[INST_CY])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&CY[samplepnt[INST_CY]], INST_CY);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 2)gui8_Sample_inc = -1;
      samplepnt[INST_CY] += gui8_Sample_inc;
      samplecnt[INST_CY]--;
    }
    if (samplecnt[INST_QU])
    {
      AddSample(&totalleft, &totalright, (uint8_t*)&QU[samplepnt[INST_QU]], INST_QU);
      gui8_Sample_inc = 1; if (gui16_Sample_Reverse & 1)gui8_Sample_inc = -1;
      samplepnt[INST_QU] += gui8_Sample_inc;
      samplecnt[INST_QU]--;
    }

    if (totalleft < -127) totalleft = -127;
    if (totalleft > 127) totalleft = 127;

    if (totalright < -127) totalright = -127;
    if (totalright > 127) totalright = 127;

    //timerAttachInterrupt(timer_44100Hertz, &onTimer_44100Hertz, false);    // Detach
    portENTER_CRITICAL_ISR(&timerMux_44100Hertz);
    gui8_RingbufferLeft[gui16_RingWrite] = totalleft + 128;
    gui8_RingbufferRight[gui16_RingWrite] = totalright + 128;

    gui16_RingWrite++;
    gui16_RingCount++;
    
    if (gui16_RingWrite > (MAX_BUFFER_SAMPLE-0))
      gui16_RingWrite = 0;
    if (gui16_RingCount > (MAX_BUFFER_SAMPLE-0))
      gui16_RingCount = 0;
    portEXIT_CRITICAL_ISR(&timerMux_44100Hertz);
    //timerAttachInterrupt(timer_44100Hertz, &onTimer_44100Hertz, true);    // Detach
    

    // IF START
    if (gui8_PB_startstop)
    {

      // Next Step
      if (gui8_TrigBpm)
      {
        gui8_Trigstep = 1;

        //SerialMidi.write(0x90);
        //SerialMidi.write(0x3C);
        //SerialMidi.write(0x40);

        gui32_cptit40K = 0;
        if (gui8_MidiClock)
          gui8_MidiCptClock = 0;
        uint8_t trig = pattern[(gui8_PatSelect << 4) + gui8_stepcnt++];

        /*
          sprintf(messdebug, "STEP n° %d - Trig %02X", gui8_stepcnt, trig);
          Serial.println(messdebug);
        */

        //digitalWrite(GPIO_SYNC_OUT, !digitalRead(GPIO_SYNC_OUT));
        //digitalRead(GPIO_SYNC_OUT);
        st = !st;
        if (st)
        {
          digitalWrite(GPIO_SYNC_OUT, HIGH);
        }
        else
        {
          digitalWrite(GPIO_SYNC_OUT, LOW);
        }



        if (gui8_stepcnt > gui8_patlength)
        {
          gui8_stepcnt = 0;
        }
        trig &= gui8_MuteMask;
        uint16_t taille;

        if (trig & 1)
        {
          taille = (Tab_SampleSize[INST_QU] * gui8_Decay[INST_QU]) / 100;
          samplepnt[INST_QU] = 0; if (gui16_Sample_Reverse & 1)samplepnt[INST_QU] = taille;
          samplecnt[INST_QU] = taille;
        }
        
        if (trig & 2)
        {
          taille = (Tab_SampleSize[INST_CY] * gui8_Decay[INST_CY]) / 100;
          samplepnt[INST_CY] = 0; if (gui16_Sample_Reverse & 2)samplepnt[INST_CY] = taille;
          samplecnt[INST_CY] = taille;
        }
        if (trig & 4)
        {
          taille = (Tab_SampleSize[INST_MA] * gui8_Decay[INST_MA]) / 100;
          samplepnt[INST_MA] = 0; if (gui16_Sample_Reverse & 4)samplepnt[INST_MA] = taille;
          samplecnt[INST_MA] = taille;
        }
        if (trig & 8)
        {
          taille = (Tab_SampleSize[INST_CW] * gui8_Decay[INST_CW]) / 100;
          samplepnt[INST_CW] = 0; if (gui16_Sample_Reverse & 8)samplepnt[INST_CW] = taille;
          samplecnt[INST_CW] = taille;
        }
        if (trig & 16)
        {
          taille = (Tab_SampleSize[INST_CL] * gui8_Decay[INST_CL]) / 100;
          samplepnt[INST_CL] = 0; if (gui16_Sample_Reverse & 16)samplepnt[INST_CL] = taille;
          samplecnt[INST_CL] = taille;
        }
        if (trig & 32)
        {
          taille = (Tab_SampleSize[INST_BD] * gui8_Decay[INST_BD]) / 100;
          samplepnt[INST_BD] = 0; if (gui16_Sample_Reverse & 32)samplepnt[INST_BD] = taille;
          samplecnt[INST_BD] = taille;
        }
        if (trig & 64)
        {
          taille = (Tab_SampleSize[INST_BG2] * gui8_Decay[INST_BG2]) / 100;
          samplepnt[INST_BG2] = 0; if (gui16_Sample_Reverse & 64)samplepnt[INST_BG2] = taille;
          samplecnt[INST_BG2] = taille;
        }
        if (trig & 128)
        {
          taille = (Tab_SampleSize[INST_GU] * gui8_Decay[INST_GU]) / 100;
          samplepnt[INST_GU] = 0; if (gui16_Sample_Reverse & 128)samplepnt[INST_GU] = taille;
          samplecnt[INST_GU] = taille;
        }
      }
    }
  }
}


/*****************************************/
/*                                       */
/*                                       */
/*****************************************/
void loop() {

  // Fill screen with grey so we can see the effect of printing with and without
  // a background colour defined
  tft.fillScreen(IHM_BACKGROUND_COLOR);

  //tft.setCursor(0, 0, 2);
  // Set the font colour to be yellow with no background, set to font 7
  //tft.setTextColor(TFT_GREEN); tft.setTextFont(7);
  //tft.drawString("12",0,0,7); // Overwrite the text to clear it

  // Set the font colour to be red with black background, set to font 4
  tft.setTextColor(IHM_THEME_COLOR, IHM_BACKGROUND_COLOR);
  tft.setTextFont(4);

  //--------------------------------
  //From old project pop7
  //--------------------------------
  int i;

  samplecnt[INST_BD] = 0;
  samplecnt[INST_BG2] = 0;
  samplecnt[INST_CL] = 0;
  samplecnt[INST_CW] = 0;
  samplecnt[INST_CY] = 0;
  samplecnt[INST_GU] = 0;
  samplecnt[INST_MA] = 0;
  samplecnt[INST_QU] = 0;

  
  gui32_tempo = 2000;
  gui32_cptit40K = 0;
  gui32_IHM = 0;


  gui32_tempo = (240 - 120) * 40 + 200;
  gui32_tempo /= 2;

  gui8_PatSelect = 0;
  gui8_InstSelect = 0;
  gui8_patlength = patlen[gui8_PatSelect];

  Delay_TimeEch = (uint32_t)(Delay_Time * SIZE_TAB_DELAY) / 100;

  // End Old project Pop7

  //EEPROM.write(0, 0x16);
  //EEPROM.write(1, 0x11);
  //EEPROM.write(2, 0x65);
  //EEPROM.commit();
  //uint8_t read1 = EEPROM.read(0);
  //uint8_t read2 = EEPROM.read(1);
  //uint8_t read3 = EEPROM.read(2);
  Serial.println("-----------------------------------------------------------------------------------");
  Serial.println(" GO DRUM TRACE                                                                     ");
  Serial.println(" HANSY SYNTH                                                                       ");
  Serial.println(" LACAUD Gilles                                                                     ");
  Serial.println("-----------------------------------------------------------------------------------");

  //Serial.print("Eeprom read at addr 0 - 0X");
  //Serial.println(read1, HEX);
  //Serial.print("Eeprom read at addr 1 - 0X");
  //Serial.println(read2, HEX);
  //Serial.print("Eeprom read at addr 2 - 0X");
  //Serial.println(read3, HEX);

  Delay_Init();

  Enc_Init();

  uint8_t id1, id2;

  id1 = EEPROM.read(EEP_OFF_IDENT);
  id2 = EEPROM.read(EEP_OFF_IDENT + 1);

  // Encoder CLK config
  if (!digitalRead(15))
  {
    Serial.println("CONFIG CLK");
    tft.drawString("TURN ENC A", 10, 5, 4);
    while ((Enc_A.ClkDirection = Enc_TestClk(&Enc_A)) == 0x55);
    tft.drawString("TURN ENC B", 10, 35, 4);
    while ((Enc_B.ClkDirection = Enc_TestClk(&Enc_B)) == 0x55);
    tft.drawString("TURN ENC C", 10, 65, 4);
    while ((Enc_C.ClkDirection = Enc_TestClk(&Enc_C)) == 0x55);
    tft.drawString("CONFIG DONE", 10, 95, 4);

    EEPROM.write(EEP_OFF_ENCLCK + 0, Enc_A.ClkDirection);
    EEPROM.write(EEP_OFF_ENCLCK + 1, Enc_B.ClkDirection);
    EEPROM.write(EEP_OFF_ENCLCK + 2, Enc_C.ClkDirection);
    EEPROM.commit();

  }


  // To reset all the eeprom on PB enc A
  if (!digitalRead(38))
  {
    id1 = 23;
  }

  if (id1 != 0xDE || id2 != 0xAD)
  {
    gui8_MidiRX = 10;
    gui8_GuiColor = 0;

    EEPROM.write(EEP_OFF_IDENT, 0xDE);
    EEPROM.write(EEP_OFF_IDENT + 1, 0xAD);
    EEPROM.write(EEP_OFF_LASTP, 0);
    EEPROM.write(EEP_OFF_LASTT, 120);
    EEPROM.write(EEP_OFF_LASTV, 90);
    EEPROM.write(EEP_OFF_LASTPI, 30);

    EEPROM.write(EEP_OFF_DELAY + 0, 40);
    EEPROM.write(EEP_OFF_DELAY + 1, 10);
    EEPROM.write(EEP_OFF_DELAY + 2, 1);
    // Clear Eep
    for (uint8_t p = 0; p < 16; p++)
    {
      // For the 16 step
      for (uint8_t s = 0; s < 16; s++)
        EEPROM.write(EEP_OFF_PATTERN + p * 16 + s, 0x00);
    }
    uint8_t y;
    for (y = 0; y < 8; y++)
    {
      EEPROM.write(EEP_OFF_VOLUME + y, 60);
      EEPROM.write(EEP_OFF_PAN + y, 50);
      EEPROM.write(EEP_OFF_DECAY + y, 100);
      EEPROM.write(EEP_OFF_REVERSE + y, 0);
    }

    EEPROM.write(EEP_OFF_GUICOLOR, 0);
    EEPROM.write(EEP_OFF_MIDIRX, 10);

    for (y = 0; y < 8; y++)
    {
      gui8_Volume[y] = 85;
      gui8_Decay[y] = 100;
      gui8_Pan[y] = 50;
    }
    EEPROM.commit();
    Serial.println("EEPROM INIT");
  }
  else
    Serial.println("EEPROM ALREADY INIT");


  // For all user pattern
  for (uint8_t p = 0; p < 16; p++)
  {
    // For the 16 step
    for (uint8_t s = 0; s < 16; s++)
    {
      pattern[(p + 16) * 16 + s] = EEPROM.read(EEP_OFF_PATTERN + p * 16 + s);
      /*
        Serial.print("Eeprom read: ");
        Serial.println(pattern[(p+16)*16+s], HEX);
      */
    }
  }

  Enc_A.ClkDirection = EEPROM.read(EEP_OFF_ENCLCK + 0);
  Enc_B.ClkDirection = EEPROM.read(EEP_OFF_ENCLCK + 1);
  Enc_C.ClkDirection = EEPROM.read(EEP_OFF_ENCLCK + 2);

  gui8_PatSelect = EEPROM.read(EEP_OFF_LASTP);
  gui8_Afftempo = EEPROM.read(EEP_OFF_LASTT);
  gui16_GVolume = EEPROM.read(EEP_OFF_LASTV);
  gui16_GPitch = EEPROM.read(EEP_OFF_LASTPI);
  gui8_patlength = patlen[gui8_PatSelect];
  Fct_ChangeTempo(0);
  timerAlarmWrite(timer_44100Hertz, gui16_GPitch, true);                // Set cpt timer for 20KHertz

  for (uint8_t y = 0; y < 8; y++)
  {
    gui8_Volume[y] = EEPROM.read(EEP_OFF_VOLUME + y);
    gui8_Decay[y] = EEPROM.read(EEP_OFF_DECAY + y);
    gui8_Pan[y] = EEPROM.read(EEP_OFF_PAN + y);
    gui8_Reverse[y] = EEPROM.read(EEP_OFF_REVERSE + y);
    if (gui8_Reverse[y])
      gui16_Sample_Reverse |= 0x80 >> y;
    else
      gui16_Sample_Reverse &= ~(0x80 >> y);
  }

  Delay_Time   = EEPROM.read(EEP_OFF_DELAY + 0);
  Delay_Amp    = EEPROM.read(EEP_OFF_DELAY + 1);
  Delay_Repeat = EEPROM.read(EEP_OFF_DELAY + 2);

  gui8_MidiRX   = EEPROM.read(EEP_OFF_MIDIRX);
  gui8_GuiColor = EEPROM.read(EEP_OFF_GUICOLOR);
  switch (gui8_GuiColor)
  {
    case 0: gcolor = TFT_GREEN; break;
    case 1: gcolor = TFT_ORANGE; break;
    case 2: gcolor = TFT_PINK; break;
    case 3: gcolor = TFT_GOLD; break;
    case 4: gcolor = TFT_SKYBLUE; break;
  }
  tft.setTextColor(gcolor, IHM_BACKGROUND_COLOR);


  Fct_DelayTime(0);


  /*********************************************************/
  /* HARDWARE CONFIG                                       */
  /*********************************************************/
  //tft.fillScreen(IHM_BACKGROUND_COLOR);




  // WARNING
  // NO INTERNAL PULL UP AND PULL DOWN FOR GPIO 34 TO 39 INPUT ONLY
  /*
    pinMode(Enc_A.pinA, INPUT_PULLUP);
    pinMode(Enc_A.pinB, INPUT_PULLUP);
    pinMode(Enc_A.pinBTN,INPUT_PULLUP);

    pinMode(Enc_B.pinA, INPUT_PULLUP);
    pinMode(Enc_B.pinB, INPUT_PULLUP);
    pinMode(Enc_B.pinBTN,INPUT_PULLUP);

    pinMode(Enc_C.pinA, INPUT_PULLUP);
    pinMode(Enc_C.pinB, INPUT_PULLUP);
    pinMode(Enc_C.pinBTN,INPUT_PULLUP);
  */

  pinMode(GPIO_SYNC_IN, INPUT);
  pinMode(GPIO_SYNC_OUT, OUTPUT);

  syncinstate = digitalRead(GPIO_SYNC_IN);
  oldsyncinstate = syncinstate;

  //digitalWrite(GPIO_SYNC_OUT, 0);


  dacWrite(GPIO_DAC_1, 0xFF / 2); // To init the DAC
  dacWrite(GPIO_DAC_2, 0xFF / 2); // To init the DAC

  for (uint32_t f = 0; f < SIZE_TAB_DELAY; f++)
  {
    Tab_DelayLeft[f] = 0;
    Tab_DelayRight[f] = 0;
  }

  for (uint32_t f = 0; f <MAX_BUFFER_SAMPLE; f++)
  {
    gui8_RingbufferLeft[f]=0;
    gui8_RingbufferRight[f]=0;
  }

  gi16_trash = 0x1611;

  gui16_RingWrite=0;
  gui16_RingCount=0;

  /*------------------------------------*/
  /* TEST 16 BITS OUTPUT                */
  /*------------------------------------*/
  /*
    ledcSetup(0,44100,16);        //  Channel 0 at 44.1KHertz Resolution 16bits
    ledcAttachPin(GPIO_DAC_1,0);  //  Attach Hw pin tio channel 0
    ledcWrite(0,16000);           //  Write to the pin
  */


  /*********************************************************/
  /* HW & IHM TEST PATTERN                                 */
  /*********************************************************/
#ifdef __HARDWARE_TEST__
  Hw_Test_Dev();
#endif

#ifdef __IHM_TEST__
  Ihm_Test_Dev();
  while (1);
#endif

  Serial.println(" MAIN LOOP");
  //gui8_PatSelect = 0;

  dacWrite(GPIO_DAC_1, 0x00);   // To init the DAC
  dacWrite(GPIO_DAC_2, 0x00);   // To init the DAC

  Fct_ChangeTempo(0);
  Ihm_Set_Led(&Ihm_Led_BPM);

  Ihm_Set_Led(&Ihm_Led_StartStop);

  Enc_A.val = gui8_PatSelect;
  Enc_A.delta = 0;

  Enc_B.val = gui8_Afftempo;
  Enc_B.delta = 0;

  Enc_GetValue(&Enc_B);
  Enc_A.oldval = Enc_A.val;
  Enc_B.oldval = Enc_B.val;
  Enc_C.oldval = Enc_C.val;

  gui8_Ihm_CurrentScreen = 0;
  Fct_PBChangeScreen(0xFF);

  Ihm_Set_PotDescription();

  for (i = 0; i < 8; i++)
    Ihm_Init_Pot(&Ihm_Tab_Pot[i]);

  Enc_C.val = gui16_GVolume;

  Ihm_Set_Led(&Ihm_Led_StartStop);
  
  xTaskCreatePinnedToCore(Task0UpdateIHM, "LCDTask", 2000, NULL,0, &Core0TaskHnd,0);

  while (1)
  {

    AudioLoop();
    //-------------------------------------------
    // BPM
    //-------------------------------------------
    if (gui8_TrigBpm)
    {
      // In stop mode
      if (!gui8_PB_startstop)
        gui8_TrigBpm = 0;
      else
      {
        if (gui8_Trigstep)
        {
          gui8_Trigstep = 0;
          gui8_TrigBpm = 0;
        }
      }
      if (Ihm_Led_BPM.ui8_state == LED_STATE_OFF)
      {
        Ihm_Led_BPM.ui8_state = LED_STATE_ON;
      }
      else
      {
        Ihm_Led_BPM.ui8_state = LED_STATE_OFF;
      }
      if (gui8_Ihm_CurrentScreen == IHM_SCREEN_PATTERN)
        Ihm_Set_Led(&Ihm_Led_BPM);

      if (gui8_Ihm_CurrentScreen == IHM_SCREEN_PATTERN)
      {
        sprintf(messdebug, "VOL %03d   STEP %02d", gui16_GVolume, gui8_stepcnt + 1);
        Ihm_Set_Label(2, messdebug);
      }
      if (gui8_Ihm_CurrentScreen == IHM_SCREEN_PAD)
      {
        Ihm_Draw_PadStep(gui8_stepcnt - 1);
      }
    }


  }
  while (1) yield(); // We must yield() to stop a watchdog timeout.
}
