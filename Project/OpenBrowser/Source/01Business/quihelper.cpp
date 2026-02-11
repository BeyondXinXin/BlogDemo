// 01 Frame includes
#include "quihelper.h"
#include "quimessagebox.h"

int QUIHelper::DeskWidth() {
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static int width = 0;
    if (width == 0) {
        width = qApp->desktop()->availableGeometry().width();
    }
    return width;
}

int QUIHelper::DeskHeight() {
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static int height = 0;
    if (height == 0) {
        height = qApp->desktop()->availableGeometry().height();
    }
    return height;
}

QString QUIHelper::AppName() {
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
    }
    return name;
}

QString QUIHelper::AppPath() {
    return qApp->applicationDirPath();
}

void QUIHelper::WriteInfo(const QString &info, const QString &filePath) {
    QString fileName = QString("%1/%2/%3_runinfo_%4.txt")
                       .arg(QUIHelper::AppPath())
                       .arg(filePath)
                       .arg(QUIHelper::AppName())
                       .arg(QDate::currentDate().toString("yyyyMM"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    file.close();
}

void QUIHelper::WriteError(const QString &info, const QString &filePath) {
    //正式运行屏蔽掉输出错误信息,调试阶段才需要
    QString fileName = QString("%1/%2/%3_runerror_%4.txt")
                       .arg(QUIHelper::AppPath())
                       .arg(filePath)
                       .arg(QUIHelper::AppName())
                       .arg(QDate::currentDate().toString("yyyyMM"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    file.close();
}

void QUIHelper::SetStyle(const QString &qssFile) {
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setPalette(
            QPalette(QColor((qss.indexOf("paletteColor:") >= 0) ?
                            qss.mid(qss.indexOf("paletteColor:") +
                                    QString("paletteColor:").length(), 7) :
                            "#2E2F30")));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

QPixmap QUIHelper::NinePatch(const QString &picName,
                             int horzSplit, int vertSplit,
                             int dstWidth, int dstHeight) {
    QPixmap pix(picName);
    return NinePatch(pix, horzSplit, vertSplit, dstWidth, dstHeight);
}

QPixmap QUIHelper::NinePatch(const QPixmap &pix, int horzSplit,
                             int vertSplit, int dstWidth, int dstHeight) {
    int pixWidth = pix.width();
    int pixHeight = pix.height();
    QPixmap pix1 = pix.copy(0, 0, horzSplit, vertSplit);
    QPixmap pix2 = pix.copy(horzSplit, 0, pixWidth - horzSplit * 2, vertSplit);
    QPixmap pix3 = pix.copy(pixWidth - horzSplit, 0, horzSplit, vertSplit);
    QPixmap pix4 = pix.copy(0, vertSplit, horzSplit, pixHeight - vertSplit * 2);
    QPixmap pix5 = pix.copy(horzSplit, vertSplit, pixWidth - horzSplit * 2,
                            pixHeight - vertSplit * 2);
    QPixmap pix6 = pix.copy(pixWidth - horzSplit, vertSplit, horzSplit,
                            pixHeight - vertSplit * 2);
    QPixmap pix7 = pix.copy(0, pixHeight - vertSplit, horzSplit, vertSplit);
    QPixmap pix8 = pix.copy(horzSplit, pixHeight - vertSplit,
                            pixWidth - horzSplit * 2, pixWidth - horzSplit * 2);
    QPixmap pix9 = pix.copy(pixWidth - horzSplit,
                            pixHeight - vertSplit, horzSplit, vertSplit);
    //保持高度拉宽
    pix2 = pix2.scaled(dstWidth - horzSplit * 2, vertSplit,
                       Qt::IgnoreAspectRatio);
    //保持宽度拉高
    pix4 = pix4.scaled(horzSplit, dstHeight - vertSplit * 2,
                       Qt::IgnoreAspectRatio);
    //宽高都缩放
    pix5 = pix5.scaled(dstWidth - horzSplit * 2, dstHeight - vertSplit * 2,
                       Qt::IgnoreAspectRatio);
    //保持宽度拉高
    pix6 = pix6.scaled(horzSplit, dstHeight - vertSplit * 2,
                       Qt::IgnoreAspectRatio);
    //保持高度拉宽
    pix8 = pix8.scaled(dstWidth - horzSplit * 2, vertSplit);
    //生成宽高图片并填充透明背景颜色
    QPixmap resultImg(dstWidth, dstHeight);
    resultImg.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&resultImg);
    if (!resultImg.isNull()) {
        painter.drawPixmap(0, 0, pix1);
        painter.drawPixmap(horzSplit, 0, pix2);
        painter.drawPixmap(dstWidth - horzSplit, 0, pix3);
        painter.drawPixmap(0, vertSplit, pix4);
        painter.drawPixmap(horzSplit, vertSplit, pix5);
        painter.drawPixmap(dstWidth - horzSplit, vertSplit, pix6);
        painter.drawPixmap(0, dstHeight - vertSplit, pix7);
        painter.drawPixmap(horzSplit, dstHeight - vertSplit, pix8);
        painter.drawPixmap(dstWidth - horzSplit, dstHeight - vertSplit, pix9);
    }
    painter.end();
    return resultImg;
}

void QUIHelper::SetFormInCenter(QWidget *frm) {
    int frmX = frm->width();
    int frmY = frm->height();
    QDesktopWidget w;
    int deskWidth = w.availableGeometry().width();
    int deskHeight = w.availableGeometry().height();
    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
    frm->move(movePoint);
}

void QUIHelper::SetTranslator(const QString &qmFile) {
    QTranslator *translator = new QTranslator(qApp);
    translator->load(qmFile);
    qApp->installTranslator(translator);
}

void QUIHelper::SetCode() {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
}

void QUIHelper::Sleep(int sec) {
    QTime dieTime = QTime::currentTime().addMSecs(sec);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void QUIHelper::RunWithSystem(
    const QString &strName, const QString &strPath, bool autoRun) {
    QSettings
    reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
        , QSettings::NativeFormat);
    reg.setValue(strName, autoRun ? strPath : "");
}

bool QUIHelper::IsIP(const QString &ip) {
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.)"
                   "{3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}

bool QUIHelper::IsMac(const QString &mac) {
    QRegExp RegExp("^[A-F0-9]{2}(-[A-F0-9]{2}){5}$");
    return RegExp.exactMatch(mac);
}

bool QUIHelper::IsTel(const QString &tel) {
    if (tel.length() != 11) {
        return false;
    }
    if (!tel.startsWith("13") && !tel.startsWith("14")
            && !tel.startsWith("15") && !tel.startsWith("18")) {
        return false;
    }
    return true;
}

bool QUIHelper::IsEmail(const QString &email) {
    if (!email.contains("@") || !email.contains(".com")) {
        return false;
    }
    return true;
}

int QUIHelper::StrHexToDecimal(const QString &strHex) {
    bool ok;
    return strHex.toInt(&ok, 16);
}

int QUIHelper::StrDecimalToDecimal(const QString &strDecimal) {
    bool ok;
    return strDecimal.toInt(&ok, 10);
}

int QUIHelper::StrBinToDecimal(const QString &strBin) {
    bool ok;
    return strBin.toInt(&ok, 2);
}

QString QUIHelper::DecimalToStrHex(int decimal) {
    QString temp = QString::number(decimal, 16);
    if (temp.length() == 1) {
        temp = "0" + temp;
    }
    return temp;
}

qint32 QUIHelper::ByteToUShort(const QByteArray &data) {
    int i = data.at(1) & 0x000000FF;
    i |= ((data.at(0) << 8) & 0x0000FF00);
    if (i >= 32768) {
        i = i - 65536;
    }
    return i;
}

qint32 QUIHelper::ByteToUShortRec(const QByteArray &data) {
    int i = data.at(0) & 0x000000FF;
    i |= ((data.at(1) << 8) & 0x0000FF00);
    if (i >= 32768) {
        i = i - 65536;
    }
    return i;
}

QString QUIHelper::GetXorEncryptDecrypt(const QString &str, char key) {
    QByteArray data = str.toLatin1();
    int size = data.size();
    for (int i = 0; i < size; i++) {
        data[i] = data[i] ^ key;
    }
    return QLatin1String(data);
}

uchar QUIHelper::GetOrCode(const QByteArray &data) {
    int len = data.length();
    uchar result = 0;
    for (int i = 0; i < len; i++) {
        result ^= data.at(i);
    }
    return result;
}

uchar QUIHelper::GetCheckCode(const QByteArray &data) {
    int len = data.length();
    uchar temp = 0;
    for (uchar i = 0; i < len; i++) {
        temp += data.at(i);
    }
    return temp % 256;
}

QString QUIHelper::GetValue(quint8 value) {
    QString result = QString::number(value);
    if (result.length() <= 1) {
        result = QString("0%1").arg(result);
    }
    return result;
}

QString QUIHelper::ByteArrayToAsciiStr(const QByteArray &data) {
    QString temp;
    int len = data.size();
    for (int i = 0; i < len; i++) {
        //0x20为空格,空格以下都是不可见字符
        char b = data.at(i);
        if (0x00 == b) {
            temp += QString("\\NUL");
        } else if (0x01 == b) {
            temp += QString("\\SOH");
        } else if (0x02 == b) {
            temp += QString("\\STX");
        } else if (0x03 == b) {
            temp += QString("\\ETX");
        } else if (0x04 == b) {
            temp += QString("\\EOT");
        } else if (0x05 == b) {
            temp += QString("\\ENQ");
        } else if (0x06 == b) {
            temp += QString("\\ACK");
        } else if (0x07 == b) {
            temp += QString("\\BEL");
        } else if (0x08 == b) {
            temp += QString("\\BS");
        } else if (0x09 == b) {
            temp += QString("\\HT");
        } else if (0x0A == b) {
            temp += QString("\\LF");
        } else if (0x0B == b) {
            temp += QString("\\VT");
        } else if (0x0C == b) {
            temp += QString("\\FF");
        } else if (0x0D == b) {
            temp += QString("\\CR");
        } else if (0x0E == b) {
            temp += QString("\\SO");
        } else if (0x0F == b) {
            temp += QString("\\SI");
        } else if (0x10 == b) {
            temp += QString("\\DLE");
        } else if (0x11 == b) {
            temp += QString("\\DC1");
        } else if (0x12 == b) {
            temp += QString("\\DC2");
        } else if (0x13 == b) {
            temp += QString("\\DC3");
        } else if (0x14 == b) {
            temp += QString("\\DC4");
        } else if (0x15 == b) {
            temp += QString("\\NAK");
        } else if (0x16 == b) {
            temp += QString("\\SYN");
        } else if (0x17 == b) {
            temp += QString("\\ETB");
        } else if (0x18 == b) {
            temp += QString("\\CAN");
        } else if (0x19 == b) {
            temp += QString("\\EM");
        } else if (0x1A == b) {
            temp += QString("\\SUB");
        } else if (0x1B == b) {
            temp += QString("\\ESC");
        } else if (0x1C == b) {
            temp += QString("\\FS");
        } else if (0x1D == b) {
            temp += QString("\\GS");
        } else if (0x1E == b) {
            temp += QString("\\RS");
        } else if (0x1F == b) {
            temp += QString("\\US");
        } else if (0x7F == b) {
            temp += QString("\\x7F");
        } else if (0x5C == b) {
            temp += QString("\\x5C");
        } else if (0x20 >= b) {
            temp += QString("\\x%1").arg(DecimalToStrHex(static_cast<quint8>(b)));
        } else {
            temp += QString("%1").arg(b);
        }
    }
    return temp.trimmed();
}

QByteArray QUIHelper::HexStrToByteArray(const QString &str) {
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;
    for (int i = 0; i < len;) {
        hstr = str.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }
        i++;
        if (i >= len) {
            break;
        }
        lstr = str.at(i).toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }
        i++;
        senddata[hexdatalen] = static_cast<char>(hexdata);
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

char QUIHelper::ConvertHexChar(char ch) {
    if ((ch >= '0') && (ch <= '9')) {
        return ch - 0x30;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    } else if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    } else {
        return (-1);
    }
}

QByteArray QUIHelper::AsciiStrToByteArray(const QString &str) {
    QByteArray buffer;
    int len = str.length();
    QString letter;
    QString hex;
    for (int i = 0; i < len; i++) {
        letter = str.at(i);
        if (letter == "\\") {
            i++;
            letter = str.mid(i, 1);
            if (letter == "x") {
                i++;
                hex = str.mid(i, 2);
                buffer.append(static_cast<char>(StrHexToDecimal(hex)));
                i++;
                continue;
            } else if (letter == "N") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "U") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "L") {           //NUL=0x00
                        buffer.append(static_cast<char>(0x00));
                        continue;
                    }
                } else if (hex == "A") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "K") {           //NAK=0x15
                        buffer.append(0x15);
                        continue;
                    }
                }
            } else if (letter == "S") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "O") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "H") {           //SOH=0x01
                        buffer.append(0x01);
                        continue;
                    } else {                    //SO=0x0E
                        buffer.append(0x0E);
                        i--;
                        continue;
                    }
                } else if (hex == "T") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "X") {           //STX=0x02
                        buffer.append(0x02);
                        continue;
                    }
                } else if (hex == "I") {        //SI=0x0F
                    buffer.append(0x0F);
                    continue;
                } else if (hex == "Y") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "N") {           //SYN=0x16
                        buffer.append(0x16);
                        continue;
                    }
                } else if (hex == "U") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "B") {           //SUB=0x1A
                        buffer.append(0x1A);
                        continue;
                    }
                }
            } else if (letter == "E") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "T") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "X") {           //ETX=0x03
                        buffer.append(0x03);
                        continue;
                    } else if (hex == "B") {    //ETB=0x17
                        buffer.append(0x17);
                        continue;
                    }
                } else if (hex == "O") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "T") {           //EOT=0x04
                        buffer.append(0x04);
                        continue;
                    }
                } else if (hex == "N") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "Q") {           //ENQ=0x05
                        buffer.append(0x05);
                        continue;
                    }
                } else if (hex == "M") {        //EM=0x19
                    buffer.append(0x19);
                    continue;
                } else if (hex == "S") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "C") {           //ESC=0x1B
                        buffer.append(0x1B);
                        continue;
                    }
                }
            } else if (letter == "A") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "C") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "K") {           //ACK=0x06
                        buffer.append(0x06);
                        continue;
                    }
                }
            } else if (letter == "B") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "E") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "L") {           //BEL=0x07
                        buffer.append(0x07);
                        continue;
                    }
                } else if (hex == "S") {        //BS=0x08
                    buffer.append(0x08);
                    continue;
                }
            } else if (letter == "C") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "R") {               //CR=0x0D
                    buffer.append(0x0D);
                    continue;
                } else if (hex == "A") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "N") {           //CAN=0x18
                        buffer.append(0x18);
                        continue;
                    }
                }
            } else if (letter == "D") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "L") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "E") {           //DLE=0x10
                        buffer.append(0x10);
                        continue;
                    }
                } else if (hex == "C") {
                    i++;
                    hex = str.mid(i, 1);
                    if (hex == "1") {           //DC1=0x11
                        buffer.append(0x11);
                        continue;
                    } else if (hex == "2") {    //DC2=0x12
                        buffer.append(0x12);
                        continue;
                    } else if (hex == "3") {    //DC3=0x13
                        buffer.append(0x13);
                        continue;
                    } else if (hex == "4") {    //DC2=0x14
                        buffer.append(0x14);
                        continue;
                    }
                }
            } else if (letter == "F") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "F") {               //FF=0x0C
                    buffer.append(0x0C);
                    continue;
                } else if (hex == "S") {        //FS=0x1C
                    buffer.append(0x1C);
                    continue;
                }
            } else if (letter == "H") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "T") {               //HT=0x09
                    buffer.append(0x09);
                    continue;
                }
            } else if (letter == "L") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "F") {               //LF=0x0A
                    buffer.append(0x0A);
                    continue;
                }
            } else if (letter == "G") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "S") {               //GS=0x1D
                    buffer.append(0x1D);
                    continue;
                }
            } else if (letter == "R") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "S") {               //RS=0x1E
                    buffer.append(0x1E);
                    continue;
                }
            } else if (letter == "U") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "S") {               //US=0x1F
                    buffer.append(0x1F);
                    continue;
                }
            } else if (letter == "V") {
                i++;
                hex = str.mid(i, 1);
                if (hex == "T") {               //VT=0x0B
                    buffer.append(0x0B);
                    continue;
                }
            } else if (letter == "\\") {
                //如果连着的是多个\\则对应添加\对应的16进制0x5C
                buffer.append(0x5C);
                continue;
            } else {
                //将对应的\[前面的\\也要加入
                buffer.append(0x5C);
                buffer.append(letter.toLatin1());
                continue;
            }
        }
        buffer.append(str.mid(i, 1).toLatin1());
    }
    return buffer;
}

