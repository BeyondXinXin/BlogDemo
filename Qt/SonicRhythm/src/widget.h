#ifndef WIDGET_H
#define WIDGET_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioBuffer>
#include <QWidget>

class TimeWaveformWidget;
class FrequencySpectrumWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget * parent = nullptr);

private:
    void OnAudioBufferReceived(const QAudioBuffer & buffer);
    std::vector<float> PrepareAudioData(const std::vector<float> & audioData);

private:
    TimeWaveformWidget * m_TimeWaveform = nullptr;
    FrequencySpectrumWidget * m_FrequencySpectrum = nullptr;
};

#endif
