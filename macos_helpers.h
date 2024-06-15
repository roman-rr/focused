#ifndef MACOS_HELPERS_H
#define MACOS_HELPERS_H

#include <QObject>
#include "mainwindow.h"

class MacOSHelpers : public QObject
{
    Q_OBJECT

public:
    static void registerForAppEvents(MainWindow *mainWindow);
};

#endif // MACOS_HELPERS_H
