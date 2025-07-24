#include "host.hpp"

namespace device {

// Define static singleton instance
Host& Host::getInstance() {
    static Host instance;
    return instance;
}

// Constructor and destructor
Host::Host() {}
Host::~Host() {}

// Dummy implementation
float Host::getCPUTemperature() {
    return 42.5f;  // Simulated CPU temp
}

}
