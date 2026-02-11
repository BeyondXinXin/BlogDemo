#ifndef QUIHELPER_H
#define QUIHELPER_H

/*
 * 全局静态方法类
*/

// 01 Frame includes
#include "stable.h"

class QUIHelper : public QObject {
    Q_OBJECT
  public:
    // 获取全局路径
    static QString GetFullPath(const QString &path);
    // 新建路径（相对路径在程序目录，全局路径在全局目录）
    static bool DirMake(const QString &path);
    // 获取保存的文件
    static QString GetSaveName(const QString &filter,
                               QString defaultDir =
                                   QCoreApplication::applicationDirPath());
    // 获取选择的文件
    static QString GetFileName(const QString &filter,
                               QString defaultDir =
                                   QCoreApplication::applicationDirPath());
    // 获取选择的文件集合
    static QStringList GetFileNames(const QString &filter,
                                    QString defaultDir =
                                        QCoreApplication::applicationDirPath());
    // 获取选择的目录
    static QString GetFolderName();
    // 获取文件名,含拓展名
    static QString GetFileNameWithExtension(const QString &strFilePath);
    // 获取选择文件夹中的文件
    static QStringList GetFolderFileNames(const QStringList &filter);
    // 文件夹是否存在
    static bool FolderIsExist(const QString &strFolder);
    // 文件是否存在
    static bool FileIsExist(const QString &strFile);
    // 复制文件
    static bool CopyFile(const QString &sourceFile, const QString &targetFile);
    // 删除文件夹下所有文件
    static void DeleteDirectory(const QString &path);
    //桌面宽度高度
    static int DeskWidth();
    static int DeskHeight();
    //程序本身文件名称
    static QString AppName();
    //程序当前所在路径
    static QString AppPath();
    //写入消息到额外的的消息日志文件
    static void WriteInfo(const QString &info, const QString &filePath = "log");
    static void WriteError(const QString &info, const QString &filePath = "log");
    //设置全局样式
    static void SetStyle(const QString &qssFile);
    //九宫格图片 horzSplit-宫格1/3/7/9宽度  vertSplit-宫格1/3/7/9高度
    //dstWidth-目标图片宽度  dstHeight-目标图片高度
    static QPixmap NinePatch(const QString &picName,
                             int horzSplit, int vertSplit,
                             int dstWidth, int dstHeight);
    static QPixmap NinePatch(const QPixmap &pix,
                             int horzSplit, int vertSplit,
                             int dstWidth, int dstHeight);
    //设置窗体居中显示
    static void SetFormInCenter(QWidget *frm);
    //设置翻译文件
    static void SetTranslator(const QString &qmFile = ":/Image/qt_zh_CN.qm");
    //设置编码
    static void SetCode();
    //设置延时
    static void Sleep(int sec);
    //设置开机自启动
    static void RunWithSystem(const QString &strName,
                              const QString &strPath, bool autoRun = true);
    //判断是否是IP地址
    static bool IsIP(const QString &ip);
    //判断是否是MAC地址
    static bool IsMac(const QString &mac);
    //判断是否是合法的电话号码
    static bool IsTel(const QString &tel);
    //判断是否是合法的邮箱地址
    static bool IsEmail(const QString &email);
    //16进制字符串转10进制
    static int StrHexToDecimal(const QString &strHex);
    //10进制字符串转10进制
    static int StrDecimalToDecimal(const QString &strDecimal);
    //2进制字符串转10进制
    static int StrBinToDecimal(const QString &strBin);
    //10进制转16进制字符串,补零.
    static QString DecimalToStrHex(int decimal);
    //字节数组转ushort
    static qint32 ByteToUShort(const QByteArray &data);
    static qint32 ByteToUShortRec(const QByteArray &data);
    //异或加密算法
    static QString GetXorEncryptDecrypt(const QString &str, char key);
    //异或校验
    static uchar GetOrCode(const QByteArray &data);
    //计算校验码
    static uchar GetCheckCode(const QByteArray &data);
    //字符串补全
    static QString GetValue(quint8 value);
    //字节数组转Ascii字符串
    static QString ByteArrayToAsciiStr(const QByteArray &data);
    //16进制字符串转字节数组
    static QByteArray HexStrToByteArray(const QString &str);
    static char ConvertHexChar(char ch);
    //Ascii字符串转字节数组
    static QByteArray AsciiStrToByteArray(const QString &str);
    //字节数组转16进制字符串
    static QString ByteArrayToHexStr(const QByteArray &data);
    //判断IP地址及端口是否在线
    static bool IpLive(const QString &ip, int port, int timeout = 1000);
    //获取网页所有源代码
    static QString GetHtml(const QString &url);
    //获取本机公网IP地址
    static QString GetNetIP(const QString &webCode);
    //获取本机IP
    static QString GetLocalIP();
    //Url地址转为IP地址
    static QString UrlToIP(const QString &url);
    //判断是否通外网
    static bool IsWebOk();
    //弹出消息框
    static void ShowMessageBoxInfo(const QString &info,
                                   int closeSec = 0, bool exec = false);
    //弹出错误框
    static void ShowMessageBoxError(const QString &info,
                                    int closeSec = 0, bool exec = false);
    //弹出询问框
    static qint32 ShowMessageBoxQuestion(const QString &info);
};

#endif // QUIHELPER_H
