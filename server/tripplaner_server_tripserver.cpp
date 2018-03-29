#include "tripplaner_server_tripserver.h"
#include "tripplanner_server_clientsocket.h"
#include <QMessageBox>

TripServer::TripServer(QObject *parent)
    : QTcpServer(parent)
{
//    if (!listen(QHostAddress::Any, 6666))
//    {
//        qDebug() << QObject::tr("Failed to bind to port");
//        return;
//    }
}

void TripServer::incomingConnection(qintptr socket_id)
{
    ClientSocket *client_socket = new ClientSocket(this);
    client_socket->setSocketDescriptor(socket_id);
}
