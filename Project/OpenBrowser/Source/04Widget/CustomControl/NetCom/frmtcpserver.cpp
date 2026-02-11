//01frame
#include "frmtcpserver.h"
#include "ui_frmtcpserver.h"
#include "quihelper.h"

//02control
#include "setupini.h"


frmTcpServer::frmTcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmTcpServer) {
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmTcpServer::~frmTcpServer() {
    delete ui;
}

void frmTcpServer::initForm() {
    isOk = false;
    tcpServer = new TcpServer(this);
    connect(tcpServer, SIGNAL(clientConnected(QString, int)),
            this, SLOT(clientConnected(QString, int)));
    connect(tcpServer, SIGNAL(clientDisconnected(QString, int)),
            this, SLOT(clientDisconnected(QString, int)));
    connect(tcpServer, SIGNAL(sendData(QString, int, QString)),
            this, SLOT(sendData(QString, int, QString)));
    connect(tcpServer, SIGNAL(receiveData(QString, int, QString)),
            this, SLOT(receiveData(QString, int, QString)));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));
    ui->cboxInterval->addItems(SetUpIni::net_Intervals);
    ui->cboxData->addItems(SetUpIni::net_Datas);
}

void frmTcpServer::initConfig() {
    ui->ckHexSend->setChecked(SetUpIni::HexSendTcpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->ckHexReceive->setChecked(SetUpIni::HexReceiveTcpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->ckAscii->setChecked(SetUpIni::AsciiTcpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->ckDebug->setChecked(SetUpIni::DebugTcpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->ckAutoSend->setChecked(SetUpIni::AutoSendTcpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(SetUpIni::IntervalTcpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    ui->txtListenPort->setText(QString::number(SetUpIni::TcpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    timer->setInterval(SetUpIni::IntervalTcpServer);
    SetUpIni::AutoSendTcpServer ? timer->start() : timer->stop();
}

void frmTcpServer::saveConfig() {
    SetUpIni::HexSendTcpServer = ui->ckHexSend->isChecked();
    SetUpIni::HexReceiveTcpServer = ui->ckHexReceive->isChecked();
    SetUpIni::AsciiTcpServer = ui->ckAscii->isChecked();
    SetUpIni::DebugTcpServer = ui->ckDebug->isChecked();
    SetUpIni::AutoSendTcpServer = ui->ckAutoSend->isChecked();
    SetUpIni::IntervalTcpServer = ui->cboxInterval->currentText().toInt();
    SetUpIni::TcpListenPort = ui->txtListenPort->text().trimmed().toInt();
    SetUpIni::writeConfig();
    timer->setInterval(SetUpIni::IntervalTcpServer);
    SetUpIni::AutoSendTcpServer ? timer->start() : timer->stop();
}

void frmTcpServer::append(int type, const QString &data, bool clear) {
    static int currentCount = 0;
    static int maxCount = 100;
    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }
    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }
    if (ui->ckShow->isChecked()) {
        return;
    }
    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");
    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain->setTextColor(QColor("darkgreen"));
    } else {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("red"));
    }
    strData = QString("时间[%1] %2: %3")
              .arg(QDateTime::currentDateTime().
                   toString("yyyy_MM_dd_hh_mm_ss")).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}

void frmTcpServer::sendData(const QString &data) {
    if (ui->ckAll->isChecked()) {
        tcpServer->writeData(data);
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            tcpServer->writeData(list.at(0), list.at(1).toInt(), data);
        }
    }
}

void frmTcpServer::clientConnected(const QString &ip, int port) {
    QString str = QString("%1:%2").arg(ip).arg(port);
    ui->listWidget->addItem(str);
    ui->labCount->setText(QString("共 %1 个连接").arg(ui->listWidget->count()));
}

void frmTcpServer::clientDisconnected(const QString &ip, int port) {
    int row = -1;
    QString str = QString("%1:%2").arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->text() == str) {
            row = i;
            break;
        }
    }
    ui->listWidget->takeItem(row);
    ui->labCount->setText(QString("共 %1 个连接").arg(ui->listWidget->count()));
}

void frmTcpServer::sendData(const QString &ip, int port, const QString &data) {
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    bool error = (data.contains("下线") || data.contains("离线"));
    append(error ? 1 : 0, str);
}

void frmTcpServer::receiveData(const QString &ip, int port, const QString &data) {
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(1, str);
}

void frmTcpServer::on_btnListen_clicked() {
    if (ui->btnListen->text() == "监听") {
        isOk = tcpServer->start();
        if (isOk) {
            append(0, "监听成功");
            ui->btnListen->setText("关闭");
        }
    } else {
        isOk = false;
        tcpServer->stop();
        ui->btnListen->setText("监听");
    }
}

void frmTcpServer::on_btnSave_clicked() {
    QString data = ui->txtMain->toPlainText();
    if (data.length() <= 0) {
        return;
    }
    QString fileName = QString("%1/%2.txt").arg(QUIHelper::AppPath())
                       .arg(QDateTime::currentDateTime().
                            toString("yyyy_MM_dd_hh_mm_ss"));
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(data.toUtf8());
        file.close();
    }
    on_btnClear_clicked();
}

