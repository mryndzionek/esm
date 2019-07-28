#include "esm/hesm.h"
#include "esm/esm_timer.h"

#include "pitches.h"
#include "keycodes.h"
#include "board.h"

ESM_THIS_FILE;

typedef struct
{
} player_cfg_t;

typedef struct
{
    const char *p;
    uint8_t default_dur;
    uint8_t default_oct;
    int bpm;
    long wholenote;
} track_t;

typedef struct
{
    hesm_t esm;
    track_t curr_track;
    uint8_t i;
    esm_timer_t timer;
    player_cfg_t const *const cfg;
} player_esm_t;

ESM_COMPLEX_STATE(base, top, 1);
ESM_LEAF_STATE(off, base, 2);
ESM_LEAF_STATE(playing, base, 2);
ESM_LEAF_STATE(wait, base, 2);

static const char Tone1[] = "DrNo:d=4,o=5,b=112:2b4,2c,2c#,2c,2b4,2c,2c#,2c,2p,8e,16f#,16f#,8f#,f#,8e,8e,"
                            "8e,8e,16g,16g,8g,g,8f#,8f#,8e,8e,16f#,16f#,8f#,f#,8e,8e,8e,8e,16g,16g,8g,g,8"
                            "f#,8f#,8e,8d#6,8d6,2d6,8b,8a,2b,8e,8g,8p,8d#6,d.6,8g,8a#,2b.,8g,16a,16g,f#.,"
                            "16b4,8e,2c#.,p,8e,8g,8p,8d#6,d.6,8g,8a#,2b.,8g,16a,16g,f#.,16b4,8d#,2e.";

static const char Tone2[] = "PowerRang:d=4,o=6,b=180:e5,e5,8d5,e5,g5,2e.5,p,e.,b.,a.,g.,f_,e5,e5,"
                            "8d5,e5,g5,2e.5,p,e.,b.,a.,g.,f_,f5,f5,8f5,f5,f5,2f.5,f5,8f5,f5,f5,f5,"
                            "f5,f5,g5,f.5,2e5";

static const char Tone3[] = "we-rock:d=4,o=6,b=355:16b5,16d6,16b5,16d6,16b5,16d6,16b5,16d6,16d6,16f6,16d6,"
                            "16f6,16d6,16f6,16d6,16f6,16f6,16a6,16f6,16a6,16f6,16a6,16f6,16a6";

static const char Tone4[] = "KnightRider:d=4,o=5,b=63:16e,32f,32e,8b,16e6,32f6,32e6,8b,16e,32f,32e,16b,16e6,"
                            "d6,8p,16e,32f,32e,8b,16e6,32f6,32e6,8b,16e,32f,32e,16b,16e6,f6";

static const char Tone5[] = "Xfiles:d=4,o=5,b=125:e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,g6,f#6,e6,d6,e6,2b.,1p,g6,"
                            "f#6,e6,d6,f#6,2b.,1p,e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,e6,2b.";

static const char Tone6[] = "smackdown:d=8,o=5,b=200:d#,a#.,d#,a#.,d#,p,16p,d#,a#.,d#,a#.,d#,p,16p,d#,f#.,d#,p,16p,"
                            "d#,a#.,d#,a#.,d#,p,16p,d#,a#.,d#,a#.,d#,p,16p,d#,f#.,d#,p,16p";

static const char Tone8[] = "SweetDreams:d=16,o=6,b=45:32p,c#,c#,g#,c#,a,c#,g#,c#,a5,a5,e,f#,g#5,g#5,d#,e,c#,c#,g#,c#,a,c#,g#,"
                            "c#,a5,a5,e,f#,g#5,g#5,d#,e";

static const char Tone9[] = "NokiaTun:d=4,o=5,b=225:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a";

static const char Tone10[] = "Dizzie:d=4,o=6,b=225:32c5,32d5,32e5,32f5,32g5,32a5,32b5,32g5,32f5,32e5,32d5,32c5,32d5,32e5,32f5,"
                             "32g5,32a5,32b5,32a5,32g5,32e5,32d5,32c5,32d5,32e5,32f5,32g5,32a5,32b5,32a5,32g5,32f5,32e5,32d5,"
                             "32e5,32f5,32d5,32e5,32f5";

static const char Tone11[] = "DieSonne:d=16,o=5,b=160:4d,8g#,8d,8g#,8g,8d,8p,4d,8g#,8d,8g#,4g.,8p,4d,8g#,8d,8g#,8g,8d,8p,4d,8g#,"
                             "8d,8g#,4g.,8p,4d,8g#,8d,8g#,8g,8d,8p,4d,8g#,8d,8g#,4g.,8p";

static const char Tone12[] = "Mutter:d=16,o=5,b=140:d#,2e.,8f#,8g,2b.,8b,8a,8b,8a,1f#,4f#,4g,8f#.,d#,2e.,8f#,8g,2b.,8b,8a,8b,8a,1f#,"
                             "4f#,4g,4f#,2e,2p";

