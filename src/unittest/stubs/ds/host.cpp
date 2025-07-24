#include "host.hpp"

namespace device {


// Constructor and destructor
Host::Host() {}
Host::~Host() {}

// Dummy implementation
float Host::getCPUTemperature() {
    return 42.5f;  // Simulated CPU temp
}

}