void frmTcpServer::on_btnClear_clicked() {
    append(0, "", true);
}

void frmTcpServer::on_btnSend_clicked() {
    if (!isOk) {
        return;
    }
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }
    sendData(data);
}

/*-----------------------------------tcp客户端-----------------------------------*/
TcpClient::TcpClient(QObject *parent) :  QTcpSocket(parent) {
    ip = "127.0.0.1";
    port = 6000;
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

void TcpClient::setIP(const QString &ip) {
    this->ip = ip;
}

QString TcpClient::getIP() const {
    return this->ip;
}

void TcpClient::setPort(int port) {
    this->port = port;
}

int TcpClient::getPort() const {
    return this->port;
}

void TcpClient::readData() {
    QByteArray data = this->readAll();
    if (data.length() <= 0) {
        return;
    }
    QString buffer;
    if (SetUpIni::HexReceiveTcpServer) {
        buffer = QUIHelper::ByteArrayToHexStr(data);
    } else if (SetUpIni::AsciiTcpServer) {
        buffer = QUIHelper::ByteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }
    emit receiveData(ip, port, buffer);
    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (SetUpIni::DebugTcpServer) {
        int count = SetUpIni::net_Keys.count();
        for (int i = 0; i < count; i++) {
            if (SetUpIni::net_Keys.at(i) == buffer) {
                sendData(SetUpIni::net_Values.at(i));
                break;
            }
        }
    }
}

void TcpClient::sendData(const QString &data) {
    QByteArray buffer;
    if (SetUpIni::HexSendTcpServer) {
        buffer = QUIHelper::HexStrToByteArray(data);
    } else if (SetUpIni::AsciiTcpServer) {
        buffer = QUIHelper::AsciiStrToByteArray(data);
    } else {
        buffer = data.toLatin1();
    }
    this->write(buffer);
    emit sendData(ip, port, data);
}



/*-----------------------------------tcp服务器-----------------------------------*/
TcpServer::TcpServer(QObject *parent) : QTcpServer(parent) {
}

void TcpServer::incomingConnection(qintptr handle) {
    TcpClient *client = new TcpClient(this);
    client->setSocketDescriptor(handle);
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(sendData(QString, int, QString)), this, SIGNAL(sendData(QString, int, QString)));
    connect(client, SIGNAL(receiveData(QString, int, QString)), this, SIGNAL(receiveData(QString, int, QString)));
    QString ip = client->peerAddress().toString();
    int port = client->peerPort();
    client->setIP(ip);
    client->setPort(port);
    emit clientConnected(ip, port);
    emit sendData(ip, port, "客户端上线");
    //连接后加入链表
    clients.append(client);
}

void TcpServer::disconnected() {
    TcpClient *client = (TcpClient *)sender();
    QString ip = client->getIP();
    int port = client->getPort();
    emit clientDisconnected(ip, port);
    emit sendData(ip, port, "客户端下线");
    //断开连接后从链表中移除
    clients.removeOne(client);
}

bool TcpServer::start() {
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    bool ok = listen(QHostAddress::AnyIPv4, SetUpIni::TcpListenPort);
#else
    bool ok = listen(QHostAddress::Any, SetUpIni::TcpListenPort);
#endif
    return ok;
}

void TcpServer::stop() {
    foreach (TcpClient *client, clients) {
        client->disconnectFromHost();
    }
    this->close();
}

void TcpServer::writeData(const QString &ip, int port, const QString &data) {
    foreach (TcpClient *client, clients) {
        if (client->peerAddress().toString() == ip && client->peerPort() == port) {
            client->sendData(data);
            break;
        }
    }
}

void TcpServer::writeData(const QString &data) {
    foreach (TcpClient *client, clients) {
        client->sendData(data);
    }
}
