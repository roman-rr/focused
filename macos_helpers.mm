#include "macos_helpers.h"
#include <Cocoa/Cocoa.h>

@interface AppDelegateHelper : NSObject <NSApplicationDelegate>
@property (nonatomic, assign) MainWindow *mainWindow;
@end

@implementation AppDelegateHelper

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // Application finished launching
}

- (void)applicationDidBecomeActive:(NSNotification *)notification {
    // Application became active (e.g., clicked from Dock)
    if (self.mainWindow) {
        QMetaObject::invokeMethod(self.mainWindow, "restoreWindow", Qt::QueuedConnection);
    }
}

@end

void MacOSHelpers::registerForAppEvents(MainWindow *mainWindow) {
    AppDelegateHelper *appDelegate = [[AppDelegateHelper alloc] init];
    appDelegate.mainWindow = mainWindow;
    [NSApplication sharedApplication].delegate = appDelegate;
}
