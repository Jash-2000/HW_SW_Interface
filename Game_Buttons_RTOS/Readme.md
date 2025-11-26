# Real Time Game Controller on Low Memory FPGA

This Project involves using a Micrblaze Urbana FPGA Board to generate a game that can be played on the interfaced LCD TFT display. The display is a very simple TFT LCD screen with SPI communication protocol, restricting any parallel updates for the graphics. The available code is pure bare-metal and thus can be ported to other platforms as well (change your BSP_Settings file). The verilog implementation and interfacing for all the peripherals has been compiled in "bd_wrapper.xsa" file and can be reused if using the same set of hardware. 

## Game Outline
Grid: an M×N orthogonal grid of square cells. Some cells are obstacles (walls), one is the start cell, one is goal cell.

Player: occupies the center of one cell and has one of 4 discrete orientations (N, E, S, W).

Actions (atomic turn): Rotate left/right in place (changes orientation only).
					   Move forward OR backward: when chosen, the player translates (forward = in direction of orientation; backward = opposite) and keeps moving until it is about to enter an obstacle or leaves grid — then stops in the last free cell before that obstacle (i.e., slide-to-wall). Note: Movement is atomic: user triggers it and it executes until stop.

Win: Reach the goal cell (any orientation) after a move/rotation.

Constraints: collisions disallowed; cannot stop mid-way during slide.

## Game Design Rules for the Software
The game is desinged using QP-Nano's real time OS processing framework. Its Hierarchy is as follows:

	-Init_State(As QP Nano Start Running)
		- Set the "counter" and "running" flags to 0.
		- Display the initial screen(fancy triangle pattern) in the background
		- Show the menu text("Turn the encoder for scrolling levels\n Press the Encoder Button to start/stop the game\n Use 4 push buttons to move N,W,S,E") for user and wait for any input to be made.
		
	-Level_Select_State(Enter from Init_State if any user input is seen || Enter from Result_State if "Timeout")
		- Display Level 1 of the game
		- Change to show different levels if "Enc_Twist" is observed and update the text.

	-Pause_State(Enter from Level_Select_State if "Enc_Click" is pressed)
		- Set the stage to play the game and display the player position and destination grid box.

	-Running State(Enter from Pause_State if "Enc_Click" is pressed)
		- Start the timer on seven segment display.
		- Update the Board position on basis of what is being pressed.
		- On BTN0 -- Move North
		- On BTN1 -- Move West
		- On BTN2 -- Move South
		- On BTN3 -- Move East

	-Result_State(Enter from Running_State is "Enc_Click" is pressed || Enter from the Running_State if "GAME_OVER" is triggered)
		- Display the current counter value and show the win/loose status
		- Display the Level you player
		- Set the "counter" and "running" flags to 0.

	-Idle_State
		- Clear Interrupts
		- Update digits on the seven segment display(Make sure that it is non-blocking).


---

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


