To compile this into an exectuable that allows you to communicate between 
and smartphone and ZYBO board, uncomment the #add_subdirectory(bluetooth)
in the lasertag CMakeLists.txt file. To test this program, download the 
Bluefruit Connect  app onto either IOS or Android phone, pair with the
Adafruit Bluefruit LE device, and then select the UART panel. Type a single
letter in the box at the bottom, hit send, and its uppercase version should
appear in the upper window.

Note that the blue LED on the bluetooth modem will glow when paired with the app.

bluetooth.c is provided solely as documenation so that you can write your
own communication routines as necessary. Do not compile it as because it
will cause multiple symbol defintions as this file is already compiled into
the ZYBO libraries that are linked with all executables.

