#include <QApplication>
#include <QtWidgets>

#include <PouringDucks>

QList<Subtitle> GenerateBaseSubtitles(QStringList strs, int aTime)
{
    QList<Subtitle> subtitles;

    for (int i = 0; i < strs.size(); ++i) {
        Subtitle subtitle;
        subtitle.startTimeMs = aTime * 2 * i;
        subtitle.durationMs = aTime * 2 * i + aTime;
        subtitle.text = strs.at(i);
        subtitles.push_back(subtitle);
    }

    return subtitles;
}

QList<Subtitle> GenerateBaseSubtitles(int count, int aTime)
{
    QList<Subtitle> subtitles;

    for (int i = 0; i < count; ++i) {
        Subtitle subtitle;
        subtitle.startTimeMs = aTime * 2 * i;
        subtitle.durationMs = aTime * 2 * i + aTime;
        subtitle.text = QString::number(10000000000 + i);
        subtitles.push_back(subtitle);
    }

    return subtitles;
}

QList<Subtitle> AddRandomAttributes(const QList<Subtitle> & subtitles)
{
    QList<Subtitle> modifiedSubtitles = subtitles;
    bool alternateRotation = true;
    int nextRotationIndex = QRandomGenerator::global()->bounded(4, 8);

    for (int i = 0; i < modifiedSubtitles.size(); ++i) {
        Subtitle & subtitle = modifiedSubtitles[i];
        QList<QColor> colors = { Qt::blue, Qt::red, Qt::green, Qt::darkRed, Qt::darkCyan, Qt::darkMagenta };
        QColor randomColor = colors[QRandomGenerator::global()->bounded(colors.size())];
        subtitle.fontColor = randomColor;
        subtitle.font = QFont("Microsoft YaHei", QRandomGenerator::global()->bounded(20, 31));

        if (QRandomGenerator::global()->bounded(2) == 0) {
            subtitle.enterAnimation = EnterAnimationType::FadeIn;
        } else {
            subtitle.enterAnimation = EnterAnimationType::ZoomIn;
        }

        if (QRandomGenerator::global()->bounded(2) == 0) {
            subtitle.moveAnimation = MoveAnimationType::MoveUp;
        } else {
            subtitle.moveAnimation = MoveAnimationType::Narrow;
        }

        if (i == nextRotationIndex) {
            if (alternateRotation) {
                subtitle.moveAnimation = MoveAnimationType::RotateClockwise;
            } else {
                subtitle.moveAnimation = MoveAnimationType::RotateCounterclockwise;
            }
            alternateRotation = !alternateRotation;
            nextRotationIndex += QRandomGenerator::global()->bounded(4, 8);
        }
    }

    return modifiedSubtitles;
}

QList<Subtitle> subtitles;

