/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "sound.h"
#include "interrupts.h" // Just for sound_runTest().
#include "sounds/bcfire01_48k.wav.h"
#include "sounds/gameBoyStartup.wav.h"
#include "sounds/gameOver48k.wav.h"
#include "sounds/gunEmpty48k.wav.h"
#include "sounds/ouch48k.wav.h"
#include "sounds/pacmanDeath.wav.h"
#include "sounds/powerUp48k.wav.h"
#include "sounds/screamAndDie48k.wav.h"
#include "timer_ps.h"
#include "xiicps.h"
#include "xil_printf.h"
#include "xil_types.h"
#include <stdio.h>

/***************************************************************
 * Quite a bit of this code was obtained from digilentinc.com
 * so it does not necessarily meet the coding standard.
 ****************************************************************/

/* Redefine the XPAR constants */
#define IIC_DEVICE_ID XPAR_XIICPS_0_DEVICE_ID
#define I2S_ADDRESS XPAR_AXI_I2S_ADI_0_BASEADDR
#define TIMER_DEVICE_ID XPAR_SCUTIMER_DEVICE_ID
#define AUDIO_IIC_ID XPAR_XIICPS_0_DEVICE_ID
#define AUDIO_CTRL_BASEADDR XPAR_AXI_I2S_ADI_1_S_AXI_BASEADDR
#define SCU_TIMER_ID XPAR_SCUTIMER_DEVICE_ID
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR

#define SOUND_MULTIPLIER INT16_MAX / 3 // Primitive volume control.

#define ONE_SECOND_OF_SOUND_ARRAY_SIZE                                         \
  48000 // The sample rate is 48k so that is 1 second's worth.
uint16_t soundOfSilence[ONE_SECOND_OF_SOUND_ARRAY_SIZE];

// Declared below the sound state-machine code.
int AudioInitialize(u16 timerID, u16 iicID, u32 i2sAddr);

/****************************************************************
 *                 sound state machine code                     *
 ****************************************************************/
// True if sound_init() has been called, false otherwise.
static bool sound_initFlag = false;

// True if a sound should be played, false otherwise.
// Note that the state-machine sets this back to false once it has completed
// playing a sound.
static volatile bool sound_playSoundFlag = false;

// Keep track of the base pointer to the sound array with current sample-rate
// and sample count.
static uint16_t *sound_array; // Base pointer to the sound array.

// static uint32_t sound_sampleRate;  // Sample rate for this sound.
static uint32_t sound_sampleCount; // Number of samples in this sound.

// Keep track of the current volume setting.
static sound_volume_t sound_currentVolume = sound_minimumVolume_e;

// Sound state-machine states.
typedef enum {
  sound_init_st, // Waiting for sound_init() to be invoked.
  sound_wait_st, // Waiting for enable to play sound.
  sound_play_st  // In the process of playing the sound.
} sound_st_t;

// Reset the TX FIFO.
void sound_resetTxFifo() {
  Xil_Out32(AUDIO_CTRL_BASEADDR + I2S_RESET_REG, 0b010); // Reset TX Fifo
}

// Enable the TX FIFO.
void sound_enableTxFifo() {
  Xil_Out32(AUDIO_CTRL_BASEADDR + I2S_CTRL_REG,
            0b001); // Enable TX Fifo, disable mute
}

// Disables the TX FIFO.
void sound_disableTxFifo() {
  Xil_Out32(AUDIO_CTRL_BASEADDR + I2S_CTRL_REG, 0b00); // Disable TX FIFO.
}

// sampleValue is sent to both the left and right channels.
void sound_sendDataToBothChannels(uint32_t sampleValue) {
  Xil_Out32(AUDIO_CTRL_BASEADDR + I2S_TX_FIFO_REG,
            sampleValue); // add to left Channel.
  Xil_Out32(AUDIO_CTRL_BASEADDR + I2S_TX_FIFO_REG,
            sampleValue); // add to right Channel.
}

// Used to set the volume. Use one of the provided values.
void sound_setVolume(sound_volume_t volume) { sound_currentVolume = volume; }

// Must be called before using the sound state machine.
sound_status_t sound_init() {
  // Setup the audio CODEC.
  AudioInitialize(SCU_TIMER_ID, AUDIO_IIC_ID, AUDIO_CTRL_BASEADDR);
  sound_initFlag = true;
  // Initialize the silence array.
  for (uint32_t i = 0; i < ONE_SECOND_OF_SOUND_ARRAY_SIZE; i++)
    soundOfSilence[i] = NO_SOUND;
  sound_setVolume(sound_minimumVolume_e); // Init the volume level.
  return SOUND_STATUS_OK;
}

