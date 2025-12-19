# HW_Src

nopll_mic_block -- This hardware module is used to convert the analog microphone's PDM modulation(Pulse Density Modulation is used because it's a cost-effective way to get digital audio from small MEMS mics to 
processors, offering digital benefits like noise immunity and simpler routing (just two wires)) into FPGA readable PCM modulated((Pulse Code Modulation)) signal. It takes sys_clk and sys_reset as inputs, generates a 
custom clock to access the mic (decimation of 32) and finally implements a 5th order SINC/CIC filter for filtering. Finally an oversampling ration of 64 is used, making the output frequency close to 48kHz. This output
