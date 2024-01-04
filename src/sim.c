#include <assert.h>
#include <string.h>
#include <dispatch/dispatch.h>
#include "Launchpad.h"
#include "sim.h"

u16 adc_buffer[PAD_COUNT];

u8 flash_storage[USER_AREA_SIZE];

typedef struct button button;
struct button {
  u8 value;
  u8 red;
  u8 green;
  u8 blue;
};

button buttons[BUTTONS_NUMBER];

void hal_plot_led(u8 type, u8 index, u8 red, u8 green, u8 blue) {
  assert((type == TYPESETUP) || (type == TYPEPAD));
  assert(index < 100);

  if (type == TYPESETUP) {
    index = SETUP_BUTTON_INDEX;
  } else if ((type == TYPEPAD) && (index == SETUP_BUTTON_INDEX)) {
    index = 0;
  }

  buttons[index].red = red & MAXLED;
  buttons[index].green = green & MAXLED;
  buttons[index].blue = blue & MAXLED;
}

void hal_read_led(u8 type, u8 index, u8* red, u8* green, u8* blue) {
  assert((type == TYPESETUP) || (type == TYPEPAD));
  assert(index < 100);

  if (type == TYPESETUP) {
    index = SETUP_BUTTON_INDEX;
  } else if ((type == TYPEPAD) && (index == SETUP_BUTTON_INDEX)) {
    index = 0;
  }

  *red = buttons[index].red;
  *green = buttons[index].green;
  *blue = buttons[index].blue;
}

void hal_send_midi(u8 port, u8 status, u8 data1, u8 data2) {

}

void hal_send_sysex(u8 port, const u8* data, u16 length) {

}

void hal_read_flash(u32 offset, u8 *data, u32 length) {
  memcpy(data, flash_storage + offset, length);
}

void hal_write_flash(u32 offset,const u8 *data, u32 length) {
  memcpy(flash_storage + offset, data, length);
}

u8 hal_read_device_id() {
  return 15;
}

u8 hal_read_layout_text() {
  return 0;
}

// initializes 1KHz timer to trigger Launchpad API timer event
//
// currently works on macos only, see
// https://developer.apple.com/library/archive/documentation/General/Conceptual/ConcurrencyProgrammingGuide/Introduction/Introduction.html
// https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Blocks/Articles/00_Introduction.html
void startTimer() {
  dispatch_queue_t queue = dispatch_queue_create("com.example.lppsim.timer",
                                                 DISPATCH_QUEUE_SERIAL);
  dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,
                                                   0, 0, queue);
  dispatch_source_set_event_handler(timer, ^{app_timer_event();});
  dispatch_time_t start = dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC / 100);
  dispatch_source_set_timer(timer, start, NSEC_PER_SEC / 1000, 0);
  dispatch_resume(timer);
}

void initSimulator() {
  startTimer();
  app_init(adc_buffer);
}

inline u8 buttonType(size_t index) {
  return (index == SETUP_BUTTON_INDEX) ? TYPESETUP : TYPEPAD;
}

void pressButton(size_t index) {
  app_surface_event(buttonType(index), index, 1);
}

void releaseButton(size_t index) {
  app_surface_event(buttonType(index), index, 0);
}

inline unsigned char normalizeColor(unsigned char value) {
  return (float) value / MAXLED * 255;
}

// returns button color in normal RGB range [0..255]
void getButtonColor(size_t index,
                    unsigned char* red, unsigned char* green, unsigned char* blue) {
  hal_read_led(buttonType(index), index, red, green, blue);
  *red = normalizeColor(*red);
  *green = normalizeColor(*green);
  *blue = normalizeColor(*blue);
}
