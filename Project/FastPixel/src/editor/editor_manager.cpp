#include "editor_manager.h"

#include "edit_scene.h"
#include "edit_view.h"
#include "editor_data.h"
#include "editor_mediation.h"
#include "singleton.h"
#include "tool_manager.h"

EditorManager::EditorManager()
  : new_uid_(1)
{
}

EditorManager::~EditorManager()
{
}

QWidget* EditorManager::InitNewImage()
{
    EditorMediation* mediation = new EditorMediation(new_uid_);
    mediators_[new_uid_] = mediation;

    cur_uid_ = new_uid_;
    new_uid_++;

    return mediation->GetEditorView();
}

// 视图居中显示
void EditorManager::ViewCamReset(int uid)
{
    mediators_[uid]->ViewCamReset();
}

void EditorManager::ViewCamReset()
{
    mediators_[cur_uid_]->ViewCamReset();
}

void EditorManager::ViewsCamReset()
{
    foreach (auto mediator, mediators_) {
        mediator->ViewCamReset();
    }
}

// 工具发生修改
void EditorManager::ChangeTool()
{
    auto* tool = Singleton<ToolManager>::Inst().CurToolBar();

    foreach (auto mediator, mediators_) {
        mediator->GetEditorView()->setCursor(tool->ToolCursor());
    }
}

// 修改主色副色
void EditorManager::ChangePSColor(const QString& pcolor, const QString& scolor)
{
    foreach (auto mediator, mediators_) {
        mediator->ChangePSColor(pcolor, scolor);
    }
}

// 处理工具事件
void EditorManager::HandelToolEvent(const ToolEvent& data)
{
    mediators_[cur_uid_]->HandelToolEvent(data);
}

void EditorManager::Redo()
{
    mediators_[cur_uid_]->Redo();
}

void EditorManager::Undo()
{
    mediators_[cur_uid_]->Undo();
}

void EditorManager::SaveFile()
{
    mediators_[cur_uid_]->SaveFile();
}
