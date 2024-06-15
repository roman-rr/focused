#include "macos_privileges.h"
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <cerrno>
#include <cstring>

AuthorizationRef authorizationRef = NULL;

extern "C" {
    bool initialize_authorization() {
        if (authorizationRef != NULL) {
            return true; // Authorization already initialized
        }

        OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);
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
            authorizationRef = NULL;
            return false;
        }
        std::cerr << "AuthorizationCopyRights succeeded." << std::endl;

        return true;
    }

    bool execute_with_privileges(const std::string &command) {
        if (!initialize_authorization()) {
            return false;
        }

        const char *args[] = { "-c", command.c_str(), NULL };
        FILE *pipe = NULL;

        std::cerr << "Executing command: /bin/sh -c \"" << command << "\"" << std::endl;
        OSStatus status = AuthorizationExecuteWithPrivileges(authorizationRef, "/bin/sh", kAuthorizationFlagDefaults, (char *const *)args, &pipe);
        if (status != errAuthorizationSuccess) {
            std::cerr << "AuthorizationExecuteWithPrivileges failed with status " << status << std::endl;
            AuthorizationFree(authorizationRef, kAuthorizationFlagDefaults);
            authorizationRef = NULL;
            return false;
        }
        std::cerr << "AuthorizationExecuteWithPrivileges succeeded." << std::endl;

        // Optionally read the output of the command
        if (pipe) {
            char buffer[128];
            std::string result;
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                result += buffer;
            }
            std::cerr << "Command output: " << result << std::endl;
            int status_code = pclose(pipe);
            std::cerr << "Command executed with status code " << status_code << std::endl;
        }

        return true;
    }
}
