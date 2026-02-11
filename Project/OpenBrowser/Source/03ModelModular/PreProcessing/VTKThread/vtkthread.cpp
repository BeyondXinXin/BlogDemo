// 01 Frame includes
#include "vtkthread.h"

// VTK inludes
#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>


VtkThread::VtkThread(QObject *parent) : QThread(parent) {
    this->InitialResult();
}

VtkThread::~VtkThread() {
}

bool VtkThread::GetThreadResult() const {
    return this->result_;
}

void VtkThread::run() {
}

void VtkThread::InitialResult() {
    this->result_ = false;
}

void VtkThread::SetResult(const bool result) {
    result_ |= result;
}

void VtkThread::PrintProgress(
    vtkObject *caller, unsigned long vtk_event, void *call_data) {
    Q_UNUSED(vtk_event)
    Q_UNUSED(call_data)
    vtkSmartPointer<vtkAlgorithm> object =
        vtkAlgorithm::SafeDownCast(caller);
    if (object) {
        static double progress = object->GetProgress();
        if (fabs(progress - object->GetProgress()) > 1e-3) {
            progress = object->GetProgress();
            emit SignalVtkThreadProgressOut("", static_cast<qint32>(progress * 100));
        }
    }
}
