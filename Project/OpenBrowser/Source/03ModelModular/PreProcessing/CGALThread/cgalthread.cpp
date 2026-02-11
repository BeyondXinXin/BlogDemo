// 01 Frame includes
#include "cgalthread.h"

CGALThread::CGALThread(QObject *parent) : QThread(parent) {
    this->InitialResult();
}

CGALThread::~CGALThread() {
}

bool CGALThread::GetThreadResult() const {
    return this->result_;
}

void CGALThread::run() {
}

void CGALThread::InitialResult() {
    this->result_ = false;
}

void CGALThread::SetResult(const bool result) {
    result_ |= result;
}



