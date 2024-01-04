#include "ext.h"

// draws a color-filled circle
void DrawCircleCir(Circle cir, Color color) {
  DrawCircleV(cir.center, cir.radius, color);
}

// draws text in the center of the rectangle
void DrawTextCenterRec(const char *text, Rectangle rec, int fontSize,
                       Color color) {
  const int offsetX = rec.width / 2 - MeasureText(text, fontSize) / 2;
  const int offsetY = rec.height / 2 - fontSize / 2;
  DrawText(text, rec.x + offsetX, rec.y + offsetY, fontSize, color);
}

// draws text in the center of the circle
void DrawTextCenterCir(const char *text, Circle cir, int fontSize,
                       Color color) {
  const int offsetX = MeasureText(text, fontSize) / 2;
  const int offsetY = fontSize / 2;
  DrawText(text, cir.center.x - offsetX, cir.center.y - offsetY, fontSize,
           color);
}