// Standard tick function.
static sound_st_t currentState = sound_init_st;

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint() {
  static sound_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case sound_init_st:
      printf("sound_init_st\n\r");
      break;
    case sound_wait_st:
      printf("sound_wait_st\n\r");
      break;
    case sound_play_st:
      printf("sound_play_st\n\r");
      break;
    }
  }
}

void sound_tick() {
  //  debugStatePrint();
  static uint32_t arrayIndex = 0;
  // Action switch statement.
  switch (currentState) {
  case sound_init_st:
    // Does nothing.
    break;
  case sound_wait_st:
    // Does nothing.
    break;
  case sound_play_st:
    // Does nothing.
    break;
  }
  // Transistion switch statement.
  switch (currentState) {
  case sound_init_st:
    if (sound_initFlag) {
      currentState = sound_wait_st;
    }
    break;
  case sound_wait_st:
    if (sound_playSoundFlag) {
      arrayIndex = 0;
      currentState = sound_play_st;
      sound_resetTxFifo();  // Reset the TX FIFO.
      sound_enableTxFifo(); // Enable the TX FIFO, disable mute.
    }
    break;
  case sound_play_st:
    // Each time you enter this state, add as many samples as will fit in the
    // FIFO.
    if (sound_array == NULL) {
      printf("ERROR, sound_tick: sound array has not been set.\n");
      return;
    }
    // This while-loop continues to load sound-data into the FIFOs until it is
    // full or the sound data are exhausted.
    while (!(Xil_In32(AUDIO_CTRL_BASEADDR + I2S_FIFO_STS_REG) &
             0b0010)) { // while room in FIFO.
      uint32_t sampleValue =
          sound_array[arrayIndex] * sound_currentVolume; // Scale by volume.
      sound_sendDataToBothChannels(
          sampleValue); // Send the sound data to the left and right channels.
      arrayIndex++;     // Go to next sample.
      if (arrayIndex == sound_sampleCount) { // All done?
        sound_playSoundFlag = false;         // Yes.
        sound_disableTxFifo();               // Disable the TX FIFO.
        currentState = sound_wait_st;        // Go back to the wait state.
      }
    }
    break;
  }
}

// Returns true if the sound state machine is not back in its initial state.
bool sound_isBusy() {
  return (sound_playSoundFlag); // Busy if NOT in the wait state.
}

// Stops the sound and resets the state-machine to the wait state.
void sound_stopSound() {
  sound_playSoundFlag = false; // disable the state-machine.
  currentState =
      sound_wait_st; // Force the state-machine back to the wait state.
}

// Use this to set the base address for the array containing sound data.
// Allow sounds to be interrupted.
void sound_setSound(sound_sounds_t sound) {
  if (sound_isBusy()) { // You are currently playing some sound.
    sound_stopSound(); // Stop the sound and reset the state-machine, FIFO, etc.
  }
  sound_array =
      NULL; // Set the pointer to NULL so you can detect it never being set.
  switch (sound) {
  case sound_gameStart_e:
    sound_array = gameBoyStartup_wav; // Set the array holding the data.
    sound_sampleCount =
        GAMEBOYSTARTUP_WAV_NUMBER_OF_SAMPLES; // Size of the array.
    break;
  case sound_gunFire_e:
    sound_array = bcfire01_48k_wav; // Set the array holding the data.
    sound_sampleCount =
        BCFIRE01_48K_WAV_NUMBER_OF_SAMPLES; // Size of the array.
    break;
  case sound_hit_e:
    sound_array = ouch48k_wav; // You get the idea...
    sound_sampleCount = OUCH48K_WAV_NUMBER_OF_SAMPLES;
    break;
  case sound_gunClick_e:
    sound_array = gunEmpty48k_wav;
    sound_sampleCount = GUNEMPTY48K_WAV_NUMBER_OF_SAMPLES;
    break;
  case sound_gunReload_e:
    sound_array = powerUp48k_wav;
    sound_sampleCount = POWERUP48K_WAV_NUMBER_OF_SAMPLES;
    break;
  case sound_loseLife_e:
    sound_array = screamAndDie48k_wav;
    sound_sampleCount = SCREAMANDDIE48K_WAV_NUMBER_OF_SAMPLES;
    break;
  case sound_gameOver_e:
    sound_array = pacmanDeath_wav;
    sound_sampleCount = PACMANDEATH_WAV_NUMBER_OF_SAMPLES;
    break;
  case sound_returnToBase_e:
    sound_array = gameOver48k_wav;
    sound_sampleCount = GAMEOVER48K_WAV_NUMBER_OF_SAMPLES;
    break;
  case sound_oneSecondSilence_e:
    sound_array = soundOfSilence;
    sound_sampleCount = ONE_SECOND_OF_SOUND_ARRAY_SIZE;
    break;
  default:
    printf("sound_setSound(): bogus sound value(%d)\n", sound);
  }
}

// Tell the state machine to start playing the sound.
void sound_startSound() { sound_playSoundFlag = true; }

