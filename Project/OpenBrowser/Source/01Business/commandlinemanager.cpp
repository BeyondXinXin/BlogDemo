// 01 Frame includes
#include "commandlinemanager.h"
#include "quiconfig.h"

void CommandLineManager::Initial(const QApplication &app) {
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    QCommandLineOption debug_option(
        "debug", "Open debug mode");
    QCommandLineOption test_option(
        "test", "Open test dialog");
    QCommandLineOption relese_option(
        "phatom", "Phatom Test");
    parser.addOption(debug_option);
    parser.addOption(test_option);
    parser.addOption(relese_option);
    parser.process(app);

    if (parser.isSet(debug_option)) {
        QUIConfig::cmd_option_.debug_mode = true;
    }
    if (parser.isSet(test_option)) {
        QUIConfig::cmd_option_.test_mode = true;
    }
    if (parser.isSet(relese_option)) {
        QUIConfig::cmd_option_.relese_mode = true;
    }

}

CommandLineManager::CommandLineManager(QObject *parent) : QObject(parent) {

}

CommandLineManager::~CommandLineManager() {

}
