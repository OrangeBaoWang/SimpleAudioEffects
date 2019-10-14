#include "paudiopipe.h"

int main() {
    PAudioPipe a;

    // Set to true to list audio devices and see their info.
    bool dispDevices = true;
    if (dispDevices) {
        a.listDevices();

        // Default input device.
        unsigned int inputDeviceIdx = 0;
        a.getDeviceInfo(inputDeviceIdx);

        // Default output device.
        unsigned int outputDeviceIdx = 1;
        a.getDeviceInfo(outputDeviceIdx);
    }
    a.start();
    return 0;
}
