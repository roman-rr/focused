#include "macos_privileges.h"
#include <string>
#include <iostream>
#include <unistd.h> // For access, sleep
#include <stdlib.h> // For system
#include <sys/stat.h> // For chmod
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include <cerrno> // For errno
#include <cstring> // For strerror

extern "C" {
    bool execute_with_privileges(const std::string &command) {
        // Authorization services setup
        AuthorizationRef authorizationRef;
        OSStatus status;

        status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);
        if (status != errAuthorizationSuccess) {
            std::cerr << "AuthorizationCreate failed with status " << status << std::endl;
            return false;
        }
        std::cerr << "AuthorizationCreate succeeded." << std::endl;

        AuthorizationItem right = { kAuthorizationRightExecute, 0, NULL, 0 };
        AuthorizationRights rights = { 1, &right };
        AuthorizationFlags flags = kAuthorizationFlagDefaults | kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize | kAuthorizationFlagExtendRights;

        status = AuthorizationCopyRights(authorizationRef, &rights, kAuthorizationEmptyEnvironment, flags, NULL);
        if (status != errAuthorizationSuccess) {
            std::cerr << "AuthorizationCopyRights failed with status " << status << std::endl;
            AuthorizationFree(authorizationRef, kAuthorizationFlagDefaults);
            return false;
        }
        std::cerr << "AuthorizationCopyRights succeeded." << std::endl;

        // Construct the command with sudo
        const char *args[] = { "-c", command.c_str(), NULL };
        FILE *pipe = NULL;

        std::cerr << "Executing command: /bin/sh -c \"" << command << "\"" << std::endl;
        status = AuthorizationExecuteWithPrivileges(authorizationRef, "/bin/sh", kAuthorizationFlagDefaults, (char *const *)args, &pipe);
        if (status != errAuthorizationSuccess) {
            std::cerr << "AuthorizationExecuteWithPrivileges failed with status " << status << std::endl;
            AuthorizationFree(authorizationRef, kAuthorizationFlagDefaults);
            return false;
        }
        std::cerr << "AuthorizationExecuteWithPrivileges succeeded." << std::endl;

        // Clean up
        AuthorizationFree(authorizationRef, kAuthorizationFlagDefaults);
        std::cerr << "Cleaned up authorization." << std::endl;

        return true;
    }
}
