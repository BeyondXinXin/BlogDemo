#include "widget.h"

#include <QtWidgets>
#include <QAudioBufferOutput>

#include "kiss_fft.h"
#include "FrequencySpectrumWidget.h"
#include "TimeWaveformWidget.h"

static float GlobalMaxMagnitude = 1e-6;
static const float DynamicThreshold = 0.1;

Widget::Widget(QWidget * parent)
  : QWidget(parent)
{
    QMediaPlayer * player = new QMediaPlayer(this);
    QAudioOutput * audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    // player->setSource(QUrl::fromLocalFile("./03.mp3"));
    player->setSource(QUrl::fromLocalFile("./111.flac"));

    QAudioFormat formatAudio;
    formatAudio.setSampleRate(44100);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleFormat(QAudioFormat::UInt8);
    QAudioBufferOutput * buffer = new QAudioBufferOutput(formatAudio, this);
    player->setAudioBufferOutput(buffer);
    player->play();
    connect(buffer, &QAudioBufferOutput::audioBufferReceived, this, &Widget::OnAudioBufferReceived);

    m_TimeWaveform = new TimeWaveformWidget;
    m_FrequencySpectrum = new FrequencySpectrumWidget;
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(m_TimeWaveform, 1);
    layout->addWidget(m_FrequencySpectrum, 1);
    setLayout(layout);
}

void Widget::OnAudioBufferReceived(const QAudioBuffer & buffer)
{
    QList<int> timeData;
    QList<int> freqData;
    const quint8 * data = buffer.constData<quint8>();
    int sampleCount = buffer.sampleCount();
    std::vector<float> audioData(sampleCount);

    for (int i = 0; i < sampleCount; ++i) {
        double value = (data[i] - 128.0) / 128.0; // 标准化到 -1.0 到 1.0
        audioData[i] = value;
        timeData << data[i];
    }
    audioData = PrepareAudioData(audioData);

    int N = audioData.size();
    kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, NULL, NULL);

    // 准备输入和输出
    std::vector<kiss_fft_cpx> input(N);
    std::vector<kiss_fft_cpx> output(N);
    for (int i = 0; i < N; ++i) {
        input[i].r = audioData[i];
        input[i].i = 0.0f;
    }

    // FFT
    kiss_fft(cfg, input.data(), output.data());
    free(cfg);

    // 计算 FFT 结果的幅值
    float frameMaxMagnitude = 0.0f;
    std::vector<float> magnitude(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        magnitude[i] = std::sqrt(output[i].r * output[i].r + output[i].i * output[i].i);
        frameMaxMagnitude = qMax(frameMaxMagnitude, magnitude[i]);
    }

    // 更新全局最大值（带平滑）
    GlobalMaxMagnitude = qMax(GlobalMaxMagnitude * 0.9f, frameMaxMagnitude);

    // 计算动态范围的有效最大值
    float effectiveMaxMagnitude = qMax(frameMaxMagnitude, GlobalMaxMagnitude * DynamicThreshold);

    // 归一化
    for (int i = 0; i < N / 2; ++i) {
        magnitude[i] = (magnitude[i] / effectiveMaxMagnitude) * 100.0f;
    }

    // 分组频段范围
    int sampleRate = 44100;

    int lowEnd = 400;
    int midEnd = 4000;
    int highEnd = 20000;

    int lowBins = lowEnd * N / sampleRate;
    int midBins = midEnd * N / sampleRate;
    int highBins = highEnd * N / sampleRate;

    auto addBands = [&](int startBin, int endBin, int bands) {
        int binRange = (endBin - startBin) / bands; // 每个小频段包含的 bin 数量
        for (int i = 0; i < bands; ++i) {
            float bandSum = 0;
            int bandStart = startBin + i * binRange;
            int bandEnd = bandStart + binRange;

            for (int j = bandStart; j < bandEnd && j < magnitude.size(); ++j) {
                bandSum += magnitude[j];
            }

            freqData << bandSum;
        }
    };

    addBands(0, lowBins, 5);
    addBands(lowBins, midBins, 10);
    addBands(midBins, highBins, 5);

    m_TimeWaveform->SetSeriesData(freqData, timeData);
    m_FrequencySpectrum->SetFrequencyData(freqData);
}

std::vector<float> Widget::PrepareAudioData(const std::vector<float> & audioData)
{
    int originalSize = audioData.size();
    int targetSize = pow(2, ceil(log2(originalSize)));

    std::vector<float> preparedData(targetSize, 0.0f); // 初始化为零

    // 拷贝数据，截断或零填充
    for (int i = 0; i < std::min(originalSize, targetSize); ++i) {
        preparedData[i] = audioData[i];
    }

    return preparedData;
}