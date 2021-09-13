#define __IHM__
#include "IHM.h"

#include "Global.h"

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
extern TFT_eSPI tft;

extern void espDelay(int ms);

/*****************************************************************************************************/
/* LABEL                                                                                             */
/*                                                                                                   */
/*****************************************************************************************************/
void Ihm_Set_Label(uint8_t num, char *txt)
{
  if (num >= IHM_LABEL_MAX)
    return;
  tft.drawRect(LABEL_RECT_Sx, LABEL_RECT_Sy + LABEL_RECT_OFF * num + 1, LABEL_RECT_W, LABEL_RECT_H,gcolor);
  tft.drawString(txt, 10, LABEL_RECT_OFF * num + 1 + 10, 4);
}


/*****************************************************************************************************/
/* LED                                                                                               */
/*                                                                                                   */
/*****************************************************************************************************/



/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Set_LedDescription()
{
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    Ihm_Tab_Led[i].ui8_id = i;
    Ihm_Tab_Led[i].ui8_group = 0;
    Ihm_Tab_Led[i].ui8_x = 10 + i * 30;
    Ihm_Tab_Led[i].ui8_y = 100;
    Ihm_Tab_Led[i].ui8_rad = 8;
    Ihm_Tab_Led[i].ui16_coloroff = TFT_DARKGREY;
    Ihm_Tab_Led[i].ui16_colorinactif = TFT_PURPLE;
    Ihm_Tab_Led[i].ui16_coloron = TFT_RED;
    Ihm_Tab_Led[i].ui8_state = LED_STATE_INACTIF;
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Set_Led(Ihm_Led* led)
{
  switch (led->ui8_state)
  {
    case LED_STATE_OFF:
      tft.fillCircle(led->ui8_x, led->ui8_y, led->ui8_rad, led->ui16_coloroff);
      break;
    case LED_STATE_INACTIF:
      tft.fillCircle(led->ui8_x, led->ui8_y, led->ui8_rad, led->ui16_colorinactif);
      break;
    case LED_STATE_ON:
      tft.fillCircle(led->ui8_x, led->ui8_y, led->ui8_rad, led->ui16_coloron);
      break;
  }
}


/*****************************************************************************************************/
/* POT                                                                                               */
/*                                                                                                   */
/*****************************************************************************************************/

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Set_PotDescription()
{
  uint8_t i;

  for (i = 0; i < 8; i++)
  {
    Ihm_Tab_Pot[i].ui8_id = i;
    Ihm_Tab_Pot[i].ui8_group = IHM_POT_GROUP_VOL;
    Ihm_Tab_Pot[i].ui8_type = IHM_POT_TYPE_VER;
    Ihm_Tab_Pot[i].ui8_x = 10 + i * 30;
    Ihm_Tab_Pot[i].ui8_y = 10;
    Ihm_Tab_Pot[i].ui8_sizex = 20;
    Ihm_Tab_Pot[i].ui8_sizey = 60;
    Ihm_Tab_Pot[i].ui8_grid = 0;
    Ihm_Tab_Pot[i].ui16_framecolor = IHM_POT_COLOR;
    Ihm_Tab_Pot[i].ui16_slidercolor = IHM_POT_COLOR;
    Ihm_Tab_Pot[i].ui8_min = 0;
    Ihm_Tab_Pot[i].ui8_max = 100;
    Ihm_Tab_Pot[i].ptui8_val = &gui8_Volume[i];
    Ihm_Tab_Pot[i].f_div = 0;
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Init_Pot(Ihm_Pot* pot)
{
  if (pot->ui8_type == IHM_POT_TYPE_VER)
    pot->f_div = (float)(pot->ui8_max - pot->ui8_min) / (float)pot->ui8_sizey;
  else
    pot->f_div = (float)(pot->ui8_max - pot->ui8_min) / (float)pot->ui8_sizex;
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_Pottest(Ihm_Pot* pot)
{
  uint8_t val;
  uint8_t diff;

  if (pot->ui8_type == IHM_POT_TYPE_VER)
  {
    val = (uint8_t)((float) * pot->ptui8_val / pot->f_div);
    if (val > pot->ui8_sizey)
      val = pot->ui8_sizey;

    if (val < pot->ui8_oldvaldiv)
    {
      diff = pot->ui8_oldvaldiv - val;
      tft.drawRect(pot->ui8_x, pot->ui8_y + pot->ui8_sizey - pot->ui8_oldvaldiv, pot->ui8_sizex, diff, IHM_BACKGROUND_COLOR); // Only 1 pixel Cursor
    }
    else
    {
      diff = val - pot->ui8_oldvaldiv;
      tft.drawRect(pot->ui8_x, pot->ui8_y + pot->ui8_sizey - pot->ui8_oldvaldiv, pot->ui8_sizex, diff, pot->ui16_slidercolor); // Only 1 pixel Cursor
    }

    pot->ui8_oldvaldiv = val;
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_Pot(Ihm_Pot* pot)
{
  uint8_t val;

  if (pot->ui8_type == IHM_POT_TYPE_VER)
  {
    // Erase the old cursor
    tft.fillRect(pot->ui8_x, pot->ui8_y + pot->ui8_sizey - pot->ui8_oldvaldiv, pot->ui8_sizex, IHM_SIZE_CURSOR, IHM_BACKGROUND_COLOR); // Only 1 pixel Cursor
    // Draw the frame
    tft.drawRect(pot->ui8_x, pot->ui8_y, pot->ui8_sizex, pot->ui8_sizey, pot->ui16_framecolor);
    // Draw the slider
    val = (uint8_t)((float) * pot->ptui8_val / pot->f_div);
    if (val > pot->ui8_sizey)
      val = pot->ui8_sizey;
    tft.fillRect(pot->ui8_x, pot->ui8_y + pot->ui8_sizey - val, pot->ui8_sizex, IHM_SIZE_CURSOR, pot->ui16_slidercolor); // Only 1 pixel Cursor

    pot->ui8_oldvaldiv = val;
  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Select_Pot(Ihm_Pot* pot)
{
 uint8_t i;
 static Ihm_Pot* oldpot=NULL;

  /*
  for (i = 0; i < 8; i++)
  {
    Ihm_Tab_Pot[i].ui16_framecolor = gcolor;
    Ihm_Tab_Pot[i].ui16_slidercolor = gcolor;
    Ihm_Draw_Pot(&Ihm_Tab_Pot[i]);
  }
  */
  if(oldpot)
  {
    oldpot->ui16_framecolor = gcolor;
    oldpot->ui16_slidercolor = gcolor;
    Ihm_Draw_Pot(oldpot); 
  }
  
  // Invert the color of the selected pot
  pot->ui16_framecolor = ~pot->ui16_framecolor;
  pot->ui16_slidercolor = ~pot->ui16_slidercolor;
  Ihm_Draw_Pot(pot);
  oldpot = pot;
}

/*****************************************************************************************************/
/* TEST                                                                                              */
/*                                                                                                   */
/*****************************************************************************************************/
void Ihm_Test_Dev()
{

  tft.fillScreen(IHM_BACKGROUND_COLOR);
  Ihm_Set_Label(0, "PATTERN             ");
  Ihm_Set_Label(1, "TEMPO               ");
  Ihm_Set_Label(2, "VOLUME              ");
  espDelay(1000);
  Ihm_Set_Label(0, "0x01                ");
  Ihm_Set_Label(1, "0x02                ");
  Ihm_Set_Label(2, "0x03                ");
  espDelay(1000);
  tft.fillScreen(IHM_BACKGROUND_COLOR);

  Ihm_Set_PotDescription();
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    Ihm_Init_Pot(&Ihm_Tab_Pot[i]);
    //Ihm_Tab_Pot[i].ui8_val = 0;
    Ihm_Draw_Pot(&Ihm_Tab_Pot[i]);
  }
  int8_t newval = 0;
  int8_t newinc = 1;

  *Ihm_Tab_Pot[0].ptui8_val = 50;    // 50 max100 min 0
  Ihm_Draw_Pot(&Ihm_Tab_Pot[0]);  //
  espDelay(1000);

  *Ihm_Tab_Pot[0].ptui8_val = 0;    // 00 max100 min 0
  Ihm_Draw_Pot(&Ihm_Tab_Pot[0]);  //
  espDelay(1000);

  *Ihm_Tab_Pot[0].ptui8_val = 100;    // 100 max100 min 0
  Ihm_Draw_Pot(&Ihm_Tab_Pot[0]);  //
  espDelay(1000);

  Ihm_Select_Pot(&Ihm_Tab_Pot[4]);
  espDelay(1000);
  Ihm_Select_Pot(&Ihm_Tab_Pot[1]);
  espDelay(1000);

  Ihm_Set_LedDescription();
  for (i = 0; i < 8; i++)
  {
    Ihm_Set_Led(&Ihm_Tab_Led[i]);
  }

  while (1)
  {
    for (i = 0; i < 8; i++)
      *Ihm_Tab_Pot[i].ptui8_val = newval;
    espDelay(200);
    newval += newinc;
    if (newval == 100)
    {
      newinc = -1;
    }
    if (newval < 0)
    {
      newinc = 1;
    }
    //for(i=0;i<8;i++)
    //  Ihm_Draw_Pot(&Ihm_Tab_Pot[i]);

    Ihm_Select_Pot(&Ihm_Tab_Pot[newval % 8]);

    Ihm_Tab_Led[0].ui8_state = newval % 3;
    Ihm_Set_Led(&Ihm_Tab_Led[0]);

  }
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_Grid()
{
  for (uint8_t s = 0; s < 16; s++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      tft.drawRect(s * IHM_GRIDX, j * IHM_GRIDY, IHM_GRIDX, IHM_GRIDY, gcolor);
    }
  }
}


/****************************************************/
/* Draw the actif step                              */
/*                                                  */
/***************************************************/
void Ihm_Draw_PadStep(uint8_t step)
{
  static uint8_t  oldstep;
  if(oldstep !=oldsel)
  {
    if (oldstep < 8)
      tft.drawRect(oldstep * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS, gcolor);
    else
      tft.drawRect((oldstep-8) * (IHM_PADS + 5), 5 + IHM_PADS + 20, IHM_PADS, IHM_PADS, gcolor);
  }
  else
  {
    if (oldstep < 8)
      tft.drawRect(oldstep * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS, IHM_PAD_SEL);
    else
      tft.drawRect((oldstep-8) * (IHM_PADS + 5), 5 + IHM_PADS + 20, IHM_PADS, IHM_PADS, IHM_PAD_SEL);
  }

  if (step < 8)
    tft.drawRect(step * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS,~gcolor);
  else
    tft.drawRect((step-8) * (IHM_PADS + 5), 5 + IHM_PADS + 20, IHM_PADS, IHM_PADS,~gcolor);
    
  //tft.fillRect(step*(IHM_PADS+5),5,IHM_PADS,IHM_PADS,TFT_ORANGE);

  oldstep = step;
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_PadOnoff()
{

}

#define OFF 6




/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_PadSelect(uint8_t sel)
{
  
  if (oldsel < 8)
    tft.drawRect(oldsel * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS, gcolor);
  else
    tft.drawRect((oldsel-8) * (IHM_PADS + 5), 5 + IHM_PADS + 20, IHM_PADS, IHM_PADS, gcolor);

  if (sel < 8)
    tft.drawRect(sel * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS, IHM_PAD_SEL);
  else
    tft.drawRect((sel-8) * (IHM_PADS + 5), 5 + IHM_PADS + 20, IHM_PADS, IHM_PADS, IHM_PAD_SEL);
    
  oldsel = sel;
}


/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void oldIhm_Draw_PadSelect(uint8_t sel)
{

static uint8_t  oldsel;

  if (oldsel < 8)
  {
    tft.fillRect(oldsel * (IHM_PADS + 5)+OFF, 5+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF,IHM_BACKGROUND_COLOR); 
    //tft.drawRect(oldsel * (IHM_PADS + 5)+OFF, 5+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, gcolor);
  }
  else
  {
    tft.fillRect((oldsel-8) * (IHM_PADS + 5)+OFF, 5 + IHM_PADS + 20+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, IHM_BACKGROUND_COLOR);
    //tft.drawRect((oldsel-8) * (IHM_PADS + 5)+OFF, 5 + IHM_PADS + 20+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, gcolor);
  }

  if (sel < 8)
    tft.fillRect(sel * (IHM_PADS + 5)+OFF, 5+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, IHM_PAD_SEL);
  else
    tft.fillRect((sel-8) * (IHM_PADS + 5)+OFF, 5 + IHM_PADS + 20+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, IHM_PAD_SEL);
    
  //tft.fillRect(step*(IHM_PADS+5),5,IHM_PADS,IHM_PADS,TFT_ORANGE);

  oldsel = sel;

}


/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_PadOn(uint8_t sel)
{

  if (sel < 8)
    tft.fillRect(sel * (IHM_PADS + 5)+OFF, 5+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF,gcolor);
  else
    tft.fillRect((sel-8) * (IHM_PADS + 5)+OFF, 5 + IHM_PADS + 20+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF,gcolor);
}

/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_PadOff(uint8_t sel)
{

  if (sel < 8)
    tft.fillRect(sel * (IHM_PADS + 5)+OFF, 5+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, IHM_BACKGROUND_COLOR);
  else
    tft.fillRect((sel-8) * (IHM_PADS + 5)+OFF, 5 + IHM_PADS + 20+OFF, IHM_PADS-2*OFF, IHM_PADS-2*OFF, IHM_BACKGROUND_COLOR);
}





/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_Pad()
{
  // First serie
  for (uint8_t p = 0; p < 8; p++)
  {
    tft.drawRect(p * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS, gcolor);
  }
  // Secondt serie
  for (uint8_t p = 0; p < 8; p++)
  {
    tft.drawRect(p * (IHM_PADS + 5), 5 + IHM_PADS + 20, IHM_PADS, IHM_PADS,gcolor);
  }
}


/****************************************************/
/*                                                  */
/*                                                  */
/***************************************************/
void Ihm_Draw_Reverse()
{
  for (uint8_t p = 0; p < 8; p++)
  {
    tft.drawRect(p * (IHM_PADS + 5), 5, IHM_PADS, IHM_PADS, gcolor);
  }
}
