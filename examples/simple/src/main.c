#include <Launchpad.h>

void app_init(const u16 *adc_buffer) {

}

void app_timer_event() {
  static u16 counter;
  static u8 led;
  if (++counter >= 500) {
    led ^= MAXLED;
    hal_plot_led(TYPESETUP, 0, led, 0, 0);
    counter = 0;
  }
}

void app_midi_event(u8 port, u8 status, u8 d1, u8 d2) {

}

void app_sysex_event(u8 port, u8 * data, u16 count) {

}

void app_cable_event(u8 type, u8 value) {

}

void app_surface_event(u8 type, u8 index, u8 value) {
  u8 col = index % 10;
  u8 row = 9 - index / 10;
  if (col % 9 == 0 || row % 9 == 0) {
    return;
  }

  if (value == 0) {
    hal_plot_led(type, index, 0, 0, 0);
  } else {
    u8 c = (col - 1) * 9;
    switch(row) {
    case 1:
      hal_plot_led(type, index, c, 0, 0);
      break;
    case 2:
      hal_plot_led(type, index, 0, c, 0);
      break;
    case 3:
      hal_plot_led(type, index, 0, 0, c);
      break;
    case 4:
      hal_plot_led(type, index, c, c, 0);
      break;
    case 5:
      hal_plot_led(type, index, c, 0, c);
      break;
    case 6:
      hal_plot_led(type, index, 0, c, c);
      break;
    case 7:
      hal_plot_led(type, index, c, c, c);
      break;
    case 8:
      switch (col) {
      case 1:
        hal_plot_led(type, index, 255/4, 109/4, 194/4); // pink
        break;
      case 2:
        hal_plot_led(type, index, 102/4, 191/4, 255/4); // skyblue
        break;
      case 3:
        hal_plot_led(type, index, 255/4, 203/4, 0/4); // gold
        break;
      case 4:
        hal_plot_led(type, index, 135/4, 60/4, 190/4); // violet
        break;
      case 5:
        hal_plot_led(type, index, 0/4, 158/4, 47/4); // lime
        break;
      case 6:
        hal_plot_led(type, index, 127/4, 106/4, 79/4); // brown
        break;
      case 7:
        hal_plot_led(type, index, 190/4, 33/4, 55/4); // maroon
        break;
      case 8:
        hal_plot_led(type, index, 255/4, 161/4, 0/4); // orange
        break;
      }
      break;
    }
  }
}

void app_aftertouch_event(u8 index, u8 value) {

}
