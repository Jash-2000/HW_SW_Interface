#ifndef WAKE_WORD_H
#define WAKE_WORD_H

// =====================================================
// Ultra-lightweight wake-word detection
// Optimized for minimal processing overhead
// =====================================================

// Capture 100 frames of RAW audio data, average in time domain,
// then process ONCE - this is extremely efficient!
#define CAPTURE_FRAMES 100    // Raw frames to average (~2100ms)

// After averaging, we get ONE 512-sample frame
// Process this single averaged frame
#define NUM_BANDS 8           // Frequency bands for feature extraction

// Feature vector: 8 bands (extracted from ONE averaged FFT)
typedef struct {
    float bands[NUM_BANDS];
} FeatureVector;

// Wake-word template (stores ONE feature vector)
typedef struct {
    FeatureVector template;
    int is_trained;
} WakeWordTemplate;

// =====================================================
// Public API
// =====================================================

// Initialize the wake-word system
void wake_word_init(void);

// Extract features from FFT magnitude spectrum
// mag_spectrum: FFT magnitudes (n/2 values)
// n: FFT size (e.g., 256)
// Returns: pointer to extracted feature vector (internal storage)
FeatureVector* wake_word_extract_features(const float *mag_spectrum, int n);

// Compare a feature vector against the trained template
// Returns: 1 if match detected, 0 otherwise
int wake_word_compare(FeatureVector *features);

// Training: store a feature vector as the template
void wake_word_train(FeatureVector *features);

// Print the current template for hardcoding
void wake_word_print_template(void);

// Load hardcoded template
void wake_word_load_template(const float template_data[NUM_BANDS]);

#endif // WAKE_WORD_LITE_H
