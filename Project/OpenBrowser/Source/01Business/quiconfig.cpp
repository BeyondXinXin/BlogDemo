// 01 Frame includes
#include "quiconfig.h"

QUIConfig::CmdOption QUIConfig::cmd_option_ = {
    false,
    false,
    false
};

QUIConfig::QUIConfig(QObject *parent) : QObject(parent) {

}

QUIConfig::~QUIConfig() {

}
