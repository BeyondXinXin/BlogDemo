#ifndef FRMTCPSERVER_H
#define FRMTCPSERVER_H

//01frame
#include "stable.h"



class TcpClient;
class TcpServer;

namespace Ui {
    class frmTcpServer;
}

class frmTcpServer : public QWidget {
    Q_OBJECT

  public:
    explicit frmTcpServer(QWidget *parent = nullptr);
    ~frmTcpServer();

  private:
    Ui::frmTcpServer *ui;

    bool isOk;
    TcpServer *tcpServer;
    QTimer *timer;

  private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void append(int type, const QString &data, bool clear = false);

  private slots:
    void sendData(const QString &data);

    void clientConnected(const QString &ip, int port);
    void clientDisconnected(const QString &ip, int port);
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

  private slots:
    void on_btnListen_clicked();
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
};


class TcpClient : public QTcpSocket {
    Q_OBJECT
  public:
    explicit TcpClient(QObject *parent = nullptr);

  private:
    QString ip;
    int port;

  public:
    void setIP(const QString &ip);
    QString getIP()const;

    void setPort(int port);
    int getPort()const;

  private slots:
    void readData();

  signals:
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

  public slots:
    void sendData(const QString &data);

};


class TcpServer : public QTcpServer {
    Q_OBJECT
  public:
    explicit TcpServer(QObject *parent = nullptr);

  private:
    QList<TcpClient *> clients;

  protected:
    void incomingConnection(qintptr  handle);

  private slots:
    void disconnected();

  signals:
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);
    void clientConnected(const QString &ip, int port);
    void clientDisconnected(const QString &ip, int port);

  public slots:
    //启动服务
    bool start();
    //停止服务
    void stop();

    //指定连接发送数据
    void writeData(const QString &ip, int port, const QString &data);

    //对所有连接发送数据
    void writeData(const QString &data);

};

#endif // FRMTCPSERVER_H
