// 01 Frame includes
#include "setupini.h"
#include "quihelper.h"


//---------------------全局变量（ini内）//
QString SetUpIni::ConfigFile = "config.ini";
int SetUpIni::Ini_Int_eg = 5;
QString SetUpIni::SendFileName = "send.txt";
QString SetUpIni::DeviceFileName = "device.txt";
bool SetUpIni::HexSendTcpClient = false;
bool SetUpIni::HexReceiveTcpClient = false;
bool SetUpIni::AsciiTcpClient = false;
bool SetUpIni::DebugTcpClient = false;
bool SetUpIni::AutoSendTcpClient = false;
int SetUpIni::IntervalTcpClient = 1000;
QString SetUpIni::TcpServerIP = "127.0.0.1";
int SetUpIni::TcpServerPort = 6000;

bool SetUpIni::HexSendTcpServer = false;
bool SetUpIni::HexReceiveTcpServer = false;
bool SetUpIni::AsciiTcpServer = false;
bool SetUpIni::DebugTcpServer = false;
bool SetUpIni::AutoSendTcpServer = false;
int SetUpIni::IntervalTcpServer = 1000;
int SetUpIni::TcpListenPort = 6000;

bool SetUpIni::HexSendUdpServer = false;
bool SetUpIni::HexReceiveUdpServer = false;
bool SetUpIni::AsciiUdpServer = false;
bool SetUpIni::DebugUdpServer = false;
bool SetUpIni::AutoSendUdpServer = false;
int SetUpIni::IntervalUdpServer = 1000;
int SetUpIni::UdpListenPort = 6000;
QString SetUpIni::UdpServerIP = "127.0.0.1";
int SetUpIni::UdpServerPort = 6000;

QStringList SetUpIni::net_Intervals = QStringList();
QStringList SetUpIni::net_Datas = QStringList();
QStringList SetUpIni::net_Keys = QStringList();
QStringList SetUpIni::net_Values = QStringList();

//---------------------全局变量（ini外）//
int SetUpIni::NoIni_int_eg = 5;
QMap<int, QRectF> SetUpIni::points;

/*-----------------------------------
 * 读取配置文件
 * -----------------------------------*/
//读取配置文件,在main函数最开始加载程序载入
void SetUpIni::readConfig() {

    if (!checkConfig()) {
        return;
    }

    QSettings set(SetUpIni::ConfigFile, QSettings::IniFormat);
    set.beginGroup("AppConfig");
    SetUpIni::Ini_Int_eg = set.value("ini_int_eg", SetUpIni::Ini_Int_eg).toInt();
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    SetUpIni::HexSendTcpClient = set.value("HexSendTcpClient").toBool();
    SetUpIni::HexReceiveTcpClient = set.value("HexReceiveTcpClient").toBool();
    SetUpIni::AsciiTcpClient = set.value("AsciiTcpClient").toBool();
    SetUpIni::DebugTcpClient = set.value("DebugTcpClient").toBool();
    SetUpIni::AutoSendTcpClient = set.value("AutoSendTcpClient").toBool();
    SetUpIni::IntervalTcpClient = set.value("IntervalTcpClient").toInt();
    SetUpIni::TcpServerIP = set.value("TcpServerIP").toString();
    SetUpIni::TcpServerPort = set.value("TcpServerPort").toInt();
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    SetUpIni::HexSendTcpServer = set.value("HexSendTcpServer").toBool();
    SetUpIni::HexReceiveTcpServer = set.value("HexReceiveTcpServer").toBool();
    SetUpIni::AsciiTcpServer = set.value("AsciiTcpServer").toBool();
    SetUpIni::DebugTcpServer = set.value("DebugTcpServer").toBool();
    SetUpIni::AutoSendTcpServer = set.value("AutoSendTcpServer").toBool();
    SetUpIni::IntervalTcpServer = set.value("IntervalTcpServer").toInt();
    SetUpIni::TcpListenPort = set.value("TcpListenPort").toInt();
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    SetUpIni::HexSendUdpServer = set.value("HexSendUdpServer").toBool();
    SetUpIni::HexReceiveUdpServer = set.value("HexReceiveUdpServer").toBool();
    SetUpIni::AsciiUdpServer = set.value("AsciiUdpServer").toBool();
    SetUpIni::DebugUdpServer = set.value("DebugUdpServer").toBool();
    SetUpIni::AutoSendUdpServer = set.value("AutoSendUdpServer").toBool();
    SetUpIni::IntervalUdpServer = set.value("IntervalUdpServer").toInt();
    SetUpIni::UdpServerIP = set.value("UdpServerIP").toString();
    SetUpIni::UdpServerPort = set.value("UdpServerPort").toInt();
    SetUpIni::UdpListenPort = set.value("UdpListenPort").toInt();
    set.endGroup();
}

