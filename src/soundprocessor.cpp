// In all the equations for the sound effects below, this is the
// convention:
// x -> input signal.
// y -> output singal.
// x[n-d] -> signal at time inddex n delayed by d samples.
// cos(wn) -> w is angular frequency and n is time index.
//
// For all effects, the idea is to process the input singal x through
// a function f such that
// y = f(x), where the function f represents the audio effect.
//
// The function for each effect is provided as comment in the respective
// member function below.
//
// Understanding these equations requires some basic understanding of
// discrete-time signals and systems.

#include "soundprocessor.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define PI 3.14159265359


SoundProcessor::SoundProcessor()
        : m_sampleRate(44100),
          m_idxX(0),
          m_idxY(0),
          m_idxF(0) {
    memset(m_pX, 0, sizeof(float) * BUF_LEN);
    memset(m_pY, 0, sizeof(float) * BUF_LEN);
}


SoundProcessor::~SoundProcessor() {
}


void SoundProcessor::setFunction(int idxF) {
    m_idxF = idxF;
    m_idxX = 0;
    m_idxY = 0;
    memset(m_pX, 0, sizeof(float) * BUF_LEN);
    memset(m_pY, 0, sizeof(float) * BUF_LEN);
}


float SoundProcessor::process(float sample) {
    // Push input sample up input buffer.
    m_pX[m_idxX] = sample;

    float y = coreProcess();

    // Push output sample up output buffer.
    m_pY[m_idxY] = y;

    // Increment pointers.
    m_idxX = (m_idxX + 1) % BUF_LEN;
    m_idxY = (m_idxY + 1) % BUF_LEN;

    return y;
}

float SoundProcessor::coreProcess() {
    // Numbering should match with the indices
    // in kCoreProcesses array defined above.
    float y;
    switch (m_idxF) {
        case 0:
        default:
            y = pass();
            break;
        case 1:
            y = echo();
            break;
        case 2:
            y = iirEcho();
            break;
        case 3:
            y = naturalEcho();
            break;
        case 4:
            y = reverb();
            break;
        case 5:
            y = biQuad();
            break;
        case 6:
            y = fuzz();
            break;
        case 7:
            y = flanger();
            break;
        case 8:
            y = tremolo();
            break;
    }
    return y;
}

float SoundProcessor::pass() {
  return m_pX[m_idxX];
}


float SoundProcessor::echo() {
    // Echo signal model:
    // y[n] = (ax[n] + bx[n-N] + cx[n-2N])/(a+b+c)
    static float a = 1;
    static float b = 0.7f;
    static float c = 0.5f;
    static float norm = 1.0f / (a+b+c);
    static int N = (int)(0.3 * m_sampleRate);

    return norm * (
      a * m_pX[m_idxX]
    + b * m_pX[(m_idxX + BUF_LEN - N) %  BUF_LEN]
    + c * m_pX[(m_idxX + BUF_LEN - 2*N) % BUF_LEN]);
}


float SoundProcessor::iirEcho() {
    // Echo signal model:
    // y[n] = x[n] + ay[n-N]
    static float a = 0.7f;
    static float norm = (1 - a * a);
    static int N = (int)(0.3 * m_sampleRate);

    return norm * (
            m_pX[m_idxX]
      + a * m_pY[(m_idxY + BUF_LEN - N) % BUF_LEN]);
}


float SoundProcessor::naturalEcho() {
    // Echo signal model:
    // y[n] = x[n] + y[n-N] * h[n], h[n] is leaky integrator.
    static float a = 0.7f;
    static float norm = 1.0f / (1+a);
    static int N = (int)(0.3 * m_sampleRate);

    return norm * (
                m_pX[m_idxX]
      -     a * m_pX[(m_idxX + BUF_LEN - 1) % BUF_LEN]
      +     a * m_pY[(m_idxY + BUF_LEN - 1) % BUF_LEN]
      + (1-a) * m_pY[(m_idxY + BUF_LEN - N) % BUF_LEN]);
}


float SoundProcessor::reverb() {
    // Reverb model:
    // y[n] = -ax[n] + x[n-N] + ay[n-N]
    static float a = 0.8f;
    static float norm = 1.0f;
    static int N = (int)(0.02 * m_sampleRate);

    return norm * (
      - a * m_pX[m_idxX]
      +     m_pX[(m_idxX + BUF_LEN - N) % BUF_LEN]
      + a * m_pY[(m_idxY + BUF_LEN - N) % BUF_LEN]);
}


float SoundProcessor::biQuad() {
    // Filtering operation:
    // y[n] = x[n] + b_1x[n-1] + b_2x[n-2] - a_1y[n-1] - a_2y[n-2]

    // Pole (magnitude and phase).
    static float pm = 0.98f;
    static float pp = (float)(0.1 * PI);
    // Zero (magnitude and phase).
    static float zm = 0.9f;
    static float zp = (float)(0.06 * PI);
    static float norm = 0.5f;

    float b1 = -2*zm*cos(zp);
    float b2 = zm*zm;
    float a1 = -2*pm*cos(pp);
    float a2 = pm*pm;

    return norm * (
               m_pX[m_idxX]
        + b1 * m_pX[(m_idxX + BUF_LEN - 1) % BUF_LEN]
        + b2 * m_pX[(m_idxX + BUF_LEN - 2) % BUF_LEN]
        - a1 * m_pY[(m_idxY + BUF_LEN - 1) % BUF_LEN]
        - a2 * m_pY[(m_idxY + BUF_LEN - 2) % BUF_LEN]);
}


float SoundProcessor::fuzz() {
    // Fuzz operation:
    // y[n] = a trunc(x[n]/a)
    static float T = 0.005f;
    static float G = 5;

    static float limit = 32767 * T;

    float y = m_pX[m_idxX];
    if (y > limit)
    y = limit;
    if (y < -limit)
    y = -limit;
    return G*y;
}


float SoundProcessor::tremolo() {
    // Tremolo model:
    // y[n] = (1 + cos(wn)) x[n]

    static double phi = 5 * 2*PI / m_sampleRate;  // 5Hz oscillator
    static double omega = 0;

    omega = omega + phi;
    return (float)(((1 + cos(omega))/2) * m_pX[m_idxX]);
}


float SoundProcessor::flanger() {
    // Flanger model:
    // y[n] = x[n] + x[n - d ( 1+cos(wn) )]

    static int N = (int)(0.002 * m_sampleRate);  // minimum delay
    static int FD = 2;  // maximum delay factor
    static double phi = 1 * 2*PI / m_sampleRate;  // 1Hz oscillator
    static double omega = 0;

    int d = (int)(N * FD * (1 + cos(omega))/2);
    omega = omega + phi;
    return 0.5f + (m_pX[m_idxX] + m_pX[(m_idxX + BUF_LEN - d) % BUF_LEN]);
}
