#ifndef TRIPPLANER_SERVER_TRIPSERVER_H
#define TRIPPLANER_SERVER_TRIPSERVER_H

#include <QObject>
#include <QTcpServer>

class TripServer : public QTcpServer
{
    Q_OBJECT
public:
    TripServer(QObject *parent = 0);
private:
    void incomingConnection(qintptr socket_id);
};

#endif // TRIPPLANER_SERVER_TRIPSERVER_H
