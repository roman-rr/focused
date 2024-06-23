#include <Cocoa/Cocoa.h>
#include <QPoint>
#include <QDebug>

QPoint getTrayIconPosition() {
    // Get the array of screens
    NSArray *screenArray = [NSScreen screens];
    if ([screenArray count] == 0) {
        qDebug() << "No screens found";
        return QPoint(0, 0);
    }

    NSScreen *mainScreen = [screenArray objectAtIndex:0];
    CGRect screenFrame = [mainScreen frame];

    // Get the status bar window
    NSWindow *statusBarWindow = nil;
    for (NSWindow *window in [[NSApplication sharedApplication] windows]) {
        // qDebug() << "Window class: " << [[window className] UTF8String] << " Frame: " << window.frame.origin.x << window.frame.origin.y << window.frame.size.width << window.frame.size.height;
        if ([[window className] isEqualToString:@"NSStatusBarWindow"]) {
            statusBarWindow = window;
            break;
        }
    }

    if (statusBarWindow == nil) {
        qDebug() << "Status bar window not found";
        return QPoint(0, 0);
    }

    // Get the frame of the status bar window
    CGRect statusBarFrame = [statusBarWindow frame];

    // Assuming the tray icon is the first icon in the status bar (left to right)
    // Assuming the tray icon size (24x24 pixels)
    CGFloat iconWidth = 24.0; // Change as per the actual icon size
    CGFloat iconHeight = 24.0; // Change as per the actual icon size

    // Get the coordinates for the top-left corner of the tray icon
    CGPoint trayIconTopLeft = CGPointMake(CGRectGetMaxX(statusBarFrame) - iconWidth, CGRectGetMinY(statusBarFrame));

    return QPoint(static_cast<int>(trayIconTopLeft.x), static_cast<int>(trayIconTopLeft.y));
}
