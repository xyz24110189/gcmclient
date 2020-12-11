#ifndef UTILS_H
#define UTILS_H
#include <string>

namespace GCM {
    namespace Utils {

        std::string getMacAddress();

        std::string getMachineID();

        void Sleep(unsigned int ms);

    }
}

#endif // UTILS_H
