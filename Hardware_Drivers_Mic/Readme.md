# Microphone Access and Frequency Identification for the FPGA

This project involved integrating the hardware components of an analog microphone(PCM outputs) with the AMBA/AXI bus of the processor, and creating a software DFT (Discrete Fourier Transform) that can identify the 
frequency of the tone being played. The aim was to challenge myself with reduction in latency and imrpovement in accuracy for even low frequency tones. I achieved a 14us latency with 99.999% (0.001% devation from 
base frequency) accuracy for sine tones ranginging from 50Hz to 2KHz. Hardware simply saved the microhpones bit stream (CIC/Sinc filter to convert high-rate 1-bit PDM data into lower-rate multi-bit PCM samples ) 
into a memory register and all the optimizations were realized through the software:

---
1. Sine and Cosine Calculations
  -  Implemented the Taylor Series Expansion to evaluate the sines and cosines using custom defined "factorial" function. This saved the processor from loading up the entire "math.c" file
  -  For |x|>PI/2, I added a reccursive loop to break down the input angle by using the property sin(x)=2sin(x/2)cos(x/2). This helped with acurate convergance of the series.
  -  The Taylor Series was implemented for only the first 10 set of elements as further calculation had nearly zero improvements 
2. LUT Based Trig Function Evaluation
  - All the above mentione calculations take place only once per program (i.e. at the very start) and the data is stored in a look-up table.
  - My table implementation had 1024 entries which mapped input angle from -PI to PI.
  - For the entire course of operation, the requested angle was extracted from the lookup table (either directly of by linear interpolation from neighbouring values), thus reducing heavy calcuations
3. 
---
