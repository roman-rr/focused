#include "privileges.h"

#if defined(Q_OS_MAC)
    include "macos_privileges.mm"

// Right now only mac
// #elif defined(Q_OS_WIN)
//     #include "windows_privileges.h"
//     #include "windows_privileges.cpp"
#endif
