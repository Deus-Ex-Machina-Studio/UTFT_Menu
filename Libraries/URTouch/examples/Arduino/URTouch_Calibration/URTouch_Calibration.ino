// URTouch_Calibration
// Авторское право (C)2015 Rinky-Dink Electronics, Henning Karlsen. Все права защищены
// web: http://www.RinkyDinkElectronics.com/
//
// Эта программа предназначена для калибровки сенсорных экранов.
// Для этой программы требуется не только библиотека URTouch, но и библиотека UTFT
// Модуль с сенсорным экранаом должен поддерживаться библиотекой UTFT.
//
// Предполагается, что Вы знаете, как изменить номера выводов дисплея и сенсорного экрана.
//
// Следуйте инструкциям которые будут отображаться на дисплее.

#include <UTFT.h>
#include <URTouch.h>

// Определяем ориентацию сенсорного экрана. Дополнительную информацию можно найти в инструкциях.
   #define TOUCH_ORIENTATION  PORTRAIT  // Портретная  (вертикальная)   ориентация
// #define TOUCH_ORIENTATION  LANDSCAPE // Ландшафтная (горизонтальная) ориентация

// Инициализация дисплея:
// ----------------------
// UTFT myGLCD(TFT395UNO,  A2,A1,A3,A4,A0);            // тип дисплея 3,95 UNO  (480x320 chip ILI9327)
// UTFT myGLCD(TFT32MEGA,  38,39,40,41   );            // тип дисплея 3,2  MEGA (480x320 chip ILI9481)
// UTFT myGLCD(TFT28UNO,   A2,A1,A3,A4,A0);            // тип дисплея 2,8  UNO  (320x240 chip ILI9341)
   UTFT myGLCD(TFT01_24SP,  5, 4, 8, 7, 6);            // тип дисплея 2,4  TFT  (240×320 chip ILI9341)
// UTFT myGLCD(TFT01_22SP,  5, 4, 8, 7, 6);            // тип дисплея 2,2  TFT  (320×240 chip ILI9340)
// UTFT myGLCD(TFT18SHLD,   5, 4, 8, 7, 6);            // тип дисплея 1,8  TFT  (128x160 chip ST7735S)
// UTFT myGLCD(ТИП_МОДЕЛИ, 19,18,17,16   );            // тип дисплея шилд UNO      - подходит для большинства стандартных дисплеев
// UTFT myGLCD(ТИП_МОДЕЛИ, 38,39,40,41   );            // тип дисплея шилд Mega/Due - подходит для большинства стандартных дисплеев
// UTFT myGLCD(ТИП_МОДЕЛИ, 25,26,27,28   );            // тип дисплея шилд Due      - CTE TFT LCD/SD
// UTFT myGLCD(ТИП_МОДЕЛИ, 22,23,31,33   );            // тип дисплея шилд Due      - ElecHouse TFT LCD/SD
// UTFT myGLCD(ТИП_МОДЕЛИ, 23,22, 3, 4   );            // тип дисплея               - Teensy 3.x TFT Test Board

// Инициализация touchscreen:
// --------------------------
   URTouch  myTouch(13,12,11,10, 9);                   // дисплей 2,4 TFT 240×320
// URTouch  myTouch(15,10,14, 9, 8);                   // подходит для большинства стандартных шилдов под Arduino Uno/2009
// URTouch  myTouch( 6, 5, 4, 3, 2);                   // подходит для большинства стандартных шилдов под Arduino Mega/Due
// URTouch  myTouch( 6, 5, 4, 3, 2);                   // шилд CTE TFT LCD/SD под Arduino Due
// URTouch  myTouch(26,31,27,28,29);                   // Teensy 3.x TFT Test Board
// URTouch  myTouch(25,26,27,29,30);                   // шилд ElecHouse TFT LCD/SD под Arduino Due

// ***************************************
// НЕ ИЗМЕНЯЙТЕ КОД СКЕТЧА НИЖЕ ЭТОЙ ЛИНИИ
// ***************************************

// Объявление используемых шрифтов
extern uint8_t SmallFont[];

uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center;
uint32_t calx, caly, cals;
char buf[13];

void setup()
{
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);

  myTouch.InitTouch(TOUCH_ORIENTATION);
  dispx=myGLCD.getDisplayXSize();
  dispy=myGLCD.getDisplayYSize();
  text_y_center=(dispy/2)-6;
}

void drawCrossHair(int x, int y)
{
  myGLCD.drawRect(x-10, y-10, x+10, y+10);
  myGLCD.drawLine(x-5, y, x+5, y);
  myGLCD.drawLine(x, y-5, x, y+5);
}

