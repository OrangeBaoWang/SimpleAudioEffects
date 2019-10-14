#include <iostream>
#include <sstream>

#include "paudiopipe.h"

PAudioPipe::PAudioPipe()
        : sampleFormat(paInt16),
          sampleSizeInBytes(2),
          numChannels(1),
          framesPerBuffer(4),
          sampleRate(44100),
          inputDevice(-1),
          outputDevice(-1) {
    initialize();
}

PAudioPipe::~PAudioPipe() {
    Pa_Terminate();
}

void PAudioPipe::start() {
    m_soundProcessor.setFunction(printOptionsAndSelect());
    std::cout << m_soundProcessor.option() << std::endl;
    startStream();
}

void PAudioPipe::stop() {

    PaError err = 0;

    if(!Pa_IsStreamStopped(stream))
        err = Pa_StopStream( stream );

    if( err !=  paNoError)
        reportStreamError(err);
}

void PAudioPipe::terminate() {
    Pa_Terminate();
}

void PAudioPipe::setSampleFormat(PaSampleFormat format) {
    sampleFormat = format;
}

void PAudioPipe::setInputDevice(unsigned int &index) {
    int numdevices = 0;
    numdevices = Pa_GetDeviceCount();
    if(numdevices == 0){
        printf("\nNo devices found\n");
        return;
    }
    if(index >= numdevices){
        printf("\nInvalid index\n");
        return;
    }
    const PaDeviceInfo* info = Pa_GetDeviceInfo(index);
    if(info == NULL){
         printf("\nNInvalid index\n");
        return;
    }
    if(info->maxInputChannels == 0){
         printf("\nInvalid index\n");
        return;
    }
    inputDevice = index;
    printf("\nInput device set to: %u: %s\n", index, info->name);
}

void PAudioPipe::setOutputDevice(unsigned int &index) {
    int numdevices = 0;
    numdevices = Pa_GetDeviceCount();
    if(numdevices == 0){
        printf("\nNo devices found\n");
        return;
    }
    if(index >= numdevices){
        printf("\nInvalid index\n");
        return;
    }
    const PaDeviceInfo* info = Pa_GetDeviceInfo(index);
    if(info == NULL){
         printf("\nNInvalid index\n");
        return;
    }
    if(info->maxOutputChannels == 0){
         printf("\nInvalid index\n");
        return;
    }
    outputDevice = index;
    printf("\nOutput device set to: %u: %s\n", index, info->name);
}

void PAudioPipe::listDevices() {
    const PaDeviceInfo *info;
    int numdevices = 0;
    numdevices = Pa_GetDeviceCount();
    int defaultin = -1, defaultout = -1;

    if(numdevices <= 0){
      printf("no devices found\n");
      return;
    }
    defaultin = Pa_GetDefaultInputDevice();
    defaultout = Pa_GetDefaultOutputDevice();
    if(defaultin == paNoDevice){ defaultin = -1;}
    if(defaultout == paNoDevice){ defaultout = -1;}
    printf("Devices:\n");
    for (int i = 0; i < numdevices; i++) {
        info = Pa_GetDeviceInfo(i);
        const char* inout = "";
        if (info->maxInputChannels > 0 && info->maxOutputChannels == 0) {
            inout = "input";
        } else if (info->maxInputChannels == 0 && info->maxOutputChannels > 0) {
            inout = "output";
        }

        if (i == defaultin || i == defaultout){
            printf("%i: (%s) %s (%s) --default--\n", i, inout, info->name, apiName(info->hostApi));
        } else {
            printf("%i: (%s) %s (%s)\n", i, inout, info->name, apiName(info->hostApi));
        }
    }
}

void PAudioPipe::getDeviceInfo(unsigned int &index) {
    const PaDeviceInfo *info;
    int numdevices = 0;
    numdevices = Pa_GetDeviceCount();
    int defaultin = -1, defaultout = -1;

    if(numdevices <= 0){
        printf("no devices found\n");
        return;
    }
    if(index >= numdevices ){
        printf("invalid index\n");
        return;
    }

    info = Pa_GetDeviceInfo(index);
    printf("\nDevice: (%u)\n", index);
    printf("%s\n", info->name);
    printf("API: %s\n", apiName(info->hostApi));
    printf("Input channels: %i\n", info->maxInputChannels);
    printf("Output channels: %i\n", info->maxOutputChannels);
    printf("Default sampling rate (Hz): %f\n", info->defaultSampleRate);
    printf("Default low input latency: %f\n", info->defaultLowInputLatency);
    printf("Default low input latency: %f\n", info->defaultLowOutputLatency);
    printf("Default high input latency: %f\n", info->defaultHighInputLatency);
    printf("Default high output latency: %f\n", info->defaultHighOutputLatency);
}

