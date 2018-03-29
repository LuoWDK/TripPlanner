#ifndef TRIPPLANNER_SERVER_CLIENTSOCKET_H
#define TRIPPLANNER_SERVER_CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QDate>
#include <QTime>

class ClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
    ClientSocket(QObject *parent = 0);

private slots:
    void ReadClient();

private:
    void GenerateRandomTrip(const QString &from, const QString &to, const QDate &date, const QTime &time);
    quint16 next_block_size;
};

#endif // TRIPPLANNER_SERVER_CLIENTSOCKET_H
