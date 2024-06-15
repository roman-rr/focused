#include "mainwindow.h"
#include "macos_helpers.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set the application icon
    a.setWindowIcon(QIcon(":/public/icon.png"));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Focused_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }


    MainWindow w;

    // Register for macOS-specific events
    MacOSHelpers::registerForAppEvents(&w);


    w.show();
    return a.exec();
}