void PAudioPipe::startStream() {
    PaError err;
    PaStreamParameters outputParameters;
    PaStreamParameters inputParameters;

    // -set parameters
    if(outChannels > 0) {
        if( outputDevice < 0) {
            outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
        } else {
            outputParameters.device = outputDevice;
        }
        if (outputParameters.device == paNoDevice) {
         fprintf(stderr,"\nError: No default output device.\n");
         exit(1);
        }
        outputParameters.channelCount = outChannels;
        outputParameters.sampleFormat = sampleFormat;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
    }

    if(inChannels > 0) {
        if( inputDevice < 0) {
            inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
        } else {
            inputParameters.device = inputDevice;
        }
        if (inputParameters.device == paNoDevice) {
          fprintf(stderr,"\nError: No default input device.\n");
          exit(1);
        }
        inputParameters.channelCount = inChannels;
        inputParameters.sampleFormat = sampleFormat;
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;
    }

    err = Pa_OpenStream (
                &stream,
                &inputParameters,
                &outputParameters,
                sampleRate,
                framesPerBuffer,
                paClipOff,
                NULL,
                NULL );

    if( err != paNoError )
        reportStreamError(err);

    int numBytes = framesPerBuffer * numChannels * sampleSizeInBytes;
    std::shared_ptr<char> sampleBlock (new char[numBytes]);
    if (sampleBlock == NULL) {
        printf("Could not allocate record array.\n");
        exit(1);
    }

    err = Pa_StartStream( stream );
    if( err != paNoError )
        reportStreamError(err);

     while (true) {

        err = Pa_ReadStream(stream, sampleBlock.get(), framesPerBuffer);
        if (err)
            reportStreamError(err);

        char *currentBlock = sampleBlock.get();

        // Read samples from the buffer and put them back after processing.
        for (int i = 0; i < framesPerBuffer; i++) {
            int16_t tmp = *((int16_t*)(currentBlock));
            *((int16_t*)(currentBlock)) = (int16_t) m_soundProcessor.process( (float)tmp );
            currentBlock += sampleSizeInBytes;
        }
        err = Pa_WriteStream(stream, sampleBlock.get(), framesPerBuffer);
        if (err)
            reportStreamError(err);
    }
}

void PAudioPipe::initialize(){
    inChannels = numChannels;
    outChannels = numChannels;

     PaError err;
     err = Pa_Initialize();
     if( err != paNoError ) {
      reportStreamError(err);
     }

     m_soundProcessor.initialize(sampleRate);
}

void PAudioPipe::reportStreamError(PaError err) {
     fprintf( stderr, "\nAn error occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
     fflush(stdout);
     Pa_Terminate();
}

const char* PAudioPipe::apiName(unsigned int index) {
    const PaHostApiInfo* info;
    int hostindex = -1;
    int apicount = 0;
    int defaultapi = -1;

    apicount =  Pa_GetHostApiCount();
    hostindex = Pa_GetDefaultHostApi();
    defaultapi = Pa_GetDefaultHostApi();

    if(apicount <= 0){
        return "";
    }
    if(index > apicount-1){
        return "";
    }

    info =  Pa_GetHostApiInfo(index);

    return info->name;
}

int PAudioPipe::printOptionsAndSelect() {
    fprintf(stdout, "-----------------------------\n");
    fprintf(stdout, "Simple Audio Effects:\n");
    fprintf(stdout, "-----------------------------\n");
    std::stringstream option_msg;
    int numOptions = sizeof(kCoreProcesses) / sizeof(kCoreProcesses[0]);
    int i;
    for (i = 0; i < numOptions - 1; ++i) {
        option_msg << i << ":" << kCoreProcesses[i] << ", ";
    }
    option_msg << i << ":" << kCoreProcesses[i] << "\n";
    option_msg<< "Press Ctrl-C to quit\nQuit and restart to try another effect\n";
    fprintf(stdout, "%s", option_msg.str().c_str());

    int defaultSelection = 0;
    int selection;
    std::cout << "\nSelect an effect: ";
    std::cin >> selection;
    if (selection < 0 || selection >= numOptions)
        selection = defaultSelection;
    return selection;
}
