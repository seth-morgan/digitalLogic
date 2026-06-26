#include "digitallogic/ui/MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Digital Logic Simulator"));
    QApplication::setOrganizationName(QStringLiteral("DigitalLogic"));

    digitallogic::ui::MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
