#include "kmainwindow.h"
#include <QtWidgets/QApplication>
#include <QSettings>

void addToStartup() {
    QString program = QCoreApplication::applicationFilePath(); 
    QSettings settings("D:/����/qt/build/bin/Release/QtFile.exe", QSettings::Registry64Format);
    settings.setValue("MyQtApp", program);  // �����Ӧ�ó���·����ӵ�������
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
