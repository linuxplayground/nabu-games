#define AY_CHA 0
#define AY_CHB 1
#define AY_CHC 2
#define AY_CHN 3

#define AY_CHA_TONE_L   0
#define AY_CHA_TONE_H   1
#define AY_CHB_TONE_L   2
#define AY_CHB_TONE_H   3
#define AY_CHC_TONE_L   4
#define AY_CHC_TONE_H   5
#define AY_NOISE_GEN    6
#define AY_ENABLES      7
#define AY_CHA_AMPL     8
#define AY_CHB_AMPL     9
#define AY_CHC_AMPL     10
#define AY_ENV_PERIOD_L 11
#define AY_ENV_PERIOD_H 12
#define AY_ENV_SHAPE    13
#define AY_PORTA        14
#define AY_PORTB        15

#define AY_INACTIVE 0x03
#define AY_READ     0x02
#define AY_WRITE    0x01
#define AY_ADDR     0x00

#define AY_ENV_SHAPE_CONTINUE  0x08
#define AY_ENV_SHAPE_ATTACK    0x04
#define AY_ENV_SHAPE_ALTERNATE 0x02
#define AY_ENV_SHAPE_HOLD      0x01
#define AY_ENV_SHAPE_OFF       0x00

//  /\  /\  /\  /\  /\ 
// /  \/  \/  \/  \/  
#define AY_ENV_SHAPE_TRIANGLE  (AY_ENV_SHAPE_CONTINUE | AY_ENV_SHAPE_ATTACK | AY_ENV_SHAPE_ALTERNATE)

//  /------------------
// /
#define AY_ENV_SHAPE_FADE_IN  (AY_ENV_SHAPE_CONTINUE | AY_ENV_SHAPE_ATTACK | AY_ENV_SHAPE_HOLD)

// \ 
//  \__________________
#define AY_ENV_SHAPE_FADE_OUT (AY_ENV_SHAPE_CONTINUE | AY_ENV_SHAPE_HOLD)

// \ |\ |\ |\ |\ |\ |\ |
//  \| \| \| \| \| \| \|
#define AY_ENV_SHAPE_SAW1     (AY_ENV_SHAPE_CONTINUE)

//  /| /| /| /| /| /| /|
// / |/ |/ |/ |/ |/ |/ |
#define AY_ENV_SHAPE_SAW2     (AY_ENV_SHAPE_CONTINUE | AY_ENV_SHAPE_ATTACK)

//  /|
// / |__________________
#define AY_ENV_SHAPE_FADE_IN_STOP (AY_ENV_SHAPE_ATTACK)

