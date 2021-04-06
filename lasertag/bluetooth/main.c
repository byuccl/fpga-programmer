//#include "Adafruit_BluefruitLE_UART.h"
#include "bluetooth.h"
#include "display.h"
#include "interrupts.h"
#include "utils.h"
#include "xparameters.h"
#include <ctype.h>

#define BUFSIZE 1000
#define TEXT_SIZE 2

// Quick hack for non-blocking IO read.
// bool peek() { return XUartPs_IsReceiveData(XPAR_XUARTPS_0_BASEADDR); }

// void getUserInput(char buffer[], uint32_t maxSize) {
//   memset(buffer, 0, maxSize);
//   while (!peek())
//     ;
//   utils_msDelay(2);

//   gets(buffer);
// }

int main() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  display_print("*************************************************\n\r");
  display_print("*        bluetooth test program                 *\n\r");
  display_print("*   Start up the Adafruit BlueFruit App         *\n\r");
  display_print("* This will echo everything in upper case.      *\n\r");
  display_print("* 1. Start Bluefruit LE App on phone.           *\n\r");
  display_print("* 2. In app, Connect with Adafuit Bluefruit LE. *\n\r");
  display_print("* 3. Select UART in the Bluefruit LE App.       *\n\r");
  display_print("* 4. Type characters in input window.           *\n\r");
  display_print("* 5. Charcters echo back in upper case in app & *\n\r");
  display_print("*    on TFT.                                    *\n\r");
  display_print("*************************************************\n\r");
  display_setTextColor(DISPLAY_YELLOW);
  display_setTextSize(TEXT_SIZE);
  interrupts_initAll(true); // Init all interrupts (but does not enable the
                            // interrupts at the devices).
  interrupts_setPrivateTimerLoadValue(324900);
  interrupts_enableTimerGlobalInts(); // Allows the timer to generate
                                      // interrupts.
  interrupts_startArmPrivateTimer();  // Start the private ARM timer running.
  bluetooth_init();
  interrupts_enableArmInts();
  // Implement a simple echo that reads incoming characters, converts them to
  // upper-case, and transmits them back out.
  uint8_t incomingData[1];
  uint8_t outgoingData[1];
  while (1) {
    interrupts_disableArmInts();
    uint16_t bytesRead = bluetooth_receiveQueueRead(incomingData, 1);
    if (bytesRead == 1) {
      outgoingData[0] = toupper(incomingData[0]);
      char c = outgoingData[0];
      display_printChar(c);
      bluetooth_transmitQueueWrite(outgoingData, 1);
    }
    interrupts_enableArmInts();
  }
}

static uint16_t tickCount = 0;
#define BLUETOOTH_SERVICE_INTERVAL 5

void isr_function() {
  //    printf("#\n\r");
  tickCount++;
  if (tickCount > BLUETOOTH_SERVICE_INTERVAL) {
    bluetooth_poll();
    tickCount = 0;
  }
  bluetooth_poll();
}

//#define TEXT_SIZE 1
// int main() {
//  Adafruit_BluefruitLE_UART ble;
//  display_init();   // Print everything on the TFT so you don't need a
//  computer connection. display_setTextSize(TEXT_SIZE);             //
//  Reasonable size. display_println("Power on ZYBO board, Start Adafruit
//  Bluefruit LE app, "); display_println("connect with Adafruite Bluefruit LE,
//  select UART"); display_println("Any characters typed in the app will appear
//  below"); while (1)
//    ble.waitForOK(true);
//}

// int main() {
//  Adafruit_BluefruitLE_UART ble;
//  printf("Bluetooth Test Program\n\r");
////  ble.info();
////  utils_msDelay(100);
//  char deviceName[BUFSIZE];
////  printf("Advertised Name: ");
//  ble.getGapDeviceName(deviceName, BUFSIZE);
//  printf("Advertised Name: %s\n\r", deviceName);
////  ble.setGapDeviceName("blahblahblah");
//  // Display command prompt
//  bool printResult = true;
//  while (1) {
//    printf("Enter AT Command > ");
//    fflush(stdout);
//
//
//    // Check for user input and echo it back if anything was found
//    char command[BUFSIZE+1];
//    getUserInput(command, BUFSIZE);
//
//    // Send command to bluetooth module via a UART.
//    ble.print(command);
//
//    // Check response status and print result.
//    ble.waitForOK(printResult);
//  }
//
//}
