#ifndef QUIQUIlOGMANAGER_H
#define QUIQUIlOGMANAGER_H

/*
 * 打印日志
*/

// 01 Frame includes
#include <stable.h>

class QUILogManager : public QObject {
    Q_OBJECT
  public:
    explicit QUILogManager(QObject *parent = nullptr);
    virtual ~QUILogManager() override;
  signals:

  public slots:

  private:
    void Initial();
    static void LogMessageOutput(QtMsgType type,
                                 const QMessageLogContext &context,
                                 const QString &msg);
  private:
    const static QString log_file_name_;
    static QFile log_file_;
    static QDataStream log_text_stream_;
};

#endif // QUIQUIlOGmANAGER_H
