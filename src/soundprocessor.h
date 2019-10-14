#pragma once

#include <string>

// Buffer length 1.5 seconds @ 44100Hz.
#define BUF_LEN 66150

// List of sound effects that SoundProcessor class supports.
const std::string kCoreProcesses[] = {
    "Pass",
    "Echo",
    "IIR Echo",
    "Natural Echo",
    "Reverb",
    "Filter Out",
    "Fuzz",
    "Flanger",
    "Tremolo"};

// SoundProcessor
//
// Class responsible for generating audio or sound effects.
class SoundProcessor {
    public:
        // Constructor.
        SoundProcessor();

        // Destructor.
        ~SoundProcessor();

        // Initialize or set sample rate as desired before using this class.
        // Default is 44.1kHz.
        void initialize(int sampleRate) { m_sampleRate = sampleRate; }

        // Select the audio/sound effect function.
        // idxF: Index to the sound effect or dsp function (see kCoreProcesses at the top).
        void setFunction(int idxF);

        // Process an audio sample to produce sound effect.
        // sample: input audio sample
        // Returns processed audio sample.
        float process(float sample);

        // Returns the index of the current audo effect in use.
        int option() { return m_idxF; }

    private:
        // Process dispatcher.
        float coreProcess();

        // Core processing algorithms.
        // No effect.
        float pass();

        // Echo (ideal).
        float echo();

        // Echo (ideal with feedback).
        float iirEcho();

        // Echo (natural, something closer to what happens in real life).
        float naturalEcho();

        // Reverberation.
        float reverb();

        // Filter the input to discard high frequencies.
        float biQuad();

        // Tremolo effect.
        float tremolo();

        // Fuzz effect.
        float fuzz();

        // Flanger effect.
        float flanger();

        // Sample rate or frequency in Hz.
        int m_sampleRate;

        // Buffer length.
        //enum {BUF_LEN = 60000};
        //enum {BUF_MASK = BUF_LEN};

        // Output buffer.
        float m_pY[BUF_LEN];
        // Output buffer index.
        int m_idxY;

        // Input buffer.
        float m_pX[BUF_LEN];
        // Input buffer index.
        int m_idxX;

        // Index for sound effect or dsp function (see kCoreProcesses at the top).
        int m_idxF;
};
