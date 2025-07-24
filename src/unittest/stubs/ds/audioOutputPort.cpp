#include "audioOutputPort.hpp"
#include <iostream>

namespace device {

// Constructor
AudioOutputPort::AudioOutputPort() {
    std::cout << "[AudioOutputPort] Constructed" << std::endl;
}

// Destructor
AudioOutputPort::~AudioOutputPort() {
    std::cout << "[AudioOutputPort] Destroyed" << std::endl;
}

// Stub implementation of enableLEConfig
bool AudioOutputPort::enableLEConfig(bool enable) {
    leConfigEnabled = enable;
    std::cout << "[AudioOutputPort] LE Config set to: " << (enable ? "Enabled" : "Disabled") << std::endl;
    return true;  // Always returns success for testing
}

}
