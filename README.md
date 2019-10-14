# Simple Audio Effects

Generate different audio/sound effects.

Headphone (or earphone) mic is used to capture the input. The input is then processed using well-known digital
signal processing (DSP) algorithms to product the sound effects. The processed data is then played back through
the headphone speakers.

You can also use PC mic (without headphones) but due to the feedback, output might be undesirable. So use of
headphone is highly recommended.

The application implements following sound effects:
* **Pass** - Output audio should just sound like input audio (no effect)
* **Echo** - Ideal echo without feedback
* **IIR Echo** - Ideal echo with feedack
* **Natural Echo** - More realistic echo
* **Reverb** - Reverberation
* **Flilter Out** - Filter out high frquencies
* **Fuzz**
* **Flanger**
* **Tremolo**

You can compare the **Pass** output with others to get the idea of what difference the effects are making.


## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
* PortAudio
  * Details can be found [here](https://wiki.libsdl.org/Installation)
  * For Linux, an `apt` or `apt-get` installation is preferred to building from source.
  * For Mac, `brew install` is preferred to building from source.
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
* OS
  * MacOS - the project was developed in MacOS
  * Linux - Should work as long as above specified dependencies are met
  * Windows - Not supported

## Basic Build Instructions
1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./SimpleAudioEffects`

## Code organization (folders/files)
* `cmake/`
  * `FindPortAudio.cmake` - Cmake script to find PortAudio
* `src/` - sources/headers directory
  * `main.cpp` - entry point to the program
  * `paudiopipe.h` - definition of the class that acts as the wrapper around portadudio
  * `paudiopipe.cpp` - implementation of the class defined in paudiopipe.h
  * `soundprocessor.h` - definition for the class that implements sound effects
  * `soundprocessor.cpp` - implementation of the class defined in soundprocessor.h

## Code organization (classes)
* Low level audio calls - these are provided by PortAudio library
* PAudioPipe - wrapper class around portaudio library
  * See the class definition (in `paudiopipe.h`) for further details. All the functions,
     especially the public ones, are explained with comments.
* SoundPorcessor - Audio processing class (mainly math/dsp functions)
  * See the class definition (in `soundprocessor.h`) for further details. All the functions,
     especially the public ones, are explained with comments.
  * See at the top of `soundprocessor.cpp` for brief explanation of the audio effect math models.

## Rubric
* **README (All 4 Rubric Points REQUIRED, ***all 4 met***)**
  1. :white_check_mark: The README is included with the project and has instructions for building/running the project.
  2. :white_check_mark:If any additional libraries are needed to run the project, these are indicated with cross-platform installation instructions.
  3. :white_check_mark: You can submit your writeup as markdown or pdf.
  4. :white_check_mark: The README describes the project you have built.
* **Compiling and Testing (All 2 Rubric Points REQUIRED, ***all 2 met***)**
  1. :white_check_mark: The project code must compile and run without errors.
  2. :white_check_mark: We strongly recommend using cmake and make, as provided in the starter repos.
* **Other (at lesat 5 REQUIRED, ***14 met***)**
  1. :white_check_mark: A variety of control structures are used in the project.
    * Very obvious.
    * Selected examples: `switch` statement in `SoundProcessor::coreProcess` in `soundprocessor.cpp`, several `if-else` statements, `for` and `while` loops in many functions in `paudiopipe.cpp`.
  2. :white_check_mark: The project code is clearly organized into functions.
    * This one is so obvious - it's all functions everywhere!
  3. :white_check_mark: The project reads data from an external file or writes data to a file as part of the necessary operation of the program.
    * The project reads the data from mic in real time (that is much more channelnging than reading something from a file!), see `PAudioPipe::startStream` in `paudiopipe.cpp`.
    * It also writes processed audio data to playback device (headphone), see `PAudioPipe::startStream` in `paudiopipe.cpp`.
  4. :white_check_mark: The project accepts input from a user as part of the necessary operation of the program.
    * See `PAudioPipe::printOptionsAndSelect` in `paudiopipe.cpp`.
  5. :white_check_mark: The project code is organized into classes with class attributes to hold the data, and class methods to perform tasks.
    * This is also obvious one.
    * All the low-level audio i/o calls are wrapped in `PAudioPipe` class and audio processing logic/data is encapsulated in `SoundProcessor` class.
  6. :white_check_mark: All class data members are explicitly specified as public, protected, or private.
    * This is anohter obvious one.
    * There are private and public data and member functions in both the classes.
  7. :white_check_mark: All class members that are set to argument values are initialized through member initialization lists.
    * See the constructor `SoundProcessor` in `soundprocessor.cpp`.
    * See the constructor `PAudioPipe` in `paudiopipe.cpp`.
  8. :white_check_mark: All class member functions document their effects, either through function names, comments, or formal documentation. Member functions do not change program state in undocumented ways.
    * The data and member functions are documented in the class definitions or headers.
    * See `soundprocessor.h` and `paudiopipe.h`.
  9. :white_check_mark: Appropriate data and functions are grouped into classes. Member data that is subject to an invariant is hidden from the user. State is accessed via member functions.
    * There are private data and member functions in each class that does not need to be exposed outside the class.
  10. :white_check_mark: Inheritance hierarchies are logical. Composition is used instead of inheritance when appropriate. Abstract classes are composed of pure virtual functions. Override functions are specified.
    * Composition is used - `PAudioPipe` object has a `SoundProcessor` object.
  11. :white_check_mark: At least two variables are defined as references, or two functions use pass-by-reference in the project code.
    * See `PAudioPipe::setInputDevice`, `PAudioPipe::setOutputDevice` and `PAudioPipe::getDeviceInfo`.
  12. :white_check_mark: At least one class that uses unmanaged dynamically allocated memory, along with any class that otherwise needs to modify state upon the termination of an object, uses a destructor.
    * See `PAudioPipe::startStream`
  13. :white_check_mark: The project follows the Resource Acquisition Is Initialization pattern where appropriate, by allocating objects at compile-time, initializing objects when they are declared, and utilizing scope to ensure their automatic destruction.
  14. :white_check_mark: The project uses at least one smart pointer: unique_ptr, shared_ptr, or weak_ptr. The project does not use raw pointers.
    * See `PAudioPipe::startStream` or use of smart pointer where it makes sense.
    * The project does not use raw pointers for dynamic memory allocation - that's where they are most dangerous (memory leaks, crashes).
    * Use of raw pointers in local scope to point to audio buffers provided by the underlying audio library (portaudio) is I think better than just using smart pointers for the sake of just using them.

## References/Help:
  * PortAudio documentation
  * Several stack-overflow posts / google search results related to PortAudio
  * DSP textbook / online DSP course by Martin Vetterli