QString QUIHelper::ByteArrayToHexStr(const QByteArray &data) {
    QString temp = "";
    QString hex = data.toHex();
    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }
    return temp.trimmed().toUpper();
}

QString QUIHelper::GetSaveName(const QString &filter, QString defaultDir) {
    return QFileDialog::getSaveFileName(nullptr, "选择文件", defaultDir, filter);
}

QString QUIHelper::GetFileName(const QString &filter, QString defaultDir) {
    return QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDir, filter);
}

QStringList QUIHelper::GetFileNames(const QString &filter, QString defaultDir) {
    return QFileDialog::getOpenFileNames(nullptr, "选择文件", defaultDir, filter);
}

QString QUIHelper::GetFolderName() {
    return QFileDialog::getExistingDirectory();
}

QString QUIHelper::GetFileNameWithExtension(const QString &strFilePath) {
    QFileInfo fileInfo(strFilePath);
    return fileInfo.fileName();
}

QStringList QUIHelper::GetFolderFileNames(const QStringList &filter) {
    QStringList fileList;
    QString strFolder = QFileDialog::getExistingDirectory();
    if (0 != strFolder.length()) {
        QDir myFolder(strFolder);
        if (myFolder.exists()) {
            fileList = myFolder.entryList(filter);
        }
    }
    return fileList;
}

