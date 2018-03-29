#ifndef TRIPPLANNER_CLIENT_TRIPPLANNER_H
#define TRIPPLANNER_CLIENT_TRIPPLANNER_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QDateEdit>
#include <QLabel>
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>
#include <QTimeEdit>
#include <QTableWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QButtonGroup>
#include <QTcpSocket>
#include <QAbstractButton>

namespace Ui {
class TripPlanner;
}

class TripPlanner : public QDialog
{
    Q_OBJECT

public:
    explicit TripPlanner(QWidget *parent = 0);
    ~TripPlanner();

private:
    void CloseConnection();

    Ui::TripPlanner *ui;
    QDialogButtonBox *button_box_;
    QPushButton *search_button_;
    QPushButton *stop_button_;
    QPushButton *quit_button_;
    QButtonGroup *group_;
    QTcpSocket tcp_socket_;
    quint16 next_block_size_;

private slots:
    void ConnectToServer();
    void SendRequest();
    void UpdateTableWidget();
    void StopSearch();
    void ConnectClosedByServer();
    void Error();
};

#endif // TRIPPLANNER_CLIENT_TRIPPLANNER_H
