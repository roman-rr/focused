#ifndef MACOS_PRIVILEGES_H
#define MACOS_PRIVILEGES_H

#include <string>

extern "C" {
    bool initialize_authorization();
    bool execute_with_privileges(const std::string &command);
}

#endif // MACOS_PRIVILEGES_H