// Returns true if the sound has been played. State machine will have returned
// to its initial state.
bool sound_isSoundComplete() { return (!sound_isBusy()); }

// Sets the sound and starts playing the sound immediately.
void sound_playSound(sound_sounds_t sound) {
  sound_setSound(sound); // Set the sound to be played.
  sound_startSound();    // Start playing the sound.
}

// Plays several sounds.
// To invoke, just place this in your main.
// Completely stand alone, doesn't require interrupts, etc.
void sound_runTest() {
  printf("****************** sound_runTest() ****************** \n");

  sound_init();
  sound_tick();
  sound_setSound(sound_gunClick_e);
  printf("playing gunClick_e\n");
  sound_startSound();
  while (1) {
    sound_tick();
    if (!sound_isBusy())
      break;
  }
  sound_setSound(sound_gunFire_e);
  printf("playing gunFire_e\n");
  sound_startSound();
  while (1) {
    sound_tick();
    if (!sound_isBusy())
      break;
  }
  sound_setSound(sound_gunReload_e);
  printf("playing gunReload_e\n");
  sound_startSound();
  while (1) {
    sound_tick();
    if (!sound_isBusy())
      break;
  }
  sound_setSound(sound_loseLife_e);
  printf("playing loseLife_e\n");
  sound_startSound();
  while (1) {
    sound_tick();
    if (!sound_isBusy())
      break;
  }
  sound_setSound(sound_gameOver_e);
  printf("playing gameOver_e\n");
  sound_startSound();
  while (1) {
    sound_tick();
    if (!sound_isBusy())
      break;
  }
  printf("done.\n");
}

/**********************************************************************************
 * Note from BLH: * Most of this code was re-purposed from the original Digilent
 *demonstration code. * The code initializes the IIC controller that is
 *connected to the audio CODEC.   * It also provides functions to initialize the
 *audio CODEC and to send/received   * to/from CODEC.
 **********************************************************************************/

static XIicPs Iic; /* Instance of the IIC Device */

/***************************************************************************
 * Procedural definitions from the original audio_demo files from digilent.
 ***************************************************************************/

/***  AudioRegSet(XIicPs *IIcPtr, u8 regAddr, u16 regData)
**
**  Parameters:
**    IIcPtr - Pointer to the initialized XIicPs struct
**    regAddr - Register in the SSM2603 to write to
**    regData - Data to write to the register (lower 9 bits are used)
**
**  Return Value: int
**    XST_SUCCESS if successful
**
**  Errors:
**
**  Description:
**    Writes a value to a register in the SSM2603 device over IIC.
**
*/

#define SEND_BUFFER_SIZE 2
int AudioRegSet(XIicPs *IIcPtr, u8 regAddr, u16 regData) {
  int Status;
  //  u8 SendBuffer[2];
  u8 SendBuffer[SEND_BUFFER_SIZE]; // We will send 2 bytes at a time.
  // Register address is stored in bits 7 - 1.
  SendBuffer[0] = regAddr << 1;
  // Store data bit 9 in bit 7 of 0th word.
  SendBuffer[0] = SendBuffer[0] | ((regData >> 8) & 0b1);
  // Bits 7-0 of data are stored in 8 bits of 1th word.
  SendBuffer[1] = regData & 0xFF;
  // Send 2 bytes to the IIC controller attached to the audio CODEC.
  Status = XIicPs_MasterSendPolled(IIcPtr, SendBuffer, 2, IIC_SLAVE_ADDR);
  // Always check for success.
  if (Status != XST_SUCCESS) {
    printf("IIC send failed\n\r");
    return XST_FAILURE;
  }
  // This function blocks until the IIC is idle.
  /*
   * Wait until bus is idle to start another transfer.
   */
  volatile int no_op;
  while (1) {
    no_op = XIicPs_BusIsBusy(IIcPtr);
    if (!no_op)
      break;
  }

  // while (XIicPs_BusIsBusy(IIcPtr)) {
  //   /* NOP */
  // }
  // return XST_SUCCESS;
}

