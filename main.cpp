#include "mainwindow.h"
#include "macos_helpers.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

void setupTranslator(QApplication &app) {
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Focused_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/public/icon.png"));

    setupTranslator(app);

    MainWindow mainWindow;
    MacOSHelpers::registerForAppEvents(&mainWindow);

    mainWindow.show();

    return app.exec();
}
