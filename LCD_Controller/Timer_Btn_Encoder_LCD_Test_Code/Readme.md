# Timer Display in Real Time Processing

This file contains code to show a fancy background on the LED, while displaying a timer clock updating in 10 milisecons update rule. (The LCD screen in itself can be a bit slow/fast than processor hardware and thus one needs to calibrate every time using a new hardware in case you want to actually see updates in "timer-scale").

The project shows a constantly updating timer, with each button press showing a different text and the encoder's push button resetting the timer to 0. The overlay of the text disappears after 2 seconds. All the processing, involving real time refreshes on the screen, are handled through interrrupts. Real-time OS like RTOS and/or QP-Nano have not been used in this version.
