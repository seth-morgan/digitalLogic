#include "digitallogic/ui/MainWindow.h"

#include "digitallogic/ui/AppTheme.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Digital Logic Simulator"));
    QApplication::setOrganizationName(QStringLiteral("DigitalLogic"));
    application.setStyleSheet(digitallogic::ui::AppTheme::applicationStylesheet());

    digitallogic::ui::MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
