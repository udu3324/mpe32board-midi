#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "tusb.h"
#include "helper.h"
#include "tmag-helper.h"


#define MAX_MPE_CHANNELS 15
#define BASE_MPE_CHANNEL 1 // Channels 1 to 15 for notes
#define MPE_GLOBAL_CHANNEL 0 // Channel 0 is the global/master channel

typedef struct {
	uint8_t channel;
	uint8_t note;
	bool active;
} MPE_NoteSlot;

MPE_NoteSlot mpe_slots[MAX_MPE_CHANNELS] = { 0 };

static bool mpe_initialized = false;

uint8_t midi_key_code_map[25]; //midi codes
int physical_board_map_index[25] = { 
	16, 19, 23, 22, 8, 11, 15, 14, 
0, 3, 7, 6, 17, 18, 20, 21, 
9, 10, 12, 13, 1, 2, 4, 5, 0};

//for each d, get where that number (d) is in the array, and save its position as x
//that x can be put in midi_key_code_map[x + 1] to get its actual midi code

void fill_midi_key_arr(int size, int start_octave) {
	int midi_note = 12 + (start_octave * 12);
	for (int i = 0; i < size; i++) {
		//map array that maps the physical io's markings to what midi code they should be
		midi_key_code_map[physical_board_map_index[i]] = midi_note; //todo only supports 25 keys atm, does not go beyond
		// Step to next note in chromatic order
		// C, C#, D, D#, E, F, F#, G, G#, A, A#, B
		// Intervals: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
		midi_note++;
	}
}

bool key_is_on[25];

float key_velocity[25];

uint32_t key_tick_start[25]; //velocity timings for calculation

float key_rot_offset[25]; //pitch bend data

int16_t cache_key_pitch_shift[25];
uint8_t cache_key_aftertouch[25];

//internal - Initialize MPE mode
void MPE_Init(void) {
	if (mpe_initialized) return;
	
	debugtalk("Initializing MPE mode...\r\n");
	
	// Wait a bit to ensure USB connection is stable
	// Note: This should be called after USB is ready
	
	// Send MPE Configuration Message (MCM) to configure MPE Zone
	// RPN 6 (MPE Configuration Message) on global channel (channel 0)
	
	// RPN MSB (101): 0
	uint8_t rpn_msb[3] = { 0xB0 | MPE_GLOBAL_CHANNEL, 101, 0 };
	tud_midi_stream_write(0, rpn_msb, 3);
	
	// RPN LSB (100): 6 (MPE Configuration)
	uint8_t rpn_lsb[3] = { 0xB0 | MPE_GLOBAL_CHANNEL, 100, 6 };
	tud_midi_stream_write(0, rpn_lsb, 3);
	
	// Data Entry MSB (6): Number of member channels (15)
	uint8_t data_msb[3] = { 0xB0 | MPE_GLOBAL_CHANNEL, 6, MAX_MPE_CHANNELS };
	tud_midi_stream_write(0, data_msb, 3);
	
	// Data Entry LSB (38): 0 (not used for MPE)
	uint8_t data_lsb[3] = { 0xB0 | MPE_GLOBAL_CHANNEL, 38, 0 };
	tud_midi_stream_write(0, data_lsb, 3);
	
	// Reset RPN to null
	uint8_t rpn_null_msb[3] = { 0xB0 | MPE_GLOBAL_CHANNEL, 101, 127 };
	tud_midi_stream_write(0, rpn_null_msb, 3);
	
	uint8_t rpn_null_lsb[3] = { 0xB0 | MPE_GLOBAL_CHANNEL, 100, 127 };
	tud_midi_stream_write(0, rpn_null_lsb, 3);
	
	printf("MPE initialization complete\r\n");	
	mpe_initialized = true;
}

//internal
int allocate_mpe_channel(uint8_t note) {
	for (int i = 0; i < MAX_MPE_CHANNELS; i++) {
		if (!mpe_slots[i].active) {
			mpe_slots[i].channel = BASE_MPE_CHANNEL + i;
			mpe_slots[i].note = note;
			mpe_slots[i].active = true;
			snprintf(buffer, sizeof(buffer), "Allocated MPE channel %d for note %d\r\n", mpe_slots[i].channel, note);
            debugtalk(buffer);
			return mpe_slots[i].channel;
		}
	}

	// no free channels
	printf("Warning: No free MPE channels for note %d\r\n", note);
	return -1;
}

//internal
int release_mpe_channel(uint8_t note) {
	for (int i = 0; i < MAX_MPE_CHANNELS; i++) {
		if (mpe_slots[i].active && mpe_slots[i].note == note) {
			int channel = mpe_slots[i].channel;
			mpe_slots[i].active = false;
			snprintf(buffer, sizeof(buffer), "Released MPE channel %d for note %d\r\n", channel, note);
            debugtalk(buffer);
			return channel;
		}
	}

	snprintf(buffer, sizeof(buffer), "Warning: Could not find active MPE channel for note %d\r\n", note);
    debugtalk(buffer);
	return -1;
}

