#include <stdint.h>
#include <string.h>

#include "mmml_player.h"

#define VOLUME (25) // Volume from 0 to 28 (TIM3 period in `main.c`)

// note table (plus an initial 'wasted' entry for rests)
static const unsigned int note[13] =
    {
        // the rest command is technically note 0 and thus requires a frequency
        255,
        // one octave of notes, equal temperament in Gb
        1024, 967, 912, 861, 813, 767, 724, 683, 645, 609, 575, 542};

// location of individual samples in sample array
static const unsigned char sample_index[6] =
    {
        0, 19, 34, 74, 118, 126};

// raw PWM sample data
static const unsigned char sample[SAMPLE_LENGTH] =
    {
        // bwoop (0)
        0b10101010, 0b10110110, 0b10000111, 0b11111000,
        0b10000100, 0b00110111, 0b11101000, 0b11000001,
        0b00000111, 0b00111101, 0b11111000, 0b11100000,
        0b10010001, 0b10000111, 0b00000111, 0b00001111,
        0b00001111, 0b00011011, 0b00011110,
        // beep (19)
        0b10101010, 0b00101010, 0b00110011, 0b00110011,
        0b00110011, 0b00110011, 0b00110011, 0b11001101,
        0b11001100, 0b11001100, 0b11001100, 0b10101100,
        0b10011001, 0b00110001, 0b00110011,
        // kick (34)
        0b10010101, 0b10110010, 0b00000000, 0b11100011,
        0b11110000, 0b00000000, 0b11111111, 0b00000000,
        0b11111110, 0b00000000, 0b00000000, 0b00000000,
        0b11111111, 0b11111111, 0b11111111, 0b00100101,
        0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b11111111, 0b11110111, 0b11111111, 0b11111111,
        0b11111111, 0b10111111, 0b00010010, 0b00000000,
        0b10000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b11101110, 0b11111111, 0b11111111,
        0b11111111, 0b11110111, 0b11111111, 0b11111110,
        // snare (74)
        0b10011010, 0b10011010, 0b10101010, 0b10010110,
        0b01110100, 0b10010101, 0b10001010, 0b11011110,
        0b01110100, 0b10100000, 0b11110111, 0b00100101,
        0b01110100, 0b01101000, 0b11111111, 0b01011011,
        0b01000001, 0b10000000, 0b11010100, 0b11111101,
        0b11011110, 0b00010010, 0b00000100, 0b00100100,
        0b11101101, 0b11111011, 0b01011011, 0b00100101,
        0b00000100, 0b10010001, 0b01101010, 0b11011111,
        0b01110111, 0b00010101, 0b00000010, 0b00100010,
        0b11010101, 0b01111010, 0b11101111, 0b10110110,
        0b00100100, 0b10000100, 0b10100100, 0b11011010,
        // hi-hat (118)
        0b10011010, 0b01110100, 0b11010100, 0b00110011,
        0b00110011, 0b11101000, 0b11101000, 0b01010101,
        0b01010101,
        // end (26)
};

void mmml_player_init(mmml_player_t *player, unsigned char const * const track)
{
    memset(player, 0, sizeof(mmml_player_t));
    // sampler variables
    player->current_byte = 0;
    player->current_bit = 0;
    player->sample_counter = 0;
    player->current_sample = 0;

    // temporary data storage variables
    player->buffer1 = 0;
    player->buffer2 = 0;
    player->buffer3 = 0;
    player->buffer4 = 0;

    // main timer variables
    player->tick_counter = 0;
    player->tick_speed = 1024; // default tempo
    player->track = track;

    /* Set each channel's data pointer to that channel's data location in the core data array.
	 * Initialise each channel's frequencies. By default they're set to zero which causes out of
	 * tune notes (due to timing errors) until every channel is assigned frequency data.
	 * Additionally, default values are set should no volume or octave be specified */

    for (unsigned char i = 0; i < CHANNELS; i++)
    {
        player->data_pointer[i] = player->track[(i * 2)] << 8;
        player->data_pointer[i] = player->data_pointer[i] | player->track[(i * 2) + 1];
        player->frequency[i] = 255; // random frequency (won't ever be sounded)
        player->volume[i] = 1;      // default volume : 50% pulse wave
        player->octave[i] = 3;      // default octave : o3
    }
}

