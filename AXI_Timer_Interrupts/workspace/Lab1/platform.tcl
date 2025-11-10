# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/jashshah/Desktop/Embedded_Lab/Lab_1/workspace/Lab1/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/jashshah/Desktop/Embedded_Lab/Lab_1/workspace/Lab1/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {Lab1}\
-hw {/home/jashshah/Desktop/Embedded_Lab/Lab_1/bd_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {/home/jashshah/Desktop/Embedded_Lab/Lab_1/workspace}

platform write
platform generate -domains 
platform active {Lab1}
bsp reload
platform generate
