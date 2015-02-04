/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */


#include "display.h"

TFT_22_ILI9225 _tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);

// ___________________________________________________________________
Display::Display(uint8_t orientation, int maxAmmo, int maxHealth) {
  _orientation = orientation;
  _maxAmmo = maxAmmo;
  _maxHealth = maxHealth;
  _ammo = 0;
  _health = 0;
}

// ___________________________________________________________________
void Display::bootUp() {
  
  _tft.begin();
  _tft.setOrientation(_orientation);
  
  int length = _tft.maxX() - 2*RECT_SIDE_OFFSET - 4;
  _fracAmmo = (float)((float)length / (float)_maxAmmo);
  _fracHealth = (float)((float)length / (float)_maxHealth);
  
  
  // Draw Health rect
  _tft.drawRectangle(RECT_SIDE_OFFSET, HEALTH_RECT_OFFSET, _tft.maxX() - RECT_SIDE_OFFSET, HEALTH_RECT_OFFSET + HEALTH_RECT_HEIGTH, RECT_COLOR);
  // Draw Ammo rect
  _tft.drawRectangle(RECT_SIDE_OFFSET, AMMO_RECT_OFFSET, _tft.maxX() - RECT_SIDE_OFFSET, AMMO_RECT_OFFSET + AMMO_RECT_HEIGTH, RECT_COLOR);
  // Draw Info Rect
  _tft.drawRectangle(0 + RECT_SIDE_OFFSET, 0 + INFO_RECT_OFFSET, _tft.maxX() - RECT_SIDE_OFFSET, INFO_RECT_OFFSET + INFO_RECT_HEIGTH, RECT_COLOR);
  
  // Draw Seperator
  
  //_tft.drawLine(SEPERATOR_X_OFFSET, SEPERATOR_Y_OFFSET, _tft.maxX() - SEPERATOR_X_OFFSET, SEPERATOR_Y_OFFSET - 1, SEPERATOR_COLOR);
  _tft.drawRectangle(SEPERATOR_X_OFFSET, SEPERATOR_Y_OFFSET, _tft.maxX() - SEPERATOR_X_OFFSET, SEPERATOR_Y_OFFSET - 1, SEPERATOR_COLOR);
  
  // Draw Ammo Headline
  _tft.setFont(HEADLINE_FONT);
  _tft.drawText(AMMO_TEXT_X_OFFSET, AMMO_TEXT_Y_OFFSET, AMMO_TEXT, HEADLINE_TEXT_COLOR);
  _tft.drawText(HEALTH_TEXT_X_OFFSET, HEALTH_TEXT_Y_OFFSET, HEALTH_TEXT, HEADLINE_TEXT_COLOR);
  
}

// ___________________________________________________________________
void Display::drawPlayerName(/*String*/char * playerName) {
  _tft.setFont(PLAYER_FONT);
  _tft.drawText(PLAYER_TEXT_X_OFFSET, PLAYER_TEXT_Y_OFFSET, playerName, PLAYER_TEXT_COLOR);
}

// ___________________________________________________________________
void Display::drawPoints(int points) {
  char _points[2];
   sprintf (_points, "%03i", points);
  _tft.setFont(POINTS_FONT);
  _tft.drawText(POINTS_TEXT_X_OFFSET, POINTS_TEXT_Y_OFFSET, _points, POINTS_TEXT_COLOR);
}


// ___________________________________________________________________
void Display::redraw(/*String*/char * playerName, int points, int ammo, int health) {
  bootUp();
  drawPlayerName(playerName);
  drawPoints(points);
  updateAmmo(ammo);
  updateHealth(health);
}

// ___________________________________________________________________
void Display::updateAmmo(int ammo) {
  //int ammoLength = (int)((float)(_fracAmmo*(ammo - _ammo)));
   // If more ammo draw RED else BLACK
  if (ammo > _ammo) {
    _tft.fillRectangle(RECT_SIDE_OFFSET + 2 + _fracAmmo*_ammo, AMMO_RECT_OFFSET + 2, RECT_SIDE_OFFSET + 2 + _fracAmmo*ammo, AMMO_RECT_OFFSET + AMMO_RECT_HEIGTH - 2, AMMO_RECT_COLOR);
  } else {
    _tft.fillRectangle(RECT_SIDE_OFFSET + 2 + _fracAmmo*ammo, AMMO_RECT_OFFSET + 2, RECT_SIDE_OFFSET + 2 + _fracAmmo*_ammo, AMMO_RECT_OFFSET + AMMO_RECT_HEIGTH - 2, BACKGROUND_COLOR);
  }
  _ammo = ammo;
}

