#ifndef QONLINETRANSLATOR_H
#define QONLINETRANSLATOR_H

#include <QAbstractTransition>
#include <QMap>
#include <QMediaContent>
#include <QPointer>
#include <QVector>

class QStateMachine;
class QState;
class QNetworkAccessManager;
class QNetworkReply;
class QOnlineTranslator;

struct QOption
{
    QString word;
    QString gender;
    QStringList translations;
    QJsonObject toJson() const;
};

class RetranslationTransition : public QAbstractTransition
{
public:
    RetranslationTransition(
      QOnlineTranslator* translator, QState* sourceState = nullptr);

protected:
    bool eventTest(QEvent*) override;
    void onTransition(QEvent*) override;

private:
    QOnlineTranslator* translator_;
};

class QOnlineTranslator : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QOnlineTranslator)

    friend class QOnlineTts;

public:
    enum Language
    {
        NoLanguage = -1,
        Auto,
        English,
        Chinese
    };
    Q_ENUM(Language)
    using LanguageCode = QMap<QOnlineTranslator::Language, QString>;

    enum Engine
    {
        Google = 0,
        Bing = 1,
        Baidu = 2
    };
    Q_ENUM(Engine)

    enum TranslationError
    {
        NoError, /** 无错误条件 */
        ParametersError, /** 不支持的参数组合 */
        NetworkError, /** 网络错误 */
        ServiceError, /** 服务不可用或最大请求数 */
        ParsingError /** 无法分析请求 */
    };

public:
    explicit QOnlineTranslator(QObject* parent = nullptr);

    // 翻译
    void translate(const QString& text,
                   Engine engine = Baidu);
    void Abort();
    TranslationError Error() const;
    QString ErrorString() const;
    bool IsRunning() const;

    void setSourceTranslitEnabled(bool enable);
    void setTranslationTranslitEnabled(bool enable);
    void setTranslationOptionsEnabled(bool enable);

    QString GetTranslation() const;
    QString GetEnglishText() const;
    Language GetSourceLanguage() const;
    Language GetTranslationLanguage() const;

    void SetBaiduKey(const QString& id, const QString& key);

    void ChangeTrLanguage();
    void ChangeSrcLanguage();
    void ResetTrData();

    QList<QMediaContent> GenerateTextToSpeech(const QString& text, QString region = "en-US");

signals:
    void SgnFinished();

private slots:
    void skipGarbageText();

    // Google
    void RequestGoogle();
    void ParseGoogle();

    // Bing
    void RequestBingCredentials();
    void ParseBingCredentials();
    void RequestBingTranslate();
    void ParseBingTranslate();
    void RequestBingDictionary();
    void ParseBingDictionary();

    // Baidu
    void RequestBaidu();
    void ParseBaidu();

private:
    void BuildGoogleStateMachine();
    void BuildBingStateMachine();
    void BuildBaiduStateMachine();

    void BuildSplitNetworkRequest(
      QState* parent,
      void (QOnlineTranslator::*request_method)(),
      void (QOnlineTranslator::*parse_method)(),
      const QString& text, int text_limit);

    void BuildNetworkRequestState(
      QState* parent,
      void (QOnlineTranslator::*request_method)(),
      void (QOnlineTranslator::*parse_method)(),
      const QString& text = {});

    void ResetData(TranslationError error = NoError,
                   const QString& error_string = {});

private:
    QStateMachine* state_machine_;

    QNetworkAccessManager* net_manager_;
    QPointer<QNetworkReply> current_reply_;

    Language source_lang_ = NoLanguage;
    Language translation_lang_ = NoLanguage;
    TranslationError error_ = NoError;

    QString source_;
    QString source_translit_;
    QString source_transcription_;
    QString translation_;
    QString translation_translit_;
    QString error_string_;
    QMap<QString, QVector<QOption>> translation_options_;

    bool m_sourceTranslitEnabled = true;
    bool m_translationTranslitEnabled = true;
    bool m_translationOptionsEnabled = true;

    QString baidu_id_;
    QString baidu_key_;

public:
    static QString GetLanguageName(Language lang);
    static QString GetLanguageCode(Language lang);
    static Language GetLanguage(const QLocale& locale);
    static Language GetLanguage(const QString& langCode);

private:
    static QString LanguageApiCode(Engine engine, Language lang);
    static Language GetLanguage(Engine engine, const QString& langCode);
    static int GetSplitIndex(const QString& untranslated_text, int limit);
    static bool IsContainsSpace(const QString& text);
    static void AddSpaceBetweenParts(QString& text);

private:
    static const LanguageCode generic_language_codes;
    static const LanguageCode google_language_codes;
    static const LanguageCode bing_language_codes;
    static const LanguageCode baidu_language_codes;

    static QByteArray bing_key;
    static QByteArray bing_token;
    static QString bing_ig;
    static QString bing_iid;

    static constexpr char text_property[] = "Text";

    static constexpr int google_translate_limit = 5000;
    static constexpr int bing_translate_limit = 5001;
    static constexpr int baidu_translate_limit = 6000;
};

#endif // QONLINETRANSLATOR_H
