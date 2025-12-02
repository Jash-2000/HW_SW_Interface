# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Voice_Control_Game/workspace/Final_Project/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Voice_Control_Game/workspace/Final_Project/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {Final_Project}\
-hw {/home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Voice_Control_Game/bd_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {/home/jashshah/Desktop/Embedded_Lab/HW_SW_Interface/Voice_Control_Game/workspace}

platform write
platform generate -domains 
platform active {Final_Project}
platform generate
