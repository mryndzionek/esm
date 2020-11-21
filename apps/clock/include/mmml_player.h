#ifndef _MMML_PLAYER_H_
#define _MMML_PLAYER_H_

// stuff you shouldn't really mess with
#define CHANNELS (4)        // the number of channels
#define SAMPLE_SPEED (5)    // the sampler playback rate
#define SAMPLE_LENGTH (127) // the length of the sample array
#define MAXLOOPS (5)        // the maximum number of nested loops

typedef struct mmml_player
{
    unsigned char const *track;
    // channel variables (grouped in arrays)
    unsigned char out[CHANNELS],
        octave[CHANNELS],
        length[CHANNELS],
        volume[CHANNELS],
        loops_active[CHANNELS],
        current_length[CHANNELS];

    unsigned int data_pointer[CHANNELS],
        waveform[CHANNELS],
        pitch_counter[CHANNELS],
        frequency[CHANNELS],
        loop_duration[MAXLOOPS][CHANNELS],
        loop_point[MAXLOOPS][CHANNELS],
        pointer_location[CHANNELS];

    // sampler variables
    unsigned char current_byte,
        current_bit,
        sample_counter,
        current_sample;

    // temporary data storage variables
    unsigned char buffer1,
        buffer2,
        buffer3;
    unsigned int buffer4;

    // main timer variables
    unsigned int tick_counter,
        tick_speed; // default tempo

} mmml_player_t;

void mmml_player_init(mmml_player_t *player, unsigned char const * const track);
uint8_t mmml_player_gen_samples(mmml_player_t *player, uint8_t *buffer, uint16_t num_samples);

#endif // _MMML_PLAYER_H_