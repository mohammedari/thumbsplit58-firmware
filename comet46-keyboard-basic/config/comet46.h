#ifdef COMPILE_LEFT
    #define PIPE_NUMBER 0
    #define S01 18
    #define S02 17
    #define S03 16
    #define S04 15
    #define S05 14
    #define S06 19
    #define S07 20
    #define S08 12
    #define S09 11
    #define S10 10
    #define S11 8
    #define S12 4
    #define S13 26
    #define S14 7
    #define S15 6
    #define S16 5
    #define S17 0
    #define S18 27
    #define S19 23
    #define S20 3
    #define S21 2
    #define S22 30
    #define S23 28
    #define S24 24
    #define S25 21
    #define S26 29
    #define S27 25
    #define S28 22
    #define S29 9
    #define S30 13
    #define LED 1
#endif

#ifdef COMPILE_RIGHT
    #define PIPE_NUMBER 1
    #define S01 21
    #define S02 22
    #define S03 23
    #define S04 24
    #define S05 25
    #define S06 26
    #define S07 27
    #define S08 20
    #define S09 16
    #define S10 8
    #define S11 0
    #define S12 30
    #define S13 29
    #define S14 28
    #define S15 19
    #define S16 15
    #define S17 10
    #define S18 9
    #define S19 6
    #define S20 4
    #define S21 1
    #define S22 18
    #define S23 13
    #define S24 11
    #define S25 7
    #define S26 5
    #define S27 2
    #define S28 17
    #define S29 14
    #define S30 12
    #define LED 3
#endif

#define INPUT_MASK (1<<S01 | \
                    1<<S02 | \
                    1<<S03 | \
                    1<<S04 | \
                    1<<S05 | \
                    1<<S06 | \
                    1<<S07 | \
                    1<<S08 | \
                    1<<S09 | \
                    1<<S10 | \
                    1<<S11 | \
                    1<<S12 | \
                    1<<S13 | \
                    1<<S14 | \
                    1<<S15 | \
                    1<<S16 | \
                    1<<S17 | \
                    1<<S18 | \
                    1<<S19 | \
                    1<<S20 | \
                    1<<S21 | \
                    1<<S22 | \
                    1<<S23 | \
                    1<<S24 | \
                    1<<S25 | \
                    1<<S26 | \
                    1<<S27 | \
                    1<<S28 | \
                    1<<S29 | \
                    1<<S30) & ~(1<<LED)

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 16,                             \
                                 .rc_temp_ctiv  = 2,                              \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
