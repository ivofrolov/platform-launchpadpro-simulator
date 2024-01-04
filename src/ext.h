#ifndef EXT_H
#define EXT_H

#include "raylib.h"

typedef struct Circle Circle;
struct Circle {
  Vector2 center;
  float radius;
};

// draws a color-filled circle
void DrawCircleCir(Circle cir, Color color);

// draws text in the center of the rectangle
void DrawTextCenterRec(const char *text, Rectangle rec, int fontSize, Color color);

// draws text in the center of the circle
void DrawTextCenterCir(const char *text, Circle cir, int fontSize, Color color);

#endif