/***  AudioInitialize(u16 timerID,  u16 iicID, u32 i2sAddr)
**
**  Parameters:
**    timerID - DEVICE_ID for the SCU timer
**    iicID   - DEVICE_ID for the PS IIC controller connected to the SSM2603
**    i2sAddr - Physical Base address of the I2S controller
**
**  Return Value: int
**    XST_SUCCESS if successful
**
**  Errors:
**
**  Description:
**    Initializes the Audio demo. Must be called once and only once before
*calling
**    AudioRunDemo
**
*/
int AudioInitialize(u16 timerID, u16 iicID, u32 i2sAddr) {
  int Status;            // Return status value.
  XIicPs_Config *Config; // Keep track of the config. value.
  u32 i2sClkDiv;         // Used to help compute the sampling frequency.

  TimerInitialize(
      timerID); // This is a timer that is used to create precise delays.

  /*
   * Initialize the IIC driver so that it's ready to use
   * Look up the configuration in the config table,
   * then initialize it.
   */
  Config = XIicPs_LookupConfig(iicID);
  if (NULL == Config) {
    return XST_FAILURE;
  }

  Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Perform a self-test to ensure that the hardware was built correctly.
   */
  Status = XIicPs_SelfTest(&Iic);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Set the IIC serial clock rate.
   */
  Status = XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Write to the SSM2603 audio codec registers to configure the device. Refer
   * to the SSM2603 Audio Codec data sheet for information on what these writes
   * do.
   */

  // Perform Reset
  Status = AudioRegSet(&Iic, 15, 0b000000000);
  if (Status)
    printf("Status1:%d\n", Status);
  TimerDelay(75000);
  // Power up
  Status |= AudioRegSet(&Iic, 6, 0b000110000);
  if (Status)
    printf("Status2:%d\n", Status);
  // Left-channel ADC input volume.
  Status |= AudioRegSet(&Iic, 0, 0b000010111);
  if (Status)
    printf("Status3:%d\n", Status);
  // Right-channel ADC input volume.
  AudioRegSet(&Iic, 1, 0b000010111);
  // Left-channel DAC volume. Also set
  Status |= AudioRegSet(&Iic, 2, 0b101111001);
  if (Status)
    printf("Status4:%d\n", Status);
  // right volume to same value.
  Status |= AudioRegSet(&Iic, 4, 0b000010000); // Analog audio path.
  if (Status)
    printf("Status5:%d\n", Status);
  Status |= AudioRegSet(&Iic, 5, 0b000000000); // Digital audio path.
  if (Status)
    printf("Status6:%d\n", Status);
  // Changed so Word length is 24  Status |= AudioRegSet(&Iic,
  // 8, 0b000000000); //Changed so no CLKDIV2
  Status |= AudioRegSet(&Iic, 7, 0b000001010);
  if (Status)
    printf("Status7:%d\n", Status);
  // Changed so no CLKDIV2
  Status |= AudioRegSet(&Iic, 8, 0b000000000);
  if (Status)
    printf("Status8:%d\n", Status);
  // Wait for things to settle down.
  TimerDelay(75000);
  // Make things active.
  Status |= AudioRegSet(&Iic, 9, 0b000000001);
  if (Status)
    printf("Status9:%d\n", Status);
  // Power-up the ouput (OSC is left
  // disabled as MCLK pin provides clock).
  Status |= AudioRegSet(&Iic, 6, 0b000100000);
  if (Status)
    printf("Status10:%d\n", Status);

  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  // BLH: This is the original value used by digilent.
  //  i2sClkDiv = 1; //Set the BCLK to be MCLK / 4
  // BLH: This value makes things sound correct.
  // Not sure what the problem is, perhaps the DLL is not running at the correct
  // frequency? or, there is a bug in the IP that drives the CODEC. In any case,
  // the sampling rate is 48k.
  i2sClkDiv = 3;
  // Set the LRCLK's to be BCLK / 64
  i2sClkDiv = i2sClkDiv | (31 << 16);
  // Write clock div register
  Xil_Out32(i2sAddr + I2S_CLK_CTRL_REG, i2sClkDiv);

  return XST_SUCCESS;
}

/* ------------------------------------------------------------ */

/***  I2SFifoWrite (u32 i2sBaseAddr, u32 audioData)
**
**  Parameters:
**    i2sBaseAddr - Physical Base address of the I2S controller
**    audioData - Audio data to be written to FIFO
**
**  Return Value: none
**
**  Errors:
**
**  Description:
**    Blocks execution until space is available in the I2S TX fifo, then
**    writes data to it.
**
*/
void I2SFifoWrite(u32 i2sBaseAddr, u32 audioData) {
  while ((Xil_In32(i2sBaseAddr + I2S_FIFO_STS_REG)) & 0b0010) {
  }
  Xil_Out32(i2sBaseAddr + I2S_TX_FIFO_REG, audioData);
}
/* ------------------------------------------------------------ */

/***  I2SFifoRead (u32 i2sBaseAddr)
**
**  Parameters:
**    i2sBaseAddr - Physical Base address of the I2S controller
**
**  Return Value: u32
**    Audio data from the I2S RX FIFO
**
**  Errors:
**
**  Description:
**    Blocks execution until data is available in the I2S RX fifo, then
**    reads it out.
**
*/
u32 I2SFifoRead(u32 i2sBaseAddr) {
  while ((Xil_In32(i2sBaseAddr + I2S_FIFO_STS_REG)) & 0b0100) {
  }
  return Xil_In32(i2sBaseAddr + I2S_RX_FIFO_REG);
}
/* ------------------------------------------------------------ */