static const char Tone13[] = "commando:d=4,o=5,b=125:8a.,16a,8a,8a,a,8a,8a,8a.,16e6,8e6,8e6,e6,p,f6,e6,f6,e6,8e6,16b,16b,8b,8b,b,p,8c.6,"
                             "16c6,8c6,8c6,c6,8c6,8c6,8c.6,16g6,8g6,8g6,g6,p,g#6,g6,g#6,g6,8g6,16d6,16d6,8d6,8d6,d6";

static const char Tone14[] = "mkombat:d=4,o=5,b=70:16a#,16a#,16c#6,16a#,16d#6,16a#,16f6,16d#6,16c#6,16c#6,16f6,16c#6,16g#6,16c#6,16f6,16c#6,"
                             "16g#,16g#,16c6,16g#,16c#6,16g#,16d#6,16c#6,16f#,16f#,16a#,16f#,16c#6,16f#,16c#6,16c6";

static const char Tone15[] = "Robocop:d=4,o=5,b=140:8f,8g#,8f,g,a#.,32p,8f,8g#,8f,2c#,8p,8f,8g#,8f,g,a#,2d#.,p,8f,8g#,8f,g,a#.,32p,8a#,8c#6,8a#,"
                             "2f6,8p,8f6,8g#6,8f6,c7,g#6,2a#.6,8p,16a#6,16a#6,2c.7";

static const char Tone16[] = "Halloween:d=4,o=5,b=180:32p,8d6,8g,8g,8d6,8g,8g,8d6,8g,8d#6,8g,8d6,8g,8g,8d6,8g,8g,8d6,8g,8d#6,"
                             "8g,8c#6,8f#,8f#,8c#6,8f#,8f#,8c#6,8f#,8d6,8f#,8c#6,8f#,8f#,8c#6,8f#,8f#,8c#6,8f#,8d6,8f#";

static char const *Tones[] = {Tone1, Tone2, Tone3, Tone4, Tone5, Tone6, Tone8,
                                Tone9, Tone10, Tone11, Tone12, Tone13, Tone14, Tone15, Tone16};

#define NUM_TONES (sizeof(Tones) / sizeof(Tones[0]))

static const uint16_t notes[] =
    {0,        //
     NOTE_C4,  //
     NOTE_CS4, //
     NOTE_D4,  //
     NOTE_DS4, //
     NOTE_E4,  //
     NOTE_F4,  //
     NOTE_FS4, //
     NOTE_G4,  //
     NOTE_GS4, //
     NOTE_A4,  //
     NOTE_AS4, //
     NOTE_B4,  //

     NOTE_C5,  //
     NOTE_CS5, //
     NOTE_D5,  //
     NOTE_DS5, //
     NOTE_E5,  //
     NOTE_F5,  //
     NOTE_FS5, //
     NOTE_G5,  //
     NOTE_GS5, //
     NOTE_A5,  //
     NOTE_AS5, //
     NOTE_B5,  //

     NOTE_C6,  //
     NOTE_CS6, //
     NOTE_D6,  //
     NOTE_DS6, //
     NOTE_E6,  //
     NOTE_F6,  //
     NOTE_FS6, //
     NOTE_G6,  //
     NOTE_GS6, //
     NOTE_A6,  //
     NOTE_AS6, //
     NOTE_B6,  //

     NOTE_C7,  //
     NOTE_CS7, //
     NOTE_D7,  //
     NOTE_DS7, //
     NOTE_E7,  //
     NOTE_F7,  //
     NOTE_FS7, //
     NOTE_G7,  //
     NOTE_GS7, //
     NOTE_A7,  //
     NOTE_AS7, //
     NOTE_B7,  //

     2 * NOTE_C7,  //
     2 * NOTE_CS7, //
     2 * NOTE_D7,  //
     2 * NOTE_DS7, //
     2 * NOTE_E7,  //
     2 * NOTE_F7,  //
     2 * NOTE_FS7, //
     2 * NOTE_G7,  //
     2 * NOTE_GS7, //
     2 * NOTE_A7,  //
     2 * NOTE_AS7, //
     2 * NOTE_B7,  //
     0};

#define isdigit(_c) ((_c >= '0') && (_c <= '9'))

static void _init(const char *pp, track_t *t)
{
    int num;

    t->default_dur = 4;
    t->default_oct = 6;
    t->bpm = 63;
    t->p = pp;

    // skip name/header
    while (*t->p != ':')
        t->p++;
    t->p++;

    // get default duration
    if (*t->p == 'd')
    {
        t->p++;
        t->p++; // skip "d="
        num = 0;
        while (isdigit(*t->p))
        {
            num = (num * 10) + (*t->p - '0');
            t->p++;
        }
        if (num > 0)
            t->default_dur = num;
        t->p++; // skip comma
    }

    // get default octave
    if (*t->p == 'o')
    {
        t->p++;
        t->p++; // skip "o="
        num = *t->p - '0';
        t->p++;
        if (num >= 3 && num <= 7)
            t->default_oct = num;
        t->p++; // skip comma
    }

    // get BPM
    if (*t->p == 'b')
    {
        t->p++;
        t->p++; // skip "b="
        num = 0;
        while (isdigit(*t->p))
        {
            num = (num * 10) + (*t->p - '0');
            t->p++;
        }
        t->bpm = num;
        t->p++; // skip colon
    }

    // BPM usually expresses the number of quarter notes per minute
    t->wholenote = (60 * 1000L / t->bpm) * 4; // this is the time for whole note (in milliseconds)
}

