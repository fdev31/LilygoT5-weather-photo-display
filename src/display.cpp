#include <Arduino.h>
#include "display.h"

EpdiyHighlevelState hl;
uint8_t* fb;
int temperature = 25;

EpdFontProperties fontStyle = {
    (uint8_t)0, //fg
    7,         //bg
    '?',        // fallback
    (EpdFontFlags)0};

void drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *image) {
  EpdRect area = {.x=x, .y=y, .width=w, .height=h-1};
  epd_draw_rotated_transparent_image(area, image, fb, 64);
}

void drawIcon(int x, int y, const char** img, int width, int height) {
  EpdRect rect = {.x=x, .y=y, .width = width, .height = height};

  epd_copy_to_framebuffer(rect, (const uint8_t*) img, fb);
//  epd_draw_base(rect, (const uint8_t*) img, epd_full_screen(), MODE_GC16, temperature, NULL, EPD_BUILTIN_WAVEFORM);
}

void print_with_shadow(const char *text, int *x, int *y, EpdFont const *font, bool outline) {

  if (outline) {
    fontStyle.fg_color = WHITE;
    fontStyle.bg_color = 7;

    int xx = *x-2;
    int yy = *y-2;
    epd_write_string(font, text, &xx, &yy, fb, &fontStyle);
    xx = *x+2;
    yy = *y+2;
    epd_write_string(font, text, &xx, &yy, fb, &fontStyle);

    xx = *x + 2;
    yy = *y - 2;
    epd_write_string(font, text, &xx, &yy, fb, &fontStyle);

    xx = *x - 2;
    yy = *y + 2;
    epd_write_string(font, text, &xx, &yy, fb, &fontStyle);
  }

  fontStyle.fg_color = BLACK;
  Serial.println(text);
  Serial.print("before: ");
  Serial.print(*x);
  Serial.print(", ");
  Serial.println(*y);
  epd_write_string(font, text, x, y, fb, &fontStyle);
  Serial.print("after: ");
  Serial.print(*x);
  Serial.print(", ");
  Serial.println(*y);
}
