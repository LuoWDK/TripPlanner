#include "tripplanner_server_clientsocket.h"
#include <QDataStream>
#include <QObject>

ClientSocket::ClientSocket(QObject *parent)
    : QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(ReadClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    next_block_size = 0;
}

void ClientSocket::ReadClient()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_9);

    if (next_block_size == 0)
    {
        if (bytesAvailable() < sizeof(quint16))
        {
            return;
        }
        in >> next_block_size;
    }
    if (bytesAvailable() < next_block_size)
    {
        return;
    }

    quint8 request_type;
    QString from;
    QString to;
    QDate date;
    QTime time;
    quint8 flag;

    in >> request_type;
    if (request_type == 'S')
    {
        in >> from >> to >> date >> time >> flag;
        std::srand(from.length() * 3600 + to.length() * 60 + time.hour());
        int num_trips = std::rand() % 8;
        for (int i = 0; i < num_trips; ++i)
        {
            GenerateRandomTrip(from, to, date, time);
        }

        QDataStream out(this);
        out << quint16(0xFFFF);
    }
    close();
}

void ClientSocket::GenerateRandomTrip(const QString & /* from*/, const QString & /* to */, const QDate &date, const QTime &time)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    quint16 duration = std::rand() % 200;
    out << quint16(0) << date << time << duration << quint8(1) << QString("InterCity");
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    write(block);
}
