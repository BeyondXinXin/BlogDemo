#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QWidget>
#include <QTimer>

#include <QVTKOpenGLNativeWidget.h>

#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkArrowSource.h>
#include <vtkDiskSource.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkAxesActor.h>

#include "interactor/vtkInteractorStyleFixedCenter.h"
#include "interactor/vtkInteractorStyleHybrid.h"
#include "interactor/vtkInteractorStylePickCenter.h"
#include "interactor/vtkInteractorStyleSceneCamera.h"
#include "interactor/vtkInteractorStyleSelectionOnly.h"
#include "interactor/vtkInteractorStyleSingleModel.h"

struct ModelInfo
{
    vtkSmartPointer<vtkActor> actor;
    QString name;

    double x, y, z;

    double initialPos[3];
    double initialOri[3];
    double initialScale[3];
    double initialOrigin[3];

    vtkSmartPointer<vtkMatrix4x4> initialUserMatrix;
};
struct CameraState
{
    double position[3];
    double focal[3];
    double viewUp[3];
    double viewAngle;
};

std::vector<ModelInfo> CreateModels(vtkRenderer * renderer);

/*
 * VTK 自定义 Interactor Demo
 * 
 * 请随便吐槽这个 main 这么长，作为 Demo ：可读性 > 结构优雅
 *
 * Qt → VTK Widget → Renderer → Models → Style → UI 绑定
 *
 * vtkInteractorStyleSceneCamera 相机模式
 *      vtk默认，直接交互相机，旋转、平移、缩放等都是直接作用于相机的
 * vtkInteractorStyleSelectionOnly 选择模式
 *      只选择模型，无交互，UI 和渲染双向绑定
 * vtkInteractorStyleSingleModel 模型模式
 *      vtk默认，直接交互指定模型
 * vtkInteractorStyleHybrid 模型+场景混合模式
 *      组合了相机模式和模型模式，根据鼠标位置判断交互整体还是单个模型
 * vtkInteractorStyleFixedCenter 指定中心模式
 *      指定一个固定的中心点，所有交互都以这个点为中心进行旋转、缩放等操作
 * vtkInteractorStylePickCenter 鼠标点中心模式
 *      根据鼠标点击的位置设置交互中心点，交互以这个点为中心进行旋转、缩放等操作
 *
 * https://vtk.org/doc/nightly/html/classvtkInteractorStyle.html#details
 * vtkInteractorStyle 定义了交互样式的接口，提供了大量交互样式。
 */

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    QWidget * central = new QWidget;
    QHBoxLayout * hLayout = new QHBoxLayout;
    central->setLayout(hLayout);
    window.setCentralWidget(central);

    QVTKOpenGLNativeWidget * vtkWidget = new QVTKOpenGLNativeWidget;
    hLayout->addWidget(vtkWidget, 1);

    QVBoxLayout * vLayout = new QVBoxLayout;
    hLayout->addLayout(vLayout);

    QPushButton * resetButton = new QPushButton("重置");
    vLayout->addWidget(resetButton);
    vLayout->addSpacing(20);

    QGroupBox * styleGroup = new QGroupBox("交互方式");
    QVBoxLayout * styleLayout = new QVBoxLayout;

    QRadioButton * rbCamera = new QRadioButton("相机模式");
    QRadioButton * rbSelect = new QRadioButton("选择模式");
    QRadioButton * rbActor = new QRadioButton("模型模式");
    QRadioButton * rbHybrid = new QRadioButton("模型+场景混合模式");
    QRadioButton * rbFixedCenter = new QRadioButton("指定中心模式");
    QRadioButton * rbPickCenter = new QRadioButton("鼠标点中心模式");

    rbCamera->setChecked(true);

    styleLayout->addWidget(rbCamera);
    styleLayout->addWidget(rbSelect);
    styleLayout->addWidget(rbActor);
    styleLayout->addWidget(rbHybrid);
    styleLayout->addWidget(rbFixedCenter);
    styleLayout->addWidget(rbPickCenter);

    styleGroup->setLayout(styleLayout);
    vLayout->addWidget(styleGroup);
    vLayout->addSpacing(20);

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    vtkWidget->renderWindow()->AddRenderer(renderer);

    auto models = CreateModels(renderer);

    QGroupBox * selectGroup = new QGroupBox("模型");
    QVBoxLayout * selectLayout = new QVBoxLayout;
    std::vector<QCheckBox *> checkBoxes(models.size());
    for (int i = 0; i < static_cast<int>(models.size()); ++i) {
        checkBoxes[i] = new QCheckBox(models[i].name);
        selectLayout->addWidget(checkBoxes[i]);
    }
    selectGroup->setLayout(selectLayout);
    vLayout->addWidget(selectGroup);
    vLayout->addStretch();

    vtkNew<vtkInteractorStyleSceneCamera> sceneCameraStyle;
    vtkNew<vtkInteractorStyleSelectionOnly> selectionStyle;
    vtkNew<vtkInteractorStyleSingleModel> singleModelStyle;
    vtkNew<vtkInteractorStyleHybrid> hybridStyle;
    vtkNew<vtkInteractorStyleFixedCenter> fixedStyle;
    vtkNew<vtkInteractorStylePickCenter> pickCenterStyle;

    auto interactor = vtkWidget->renderWindow()->GetInteractor();
    interactor->SetInteractorStyle(sceneCameraStyle);

    vtkNew<vtkAxesActor> axesActor;
    axesActor->SetTotalLength(1.0, 1.0, 1.0);
    axesActor->SetXAxisLabelText("");
    axesActor->SetYAxisLabelText("");
    axesActor->SetZAxisLabelText("");
    axesActor->SetShaftTypeToCylinder();
    renderer->AddActor(axesActor);

    window.resize(1200, 700);
    window.show();

    renderer->ResetCamera();
    renderer->ResetCameraClippingRange();
    vtkWidget->renderWindow()->Render();

    CameraState initialCamera;
    bool cameraSaved = false;

    QTimer::singleShot(0, [&]() {
        renderer->ResetCamera();
        renderer->ResetCameraClippingRange();
        vtkWidget->renderWindow()->Render();

        auto camera = renderer->GetActiveCamera();
        camera->GetPosition(initialCamera.position);
        camera->GetFocalPoint(initialCamera.focal);
        camera->GetViewUp(initialCamera.viewUp);
        initialCamera.viewAngle = camera->GetViewAngle();
        cameraSaved = true;
    });

    auto FunUpdateFixedCenter = [&]() {
        double cx = 0, cy = 0, cz = 0;
        int count = 0;
        for (int i = 0; i < static_cast<int>(models.size()); ++i) {
            if (checkBoxes[i]->isChecked()) {
                cx += models[i].actor->GetPosition()[0];
                cy += models[i].actor->GetPosition()[1];
                cz += models[i].actor->GetPosition()[2];
                count++;
            }
        }
        if (count == 0) {
            for (auto & m : models) {
                cx += m.actor->GetPosition()[0];
                cy += m.actor->GetPosition()[1];
                cz += m.actor->GetPosition()[2];
            }
            count = static_cast<int>(models.size());
        }
        fixedStyle->SetFixedCenter(cx / count, cy / count, cz / count);
    };

    selectionStyle->SetSelectionChangedCallback([&](vtkActor * actor, bool selected) {
        for (int i = 0; i < static_cast<int>(models.size()); ++i) {
            if (models[i].actor.GetPointer() == actor) {
                checkBoxes[i]->blockSignals(true);
                checkBoxes[i]->setChecked(selected);
                checkBoxes[i]->blockSignals(false);
                break;
            }
        }
    });

    for (int i = 0; i < static_cast<int>(models.size()); ++i) {
        QObject::connect(checkBoxes[i], &QCheckBox::toggled, [&, i](bool checked) {
            if (interactor->GetInteractorStyle() == selectionStyle) {
                selectionStyle->SetActorSelected(models[i].actor, checked);
            } else if (interactor->GetInteractorStyle() == fixedStyle) {
                FunUpdateFixedCenter();
            }
        });
    }

    QObject::connect(rbCamera, &QRadioButton::toggled, [&](bool checked) {
        if (checked) {
            interactor->SetInteractorStyle(sceneCameraStyle);
        }
    });

    QObject::connect(rbSelect, &QRadioButton::toggled, [&](bool checked) {
        if (checked) {
            for (QCheckBox * checkBox : checkBoxes) {
                checkBox->setChecked(false);
            }
            interactor->SetInteractorStyle(selectionStyle);
        }
    });

    QObject::connect(rbActor, &QRadioButton::toggled, [&](bool checked) {
        if (checked) {
            interactor->SetInteractorStyle(singleModelStyle);
        }
    });

    QObject::connect(rbHybrid, &QRadioButton::toggled, [&](bool checked) {
        if (checked) {
            interactor->SetInteractorStyle(hybridStyle);
        }
    });

    QObject::connect(rbFixedCenter, &QRadioButton::toggled, [&](bool checked) {
        if (checked) {
            for (QCheckBox * checkBox : checkBoxes) {
                checkBox->setChecked(false);
            }
            FunUpdateFixedCenter();
            interactor->SetInteractorStyle(fixedStyle);
        }
    });

    QObject::connect(rbPickCenter, &QRadioButton::toggled, [&](bool checked) {
        if (checked) {
            interactor->SetInteractorStyle(pickCenterStyle);
        }
    });

    QObject::connect(resetButton, &QPushButton::clicked, [&]() {
        for (auto & m : models) {
            m.actor->SetUserMatrix(nullptr);

            m.actor->SetOrigin(m.initialOrigin);
            m.actor->SetScale(m.initialScale);
            m.actor->SetOrientation(m.initialOri);
            m.actor->SetPosition(m.initialPos);

            if (!m.initialUserMatrix->IsIdentity()) {
                m.actor->SetUserMatrix(m.initialUserMatrix);
            }
        }

        if (cameraSaved) {
            auto camera = renderer->GetActiveCamera();
            camera->SetPosition(initialCamera.position);
            camera->SetFocalPoint(initialCamera.focal);
            camera->SetViewUp(initialCamera.viewUp);
            camera->SetViewAngle(initialCamera.viewAngle);
            camera->OrthogonalizeViewUp();
        } else {
            renderer->ResetCamera();
        }

        renderer->ResetCameraClippingRange();
        vtkWidget->renderWindow()->Render();
    });

    return app.exec();
}

