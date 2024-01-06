#ifndef SIM_H
#define SIM_H

#define BUTTONS_NUMBER 100
#define SETUP_BUTTON_INDEX 90

// triggers Launchpad init event
void initSimulator();

// frees resources
void haltSimulator();

// triggers Launchpad surface event
void pressButton(size_t index);
void releaseButton(size_t index);

// returns button color in normal RGB range [0..255]
void getButtonColor(size_t index,
                    unsigned char* red, unsigned char* green, unsigned char* blue);

#endif