bool QUIHelper::FolderIsExist(const QString &strFolder) {
    QDir tempFolder(strFolder);
    return tempFolder.exists();
}

bool QUIHelper::FileIsExist(const QString &strFile) {
    QFile tempFile(strFile);
    return tempFile.exists();
}

bool QUIHelper::CopyFile(const QString &sourceFile, const QString &targetFile) {
    bool ok;
    ok = QFile::copy(sourceFile, targetFile);
    //将复制过去的文件只读属性取消
    ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
    return ok;
}

void QUIHelper::DeleteDirectory(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            DeleteDirectory(fi.absoluteFilePath());
            dir.rmdir(fi.absoluteFilePath());
        }
    }
}

QString QUIHelper::GetFullPath(const QString &path) {
    if (QDir::isAbsolutePath(path)) {
        return path;
    } else {
        return QDir::currentPath() + "/" + path;
    }
}

bool QUIHelper::DirMake(const QString &path) {
    QString full_path = GetFullPath(path);
    QDir dir(full_path);
    if (dir.exists()) {
        return true;
    } else {
        return dir.mkpath(full_path);
    }
}

bool QUIHelper::IpLive(const QString &ip, int port, int timeout) {
    QTcpSocket tcpClient;
    tcpClient.abort();
    tcpClient.connectToHost(ip, static_cast<quint16>(port));
    //超时没有连接上则判断不在线
    return tcpClient.waitForConnected(timeout);
}

