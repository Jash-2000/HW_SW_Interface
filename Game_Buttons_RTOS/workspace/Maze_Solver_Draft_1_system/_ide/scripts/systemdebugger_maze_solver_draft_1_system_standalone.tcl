# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver_Draft_1_system/_ide/scripts/systemdebugger_maze_solver_draft_1_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver_Draft_1_system/_ide/scripts/systemdebugger_maze_solver_draft_1_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "RealDigital Boo 8874042404B4A" && level==0 && jtag_device_ctx=="jsn1-0362f093-0"}
fpga -file /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver_Draft_1/_ide/bitstream/bd_wrapper.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver/export/Maze_Solver/hw/bd_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Game_Buttons_RTOS/workspace/Maze_Solver_Draft_1/Debug/Maze_Solver_Draft_1.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
