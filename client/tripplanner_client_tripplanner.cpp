#include "tripplanner_client_tripplanner.h"
#include "ui_tripplanner_client_tripplanner.h"
#include <QHostAddress>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QByteArray>
#include <QDataStream>
#include <QString>

TripPlanner::TripPlanner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TripPlanner)
{
    ui->setupUi(this);
    button_box_ = new QDialogButtonBox(Qt::Vertical, this);
    button_box_->setGeometry(440, 20, 70, 400);
    search_button_ = button_box_->addButton(tr("&Search"), QDialogButtonBox::ActionRole);
    stop_button_ = button_box_->addButton(tr("S&top"), QDialogButtonBox::ActionRole);
    quit_button_ = button_box_->addButton(tr("&Quit"), QDialogButtonBox::RejectRole);
    stop_button_->setEnabled(false);

//    QDate date = QDate::currentDate();
//    QTime time = QTime::currentTime();
//    ui->date_edit_->setDate(date);
//    ui->time_edit_->setTime(QTime(time.hour(), 0));
    QDateTime date_time = QDateTime::currentDateTimeUtc().toLocalTime();
    ui->date_edit_->setDate(date_time.date());
    ui->time_edit_->setTime(QTime(date_time.time().hour(), 0));

    group_ = new QButtonGroup(this);
    group_->setExclusive(true);
    ui->departure_rb_->setChecked(true);
    group_->addButton(ui->departure_rb_);
    group_->addButton(ui->arrival_rb_);

    ui->status_pb_->hide();
    ui->status_pb_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);

    ui->result_tw_->verticalHeader()->hide();
    ui->result_tw_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(search_button_, SIGNAL(clicked()), this, SLOT(ConnectToServer()));
    connect(stop_button_, SIGNAL(clicked()), this, SLOT(StopSearch()));
    connect(quit_button_, SIGNAL(clicked()), this, SLOT(close()));

    connect(&tcp_socket_, SIGNAL(connected()), this, SLOT(SendRequest()));
    connect(&tcp_socket_, SIGNAL(disconnected()), this, SLOT(ConnectClosedByServer()));
    connect(&tcp_socket_, SIGNAL(readyRead()), this, SLOT(UpdateTableWidget()));
    connect(&tcp_socket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error()));
}

TripPlanner::~TripPlanner()
{
    delete ui;
}

void TripPlanner::ConnectToServer()
{
    tcp_socket_.connectToHost(QHostAddress::LocalHost, 6666);

    ui->result_tw_->setRowCount(0);
    search_button_->setEnabled(false);
    stop_button_->setEnabled(true);
    ui->status_label_->setText(tr("Connecting to server %1 at port %2").arg(QHostAddress(QHostAddress::LocalHost).toString()).arg(8010));
    ui->status_pb_->show();

    next_block_size_ = 0;
}

void TripPlanner::SendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out << quint16(0)
        << quint8('S')
        << ui->from_cb_->currentText()
        << ui->to_cb_->currentText()
        << ui->date_edit_->date()
        << ui->time_edit_->time();

    if (ui->departure_rb_->isChecked())
    {
        out << quint8('D');
    }
    else
    {
        out << quint8('A');
    }
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    tcp_socket_.write(block);

    ui->status_label_->setText(tr("Sending request..."));
}

void TripPlanner::UpdateTableWidget()
{
    QDataStream in(&tcp_socket_);
    in.setVersion(QDataStream::Qt_5_9);

    forever
    {
        int row = ui->result_tw_->rowCount();

        if (next_block_size_ == 0)
        {
            if (tcp_socket_.bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> next_block_size_;
        }
        if (next_block_size_ == 0xFFFF)
        {
            CloseConnection();
            ui->status_label_->setText(tr("Found %1 trip(s)").arg(row));
            break;
        }
        if (tcp_socket_.bytesAvailable() < next_block_size_)
        {
            break;
        }

        QDate date;
        QTime departure_time;
        QTime arrival_time;
        quint16 duration;
        quint8 changes;
        QString train_type;

        in >> date >> departure_time >> duration >> changes >> train_type;
        arrival_time = departure_time.addSecs(duration * 60);
        ui->result_tw_->setRowCount(row + 1);

        QStringList fields;
        fields << date.toString(Qt::LocalDate)
               << departure_time.toString(tr("hh:mm"))
               << arrival_time.toString(tr("hh:mm"))
               << tr("%1 hr %2 min").arg(duration / 60).arg(duration % 60)
               << QString::number(changes)
               << train_type;
        for (int i = 0; i < fields.count(); ++i)
        {
            ui->result_tw_->setItem(row, i, new QTableWidgetItem(fields[i]));
        }
        next_block_size_ = 0;
    }
}

void TripPlanner::CloseConnection()
{
    tcp_socket_.close();
    search_button_->setEnabled(true);
    stop_button_->setEnabled(false);
    ui->status_pb_->hide();
}

void TripPlanner::StopSearch()
{
    ui->status_label_->setText(tr("Search stopped"));
    CloseConnection();
}

void TripPlanner::ConnectClosedByServer()
{
    if (next_block_size_ != 0xFFFF)
    {
        ui->status_label_->setText(tr("Error: Connection closed by server"));
    }
    CloseConnection();
}

void TripPlanner::Error()
{
    ui->status_label_->setText(tcp_socket_.errorString());
    CloseConnection();
}
