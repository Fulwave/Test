//					Bongiovi DPS library version 1.0.1
#include <math.h>
#include <arm_math.h>
#include <stdlib.h>

#define MAX_BUFF_LEN    	512 // do not remove


//****************************************************************************

//#define BITS_16 // leave in place for 16-bit I2S, remove for 32-bit I2S
//#define USE_INTERNAL_CODEC // leave in place for NUC505-native internal CODEC, remove for external I2S
#define RATE_48K // leave in place for 48kHz sampling rate, remove for 44.1kHz sampling rate


//******************************************
// Call DPS_GlobalInitialize() before calling any other functions in this library
// Returns 0 if successful, 1 if DPS already initialized	

uint32_t DPS_GlobalInitialize(void);

void DPS_LoadPreset(uint8_t N); // N = 0, 1, 2, 3, or 4

void DPS_EnableStereoWidening(void); // turn on stereo widening (simple or normal -- whichever is active)

void DPS_BypassStereoWidening(void); // turn off stereo widening (simple or normal -- whichever is active)

void DPS_EnableDPS(void); // turn on DPS

void DPS_BypassDPS(void); // turn off DPS



void DPS_Left(float32_t *audioBuffer); // run DPS process in-place on left-channel audio buffer

void DPS_Right(float32_t *audioBuffer); // run DPS process in-place on right-channel audio buffer

void NoiseGateProcess(float32_t *data_L, float32_t *data_R); // run noise-gate process on stereo left and right audio buffers

void StereoExpand(float32_t *leftBuffer, float32_t *rightBuffer);	// automatically switches to simplified Stereo Widening when at 48k sample rate

void StereoWiden(float32_t *leftBuffer, float32_t *rightBuffer); // run stereo-widening process on left and right audio buffers

void SimpleStereoWiden(float32_t *leftBuffer, float32_t *rightBuffer);// run simplified stereo-widening process on left and right audio buffers


//******************************************
// Serial control functions

void DPS_parse_byte(uint8_t rxword);


//******************************************************
//
//	User EQ and User Gain setters and getters

void DPS_SetUserGain(uint8_t value); // set user gain to index "value"

void DPS_SetUserEQ_Fc(uint8_t whichEQ, uint8_t value); // set user EQ center frequency to index "value" (whichEQ = 0, 1)

uint8_t DPS_GetUserEQ_Fc(uint8_t whichEQ); // get user EQ center frequency (whichEQ = 0, 1)

void DPS_SetUserEQ_Q(uint8_t whichEQ, uint8_t value); // set user EQ Quality factor to index "value" (whichEQ = 0, 1)

uint8_t DPS_GetUserEQ_Q(uint8_t whichEQ); // get user EQ Quality factor (whichEQ = 0, 1)

void DPS_SetUserEQ_G(uint8_t whichEQ, uint8_t value); // set user EQ gain to index "value" (whichEQ = 0, 1) 

uint8_t DPS_GetUserEQ_G(uint8_t whichEQ); // get user EQ gain (whichEQ = 0, 1)


void DPS_EnableLevelMeters(void); // enables computation of input and output levels

void DPS_DisableLevelMeters(void); // disables computation of input and output levels



