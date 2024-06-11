#import <Foundation/Foundation.h>
#import <LocalAuthentication/LocalAuthentication.h>

bool authenticate_user() {
    LAContext *context = [[LAContext alloc] init];
    NSError *error = nil;

    // Check if biometric authentication is available
    if ([context canEvaluatePolicy:LAPolicyDeviceOwnerAuthentication error:&error]) {
        __block bool authenticated = false;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);

        // Evaluate policy with biometrics or passcode
        [context evaluatePolicy:LAPolicyDeviceOwnerAuthentication
                localizedReason:@"Authenticate to modify system settings"
                          reply:^(BOOL success, NSError * _Nullable error) {
            authenticated = success;
            dispatch_semaphore_signal(semaphore);
        }];

        // Wait for the authentication result
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

        return authenticated;
    } else {
        NSLog(@"Biometric authentication not available: %@", error.localizedDescription);
        return false;
    }
}
