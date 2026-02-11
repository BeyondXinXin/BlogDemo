#ifndef SETUPINI_H
#define SETUPINI_H

/**
 * ini配置文件设置
 */


// 01 Frame includes
#include "stable.h"

//读取配置文件相关
class SetUpIni {
  public:
    //---------------------全局变量（ini外）----------------------//
    static int NoIni_int_eg;

    static QMap <int, QRectF> points;

    //---------------------全局变量（ini内）----------------------//
    static int Ini_Int_eg;
    static QString ConfigFile;          //配置文件文件路径及名称
    static QString SendFileName;        //发送配置文件名
    static QString DeviceFileName;      //模拟设备数据文件名

    //TCP客户端配置参数
    static bool HexSendTcpClient;       //16进制发送
    static bool HexReceiveTcpClient;    //16进制接收
    static bool AsciiTcpClient;         //ASCII模式
    static bool DebugTcpClient;         //启用数据调试
    static bool AutoSendTcpClient;      //自动发送数据
    static int IntervalTcpClient;       //发送数据间隔
    static QString TcpServerIP;         //服务器IP
    static int TcpServerPort;           //服务器端口

    //TCP服务器配置参数
    static bool HexSendTcpServer;       //16进制发送
    static bool HexReceiveTcpServer;    //16进制接收
    static bool AsciiTcpServer;         //ASCII模式
    static bool DebugTcpServer;         //启用数据调试
    static bool AutoSendTcpServer;      //自动发送数据
    static int IntervalTcpServer;       //发送数据间隔
    static int TcpListenPort;           //监听端口

    //UDP服务器配置参数
    static bool HexSendUdpServer;       //16进制发送
    static bool HexReceiveUdpServer;    //16进制接收
    static bool AsciiUdpServer;         //ASCII模式
    static bool DebugUdpServer;         //启用数据调试
    static bool AutoSendUdpServer;      //自动发送数据
    static int IntervalUdpServer;       //发送数据间隔
    static QString UdpServerIP;         //服务器IP
    static int UdpServerPort;           //服务器端口
    static int UdpListenPort;           //监听端口
    static QStringList net_Intervals;
    static QStringList net_Datas;
    static QStringList net_Keys;
    static QStringList net_Values;
    static void readSendData();
    static void readDeviceData();

    //---------------------ini操作----------------------//
    static void readConfig();           //读取配置文件,在main函数最开始加载程序载入
    static void writeConfig();          //写入配置文件,在更改配置文件程序关闭时调用
    static bool checkConfig();          //校验配置文件
    static void newConfig();            //以初始值新建配置文件
};


#endif // SETUPINI_H
