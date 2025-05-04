#include "kmainwindow.h"
#include <QtWidgets/QApplication>
#include <QSettings>

void addToStartup() {
    QString program = QCoreApplication::applicationFilePath(); 
    QSettings settings("D:/桌面/qt/build/bin/Release/QtFile.exe", QSettings::Registry64Format);
    settings.setValue("MyQtApp", program);  // 将你的应用程序路径添加到启动项
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KMainWindow w;
    w.setWindowIcon(QIcon(":/icons/normal/option.png"));
    w.show();
    addToStartup(); 

    return a.exec();
}