/*
    就是个Demo，为了测试下 SubtitleAnimatorWidget 的基本功能，导出的视频位于"我的电脑:/视频"
    开发的版本是6.8.0，没做任何版本兼容。版本低于6.6删了 WindowScreenRecorder 自己重写下视频生成。

    关于动画效果:
    暂时还想不到如何实现文字翻转视频制作，直接用 GraphicsTextItem 硬画

    关于视频生成:
    一样，也是想不到好办法，目前就是一帧一帧录制
    Qt6.8 LTS 最近刚发布，WindowScreenRecorder 只是为了试一下新版本！
    Qt的录屏常见方法是 ffmpeg + QWidget::grab()
*/

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    // 初始化字幕
    QStringList strs;
    strs << u8"窗边身影远离";
    strs << u8"话别黄昏里";
    strs << u8"迷朦仍在挥一挥温暖的手";
    strs << u8"不修饰的笑容";
    strs << u8"伴在微风里";
    strs << u8"柔情投在我的心深处";
    strs << u8"从没有过这种感觉";
    strs << u8"不懂解释是为何";
    strs << u8"流露爱意发出的线索";
    strs << u8"在告诉我看透";
    strs << u8"再不愿去觅寻";
    strs << u8"再不愿去添色彩";
    strs << u8"再不面对未来";
    strs << u8"能共你往宇宙无穷天边去";
    strs << u8"指尖感到暖流";
    strs << u8"逗动人思绪";
    strs << u8"缓缓投入每句温馨爱诗";
    strs << u8"拉紧一点距离";
    strs << u8"渐令人昏醉";
    strs << u8"浓情常静听心中祷告";
    strs << u8"寻觅这个世界的奥秘";
    strs << u8"捉摸不到是如何";
    strs << u8"能让你与我一起爱过";
    strs << u8"是永远的真心";
    strs << u8"再不愿去觅寻";
    strs << u8"再不愿去添色彩";
    strs << u8"再不面对未来";
    strs << u8"能共你往宇宙无穷天边去";

    subtitles = GenerateBaseSubtitles(50, 200);
    subtitles = GenerateBaseSubtitles(strs, 200);
    subtitles = AddRandomAttributes(subtitles);

    // 动画
    SubtitleAnimatorWidget * animatorWid = new SubtitleAnimatorWidget();
    animatorWid->SetSubtitles(subtitles);

    // 进度条
    QSlider * slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, subtitles.size());
    QObject::connect(slider, &QSlider::valueChanged, animatorWid, &SubtitleAnimatorWidget::SetCurrentIndex);
    QObject::connect(animatorWid, &SubtitleAnimatorWidget::SgnIndexChange, slider, &QSlider::setValue);

    // 按钮
    QPushButton * playPauseButton = new QPushButton(u8"播放");
    playPauseButton->setCheckable(true);
    QPushButton * replaceButton = new QPushButton(u8"刷新");
    QPushButton * exportButton = new QPushButton(u8"导出");

    // 布局
    QHBoxLayout * controlLayout = new QHBoxLayout();
    controlLayout->addWidget(replaceButton);
    controlLayout->addWidget(playPauseButton);
    controlLayout->addWidget(slider);
    controlLayout->addWidget(exportButton);
    QVBoxLayout * centerLayout = new QVBoxLayout();
    centerLayout->addWidget(animatorWid);
    centerLayout->addLayout(controlLayout);
    QWidget * centerWidget = new QWidget();
    centerWidget->setLayout(centerLayout);
    QWidget * window = new QWidget();
    QVBoxLayout * mainLayout = new QVBoxLayout(window);
    mainLayout->addWidget(centerWidget);
    window->setWindowTitle("Pouring Ducks");
    window->setFixedSize(animatorWid->size() + QSize(100, 100));

    // 槽
    QObject::connect(playPauseButton, &QPushButton::clicked, [&]() {
        playPauseButton->setText(playPauseButton->isChecked() ? u8"暂停" : u8"播放");
        replaceButton->setEnabled(!playPauseButton->isChecked());
        exportButton->setEnabled(!playPauseButton->isChecked());
        slider->setEnabled(!playPauseButton->isChecked());
        animatorWid->SetAutoPlay(playPauseButton->isChecked());
    });

    QObject::connect(replaceButton, &QPushButton::clicked, [&]() {
        subtitles = AddRandomAttributes(subtitles);
        animatorWid->SetSubtitles(subtitles);
        slider->setRange(0, subtitles.size());
    });

    QObject::connect(exportButton, &QPushButton::clicked, [&]() {
        // 生成提示对话框
        QDialog * progressDialog = new QDialog(window);
        progressDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        progressDialog->setModal(false);
        QLabel * label = new QLabel(u8"正在生成录制视频，请稍候...", progressDialog);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { font-size: 24px; color: white; }");
        QVBoxLayout * dialogLayout = new QVBoxLayout(progressDialog);
        dialogLayout->setContentsMargins(0, 0, 0, 0);
        dialogLayout->addWidget(label);
        progressDialog->setGeometry(window->geometry());
        progressDialog->setAttribute(Qt::WA_TranslucentBackground);
        progressDialog->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
        progressDialog->show();

        // 创建录制窗口
        QApplication::setOverrideCursor(Qt::BlankCursor);
        SubtitleAnimatorWidget * recordingWidget = new SubtitleAnimatorWidget();
        recordingWidget->SetSubtitles(subtitles);
        recordingWidget->setWindowFlags(Qt::FramelessWindowHint);
        recordingWidget->setWindowTitle("Pouring Ducks Backstage");
        recordingWidget->show();
        QPoint globalPosition = animatorWid->mapToGlobal(QPoint(0, 0));
        QRect globalGeometry(globalPosition, animatorWid->size());
        recordingWidget->setGeometry(globalGeometry);
        recordingWidget->raise();

        // 启动录制
        WindowScreenRecorder * recorder = new WindowScreenRecorder(recordingWidget);
        QTimer::singleShot(10, nullptr, [recordingWidget, recorder]() {
            recordingWidget->SetAutoPlay(true);
            recorder->StartRecording();
        });

        // 停止录制
        QTimer::singleShot(10 * 1000, nullptr, [recordingWidget, recorder, progressDialog]() {
            recorder->StopRecording();
            recordingWidget->SetAutoPlay(false);
            recordingWidget->SetCurrentIndex(0);
            recordingWidget->close();
            progressDialog->close();
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        });
    });

    window->show();

    return app.exec();
}