void readCoordinates()
{
  int iter = 5000;
  int failcount = 0;
  int cnt = 0;
  uint32_t tx=0;
  uint32_t ty=0;
  boolean OK = false;
  
  while (OK == false)
  {
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("*  PRESS  *", CENTER, text_y_center);
    while (myTouch.dataAvailable() == false) {}
    myGLCD.print("*  HOLD!  *", CENTER, text_y_center);
    while ((myTouch.dataAvailable() == true) && (cnt<iter) && (failcount<10000))
    {
      myTouch.calibrateRead();
      if (!((myTouch.TP_X==65535) || (myTouch.TP_Y==65535)))
      {
        tx += myTouch.TP_X;
        ty += myTouch.TP_Y;
        cnt++;
      }
      else
        failcount++;
    }
    if (cnt>=iter)
    {
      OK = true;
    }
    else
    {
      tx = 0;
      ty = 0;
      cnt = 0;
    }
    if (failcount>=10000)
      fail();
  }

  cx = tx / iter;
  cy = ty / iter;
}

void calibrate(int x, int y, int i)
{
  myGLCD.setColor(255, 255, 255);
  drawCrossHair(x,y);
  myGLCD.setBackColor(255, 0, 0);
  readCoordinates();
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("* RELEASE *", CENTER, text_y_center);
  myGLCD.setColor(80, 80, 80);
  drawCrossHair(x,y);
  rx[i]=cx;
  ry[i]=cy;
  while (myTouch.dataAvailable() == true) {}
}

void waitForTouch()
{
  while (myTouch.dataAvailable() == true) {}
  while (myTouch.dataAvailable() == false) {}
  while (myTouch.dataAvailable() == true) {}
}

void toHex(uint32_t num)
{
  buf[0] = '0';
  buf[1] = 'x';
  buf[10] = 'U';
  buf[11] = 'L';
  buf[12] = 0;
  for (int zz=9; zz>1; zz--)
  {
    if ((num & 0xF) > 9)
      buf[zz] = (num & 0xF) + 55;
    else
      buf[zz] = (num & 0xF) + 48;
    num=num>>4;
  }
}

void startup()
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, dispx-1, 13);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.drawLine(0, 14, dispx-1, 14);
  myGLCD.print("URTouch Calibration", CENTER, 1);
  myGLCD.setBackColor(0, 0, 0);

  if (dispx==220)
  {  
    myGLCD.print("Use a stylus or something", LEFT, 30);
    myGLCD.print("similar to touch as close", LEFT, 42);
    myGLCD.print("to the center of the", LEFT, 54);
    myGLCD.print("highlighted crosshair as", LEFT, 66);
    myGLCD.print("possible. Keep as still as", LEFT, 78);
    myGLCD.print("possible and keep holding", LEFT, 90);
    myGLCD.print("until the highlight is", LEFT, 102);
    myGLCD.print("removed. Repeat for all", LEFT, 114);
    myGLCD.print("crosshairs in sequence.", LEFT, 126);
    myGLCD.print("Touch screen to continue", CENTER, 162);
  }
  else
  {
    myGLCD.print("INSTRUCTIONS", CENTER, 30);
    myGLCD.print("Use a stylus or something similar to", LEFT, 50);
    myGLCD.print("touch as close to the center of the", LEFT, 62);
    myGLCD.print("highlighted crosshair as possible. Keep", LEFT, 74);
    myGLCD.print("as still as possible and keep holding", LEFT, 86);
    myGLCD.print("until the highlight is removed. Repeat", LEFT, 98);
    myGLCD.print("for all crosshairs in sequence.", LEFT, 110);

    myGLCD.print("Further instructions will be displayed", LEFT, 134);
    myGLCD.print("when the calibration is complete.", LEFT, 146);

    myGLCD.print("Do NOT use your finger as a calibration", LEFT, 170);
    myGLCD.print("stylus or the result WILL BE imprecise.", LEFT, 182);

    myGLCD.print("Touch screen to continue", CENTER, 226);
  }

  waitForTouch();
  myGLCD.clrScr();
}

void done()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, dispx-1, 13);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.drawLine(0, 14, dispx-1, 14);
  myGLCD.print("URTouch Calibration", CENTER, 1);
  myGLCD.setBackColor(0, 0, 0);
  
  if (dispx==220)
  {  
    myGLCD.print("To use the new calibration", LEFT, 30);
    myGLCD.print("settings you must edit the", LEFT, 42);
    myGLCD.setColor(160, 160, 255);
    myGLCD.print("URTouchCD.h", LEFT, 54);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("file and change", 88, 54);
    myGLCD.print("the following values. The", LEFT, 66);
    myGLCD.print("values are located right", LEFT, 78);
    myGLCD.print("below the opening comment.", LEFT, 90);
    myGLCD.print("CAL_X", LEFT, 110);
    myGLCD.print("CAL_Y", LEFT, 122);
    myGLCD.print("CAL_S", LEFT, 134);
    toHex(calx);
    myGLCD.print(buf, 75, 110);
    toHex(caly);
    myGLCD.print(buf, 75, 122);
    toHex(cals);
    myGLCD.print(buf, 75, 134);
  }
  else
  {  
    myGLCD.print("CALIBRATION COMPLETE", CENTER, 30);
    myGLCD.print("To use the new calibration", LEFT, 50);
    myGLCD.print("settings you must edit the", LEFT, 62);
    myGLCD.setColor(160, 160, 255);
    myGLCD.print("URTouchCD.h", LEFT, 74);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("file and change", 88, 74);
    myGLCD.print("the following values.", LEFT, 86);
    myGLCD.print("The values are located right", LEFT, 98);
    myGLCD.print("below the opening comment in", LEFT, 110);
    myGLCD.print("the file.", LEFT, 122);
    myGLCD.print("CAL_X", LEFT, 150);
    myGLCD.print("CAL_Y", LEFT, 162);
    myGLCD.print("CAL_S", LEFT, 174);

    toHex(calx);
    myGLCD.print(buf, 75, 150);
    toHex(caly);
    myGLCD.print(buf, 75, 162);
    toHex(cals);
    myGLCD.print(buf, 75, 174);
  }
  
}

