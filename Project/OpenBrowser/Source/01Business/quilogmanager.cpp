// 01 Frame includes
#include "quilogmanager.h"
#include "quihelper.h"

const QString QUILogManager::log_file_name_ = QString("%1%2%3")
        .arg("./log/")
        .arg(QDateTime::currentDateTime().
             toString("log_yyyy_MM_dd_hh_mm_ss"))
        .arg(".txt");
QFile QUILogManager::log_file_(log_file_name_);
QDataStream QUILogManager::log_text_stream_(&log_file_);

QUILogManager::QUILogManager(QObject *parent) : QObject(parent) {
    this->Initial();
}

QUILogManager::~QUILogManager() {
    qInstallMessageHandler(nullptr);
    log_file_.close();
}

void QUILogManager::Initial() {
    QUIHelper::DirMake("./log");
    if (log_file_.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qInstallMessageHandler(LogMessageOutput);
    } else {
        qWarning() << "logfile open error!";
    }
}

void QUILogManager::LogMessageOutput(QtMsgType type,
                                     const QMessageLogContext &context,
                                     const QString &msg) {
    static QReadWriteLock lock;
    QWriteLocker locker(&lock);
    static QTextStream cout(stdout, QIODevice::WriteOnly);
    QString text;
    switch (type) {
        case QtDebugMsg: {
                text = QString("DEBUG:");
                break;
            }
        case QtInfoMsg: {
                text = QString("INFO:");
                break;
            }
        case QtWarningMsg: {
                text = QString("WARNING:");
                break;
            }
        case QtCriticalMsg: {
                text = QString("CRITICAL:");
                break;
            }
        case QtFatalMsg: {
                text = QString("FATAL:");
                break;
            }
    }
    QString currentDateTime = QDateTime::currentDateTime()
                              .toString("yyyy-MM-dd hh:mm:ss:zzz");
    QString currentThreadName = QThread::currentThread()->objectName();
    QString message = QString("%1 [%2] %3 - %4 %5")
                      .arg(currentDateTime)
                      .arg(currentThreadName)
                      .arg(text)
                      .arg(QString(context.function).remove(QRegExp("\\((.*)\\)")))
                      .arg(msg);
    log_text_stream_ << message;
    cout << message << endl;
    cout.flush();
}
