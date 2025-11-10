# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/jashshah/Desktop/Embedded_Lab/Lab_2/workspace/Pratyush_Display/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/jashshah/Desktop/Embedded_Lab/Lab_2/workspace/Pratyush_Display/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {Pratyush_Display}\
-hw {/home/jashshah/Desktop/Embedded_Lab/Lab_2/bd_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {/home/jashshah/Desktop/Embedded_Lab/Lab_2/workspace}

platform write
platform generate -domains 
platform active {Pratyush_Display}
platform generate
platform active {Pratyush_Display}
platform generate -domains 
