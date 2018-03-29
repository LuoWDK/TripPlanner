#include "tripplaner_server_tripserver.h"
#include <QApplication>
#include <QPushButton>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TripServer w;
    if (!w.listen(QHostAddress::Any, 6666))
    {
        qDebug() << QObject::tr("Failed to bind to port");
        return 1;
    }
    else
    {
        qDebug() << QObject::tr("Bind to port successfully");
    }

    QPushButton quit_btn(QObject::tr("&Quit"));
    quit_btn.setWindowTitle(QObject::tr("Trip Server"));
    QObject::connect(&quit_btn, SIGNAL(clicked()), &a, SLOT(quit()));
    quit_btn.show();
//    w.show();

    return a.exec();
}
