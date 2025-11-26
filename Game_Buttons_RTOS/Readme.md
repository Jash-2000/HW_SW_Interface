# Real Time Game Controller using Audio Processing Hardware Accelarator

This Project involves using a Micrblaze Urbana FPGA Board to generate a hardware accelarator that processes the Analog microphone input, applies FFT and other audio processing techniques and does a wake-word detection, finally producing an interrupt to the controller. Finally, the software runs a game that can be played on the interfaced LCD TFT display.

## Game Outline
Grid: an M×N orthogonal grid of square cells. Some cells are obstacles (walls), one is the start cell, one is goal cell.
Player: occupies the center of one cell and has one of 4 discrete orientations (N, E, S, W).
Actions (atomic turn):
Rotate left/right in place (changes orientation only).
Move forward OR backward: when chosen, the player translates (forward = in direction of orientation; backward = opposite) and keeps moving until it is about to enter an obstacle or leaves grid — then stops in the last free cell before that obstacle (i.e., slide-to-wall). Movement is atomic: user triggers it and it executes until stop.

Win: reach the goal cell (any orientation) after a move/rotation.

Constraints: collisions disallowed; cannot stop mid-way during slide.

## Game Design Rules for the Software
The game is desinged using QP-Nano's real time software processing. Its Hierarchy is as follows:

	-Init_State(As QP Nano Start Running)
		- Set the "counter" and "running" flags to 0.
		- Display the initial screen(triangle pattern) in the background
		- Show the menu text("Press BTN0 to Change the Levels\n Press BTN1 to Select the Levels\n Press BTN2 to Turn CW\n Press BTN3 to Turn ACW\n Press the Encoder Click to Start/Stop the game") for user and wait for input.
		
	-Level_Select_State(Enter from Init_State if any user input is seen || Enter from Result_State if "Timeout")
		- Display Level 1 of the game
		- Show the text "Level_i" in center.
		- Change to show different levels if "btn0" is pressed and update the text.

	-Pause_State(Enter from Level_Select_State if "btn1" is pressed)
		- Set the stage to play the game and display the player position and destination grid box.

	-Running State(Enter from Pause_State if "Enc_Click" is pressed)
		- Start the timer on seven segment display.
		- Update the Board position on basis of what is being pressed.
		- On BTN2 -- Turn AWC
		- On BTN3 -- Turn CW
		- On Encoder TWIST -- Move Accordingly.

	-Result_State(Enter from Running_State is "Enc_Click" is pressed || Enter from the Running_State if "GAME_OVER" is triggered)
		- Display the current counter value and show the win/loose status
		- Set the "counter" and "running" flags to 0.

	-Idle_State
		- Clear Interrupts
		- Update digits on the seven segment display.




On Bootup: 
        - The LCD display should showcase a fancy pattern in the background using 2 colours (anything like a repeating triangles)
        - Initiate the peripherals and set them up in interrupt mode "no polling".
        - The Seven Segment Display should print 0000 as the time.
        - Nothing happens on pressing any button till the encoder button is not pressed.

System Start:                                                         
        - Once the Roratry encoder's push button is pressed, the system starts running. The push button is basically a toggle switch.
        - The LCD screen prints what peripheral was accessed (4 push btns or rotary encoder's movements).
        - The prints disappear after 2 seconds or if anything else is printed (whatever comes first).
        - The timer starts running and is diplayed on seven segment display as well (this should be running in milisecond scale).
        - The printing stops as soon as the rotary encoder's button toggles and kills the system (now print the time system was up).

Note:
        - Real time processing involving interrupt based controls
        - The timer is running in milisecond precesion and should take up good amount of processnig power.
        - The LCD's screen should not refresh evidently i.e. the user should be able to see smooth transitions.