void MPE_Send_Note_On(uint8_t note, uint8_t velocity) {
	// Ensure MPE is initialized
	if (!mpe_initialized) {
		MPE_Init();
	}
	
	int channel = allocate_mpe_channel(note);
	if (channel < 0)
		return; // polyphony limit reached

	// Reset channel state BEFORE the note-on (per MPE, new note = fresh expression)
    uint8_t pb[3] = { 0xE0 | (channel & 0x0F), 0x00, 0x40 }; // center = 8192
    tud_midi_stream_write(0, pb, 3);
    uint8_t at[2] = { 0xD0 | (channel & 0x0F), 0 };
    tud_midi_stream_write(0, at, 2);

	uint8_t msg[3] = { 0x90 | (channel & 0x0F), note, velocity };
	tud_midi_stream_write(0, msg, 3);
}

void MPE_Send_Note_Off(uint8_t note) {
	int channel = release_mpe_channel(note);
	if (channel < 0)
		return; // note not found

	uint8_t msg[3] = { 0x80 | (channel & 0x0F), note, 0 };
	tud_midi_stream_write(0, msg, 3);
}

// bend_val: -8192 to +8191, do not use the max as it would be about 90 semitones
void MPE_Send_Pitch_Bend(uint8_t note, int16_t bend_val) {
	uint16_t bend = bend_val + 8192; // convert to unsigned

	for (int i = 0; i < MAX_MPE_CHANNELS; i++) {
		if (mpe_slots[i].active && mpe_slots[i].note == note) {
			uint8_t channel = mpe_slots[i].channel;
			uint8_t msg[3] = { 0xE0 | (channel & 0x0F), bend & 0x7F, (bend >> 7)
					& 0x7F };
			tud_midi_stream_write(0, msg, 3);
			
			//snprintf(buffer, sizeof(buffer), "Pitch bend: note=%d, channel=%d, bend_val=%d, bend=%d\r\n", note, channel, bend_val, bend);
            //debugtalk(buffer);
			return;
		}
	}
	
	snprintf(buffer, sizeof(buffer), "Warning: Pitch bend for note %d not found in active slots\r\n", note);
    debugtalk(buffer);
}

//0-127 pressure
void MPE_Send_Aftertouch(uint8_t note, uint8_t pressure) {
	for (int i = 0; i < MAX_MPE_CHANNELS; i++) {
		if (mpe_slots[i].active && mpe_slots[i].note == note) {
			uint8_t channel = mpe_slots[i].channel;
			uint8_t msg[2] = { 0xD0 | (channel & 0x0F), pressure };
			tud_midi_stream_write(0, msg, 2);

			//snprintf(buffer, sizeof(buffer), "Aftertouch: note=%d, channel=%d, pressure=%d\r\n", note, channel, pressure);
            //debugtalk(buffer);
			return;
		}
	}

	snprintf(buffer, sizeof(buffer), "Warning: Aftertouch for note %d not found in active slots\r\n", note);
    debugtalk(buffer);
}

void midi_parse() {
    for (int d = 0; d < 25; d++) {
        // skip sensors that are not plugged in
        if (!enabled_tmags[d]) {
          continue;
        }

        float dpos = tmag_pos[d] - tmag_pos_init[d];
        float drot = tmag_rot[d] - tmag_rot_init[d];

        //snprintf(buffer, sizeof(buffer), "i=%d p=%.2f r=%.2f\r\n", d, dpos, drot);
        //debugtalk(buffer);

        // trigger point |<--
        if ((dpos > 8) && !key_is_on[d]) {
            uint8_t measured_velocity = map_velocity_log(HAL_GetTick() - key_tick_start[d]);

            key_is_on[d] = true;
            key_rot_offset[d] = drot;
            key_velocity[d] = measured_velocity;
			cache_key_aftertouch[d] = 0;
			cache_key_pitch_shift[d] = 0;
            MPE_Send_Note_On(midi_key_code_map[d], measured_velocity);
        }
        
        // release point |-->
        if ((dpos < 2) && key_is_on[d]) {
            key_is_on[d] = false;
            MPE_Send_Note_Off(midi_key_code_map[d]);
        }

        // velocity tracking start |<<<
        if ((dpos > 1.5) && key_tick_start[d] == 0) {
            key_tick_start[d] = HAL_GetTick();
        } else if ((dpos < 0.8) && key_tick_start[d] != 0) {
            key_tick_start[d] = 0;
        }

        // pitch bend per key <--|-->
        if (key_is_on[d]) {
            // the reason for this is due to drift when the key is translated up/down
            float relative_rot = drot - key_rot_offset[d];
            int16_t shift = 0;

            // left bend <--|--
            if (relative_rot < -15) {
                shift = map_float_to_int16(relative_rot, -40.0, -15, -682, 0);
            }

            // right bend --|-->
            if (relative_rot > 15) {
                shift = map_float_to_int16(relative_rot, 15, 40.0, 0, 682);
            }

            if (cache_key_pitch_shift[d] != shift) {
                cache_key_pitch_shift[d] = shift;
                MPE_Send_Pitch_Bend(midi_key_code_map[d], shift);
            }
        }

        // aftertouch per key >>>|<<<
        if (key_is_on[d]) {
            uint8_t pressure = 0;

            if (dpos > (8 + 2)) {
                pressure = map_float_to_uint8(dpos, (8 + 2), 60, 0, 127); // + key_velocity[d]
            }

            if (cache_key_aftertouch[d] != pressure) {
                cache_key_aftertouch[d] = pressure;
                MPE_Send_Aftertouch(midi_key_code_map[d], pressure);
            }
        }
    }
}