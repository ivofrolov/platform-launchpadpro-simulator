#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "ext.h"
#include "sim.h"

enum {PAD, BTN, STP, NIL} controls[BUTTONS_NUMBER];

// assigns control types (pad, button) to their corresponding indicies
// as defined in Launchpad API
void initControls() {
  for (size_t i = 0; i < BUTTONS_NUMBER; ++i) {
    if ((i == 0) || (i == 9) || (i == 99)) {
      controls[i] = NIL;
    } else if (i == SETUP_BUTTON_INDEX) {
      controls[i] = STP;
    } else if (((i / 10) % 9 == 0) || ((i % 10) % 9 == 0)) {
      controls[i] = BTN;
    } else {
      controls[i] = PAD;
    }
  }
}

void initUI() {
  initControls();
}

// just numbers to express device proportions
#define SCREEN_SIZE 624
#define LP_BORDER_SIZE 12
#define LP_CELLS_NUMBER 10
#define LP_CELL_SIZE 60
#define LP_PAD_SIZE 50
#define LP_BTN_SIZE 38
#define LP_STP_SIZE 20

// returns controls scale based on screen dimensions
inline float scaleFactor() {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();
  const float frameSize = (screenWidth < screenHeight)
                          ? screenWidth : screenHeight;
  return SCREEN_SIZE / frameSize;
}

Rectangle panelRec() {
  const float scale = scaleFactor();
  const float panelOffset = LP_BORDER_SIZE * scale;
  const float panelSize = LP_CELL_SIZE * LP_CELLS_NUMBER * scale;
  Rectangle panel = {panelOffset, panelOffset, panelSize, panelSize};
  return panel;
}

Rectangle padRec(Rectangle panel, size_t col, size_t row) {
  const float scale = scaleFactor();
  const float cellSize = LP_CELL_SIZE * scale;
  const float padSize = LP_PAD_SIZE * scale;
  const float gap = (cellSize - padSize) / 2;
  const float x = col * cellSize + gap + panel.x;
  const float y = row * cellSize + gap + panel.y;
  Rectangle pad = {x, y, padSize, padSize};
  return pad;
}

Circle btnCir(Rectangle panel, size_t col, size_t row) {
  const float scale = scaleFactor();
  const float cellSize = LP_CELL_SIZE * scale;
  const float x = cellSize * (col + 0.5) + panel.x;
  const float y = cellSize * (row + 0.5) + panel.x;
  const float radius = (LP_BTN_SIZE * scale) / 2;
  Circle btn = {{x, y}, radius};
  return btn;
}

Circle stpCir(Rectangle panel, size_t col, size_t row) {
  const float scale = scaleFactor();
  Circle stp = btnCir(panel, col, row);
  stp.radius = (LP_STP_SIZE * scale) / 2;
  return stp;
}

// should return color as if semi transparent pad would be illuminated
// by LED, but currently uses naive approach to determine the color
Color controlColor(size_t index) {
  unsigned char r, g, b;
  getButtonColor(index, &r, &g, &b);
  unsigned char c = g > b ? g : b;
  c = r > c ? r : c;
  float intensity = c / 255.0;
  Color color = RAYWHITE;
  color.r = color.r * (1 - intensity) + r * intensity;
  color.g = color.g * (1 - intensity) + g * intensity;
  color.b = color.b * (1 - intensity) + b * intensity;
  return color;
}

void drawControls() {
  // frame
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);

  // panel
  const Rectangle panel = panelRec();
  DrawRectangleRec(panel, DARKGRAY);

  // setup led
  DrawRectangle(panel.x + panel.width / 2 - LP_BORDER_SIZE / 2,
                panel.y + panel.height,
                LP_BORDER_SIZE, LP_BORDER_SIZE,
                controlColor(SETUP_BUTTON_INDEX));

  // controls
  const size_t fontSize = panel.width / LP_CELLS_NUMBER / 6;
  for (size_t i = 0; i < 100; ++i) {
    const size_t col = i % 10;
    const size_t row = 9 - i / 10;

    // button index according to specification in Launchpad.h
    char text[2 + 1];
    snprintf(text, sizeof text, "%.2d", (int) i);

    switch(controls[i]) {
      Rectangle pad;
      Circle btn;
    // pads in the center
    case PAD:
      pad = padRec(panel, col, row);
      DrawRectangleRec(pad, controlColor(i));
      DrawTextCenterRec(text, pad, fontSize, BLACK);
      break;
    // buttons on the edges
    case BTN:
      btn = btnCir(panel, col, row);
      DrawCircleCir(btn, controlColor(i));
      DrawTextCenterCir(text, btn, fontSize, BLACK);
      break;
    // setup button at the top left corner
    case STP:
      DrawCircleCir(stpCir(panel, col, row), BLACK);
      break;
    // nothing at other corners
    case NIL:
      break;
    }
  }
}

void drawUI() {
  ClearBackground(WHITE);
  drawControls();
}

size_t locateControlUnderCursor() {
  const Vector2 cursor = GetMousePosition();
  const Rectangle panel = panelRec();
  if (CheckCollisionPointRec(cursor, panel)) {
    const size_t col = (cursor.x - panel.x) / (panel.width / LP_CELLS_NUMBER);
    const size_t row = (cursor.y - panel.y) / (panel.width / LP_CELLS_NUMBER);
    const size_t control = col + (9 - row) * 10;
    switch(controls[control]) {
      Circle cir;
    case PAD:
      if (CheckCollisionPointRec(cursor, padRec(panel, col, row))) {
        return control;
      }
      break;
    case BTN:
      cir = btnCir(panel, col, row);
      if (CheckCollisionPointCircle(cursor, cir.center, cir.radius)) {
        return control;
      }
      break;
    case STP:
      cir = stpCir(panel, col, row);
      if (CheckCollisionPointCircle(cursor, cir.center, cir.radius)) {
        return control;
      }
      break;
    case NIL:
      break;
    }
  }

  return 0;
}

void processUserInput() {
  if (!IsCursorOnScreen()) {
    return;
  }

  static size_t holdedControl;
  size_t control = locateControlUnderCursor();

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    if (control != holdedControl) {
      if (holdedControl > 0) {
        releaseButton(holdedControl);
      }
      if (control > 0) {
        pressButton(control);
      }
      holdedControl = control;
    }
  } else {
    if (holdedControl > 0) {
      releaseButton(holdedControl);
      holdedControl = 0;
    }
  }
}

int main(void) {
  InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Launchpad Pro Simulator");
  SetTargetFPS(30);

  initUI();
  initSimulator();

  while (!WindowShouldClose()) {
    processUserInput();

    BeginDrawing();
    drawUI();
    EndDrawing();
  }

  haltSimulator();

  CloseWindow();
  return EXIT_SUCCESS;
}
