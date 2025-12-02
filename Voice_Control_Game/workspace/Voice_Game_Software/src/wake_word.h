#ifndef WAKE_WORD_H
#define WAKE_WORD_H

#define NUM_MEL_BINS 16  // Number of mel-frequency bins
#define FRAME_WINDOW 8   // Number of frames to analyze (8 * ~50ms = ~400ms)
#define DETECTION_THRESHOLD 0.75  // Similarity threshold (0-1)

// Structure to hold mel-frequency features
typedef struct {
    float features[NUM_MEL_BINS];
} MelFrame;

// Initialize wake word detection system
void wake_word_init(void);

// Extract mel-frequency features from FFT output
void extract_mel_features(float* fft_mag, int n, float sample_f, MelFrame* frame);

// Add a new frame to the detection buffer
// Returns 1 if wake word detected, 0 otherwise
int wake_word_detect(MelFrame* new_frame);

// Train/store wake word template (call this to "learn" the wake word)
void wake_word_train(MelFrame* frames, int num_frames);

// Calculate cosine similarity between two feature vectors
float calculate_similarity(float* vec1, float* vec2, int size);

// Print template data in C array format (for hardcoding)
void wake_word_print_template(void);

// Load hardcoded template
void wake_word_load_hardcoded_template(void);

#endif // WAKE_WORD_H
