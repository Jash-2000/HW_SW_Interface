This is a templeate of software based voice-controlled game where I used the microphone peripheral for detecting "wake-word" but it failed miserably due to the following reasons:
1. sampling rate is = 100*1000*1000/2048.0 ~ 48kHz which means for a simple wake_word 48,828 samples/s × 4 bytes = 195,312 bytes/sec ≈ 0.20 MB/s data streaming is required on the interface bus
2. My streaming_data.v custom hardware is capable of handling 4096 samples of audio data but there is limitation on AXI slave read buffer which is 512 samples [long 4096-deep FIFO → AXI-Stream → AXI DMA → DDR memory]
3. AXI-Lite cannot continuously capture a real-time 48,828-samples/sec stream and it will drop all other samples immediately.
4. The software version of ML+GameUI+Streaming is burninig the processor and all the processes remain in "stall" condition.