QString QUIHelper::GetHtml(const QString &url) {
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)),
                     &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseData = reply->readAll();
    return QString(responseData);
}

QString QUIHelper::GetNetIP(const QString &webCode) {
    QString web = webCode;
    web = web.replace(' ', "");
    web = web.replace("\r", "");
    web = web.replace("\n", "");
    QStringList list = web.split("<br/>");
    QString tar = list.at(3);
    QStringList ip = tar.split("=");
    return ip.at(1);
}

QString QUIHelper::GetLocalIP() {
    QStringList ips;
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, addrs) {
        QString ip = addr.toString();
        if (QUIHelper::IsIP(ip)) {
            ips << ip;
        }
    }
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            break;
        }
    }
    return ip;
}

QString QUIHelper::UrlToIP(const QString &url) {
    QHostInfo host = QHostInfo::fromName(url);
    return host.addresses().at(0).toString();
}

bool QUIHelper::IsWebOk() {
    //能接通百度IP说明可以通外网
    return IpLive("115.239.211.112", 80);
}

void QUIHelper::ShowMessageBoxInfo(const QString &info, int closeSec, bool exec) {
    if (exec) {
        QUIMessageBox msg;
        msg.setMessage(info, 0, closeSec);
        msg.exec();
    } else {
        QUIMessageBox::Instance()->setMessage(info, 0, closeSec);
        QUIMessageBox::Instance()->show();
    }
}

void QUIHelper::ShowMessageBoxError(const QString &info, int closeSec, bool exec) {
    if (exec) {
        QUIMessageBox msg;
        msg.setMessage(info, 2, closeSec);
        msg.exec();
    } else {
        QUIMessageBox::Instance()->setMessage(info, 2, closeSec);
        QUIMessageBox::Instance()->show();
    }
}

qint32 QUIHelper::ShowMessageBoxQuestion(const QString &info) {
    QUIMessageBox msg;
    msg.setMessage(info, 1);
    return msg.exec();
}