void fail()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, dispx-1, 13);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.drawLine(0, 14, dispx-1, 14);
  myGLCD.print("URTouch Calibration FAILED", CENTER, 1);
  myGLCD.setBackColor(0, 0, 0);
  
  myGLCD.print("Unable to read the position", LEFT, 30);
  myGLCD.print("of the press. This is a", LEFT, 42);
  myGLCD.print("hardware issue and can", 88, 54);
  myGLCD.print("not be corrected in", LEFT, 66);
  myGLCD.print("software.", LEFT, 78);
  
  while(true) {};
}

void loop()
{
  startup();
  
  myGLCD.setColor(80, 80, 80);
  drawCrossHair(dispx-11, 10);
  drawCrossHair(dispx/2, 10);
  drawCrossHair(10, 10);
  drawCrossHair(dispx-11, dispy/2);
  drawCrossHair(10, dispy/2);
  drawCrossHair(dispx-11, dispy-11);
  drawCrossHair(dispx/2, dispy-11);
  drawCrossHair(10, dispy-11);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("***********", CENTER, text_y_center-12);
  myGLCD.print("***********", CENTER, text_y_center+12);

  calibrate(10, 10, 0);
  calibrate(10, dispy/2, 1);
  calibrate(10, dispy-11, 2);
  calibrate(dispx/2, 10, 3);
  calibrate(dispx/2, dispy-11, 4);
  calibrate(dispx-11, 10, 5);
  calibrate(dispx-11, dispy/2, 6);
  calibrate(dispx-11, dispy-11, 7);
  
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals=(long(dispx-1)<<12)+(dispy-1);
  else
    cals=(long(dispy-1)<<12)+(dispx-1);

  if (TOUCH_ORIENTATION == PORTRAIT)
    px = abs(((float(rx[2]+rx[4]+rx[7])/3)-(float(rx[0]+rx[3]+rx[5])/3))/(dispy-20));  // PORTRAIT
  else
    px = abs(((float(rx[5]+rx[6]+rx[7])/3)-(float(rx[0]+rx[1]+rx[2])/3))/(dispy-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    clx = (((rx[0]+rx[3]+rx[5])/3));  // PORTRAIT
    crx = (((rx[2]+rx[4]+rx[7])/3));  // PORTRAIT
  }
  else
  {
    clx = (((rx[0]+rx[1]+rx[2])/3));  // LANDSCAPE
    crx = (((rx[5]+rx[6]+rx[7])/3));  // LANDSCAPE
  }
  if (clx<crx)
  {
    clx = clx - (px*10);
    crx = crx + (px*10);
  }
  else
  {
    clx = clx + (px*10);
    crx = crx - (px*10);
  }
  
  if (TOUCH_ORIENTATION == PORTRAIT)
    py = abs(((float(ry[5]+ry[6]+ry[7])/3)-(float(ry[0]+ry[1]+ry[2])/3))/(dispx-20));  // PORTRAIT
  else
    py = abs(((float(ry[0]+ry[3]+ry[5])/3)-(float(ry[2]+ry[4]+ry[7])/3))/(dispx-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    cty = (((ry[5]+ry[6]+ry[7])/3));  // PORTRAIT
    cby = (((ry[0]+ry[1]+ry[2])/3));  // PORTRAIT
  }
  else
  {
    cty = (((ry[0]+ry[3]+ry[5])/3));  // LANDSCAPE
    cby = (((ry[2]+ry[4]+ry[7])/3));  // LANDSCAPE
  }
  if (cty<cby)
  {
    cty = cty - (py*10);
    cby = cby + (py*10);
  }
  else
  {
    cty = cty + (py*10);
    cby = cby - (py*10);
  }
  
  calx = (long(clx)<<14) + long(crx);
  caly = (long(cty)<<14) + long(cby);
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals = cals + (1L<<31);

  done();
  while(true) {}
}
