#ifndef MIDI_H
#define MIDI_H

//void MPE_Init(void);

extern int octave;
extern float key_trigger_d;
extern float key_vtrack_d;
extern float key_release_d;
extern float key_rot_deadzone_d;

void MPE_Send_Note_On(uint8_t note, uint8_t velocity);

void MPE_Send_Note_Off(uint8_t note);

void MPE_Send_Pitch_Bend(uint8_t note, int16_t bend_val);

void MPE_Send_Aftertouch(uint8_t note, uint8_t pressure);

void midi_parse();

void fill_midi_key_arr(int size, int start_octave);


#endif