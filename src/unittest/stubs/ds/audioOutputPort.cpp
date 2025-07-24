#include "audioOutputPort.hpp"
#include "dsError.h"
#include <iostream>

namespace device {

dsError_t AudioOutputPort::enableLEConfig(const bool enable) {
    std::cout << "[AudioOutputPort] LE config is now " << (enable ? "ENABLED" : "DISABLED") << std::endl;
    return dsERR_NONE;  // Always return success for stub
}

}
