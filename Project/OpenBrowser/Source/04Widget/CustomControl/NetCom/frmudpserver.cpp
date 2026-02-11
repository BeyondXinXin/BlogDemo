//01frame
#include "frmudpserver.h"
#include "ui_frmudpserver.h"
#include "quihelper.h"

//02control
#include "setupini.h"

frmUdpServer::frmUdpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmUdpServer) {
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmUdpServer::~frmUdpServer() {
    delete ui;
}

void frmUdpServer::initForm() {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(SetUpIni::net_Intervals);
    ui->cboxData->addItems(SetUpIni::net_Datas);
}

void frmUdpServer::initConfig() {
    ui->ckHexSend->setChecked(SetUpIni::HexSendUdpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(SetUpIni::HexReceiveUdpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(SetUpIni::AsciiUdpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(SetUpIni::DebugUdpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(SetUpIni::AutoSendUdpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(SetUpIni::IntervalUdpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(SetUpIni::UdpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(SetUpIni::UdpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(SetUpIni::UdpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    timer->setInterval(SetUpIni::IntervalUdpServer);
    SetUpIni::AutoSendUdpServer ? timer->start() : timer->stop();
}

void frmUdpServer::saveConfig() {
    SetUpIni::HexSendUdpServer = ui->ckHexSend->isChecked();
    SetUpIni::HexReceiveUdpServer = ui->ckHexReceive->isChecked();
    SetUpIni::AsciiUdpServer = ui->ckAscii->isChecked();
    SetUpIni::DebugUdpServer = ui->ckDebug->isChecked();
    SetUpIni::AutoSendUdpServer = ui->ckAutoSend->isChecked();
    SetUpIni::IntervalUdpServer = ui->cboxInterval->currentText().toInt();
    SetUpIni::UdpServerIP = ui->txtServerIP->text().trimmed();
    SetUpIni::UdpServerPort = ui->txtServerPort->text().trimmed().toInt();
    SetUpIni::UdpListenPort = ui->txtListenPort->text().trimmed().toInt();
    SetUpIni::writeConfig();

    timer->setInterval(SetUpIni::IntervalUdpServer);
    SetUpIni::AutoSendUdpServer ? timer->start() : timer->stop();
}

void frmUdpServer::append(int type, const QString &data, bool clear) {
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

void frmUdpServer::readData() {
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;

    while (udpSocket->hasPendingDatagrams()) {
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(), data.size(), &host, &port);

        if (SetUpIni::HexReceiveUdpServer) {
            buffer = QUIHelper::ByteArrayToHexStr(data);
        } else if (SetUpIni::AsciiUdpServer) {
            buffer = QUIHelper::ByteArrayToAsciiStr(data);
        } else {
            buffer = QString(data);
        }

        QString ip = host.toString();
        if (ip.isEmpty()) {
            continue;
        }

        QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(buffer);
        append(1, str);

        if (SetUpIni::DebugUdpServer) {
            int count = SetUpIni::net_Keys.count();
            for (int i = 0; i < count; i++) {
                if (SetUpIni::net_Keys.at(i) == buffer) {
                    sendData(ip, port, SetUpIni::net_Values.at(i));
                    break;
                }
            }
        }
    }
}

void frmUdpServer::sendData(const QString &ip, int port, const QString &data) {
    QByteArray buffer;
    if (SetUpIni::HexSendUdpServer) {
        buffer = QUIHelper::HexStrToByteArray(data);
    } else if (SetUpIni::AsciiUdpServer) {
        buffer = QUIHelper::AsciiStrToByteArray(data);
    } else {
        buffer = data.toLatin1();
    }

    udpSocket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);

}

void frmUdpServer::on_btnListen_clicked() {
    if (ui->btnListen->text() == "监听") {
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
        bool ok = udpSocket->bind(QHostAddress::AnyIPv4, SetUpIni::UdpListenPort);
#else
        bool ok = udpSocket->bind(QHostAddress::Any, SetUpIni::UdpListenPort);
#endif
        if (ok) {
            ui->btnListen->setText("关闭");
            append(0, "监听成功");
        }
    } else {
        udpSocket->abort();
        ui->btnListen->setText("监听");
    }
}

void frmUdpServer::on_btnSave_clicked() {
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

void frmUdpServer::on_btnClear_clicked() {
    append(0, "", true);
}

void frmUdpServer::on_btnSend_clicked() {
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(SetUpIni::UdpServerIP, SetUpIni::UdpServerPort, data);
}
