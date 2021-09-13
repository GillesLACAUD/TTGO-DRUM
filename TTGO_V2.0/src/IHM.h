#ifdef __IHM__
#define IHM_EXTRN
#else
#define IHM_EXTRN   extern
#endif
#include "stdint.h"

#include <TFT_eSPI.h>


#define   IHM_MAX_SCREEN    8
#define IHM_SCREEN_PATTERN  0
#define IHM_SCREEN_DELAY    1
#define IHM_SCREEN_VOLUME   2
#define IHM_SCREEN_PAN      3
#define IHM_SCREEN_DECAY    4
#define IHM_SCREEN_REVERSE  5
#define IHM_SCREEN_PAD      6
#define IHM_SCREEN_MISC     7


/****************************************************/
/* Define IHM GLOBAL                                */
//***************************************************/
#define IHM_BACKGROUND_COLOR  TFT_BLACK
#define IHM_THEME_COLOR       TFT_GREEN   //TFT_ORANGE    //TFT_GREEN 

//https://diyprojects.io/ttgo-t-watch-get-started-esp32-tft_espi-library/#.YCbs8Y9KguU

#define IHM_PAD_STEP  TFT_DARKGREEN
#define IHM_PAD_SEL   TFT_RED
#define IHM_PAD_ON    TFT_GREEN

/*****************************************************************************************************/
/* LABEL                                                                                             */
/*                                                                                                   */
/*****************************************************************************************************/
#define IHM_LABEL_MAX         3
// RECTANGLE FOR TEST
#define LABEL_RECT_Sx 1       // Start offset x
#define LABEL_RECT_Sy 1       // Start offset y
#define LABEL_RECT_W  239     // Width of the rect
#define LABEL_RECT_H  40      // Heigh of the rect
#define LABEL_RECT_OFF 45     // Offset betwenn rect

extern char  str_Tab_Aff[IHM_LABEL_MAX][20];

#ifdef __IHM__
uint16_t gcolor = TFT_GREEN;
#else
extern uint16_t gcolor;
#endif

/*****************************************************************************************************/
/* LED                                                                                               */
/*                                                                                                   */
/*****************************************************************************************************/

/****************************************************/
/* Define Led                                       */
/***************************************************/
#define IHM_LED_MAX         16

#define LED_STATE_OFF         0
#define LED_STATE_INACTIF     1
#define LED_STATE_ON          2

#define TFT_LED_COLOE_OFF       TFT_DARKGREY
#define TFT_LED_COLOE_INACTIF   TFT_PURPLE
#define TFT_LED_COLOE_ON        TFT_RED


typedef struct
{
  uint8_t       ui8_id;                 // To identify the pot
  uint8_t       ui8_group;              // Pot part of a group
  uint8_t       ui8_x;                  // X position
  uint8_t       ui8_y;                  // Y position
  uint8_t       ui8_rad;                // Radius
  uint16_t      ui16_coloroff;          //
  uint16_t      ui16_colorinactif;      // 
  uint16_t      ui16_coloron;           // 
  uint8_t       ui8_state;              // 
}Ihm_Led;


IHM_EXTRN Ihm_Led Ihm_Tab_Led[IHM_LED_MAX];       // 

#ifdef __IHM__
IHM_EXTRN Ihm_Led Ihm_Led_BPM       ={0,0,220,65,8,TFT_LED_COLOE_OFF,TFT_PURPLE,TFT_RED,LED_STATE_OFF};
IHM_EXTRN Ihm_Led Ihm_Led_StartStop ={1,0,220,20,8,TFT_LED_COLOE_OFF,TFT_PURPLE,TFT_RED,LED_STATE_OFF};
#else
IHM_EXTRN Ihm_Led Ihm_Led_BPM;
IHM_EXTRN Ihm_Led Ihm_Led_StartStop;
#endif


IHM_EXTRN  uint8_t gui8_Ihm_CurrentScreen;
IHM_EXTRN  uint8_t  oldsel;
extern void Ihm_Set_LedDescription();
extern void Ihm_Set_Led(Ihm_Led* led);

/*****************************************************************************************************/
/* POT                                                                                               */
/*                                                                                                   */
/*****************************************************************************************************/

/****************************************************/
/* Define a potentiometer                           */
//***************************************************/

#define IHM_POT_MAX         16

#define IHM_POT_TYPE_VER    0
#define IHM_POT_TYPE_HOR    1

#define IHM_SIZE_CURSOR     3                 // Size of the cursor in pixel
#define IHM_POT_COLOR       IHM_THEME_COLOR   // 
#define IHM_POT_GROUP_VOL   1                 // Size of the cursor in pixel

typedef struct
{
  uint8_t       ui8_id;                 // To identify the pot
  uint8_t       ui8_group;              // Pot part of a group
  uint8_t       ui8_type;               // Vertical or Horizontal
  uint8_t       ui8_x;                  // Upper left point
  uint8_t       ui8_y;                  // Upper left point
  uint8_t       ui8_sizex;              // Size of the pot
  uint8_t       ui8_sizey;              // Size of the pot
  uint8_t       ui8_grid;               // Add grid to the pot
  uint16_t      ui16_framecolor;        // Frame Color
  uint16_t      ui16_slidercolor;       // Slider Color

  uint8_t       ui8_min;                // Minimum Value
  uint8_t       ui8_max;                // Maximum Value
  uint8_t*      ptui8_val;              // Point to a value
  uint8_t       ui8_oldvaldiv;          // To delete the old cursor 

  float         f_div;                  // Factor between size and (max-min)               
  
}Ihm_Pot;

IHM_EXTRN Ihm_Pot Ihm_Tab_Pot[IHM_POT_MAX];       // Define x pot
IHM_EXTRN int Ihm_PotSelect;                     // Pot select
IHM_EXTRN int Ihm_PotVal;
IHM_EXTRN uint8_t Ihm_ui8_PotSelectAll;           // Pot select All

extern void Ihm_Set_Label(uint8_t num,char *txt);
extern void Ihm_Set_PotDescription();
extern void Ihm_Init_Pot(Ihm_Pot* pot);
extern void Ihm_Draw_Pot(Ihm_Pot* pot);
extern void Ihm_Draw_Grid();
extern void Ihm_Draw_Pad();
extern void Ihm_Select_Pot(Ihm_Pot* pot);

extern void Ihm_Draw_PadStep(uint8_t step);
extern void Ihm_Draw_PadSelect(uint8_t sel);
extern void Ihm_Draw_PadOn(uint8_t sel);
extern void Ihm_Draw_PadOff(uint8_t sel);

extern void Ihm_Draw_Reverse();

/*****************************************************************************************************/
/* GRID                                                                                              */
/*                                                                                                   */
/*****************************************************************************************************/
#define IHM_GRIDX   15
#define IHM_GRIDY   15

/*****************************************************************************************************/
/* PAD                                                                                              */
/*                                                                                                   */
/*****************************************************************************************************/
#define IHM_PADS        25


/*****************************************************************************************************/
/* OTHER                                                                                             */
/*                                                                                                   */
/*****************************************************************************************************/
extern void Ihm_Test_Dev();