uint8_t mmml_player_gen_samples(mmml_player_t *player, uint8_t *buffer, uint16_t num_samples)
{
    uint8_t res = 0;
    const unsigned char *data = player->track;

    for (uint16_t k = 0; k < num_samples;)
    {
        // sampler (channel D) code
        if (player->sample_counter-- == 0)
        {
            if (player->current_byte < player->current_sample - 1)
            {
                // read individual bits from the sample array
                player->out[3] = ((sample[player->current_byte] >> player->current_bit++) & 1);
            }
            else
                // silence the channel when the sample is over
                player->out[3] = 0;

            // move to the next byte on bit pointer overflow
            if (player->current_bit > 7)
            {
                player->current_byte++;
                player->current_bit = 0;
            }
            player->sample_counter = SAMPLE_SPEED;
        }

        // calculate pulse values
        for (unsigned char chan = 0; chan < CHANNELS - 1; chan++)
        {
            player->pitch_counter[chan] += player->octave[chan];
            if (player->pitch_counter[chan] >= player->frequency[chan])
                player->pitch_counter[chan] = player->pitch_counter[chan] - player->frequency[chan];
            if (player->pitch_counter[chan] <= player->waveform[chan])
                player->out[chan] = 1;
            if (player->pitch_counter[chan] >= player->waveform[chan])
                player->out[chan] = 0;
        }

        // output and interleave samples using PIM
        for (unsigned char chan = 0; chan < CHANNELS; chan++)
        {
            buffer[k++] = player->out[chan] ? VOLUME : 0;
        }

        if (player->tick_counter-- == 0)
        {
            // Variable tempo, sets the fastest / smallest possible clock event.
            player->tick_counter = player->tick_speed;

            for (unsigned char chan = 0; chan < CHANNELS; chan++)
            {
                // If the note ended, start processing the next byte of data.
                if (player->length[chan] == 0)
                {
                LOOP:

                    // Temporary storage of data for quick processing.
                    // first nibble of data
                    player->buffer1 = ((unsigned char)data[(player->data_pointer[chan])] >> 4);
                    // second nibble of data
                    player->buffer2 = (unsigned char)data[player->data_pointer[chan]] & 0x0F;

                    // function command
                    if (player->buffer1 == 15)
                    {
                        // Another buffer for commands that require an additional byte.
                        player->buffer3 = (unsigned char)data[player->data_pointer[chan] + 1];

                        // loop start
                        if (player->buffer2 == 0)
                        {
                            player->loops_active[chan]++;
                            player->loop_point[player->loops_active[chan] - 1][chan] = player->data_pointer[chan] + 2;
                            player->loop_duration[player->loops_active[chan] - 1][chan] = player->buffer3 - 1;
                            player->data_pointer[chan] += 2;
                        }
                        // loop end
                        else if (player->buffer2 == 1)
                        {
                            /* If we reach the end of the loop and the duration isn't zero,
							 * decrement our counter and start again. */
                            if (player->loop_duration[player->loops_active[chan] - 1][chan] > 0)
                            {
                                player->data_pointer[chan] = player->loop_point[player->loops_active[chan] - 1][chan];
                                player->loop_duration[player->loops_active[chan] - 1][chan]--;
                            }
                            // If we're done, move away from the loop.
                            else
                            {
                                player->loops_active[chan]--;
                                player->data_pointer[chan]++;
                            }
                        }
                        // macro
                        else if (player->buffer2 == 2)
                        {
                            player->pointer_location[chan] = player->data_pointer[chan] + 2;

                            player->data_pointer[chan] = data[(player->buffer3 + CHANNELS) << 1] << 8;
                            player->data_pointer[chan] = player->data_pointer[chan] | data[((player->buffer3 + CHANNELS) * 2) + 1];
                        }
                        // tempo
                        else if (player->buffer2 == 3)
                        {
                            player->tick_speed = player->buffer3 << 4;
                            player->data_pointer[chan] += 2;
                        }
                        // relative volume decrease
                        else if (player->buffer2 == 4)
                        {
                            if (player->volume[chan] < 8)
                                player->volume[chan]++;
                            player->data_pointer[chan]++;
                        }
                        // relative volume increase
                        else if (player->buffer2 == 5)
                        {
                            if (player->volume[chan] > 1)
                                player->volume[chan]--;
                            player->data_pointer[chan]++;
                        }
                        // channel end
                        else if (player->buffer2 == 15)
                        {
                            // If we've got a previous position saved, go to it...
                            if (player->pointer_location[chan] != 0)
                            {
                                player->data_pointer[chan] = player->pointer_location[chan];
                                player->pointer_location[chan] = 0;
                            }
                            // ...If not, go back to the start.
                            else
                            {
                                player->data_pointer[chan] = data[(chan * 2)] << 8;
                                player->data_pointer[chan] = player->data_pointer[chan] | data[(chan * 2) + 1];
                                res = 1;
                                goto END;
                            }
                        }

                        /* For any command that should happen 'instantaneously' (e.g. anything
						 * that isn't a note or rest - in mmml notes can't be altered once
						 * they've started playing), we need to keep checking this loop until we
						 * find an event that requires waiting. */

                        goto LOOP;
                    }

                    if (player->buffer1 == 13 || player->buffer1 == 14)
                    {
                        // octave
                        if (player->buffer1 == 13)
                            player->octave[chan] = 1 << player->buffer2;
                        // volume
                        if (player->buffer1 == 14)
                            player->volume[chan] = player->buffer2;

                        player->data_pointer[chan]++;
                        goto LOOP; //see comment above previous GOTO
                    }

                    // note value
                    if (player->buffer1 != 0 && player->buffer1 < 14)
                    {
                        if (chan < 3)
                        {
                            player->buffer4 = note[player->buffer1];
                            player->frequency[chan] = player->buffer4;

                            /* Calculate the waveform duty cycle by dividing the frequency by
							 * powers of two. */
                            player->waveform[chan] = (player->buffer4 >> player->volume[chan]) + 1;
                        }
                        else
                        {
                            // reset the sampler
                            player->current_bit = 0;
                            player->current_byte = sample_index[player->buffer1 - 1];
                            player->current_sample = sample_index[player->buffer1];
                        }
                    }
                    // rest
                    else
                        player->waveform[chan] = 0;

                    // note duration value
                    if (player->buffer2 < 8)
                        // standard duration
                        player->length[chan] = 127 >> player->buffer2;
                    else
                        // dotted (1 + 1/2) duration
                        player->length[chan] = 95 >> (player->buffer2 & 7);

                    // next element in data
                    player->data_pointer[chan]++;
                }
                // keep waiting until the note is over...
                else
                    player->length[chan]--;
            }
        }
    }

    END:
    return res;
}