// ___________________________________________________________________
void Display::updateHealth(int health) {
  
  if (health > _health) {
    _tft.fillRectangle(RECT_SIDE_OFFSET + 2 + _fracHealth*_health, HEALTH_RECT_OFFSET + 2, RECT_SIDE_OFFSET + 2 + _fracHealth*health, HEALTH_RECT_OFFSET + HEALTH_RECT_HEIGTH - 2, HEALTH_RECT_COLOR);
  } else {
    _tft.fillRectangle(RECT_SIDE_OFFSET + 2 + _fracHealth*health, HEALTH_RECT_OFFSET + 2, RECT_SIDE_OFFSET + 2 + _fracHealth*_health , HEALTH_RECT_OFFSET + HEALTH_RECT_HEIGTH - 2, BACKGROUND_COLOR);
  }
  _health = health;
}

// ___________________________________________________________________
bool Display::updateInfo(char * infoText) { 
  _tft.fillRectangle(RECT_SIDE_OFFSET + 2, INFO_RECT_OFFSET + 2, _tft.maxX() - RECT_SIDE_OFFSET - 2, INFO_RECT_OFFSET + INFO_RECT_HEIGTH - 2, BACKGROUND_COLOR);
 
  _tft.setFont(INFO_FONT);
  _tft.drawText(INFO_TEXT_X_OFFSET, INFO_TEXT_Y_OFFSET, infoText, INFO_TEXT_COLOR);
}
/*
// ___________________________________________________________________
bool Display::updateInfo(String string) { 
  _tft.fillRectangle(RECT_SIDE_OFFSET + 2, INFO_RECT_OFFSET + 2, _tft.maxX() - RECT_SIDE_OFFSET - 2, INFO_RECT_OFFSET + INFO_RECT_HEIGTH - 2, BACKGROUND_COLOR);
 
  _tft.setFont(INFO_FONT);
  _tft.drawText(INFO_TEXT_X_OFFSET, INFO_TEXT_Y_OFFSET, string, INFO_TEXT_COLOR);
}
*/
// ___________________________________________________________________
bool Display::updateInfo(Hit hit) {
  _tft.fillRectangle(RECT_SIDE_OFFSET + 2, INFO_RECT_OFFSET + 2, _tft.maxX() - RECT_SIDE_OFFSET - 2, INFO_RECT_OFFSET + INFO_RECT_HEIGTH - 2, BACKGROUND_COLOR);
  char numbHit[4]; 
  sprintf (numbHit, "%03i", hit.code);
  
 
  char myBigArray[30];
  myBigArray[0] = '\0';
  strcat(myBigArray, "Hit by: ");
  strcat(myBigArray, numbHit);
  strcat(myBigArray, " at ");
  if (hit.position == 100) {
    strcat(myBigArray, HIT_POS_100);
  } else if (hit.position == 0) {
    strcat(myBigArray, HIT_POS_0);
  } else if (hit.position == 1) {
    strcat(myBigArray, HIT_POS_1);
  } else if (hit.position == 2) {
    strcat(myBigArray, HIT_POS_2);
  } else if (hit.position == 3) {
    strcat(myBigArray, HIT_POS_3);
  }


  _tft.setFont(INFO_FONT);
  _tft.drawText(INFO_TEXT_X_OFFSET, INFO_TEXT_Y_OFFSET, myBigArray, INFO_TEXT_COLOR);
}

// ___________________________________________________________________
bool Display::updateInfo(char * playerName, uint8_t position) { 
  _tft.fillRectangle(RECT_SIDE_OFFSET + 2, INFO_RECT_OFFSET + 2, _tft.maxX() - RECT_SIDE_OFFSET - 2, INFO_RECT_OFFSET + INFO_RECT_HEIGTH - 2, BACKGROUND_COLOR);
 
 char myBigArray[30];
  myBigArray[0] = '\0';
  strcat(myBigArray, "Hit by: ");
  strcat(myBigArray, playerName);
  strcat(myBigArray, " at ");
  if (position == 100) {
    strcat(myBigArray, HIT_POS_100);
  } else if (position == 0) {
    strcat(myBigArray, HIT_POS_0);
  } else if (position == 1) {
    strcat(myBigArray, HIT_POS_1);
  } else if (position == 2) {
    strcat(myBigArray, HIT_POS_2);
  } else if (position == 3) {
    strcat(myBigArray, HIT_POS_3);
  }
  _tft.setFont(INFO_FONT);
  _tft.drawText(INFO_TEXT_X_OFFSET, INFO_TEXT_Y_OFFSET, myBigArray, INFO_TEXT_COLOR);
}
