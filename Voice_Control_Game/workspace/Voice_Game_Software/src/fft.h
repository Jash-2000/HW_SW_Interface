#ifndef FFT_H
#define FFT_H

// Compute FFT and return dominant frequency
float fft(float q[], float w[], int n, int m, float sample_f);

// Compute FFT and extract magnitude spectrum (first half only)
// mag_out must be at least n/2 floats
void fft_get_magnitude_spectrum(float q[], float w[], int n, float *mag_out);

#endif // FFT_H
