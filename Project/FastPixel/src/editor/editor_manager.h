#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QHash>
#include <QObject>

#include "tool/tool.h"

class EditorMediation;

class EditorManager : public QObject
{
    Q_OBJECT

public:
    EditorManager();
    ~EditorManager();

    QWidget* InitNewImage();

    void ViewCamReset(int uid);
    void ViewCamReset();
    void ViewsCamReset();
    void ChangeTool();

    void ChangePSColor(const QString& pcolor, const QString& scolor);

    void HandelToolEvent(const ToolEvent& data);

    void Redo();
    void Undo();

    void SaveFile();

private:
    QHash<int, EditorMediation*> mediators_;

    int cur_uid_;
    int new_uid_;
};

#endif // EDITORMANAGER_H
