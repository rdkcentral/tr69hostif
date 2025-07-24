#include "audioOutputPort.hpp"
#include <iostream>

typedef enum {
    dsERR_NONE = 0,
    dsERR_GENERAL = 1
    // Add more codes if needed
} dsError_t;

namespace device {

dsError_t AudioOutputPort::enableLEConfig(const bool enable) {
    leConfigEnabled = enable;
    std::cout << "[AudioOutputPort] LE config is now " << (enable ? "ENABLED" : "DISABLED") << std::endl;
    return dsERR_NONE;  // Always return success for stub
}

}
