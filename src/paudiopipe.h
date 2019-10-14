#pragma once

#include <cstdint>
#include <memory>
#include <stdio.h>

// Include portaudio library - low level audio apis.
#include "portaudio.h"

// Include audio or sound effects producing class.
#include "soundprocessor.h"

// PAudioPipe
//
// Wrapper class around low level audio APIs provided by portaudio library.
// Only the APIs needed for this project are used (portaudio is million times more
// extensive than what has been used here).
class PAudioPipe {
    public:
        PAudioPipe();
        ~PAudioPipe();

        // Start the audio pipe.
        void start();

        // Stop the audio pipe.
        void stop();

        // Explicity stop/terminate portaudio stream.
        void terminate();

        // Set the audio sample format (e.g. signed 16 bit int, signed 32 bit float, etc).
        void setSampleFormat(PaSampleFormat format);

        // List available audio devices.
        void listDevices();

        // Get the information of the audio device with given index.
        // index: index of audio device.
        void getDeviceInfo(unsigned int &index);

        // Set input device.
        // index: index of the audio device which would be set as input device.
        void setInputDevice(unsigned int &index);

        // Set output device.
        // index: index of the audio device which would be set as output device.
        void setOutputDevice(unsigned int &index);

        // Print available audio effects and also select one of them from user.
        int printOptionsAndSelect();

    private:
        // Pointer to audio stream.
        PaStream *stream;

        // Audio stream or data format (type).
        PaSampleFormat sampleFormat = paInt16;

        // Size of one audio sample in bytes.
        unsigned int sampleSizeInBytes;

        // Nuber of input/output channels to use.
        // Restricted to use same number of output channels as input channels.
        unsigned int numChannels;
        unsigned int inChannels ;
        unsigned int outChannels;

        // Number of audio samples passed per low level api calls (by portaudio).
        unsigned int framesPerBuffer;

        // Sample rate (frequency) in Hz.
        unsigned int sampleRate;

        // Input and output devices.
        int inputDevice;
        int outputDevice;

        // Initialize the audio pipe.
        void initialize();

        // Start portaudio stream.
        void startStream();

        // Get the API name of the device.
        // index: index of the device.
        // Returns pointer to string with API info.
        const char* apiName(unsigned int index);

        // Handle error.
        // This prints the error and exits execution.
        void reportStreamError(PaError err);

        // Audio or sound effect producer (object).
        SoundProcessor m_soundProcessor;
};
