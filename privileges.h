#ifndef PRIVILEGES_H
#define PRIVILEGES_H

#include <string>

extern "C" bool execute_with_privileges(const std::string &command);

#endif // PRIVILEGES_H
