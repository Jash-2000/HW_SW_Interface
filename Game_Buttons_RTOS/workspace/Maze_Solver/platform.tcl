# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {Maze_Solver}\
-hw {/home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/bd_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {/home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace}

platform write
platform generate -domains 
platform active {Maze_Solver}
platform generate
platform active {Maze_Solver}
platform generate -domains 
platform active {Maze_Solver}
platform generate -domains 
platform generate
platform generate
platform active {Maze_Solver}
platform generate -domains 
platform active {Maze_Solver}
platform generate -domains 
platform generate
