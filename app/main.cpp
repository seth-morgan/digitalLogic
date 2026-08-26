/**
 * @file main.cpp
 * @brief Application entry point for the Digital Logic Simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/MainWindow.h"

#include "digitallogic/ui/AppTheme.h"

#include <QApplication>

/**
 * @brief Boots the Qt app, applies the shared stylesheet, and runs the main window.
 */
int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Digital Logic Simulator"));
    QApplication::setOrganizationName(QStringLiteral("DigitalLogic"));
    // Central theme CSS so all windows share the same look without per-widget styling.
    application.setStyleSheet(digitallogic::ui::AppTheme::applicationStylesheet());

    digitallogic::ui::MainWindow mainWindow;
    mainWindow.show();

    // Hand control to Qt's event loop until the last window closes.
    return QApplication::exec();
}
