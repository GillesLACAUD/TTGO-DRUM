/*
#include "Global.h"
#include "HardwareSerial.h"
#include "Encoder.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern HardwareSerial SerialMidi(2);       // Serial instance n°2 for Midi

void Hw_Test_Dev()
{
  Serial.println("---------------------------------------------------");
  Serial.println("HARDWARE TEST...");
  Serial.println("");
  uint8_t reada = 1;
  uint8_t oldreada = 1;
  uint8_t readb = 1;
  uint8_t oldreadb = 1;
  uint8_t readc = 1;
  uint8_t oldreadc = 1;

  uint8_t reada_a = 1;
  uint8_t oldreada_a = 1;
  uint8_t reada_b = 1;
  uint8_t oldreada_b = 1;

  uint8_t readb_a = 1;
  uint8_t oldreadb_a = 1;
  uint8_t readb_b = 1;
  uint8_t oldreadb_b = 1;

  uint8_t readc_a = 1;
  uint8_t oldreadc_a = 1;
  uint8_t readc_b = 1;
  uint8_t oldreadc_b = 1;

  uint8_t outsync;
  uint8_t insync = 1;
  uint8_t oldinsync = 1;

  uint8_t val_dac1 = 0;
  uint8_t val_dac2 = 0xFF;

  uint8_t cptsync = 0;

  while (1)
  {

    //----------------------------------
    // GPIO INPUT
    //----------------------------------
    reada_a = digitalRead(Enc_A.pinA);
    reada_b = digitalRead(Enc_A.pinB);
    reada = digitalRead(Enc_A.pinBTN);

    readb_a = digitalRead(Enc_B.pinA);
    readb_b = digitalRead(Enc_B.pinB);
    readb = digitalRead(Enc_B.pinBTN);

    readc_a = digitalRead(Enc_C.pinA);
    readc_b = digitalRead(Enc_C.pinB);
    readc = digitalRead(Enc_C.pinBTN);

    if (reada != oldreada)
    {
      Serial.print("Encoder A Push button=");
      Serial.println(reada, HEX);
      oldreada = reada;
      tft.drawString("Encoder A Push button", 0, 0, 2);
    }
    if (reada_a != oldreada_a)
    {
      Serial.print("Encoder A A=");
      Serial.println(reada_a, HEX);
      oldreada_a = reada_a;
      tft.drawString("Encoder A A", 0, 20, 2);
    }
    if (reada_b != oldreada_b)
    {
      Serial.print("Encoder A B=");
      Serial.println(reada_a, HEX);
      oldreada_b = reada_b;
      tft.drawString("Encoder A B", 0, 40, 2);
    }


    if (readb != oldreadb)
    {
      Serial.print("Encoder B Push button=");
      Serial.println(readb, HEX);
      oldreadb = readb;
      tft.drawString("Encoder B Push button", 0, 0, 2);
    }
    if (readb_a != oldreadb_a)
    {
      Serial.print("Encoder B A=");
      Serial.println(readb_a, HEX);
      oldreadb_a = readb_a;
      tft.drawString("Encoder B A", 0, 20, 2);
    }
    if (readb_b != oldreadb_b)
    {
      Serial.print("Encoder B B=");
      Serial.println(readb_b, HEX);
      oldreadb_b = readb_b;
      tft.drawString("Encoder B B", 0, 40, 2);
    }


    if (readc != oldreadc)
    {
      Serial.print("Encoder C Push button=");
      Serial.println(readc, HEX);
      oldreadc = readc;
      tft.drawString("Encoder C Push button", 0, 0, 2);
    }
    if (readc_a != oldreadc_a)
    {
      Serial.print("Encoder C A=");
      Serial.println(readc_a, HEX);
      oldreadc_a = readc_a;
      tft.drawString("Encoder C A", 0, 20, 2);
    }
    if (readc_b != oldreadc_b)
    {
      Serial.print("Encoder C B=");
      Serial.println(readc_b, HEX);
      oldreadc_b = readc_b;
      tft.drawString("Encoder C B", 0, 40, 2);
    }

    //----------------------------------
    // GPIO OUTPUT
    //----------------------------------
    insync = digitalRead(GPIO_SYNC_IN);
    if (oldinsync != insync)
    {
      Serial.print("SYNC=");
      Serial.println(insync, HEX);
      oldinsync = insync;
      tft.drawString("SYNC IN", 0, 60, 2);
    }
    //----------------------------------
    // MIDI
    //----------------------------------
    SerialMidi.println("ABCDEFGHIJKLMN");

    //----------------------------------
    // DAC
    //----------------------------------
    val_dac1 ++;
    val_dac2 --;

    //dacWrite(GPIO_DAC_1, val_dac1);
    //dacWrite(GPIO_DAC_2, val_dac2);

    uint32_t reg;
    char mess[20];

    //----------------------------------
    // TIMER
    //----------------------------------
    if (gui32_IHM > 44100)
    {
      gui32_IHM = 0;
      Serial.println("TIMER 1s cpt=44100");
      if (outsync == 1)
      {
        outsync = 0;
        digitalWrite(GPIO_SYNC_OUT, HIGH);
        tft.drawString("SYNC OUT HIGH", 0, 120, 2);
      }
      else
      {
        outsync = 1;
        digitalWrite(GPIO_SYNC_OUT, LOW);
        tft.drawString("SYNC OUT LOW", 0, 120, 2);
      }
    }
    delay(100);

  }
}
*/