static void _play(esm_t *const esm, uint8_t octave_offset)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    track_t *t = &self->curr_track;
    // Absolutely no error checking in here

    int num;
    long duration;
    uint8_t note;
    uint8_t scale;

    // first, get note duration, if available
    num = 0;
    while (isdigit(*t->p))
    {

        num = (num * 10) + (*t->p - '0');
        t->p++;
    }

    if (num)
        duration = t->wholenote / num;
    else
        duration = t->wholenote / t->default_dur; // we will need to check if we are a dotted note after

    // now get the note
    note = 0;

    switch (*t->p)
    {
    case 'c':
        note = 1;
        break;
    case 'd':
        note = 3;
        break;
    case 'e':
        note = 5;
        break;
    case 'f':
        note = 6;
        break;
    case 'g':
        note = 8;
        break;
    case 'a':
        note = 10;
        break;
    case 'b':
        note = 12;
        break;
    case 'p':
    default:
        note = 0;
    }
    t->p++;

    // now, get optional '#' sharp
    if (*t->p == '#')
    {
        note++;
        t->p++;
    }

    // now, get optional '.' dotted note
    if (*t->p == '.')
    {
        duration += duration / 2;
        t->p++;
    }

    // now, get scale
    if (isdigit(*t->p))
    {
        scale = *t->p - '0';
        t->p++;
    }
    else
    {
        scale = t->default_oct;
    }

    scale += octave_offset;

    if (*t->p == ',')
        t->p++; // skip comma for next note (or we may be at the end)

    if (note)
    {
        uint16_t n = (notes[(scale - 4) * 12 + note]);

        BOARD_START_SOUND(n, duration);

        esm_signal_t sig = {
            .type = esm_sig_tmout,
            .sender = esm,
            .receiver = esm};
        esm_timer_add(&self->timer,
                      duration, &sig);
    }
    else
    {
        esm_signal_t sig = {
            .type = esm_sig_tmout,
            .sender = esm,
            .receiver = esm};
        esm_timer_add(&self->timer,
                      duration, &sig);
    }
}

static void esm_base_init(esm_t *const esm)
{
    (void)esm;
	ESM_TRANSITION(off);
}

static void esm_base_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_base_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_base_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    
    switch (sig->type)
    {
    case esm_sig_remote:
    {
        switch (sig->params.keycode)
        {
        case KEYCODE_PLAY:
        {
            esm_signal_t s = {
                .type = esm_sig_play,
                .sender = NULL,
                .receiver = esm};
            esm_send_signal(&s);
        }
        break;

        case KEYCODE_NEXT:
        {
            self->i = self->i == (int)(NUM_TONES - 1) ? 0 : self->i + 1; 
            _init(Tones[self->i], &self->curr_track);
        }
        break;

        case KEYCODE_PREV:
        {
            self->i = self->i == 0 ? (uint8_t)(NUM_TONES - 1) : self->i - 1;
            _init(Tones[self->i], &self->curr_track);
        }
        break;
        }
    }
    break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_off_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_off_exit(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    self->i = ESM_RANDOM(NUM_TONES);
}

static void esm_off_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    switch (sig->type)
    {
    case esm_sig_play:
        ESM_TRANSITION(playing);
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_playing_entry(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);

    _init(Tones[self->i], &self->curr_track);
    _play(esm, 0);
}

static void esm_playing_exit(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    esm_timer_rm(&self->timer);
    BOARD_STOP_SOUND();
}

static void esm_playing_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_tmout:
    {
        BOARD_STOP_SOUND();
        if (*self->curr_track.p)
        {
            _play(esm, 0);
        }
        else
        {
            ESM_TRANSITION(wait);
        }
    }
    break;

    case esm_sig_play:
        ESM_TRANSITION(off);
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_wait_entry(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);

    esm_signal_t sig = {
        .type = esm_sig_tmout,
        .sender = esm,
        .receiver = esm};
    esm_timer_add(&self->timer,
                  1000UL, &sig);
}

static void esm_wait_exit(esm_t *const esm)
{
    player_esm_t *self = ESM_CONTAINER_OF(esm, player_esm_t, esm);
    esm_timer_rm(&self->timer);
}

static void esm_wait_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    (void)esm;

    switch (sig->type)
    {
    case esm_sig_tmout:
    {
        ESM_TRANSITION(playing);
    }
    break;

    case esm_sig_play:
        ESM_TRANSITION(off);
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_player_init(esm_t *const esm)
{
    ESM_TRANSITION(off);
}

static const player_cfg_t player1_cfg = {};

ESM_COMPLEX_REGISTER(player, player1, esm_gr_remote, 2, 3, 0);