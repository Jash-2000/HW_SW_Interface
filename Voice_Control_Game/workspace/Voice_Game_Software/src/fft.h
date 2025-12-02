#ifndef FFT_H
#define FFT_H

// Compute FFT and return dominant frequency
float fft(float* q, float* w, int n, int m, float sample_f);

// Compute FFT and store magnitudes (for wake word detection)
void fft_magnitude(float* q, float* w, int n, int m, float* magnitudes);

#endif // FFT_H
