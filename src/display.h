#ifndef _DISPLAY_H
#define _DISPLAY_H 1
#include "epd_driver.h"
#include "epd_highlevel.h"
#include "opensans24.h"
#include "font_std.h"
#include "font_title.h"

extern EpdiyHighlevelState hl;
extern uint8_t* fb;
extern int temperature;

#define BLACK 0
#define WHITE 14

const EpdFontProperties rightAlignedStyle = {
    0,   //fg
    7,   //bg
    '?', // fallback
    EPD_DRAW_ALIGN_RIGHT};

const EpdFontProperties leftAlignedStyle = {
    0,   //fg
    7,   //bg
    '?', // fallback
    EPD_DRAW_ALIGN_LEFT};

const EpdFontProperties centeredStyle = {
    0,   //fg
    7,   //bg
    '?', // fallback
    EPD_DRAW_ALIGN_CENTER};

void drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *image);
void drawIcon(int x, int y, const char** img, int width, int height);
void write_string_with_outline(const EpdFont *font, const char *text, int *x, int *y, const EpdFontProperties *props);
void print_with_shadow(const char *text, int *x, int *y, const EpdFont *font, bool outline=true);

/*
class CompatDisplay {
  public:
    CompatDisplay() {
      this->_origX = this->x = 0;
      this->y = 50;
      this->outline = false;
      this->currentFont = OpenSans24;
      this->setTextSize(40);
    }
    void enableOutline(bool v) {
        this->outline = v;
    }
    void setCursor(int x, int y) {
      this->_origX = x;
      this->x = x;
      this->y = y;
    }
    void setFontType(int t) {
        switch(t) {
            case 1:
                this->currentFont = stdfont;
                break;
            case 2:
                this->currentFont = titlefont;
                break;
            default:
                this->currentFont = OpenSans24;
        }
    }
    void setTextSize(int size) {
      return;
      this->textSize = size;
    }
    void print(const char *text) {
      this->_print(text, false);
    }
    void _print(const char *text, bool updatelines) {
      int y = this->y;
      print_with_shadow(
          text,
          &this->x,
          &y,
          &this->currentFont,
          this->outline);
      if (updatelines) {
        this->x = this->_origX;
        this->y = y;
      }
    }
    void println(double val) {
      char text[10];
      sprintf(text, "%.1f", val);
      this->_print(text, true);
    }
    void print(double val) {
      char text[10];
      sprintf(text, "%.1f", val);
      this->_print(text, false);
    }
    void println(String text) {
      this->println(text.c_str());
    }
    void println(const char *text) {
      this->_print(text, true);
    }
    void update(EpdRect area) {
      epd_hl_update_area(&hl, MODE_GC16, temperature, area);
    }
    void update() {
      epd_hl_update_screen(&hl, MODE_GC16, temperature);
    }
  private:
    int x;
    int _origX;
    int y;
    bool outline;
    int textSize;
    EpdFont currentFont;
};

static CompatDisplay display;
*/
#endif