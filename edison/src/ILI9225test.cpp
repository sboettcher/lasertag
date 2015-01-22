// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <string>

#include "./TFT_22_ILI9225.h"

#define TFT_LED_PIN 31
#define TFT_RST_PIN 32
#define TFT_RS_PIN 33

int running = 1;
uint16_t x, y;
bool flag = false;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nclosing nicely\n");
    running = 0;
  }
}

int main(int argc, char** argv) {
  //signal(SIGINT, sig_handler);

  printf("Constructor... ");
  fflush(stdout);
  TFT_22_ILI9225 tft(TFT_LED_PIN, TFT_RST_PIN, TFT_RS_PIN);
  printf("Done.\nbegin()... ");
  fflush(stdout);
  tft.begin();
  printf("Done.\n");
  fflush(stdout);


  tft.setFont(Terminal6x8);
  tft.drawText(10, 10, "kjsdfklfkjwlkfelksdjf");
  tft.drawText(10, tft.maxY()/2, "kjsdfklfkjwlkfelksdjf");
  tft.drawText(10, tft.maxY()-10, "kjsdfklfkjwlkfelksdjf");

  for (int i = 0; i*tft.fontY() < tft.maxY(); i++) {
    tft.drawText(tft.maxX()-10, i*tft.fontY(), "|");
  }

  return 0;



  tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
  tft.setFont(Terminal6x8);
  tft.drawText(10, 10, "hello!");
  usleep(1000000);
  
  tft.clear();
  tft.drawText(10, 20, "clear");
  usleep(1000000);

  tft.drawText(10, 30, "text small");
  tft.setBackgroundColor(COLOR_YELLOW);
  tft.setFont(Terminal12x16);
  tft.drawText(90, 30, "BIG", COLOR_RED);
  tft.setBackgroundColor(COLOR_BLACK);
  tft.setFont(Terminal6x8);
  usleep(1000000);

  tft.drawText(10, 40, "setBacklight off");
  usleep(500000);
  tft.setBacklight(false);
  usleep(500000);
  tft.setBacklight(true);
  tft.drawText(10, 50, "setBacklight on");
  usleep(1000000);

  tft.drawRectangle(10, 10, 110, 110, COLOR_BLUE);
  tft.drawText(10, 60, "rectangle");
  usleep(1000000);

  tft.fillRectangle(20, 20, 120, 120, COLOR_RED);
  tft.drawText(10, 70, "solidRectangle");
  usleep(1000000);

  tft.drawCircle(80, 80, 50, COLOR_YELLOW);
  tft.drawText(10, 80, "circle");
  usleep(1000000);

  tft.fillCircle(90, 90, 30, COLOR_GREEN);
  tft.drawText(10, 90, "solidCircle");
  usleep(1000000);

  tft.drawLine(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_CYAN);
  tft.drawText(10, 100, "line");
  usleep(1000000);

  for (uint8_t i = 0; i < 127; i++)
    tft.drawPixel(rand() % tft.maxX(), rand() % tft.maxY(), rand() % 0xffff);
  tft.drawText(10, 110, "point");
  usleep(1000000);

  for (uint8_t i = 0; i < 4; i++) {
    tft.clear();
    tft.setOrientation(i);
    tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
    tft.drawText(10, 10, "setOrientation (" + std::string("0123").substr(i, i + 1) + ")");
    tft.drawRectangle(10, 20, 50, 60, COLOR_GREEN);
    tft.drawCircle(70, 80, 10, COLOR_BLUE);
    tft.drawLine(30, 40, 70, 80, COLOR_YELLOW);
    usleep(1000000);
  }
  
  tft.setOrientation(0);
  tft.clear();
  tft.setFont(Terminal12x16);
  tft.setBackgroundColor(COLOR_YELLOW);
  tft.drawText(10, 40, "bye!", COLOR_RED);
  tft.setBackgroundColor(COLOR_BLACK);
  tft.setFont(Terminal6x8);
  usleep(1000000);
  
  tft.drawText(10, 60, "off");
  usleep(1000000);
  
  tft.setBacklight(false);
  tft.setDisplay(false);

  return 0;
}