std::vector<ModelInfo> CreateModels(vtkRenderer * renderer)
{
    vtkNew<vtkNamedColors> colors;
    std::vector<ModelInfo> models;

    vtkNew<vtkConeSource> cone;
    cone->SetHeight(1.0);
    cone->SetRadius(0.5);
    cone->SetResolution(20);
    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());
    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
    models.push_back({ coneActor, "圆锥", -2, 1, 0 });

    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(0.5);
    sphere->SetPhiResolution(20);
    sphere->SetThetaResolution(20);
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
    models.push_back({ sphereActor, "球体", 0, 1, 0 });

    vtkNew<vtkCubeSource> cube;
    cube->SetXLength(1);
    cube->SetYLength(1);
    cube->SetZLength(1);
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());
    models.push_back({ cubeActor, "立方体", 2, 1, 0 });

    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetRadius(0.5);
    cylinder->SetHeight(1.0);
    cylinder->SetResolution(20);
    vtkNew<vtkPolyDataMapper> cylMapper;
    cylMapper->SetInputConnection(cylinder->GetOutputPort());
    vtkNew<vtkActor> cylActor;
    cylActor->SetMapper(cylMapper);
    cylActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
    models.push_back({ cylActor, "圆柱", -2, -1, 0 });

    vtkNew<vtkArrowSource> arrow;
    arrow->SetTipLength(0.3);
    arrow->SetTipRadius(0.1);
    arrow->SetShaftRadius(0.03);
    vtkNew<vtkPolyDataMapper> arrowMapper;
    arrowMapper->SetInputConnection(arrow->GetOutputPort());
    vtkNew<vtkActor> arrowActor;
    arrowActor->SetMapper(arrowMapper);
    arrowActor->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());
    models.push_back({ arrowActor, "箭头", 0, -1, 0 });

    vtkNew<vtkDiskSource> disk;
    disk->SetInnerRadius(0.0);
    disk->SetOuterRadius(0.5);
    disk->SetRadialResolution(20);
    disk->SetCircumferentialResolution(20);
    vtkNew<vtkPolyDataMapper> diskMapper;
    diskMapper->SetInputConnection(disk->GetOutputPort());
    vtkNew<vtkActor> diskActor;
    diskActor->SetMapper(diskMapper);
    diskActor->GetProperty()->SetColor(colors->GetColor3d("Magenta").GetData());
    models.push_back({ diskActor, "圆盘", 2, -1, 0 });

    for (auto & m : models) {
        m.actor->SetPosition(m.x, m.y, m.z);
        m.actor->SetScale(1, 1, 1);
        m.actor->SetOrientation(0, 0, 0);

        renderer->AddActor(m.actor);

        m.actor->GetPosition(m.initialPos);
        m.actor->GetOrientation(m.initialOri);
        m.actor->GetScale(m.initialScale);
        m.actor->GetOrigin(m.initialOrigin);

        m.initialUserMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        if (auto um = m.actor->GetUserMatrix()) {
            m.initialUserMatrix->DeepCopy(um);
        } else {
            m.initialUserMatrix->Identity();
        }
    }

    return models;
}