//以初始值新建配置文件
void SetUpIni::newConfig() {
    writeConfig();
}

//写入配置文件,在更改配置文件程序关闭时调用
void SetUpIni::writeConfig() {
    QSettings set(SetUpIni::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    set.setValue("ini_int_eg", SetUpIni::Ini_Int_eg);
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    set.setValue("HexSendTcpClient", SetUpIni::HexSendTcpClient);
    set.setValue("HexReceiveTcpClient", SetUpIni::HexReceiveTcpClient);
    set.setValue("DebugTcpClient", SetUpIni::DebugTcpClient);
    set.setValue("AutoSendTcpClient", SetUpIni::AutoSendTcpClient);
    set.setValue("IntervalTcpClient", SetUpIni::IntervalTcpClient);
    set.setValue("TcpServerIP", SetUpIni::TcpServerIP);
    set.setValue("TcpServerPort", SetUpIni::TcpServerPort);
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    set.setValue("HexSendTcpServer", SetUpIni::HexSendTcpServer);
    set.setValue("HexReceiveTcpServer", SetUpIni::HexReceiveTcpServer);
    set.setValue("DebugTcpServer", SetUpIni::DebugTcpServer);
    set.setValue("AutoSendTcpServer", SetUpIni::AutoSendTcpServer);
    set.setValue("IntervalTcpServer", SetUpIni::IntervalTcpServer);
    set.setValue("TcpListenPort", SetUpIni::TcpListenPort);
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    set.setValue("HexSendUdpServer", SetUpIni::HexSendUdpServer);
    set.setValue("HexReceiveUdpServer", SetUpIni::HexReceiveUdpServer);
    set.setValue("DebugUdpServer", SetUpIni::DebugUdpServer);
    set.setValue("AutoSendUdpServer", SetUpIni::AutoSendUdpServer);
    set.setValue("IntervalUdpServer", SetUpIni::IntervalUdpServer);
    set.setValue("UdpServerIP", SetUpIni::UdpServerIP);
    set.setValue("UdpServerPort", SetUpIni::UdpServerPort);
    set.setValue("UdpListenPort", SetUpIni::UdpListenPort);
    set.endGroup();
}

//校验配置文件
bool SetUpIni::checkConfig() {
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(SetUpIni::ConfigFile);

    if (file.size() == 0) {
        newConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;

        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            newConfig();
            return false;
        }
    } else {
        newConfig();
        return false;
    }

    return true;
}

void SetUpIni::readSendData() {
    //读取发送数据列表
    SetUpIni::net_Datas.clear();
    QString fileName = QString("%1/%2").
                       arg(QUIHelper::AppPath()).arg(SetUpIni::SendFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                SetUpIni::net_Datas.append(line);
            }
        }

        file.close();
    }
}

void SetUpIni::readDeviceData() {
    //读取转发数据列表
    SetUpIni::net_Keys.clear();
    SetUpIni::net_Values.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::AppPath()).
                       arg(SetUpIni::DeviceFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                SetUpIni::net_Keys.append(key);
                SetUpIni::net_Values.append(value);
            }
        }

        file.close();
    }
}
