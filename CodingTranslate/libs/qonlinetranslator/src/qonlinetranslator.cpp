#include "qonlinetranslator.h"

#include <QCoreApplication>
#include <QFinalState>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QStateMachine>
#include <QThread>
#include <QUrlQuery>

QByteArray QOnlineTranslator::bing_key;
QByteArray QOnlineTranslator::bing_token;
QString QOnlineTranslator::bing_ig;
QString QOnlineTranslator::bing_iid;

QJsonObject QOption::toJson() const
{
    QJsonObject object {
        { "gender", gender },
        { "translations", QJsonArray::fromStringList(translations) },
        { "word", word },
    };
    return object;
}

RetranslationTransition::RetranslationTransition(
  QOnlineTranslator* translator, QState* sourceState)
  : QAbstractTransition(sourceState)
  , translator_(translator)
{
}

bool RetranslationTransition::eventTest(QEvent*)
{
    bool result = false;
    if (translator_->GetSourceLanguage()
        == QOnlineTranslator::NoLanguage) {
        translator_->ChangeSrcLanguage();
        result = true;
    }
    if (translator_->GetSourceLanguage()
        == translator_->GetTranslationLanguage()) {

        translator_->ChangeTrLanguage();
        result = true;
    }

    if (result) {
        translator_->ResetTrData();
    }

    return result;
}

void RetranslationTransition::onTransition(QEvent*)
{
}

const QMap<QOnlineTranslator::Language, QString> QOnlineTranslator::generic_language_codes = {
    { Auto, QStringLiteral("auto") },
    { English, QStringLiteral("en") },
    { Chinese, QStringLiteral("zh-CN") }
};

const QMap<QOnlineTranslator::Language, QString> QOnlineTranslator::google_language_codes = {};

const QMap<QOnlineTranslator::Language, QString> QOnlineTranslator::bing_language_codes = {
    { Auto, QStringLiteral("auto-detect") },
    { Chinese, QStringLiteral("zh-Hans") }
};
const QMap<QOnlineTranslator::Language, QString> QOnlineTranslator::baidu_language_codes = {
    { Chinese, QStringLiteral("zh") }
};

QOnlineTranslator::QOnlineTranslator(QObject* parent)
  : QObject(parent)
  , state_machine_(new QStateMachine(this))
  , net_manager_(new QNetworkAccessManager(this))
{
    connect(state_machine_, &QStateMachine::finished,
            this, &QOnlineTranslator::SgnFinished);

    connect(state_machine_, &QStateMachine::stopped,
            this, &QOnlineTranslator::SgnFinished);

    source_lang_ = Auto;
    translation_lang_ = Chinese;
}

void QOnlineTranslator::translate(const QString& text, Engine engine)
{
    Abort();
    ResetData();

    source_lang_ = Auto;
    source_ = text;

    switch (engine) {
    case Google:
        BuildGoogleStateMachine();
        break;
    case Bing:
        BuildBingStateMachine();
        break;
    case Baidu:
        BuildBaiduStateMachine();
        break;
    }

    state_machine_->start();
}

void QOnlineTranslator::Abort()
{
    if (current_reply_ != nullptr) {
        current_reply_->abort();
    }
}

bool QOnlineTranslator::IsRunning() const
{
    return state_machine_->isRunning();
}

QOnlineTranslator::TranslationError QOnlineTranslator::Error() const
{
    return error_;
}

QString QOnlineTranslator::ErrorString() const
{
    return error_string_;
}

void QOnlineTranslator::setSourceTranslitEnabled(bool enable)
{
    m_sourceTranslitEnabled = enable;
}

void QOnlineTranslator::setTranslationTranslitEnabled(bool enable)
{
    m_translationTranslitEnabled = enable;
}

void QOnlineTranslator::setTranslationOptionsEnabled(bool enable)
{
    m_translationOptionsEnabled = enable;
}

QString QOnlineTranslator::GetTranslation() const
{
    return translation_;
}

QString QOnlineTranslator::GetEnglishText() const
{
    if (GetSourceLanguage() == English) {
        return source_;
    }

    if (GetTranslationLanguage() == English) {
        return translation_;
    }

    return "";
}

QOnlineTranslator::Language QOnlineTranslator::GetSourceLanguage() const
{
    return source_lang_;
}

QOnlineTranslator::Language QOnlineTranslator::GetTranslationLanguage() const
{
    return translation_lang_;
}

void QOnlineTranslator::SetBaiduKey(const QString& id, const QString& key)
{
    baidu_id_ = id;
    baidu_key_ = key;
}

void QOnlineTranslator::ChangeTrLanguage()
{
    translation_lang_ = translation_lang_ == English ? Chinese : English;
}

void QOnlineTranslator::ChangeSrcLanguage()
{
    switch (source_lang_) {
    case English:
    case Chinese: {
        break;
    }
    default: {
        source_lang_ = English;
        break;
    }
    }
}

void QOnlineTranslator::ResetTrData()
{
    translation_.clear();
    translation_translit_.clear();
    source_translit_.clear();
    source_transcription_.clear();
    translation_options_.clear();
}

void QOnlineTranslator::skipGarbageText()
{
    translation_.append(sender()->property(text_property).toString());
}

void QOnlineTranslator::RequestGoogle()
{
    const QString sourceText = sender()->property(text_property).toString();
    QUrl url(
      QStringLiteral("https://translate.googleapis.com/translate_a/single"));
    url.setQuery(
      QStringLiteral("client=gtx&ie=UTF-8&oe=UTF-8&dt=bd&dt=ex"
                     "&dt=ld&dt=md&dt=rw&dt=rm&dt=ss&dt=t"
                     "&dt=at&dt=qc&sl=%1&tl=%2&q=%3")
        .arg(LanguageApiCode(Google, source_lang_),
             LanguageApiCode(Google, translation_lang_),
             QUrl::toPercentEncoding(sourceText)));

    current_reply_ = net_manager_->get(QNetworkRequest(url));
}

void QOnlineTranslator::ParseGoogle()
{
    current_reply_->deleteLater();

    // Check for error
    if (current_reply_->error() != QNetworkReply::NoError) {
        if (current_reply_->error() == QNetworkReply::ServiceUnavailableError)
            ResetData(ServiceError, tr("Error: Engine systems have detected suspicious traffic from your computer network. Please try your request again later."));
        else
            ResetData(NetworkError, current_reply_->errorString());
        return;
    }

    // Check availability of service
    const QByteArray data = current_reply_->readAll();
    if (data.startsWith('<')) {
        ResetData(ServiceError, tr("Error: Engine systems have detected suspicious traffic from your computer network. Please try your request again later."));
        return;
    }

    // Read Json
    const QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    const QJsonArray jsonData = jsonResponse.array();

    if (source_lang_ == Auto) {
        // Parse language
        source_lang_ = GetLanguage(Google, jsonData.at(2).toString());
        if (source_lang_ == NoLanguage) {
        }
    }

    AddSpaceBetweenParts(translation_);
    AddSpaceBetweenParts(translation_translit_);
    AddSpaceBetweenParts(source_translit_);
    for (const QJsonValueRef translationData : jsonData.at(0).toArray()) {
        const QJsonArray translationArray = translationData.toArray();
        translation_.append(translationArray.at(0).toString());
        if (m_translationTranslitEnabled)
            translation_translit_.append(translationArray.at(2).toString());
        if (m_sourceTranslitEnabled)
            source_translit_.append(translationArray.at(3).toString());
    }

    if (source_.size() >= google_translate_limit) {
        return;
    }

    // Translation options
    if (m_translationOptionsEnabled) {
        for (const QJsonValueRef typeOfSpeechData : jsonData.at(1).toArray()) {
            const QJsonArray typeOfSpeechDataArray = typeOfSpeechData.toArray();
            const QString typeOfSpeech = typeOfSpeechDataArray.at(0).toString();
            for (const QJsonValueRef wordData : typeOfSpeechDataArray.at(2).toArray()) {
                const QJsonArray wordDataArray = wordData.toArray();
                const QString word = wordDataArray.at(0).toString();
                const QString gender = wordDataArray.at(4).toString();
                const QJsonArray translationsArray = wordDataArray.at(1).toArray();
                QStringList translations;
                translations.reserve(translationsArray.size());
                for (const QJsonValue& wordTranslation : translationsArray)
                    translations.append(wordTranslation.toString());
                translation_options_[typeOfSpeech].append({ word, gender, translations });
            }
        }
    }
}

void QOnlineTranslator::RequestBaidu()
{
    const QString source_text = sender()->property(text_property).toString();
    static const QString base_url = "http://api.fanyi.baidu.com/api/trans/vip/translate";
    static const QString salt =
      QString::number(QRandomGenerator::global()->bounded(50, 18000));
    const QString sign = QString("%1%2%3%4")
                           .arg(baidu_id_)
                           .arg(source_text)
                           .arg(salt)
                           .arg(baidu_key_);
    const QString sign_hash = QCryptographicHash::hash(
                                sign.toUtf8(), QCryptographicHash::Md5)
                                .toHex();

    QUrlQuery query;
    query.addQueryItem("q", source_text);
    query.addQueryItem("from", LanguageApiCode(Baidu, source_lang_));
    query.addQueryItem("to", LanguageApiCode(Baidu, translation_lang_));
    query.addQueryItem("appid", baidu_id_);
    query.addQueryItem("salt", salt);
    query.addQueryItem("sign", sign_hash);

    QUrl url(base_url);
    url.setQuery(query);
    current_reply_ = net_manager_->get(QNetworkRequest(url));
}

void QOnlineTranslator::ParseBaidu()
{
    // Check for error
    if (current_reply_->error() != QNetworkReply::NoError) {
        if (current_reply_->error() == QNetworkReply::ServiceUnavailableError) {
            ResetData(ServiceError,
                      u8"错误：引擎系统检测到来自计算机网络的可疑流量。请稍后重试您的请求。");
        } else {
            ResetData(NetworkError, current_reply_->errorString());
        }
        return;
    }

    QString all = current_reply_->readAll();
    current_reply_->deleteLater();

    QJsonDocument json_doc = QJsonDocument::fromJson(all.toUtf8());
    QJsonObject json_object = json_doc.object();

    auto err_msg = json_object["error_msg"].toString();
    if (!err_msg.isEmpty()) {
        ResetData(ParametersError, err_msg);
        return;
    }

    source_lang_ = GetLanguage(Baidu, json_object["from"].toString());
    translation_lang_ = GetLanguage(Baidu, json_object["to"].toString());

    auto result_arr = json_object.value("trans_result").toArray();
    for (auto resultLine : result_arr) {
        auto obj = resultLine.toObject();
        translation_.append(obj["dst"].toString() + "\n");
    }
}

void QOnlineTranslator::RequestBingCredentials()
{
    const QUrl url(QStringLiteral("https://www.bing.com/translator"));
    current_reply_ = net_manager_->get(QNetworkRequest(url));
}

void QOnlineTranslator::ParseBingCredentials()
{
    current_reply_->deleteLater();

    if (current_reply_->error() != QNetworkReply::NoError) {
        ResetData(NetworkError, current_reply_->errorString());
        return;
    }

    const QByteArray webSiteData = current_reply_->readAll();
    const QByteArray credentialsBeginString = "var params_RichTranslateHelper = [";
    const int credentialsBeginPos = webSiteData.indexOf(credentialsBeginString);
    if (credentialsBeginPos == -1) {
        ResetData(ParsingError, tr("Error: Unable to find Bing credentials in web version."));
        return;
    }

    const int keyBeginPos = credentialsBeginPos + credentialsBeginString.size();
    const int keyEndPos = webSiteData.indexOf(',', keyBeginPos);
    if (keyEndPos == -1) {
        ResetData(ParsingError, tr("Error: Unable to extract Bing key from web version."));
        return;
    }
    bing_key = webSiteData.mid(keyBeginPos, keyEndPos - keyBeginPos);
    // Skip two symbols instead of one because the value is enclosed in quotes
    const int tokenBeginPos = keyEndPos + 2;
    const int tokenEndPos = webSiteData.indexOf('"', tokenBeginPos);
    if (tokenEndPos == -1) {
        ResetData(ParsingError, tr("Error: Unable to extract Bing token from web version."));
        return;
    }
    bing_token = webSiteData.mid(tokenBeginPos, tokenEndPos - tokenBeginPos);

    const int igBeginPos = webSiteData.indexOf("IG");
    const int igEndPos = webSiteData.indexOf('"', igBeginPos + 2);
    if (igEndPos == -1) {
        ResetData(ParsingError, tr("Error: Unable to extract additional Bing information from web version."));
        return;
    }
    bing_ig = webSiteData.mid(igBeginPos, igEndPos - igBeginPos);

    const int iidBeginPos = webSiteData.indexOf("data-iid");
    const int iidEndPos = webSiteData.indexOf('"', iidBeginPos + 2);
    if (iidEndPos == -1) {
        ResetData(ParsingError, tr("Error: Unable to extract additional Bing information from web version."));
        return;
    }
    bing_iid = webSiteData.mid(iidBeginPos, iidEndPos - iidBeginPos);
}

void QOnlineTranslator::RequestBingTranslate()
{
    const QString sourceText = sender()->property(text_property).toString();

    // Generate POST data
    const QByteArray postData = "&text=" + QUrl::toPercentEncoding(sourceText)
      + "&fromLang=" + LanguageApiCode(Bing, source_lang_).toUtf8()
      + "&to=" + LanguageApiCode(Bing, translation_lang_).toUtf8()
      + "&token=" + bing_token
      + "&key=" + bing_key;

    QUrl url(QStringLiteral("https://www.bing.com/ttranslatev3"));
    url.setQuery(QStringLiteral("IG=%1&IID=%2").arg(bing_ig, bing_iid));

    // Setup request
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, QCoreApplication::applicationName() + '/' + QCoreApplication::applicationVersion());
    request.setUrl(url);

    // Make reply
    current_reply_ = net_manager_->post(request, postData);
}

void QOnlineTranslator::ParseBingTranslate()
{
    current_reply_->deleteLater();

    // Check for errors
    if (current_reply_->error() != QNetworkReply::NoError) {
        ResetData(NetworkError, current_reply_->errorString());
        return;
    }

    // Parse translation data
    const QJsonDocument jsonResponse = QJsonDocument::fromJson(current_reply_->readAll());
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    if (source_lang_ == Auto) {
        const QString langCode = responseObject.value(QStringLiteral("detectedLanguage")).toObject().value(QStringLiteral("language")).toString();
        source_lang_ = GetLanguage(Bing, langCode);
        if (source_lang_ == NoLanguage) {
            ResetData(ParsingError, tr("Error: Unable to parse autodetected language"));
            return;
        }
    }

    const QJsonObject translationsObject = responseObject.value(QStringLiteral("translations")).toArray().first().toObject();
    translation_ += translationsObject.value(QStringLiteral("text")).toString();
    translation_translit_ += translationsObject.value(QStringLiteral("transliteration")).toObject().value(QStringLiteral("text")).toString();
}

void QOnlineTranslator::RequestBingDictionary()
{
    // Generate POST data
    const QByteArray postData = "&text=" + QUrl::toPercentEncoding(sender()->property(text_property).toString())
      + "&from=" + LanguageApiCode(Bing, source_lang_).toUtf8()
      + "&to=" + LanguageApiCode(Bing, translation_lang_).toUtf8();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setUrl(QStringLiteral("https://www.bing.com/tlookupv3"));

    current_reply_ = net_manager_->post(request, postData);
}

void QOnlineTranslator::ParseBingDictionary()
{
    current_reply_->deleteLater();

    // Check for errors
    if (current_reply_->error() != QNetworkReply::NoError) {
        ResetData(NetworkError, current_reply_->errorString());
        return;
    }

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(current_reply_->readAll());
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    for (const QJsonValueRef dictionaryData : responseObject.value(QStringLiteral("translations")).toArray()) {
        const QJsonObject dictionaryObject = dictionaryData.toObject();
        const QString typeOfSpeech = dictionaryObject.value(QStringLiteral("posTag")).toString().toLower();
        const QString word = dictionaryObject.value(QStringLiteral("displayTarget")).toString().toLower();
        const QJsonArray translationsArray = dictionaryObject.value(QStringLiteral("backTranslations")).toArray();
        QStringList translations;
        translations.reserve(translationsArray.size());
        for (const QJsonValue& wordTranslation : translationsArray)
            translations.append(wordTranslation.toObject().value(QStringLiteral("displayText")).toString());

        translation_options_[typeOfSpeech].append({ word, {}, translations });
    }
}

void QOnlineTranslator::BuildGoogleStateMachine()
{
    // States
    auto* trans_state = new QState(state_machine_);
    auto* check_state = new QState(state_machine_);
    auto* final_state = new QFinalState(state_machine_);

    // Transitions
    state_machine_->setInitialState(trans_state);
    trans_state->addTransition(trans_state, &QState::finished, check_state);

    auto* check_transition = new RetranslationTransition(this, check_state);
    check_transition->setTargetState(trans_state);
    check_state->addTransition(final_state);

    // Setup state
    BuildSplitNetworkRequest(trans_state,
                             &QOnlineTranslator::RequestGoogle,
                             &QOnlineTranslator::ParseGoogle,
                             source_, google_translate_limit);
}

void QOnlineTranslator::BuildBingStateMachine()
{
    // States
    auto* credentials_state = new QState(state_machine_);
    auto* translation_state = new QState(state_machine_);
    auto* dictionary_state = new QState(state_machine_);
    auto* final_state = new QFinalState(state_machine_);
    // Transitions
    state_machine_->setInitialState(credentials_state);
    credentials_state->addTransition(credentials_state, &QState::finished, translation_state);
    translation_state->addTransition(translation_state, &QState::finished, dictionary_state);
    dictionary_state->addTransition(dictionary_state, &QState::finished, final_state);
    // Setup state
    if (bing_key.isEmpty() || bing_token.isEmpty()) {
        BuildNetworkRequestState(credentials_state,
                                 &QOnlineTranslator::RequestBingCredentials,
                                 &QOnlineTranslator::ParseBingCredentials);
    } else {
        credentials_state->setInitialState(new QFinalState(credentials_state));
    }

    BuildSplitNetworkRequest(translation_state,
                             &QOnlineTranslator::RequestBingTranslate,
                             &QOnlineTranslator::ParseBingTranslate, source_,
                             bing_translate_limit);

    if (m_translationOptionsEnabled && !IsContainsSpace(source_)) {
        BuildNetworkRequestState(dictionary_state,
                                 &QOnlineTranslator::RequestBingDictionary,
                                 &QOnlineTranslator::ParseBingDictionary, source_);
    } else {
        dictionary_state->setInitialState(new QFinalState(dictionary_state));
    }
}

void QOnlineTranslator::BuildBaiduStateMachine()
{
    // States
    auto* trans_state = new QState(state_machine_);
    auto* check_state = new QState(state_machine_);
    auto* final_state = new QFinalState(state_machine_);

    // Transitions
    state_machine_->setInitialState(trans_state);
    trans_state->addTransition(trans_state, &QState::finished, check_state);

    auto* check_transition = new RetranslationTransition(this, check_state);
    connect(check_transition, &RetranslationTransition::triggered, this, [&]() {
        QThread::msleep(100);

        translation_.clear();
        translation_translit_.clear();
        source_translit_.clear();
        source_transcription_.clear();
        translation_options_.clear();
    });
    check_transition->setTargetState(trans_state);
    check_state->addTransition(final_state);

    // Setup state
    BuildSplitNetworkRequest(trans_state,
                             &QOnlineTranslator::RequestBaidu,
                             &QOnlineTranslator::ParseBaidu,
                             source_, baidu_translate_limit);
}

void QOnlineTranslator::BuildSplitNetworkRequest(
  QState* parent,
  void (QOnlineTranslator::*request_method)(),
  void (QOnlineTranslator::*parse_method)(),
  const QString& text, int text_limit)
{
    QString unsended_text = text;
    auto* next_state = new QState(parent);
    parent->setInitialState(next_state);

    while (!unsended_text.isEmpty()) {
        auto* cur_state = next_state;
        next_state = new QState(parent);
        // 不翻译无用东西
        const int split_index = GetSplitIndex(unsended_text, text_limit);
        if (split_index == -1) {
            cur_state->setProperty(text_property, unsended_text.left(text_limit));
            cur_state->addTransition(next_state);
            connect(cur_state, &QState::entered, this, &QOnlineTranslator::skipGarbageText);
            // 从下一次解析中删除已解析的部分
            unsended_text = unsended_text.mid(text_limit);
        } else {
            BuildNetworkRequestState(cur_state, request_method, parse_method,
                                     unsended_text.left(split_index));
            cur_state->addTransition(cur_state, &QState::finished, next_state);
            // 从下一次解析中删除已解析的部分
            unsended_text = unsended_text.mid(split_index);
        }
    }

    next_state->addTransition(new QFinalState(parent));
}

void QOnlineTranslator::BuildNetworkRequestState(
  QState* parent, void (QOnlineTranslator::*request_method)(),
  void (QOnlineTranslator::*parse_method)(), const QString& text)
{
    // Network substates
    auto* requesting_state = new QState(parent);
    auto* parsing_state = new QState(parent);

    parent->setInitialState(requesting_state);

    // Substates transitions
    requesting_state->addTransition(
      net_manager_, &QNetworkAccessManager::finished, parsing_state);
    parsing_state->addTransition(new QFinalState(parent));

    // Setup requesting state
    requesting_state->setProperty(text_property, text);
    connect(requesting_state, &QState::entered, this, request_method);

    // Setup parsing state
    connect(parsing_state, &QState::entered, this, parse_method);
}

void QOnlineTranslator::ResetData(
  TranslationError error, const QString& error_string)
{
    error_ = error;
    error_string_ = error_string;

    translation_.clear();
    translation_translit_.clear();
    source_translit_.clear();
    source_transcription_.clear();
    translation_options_.clear();

    state_machine_->stop();
    auto states = state_machine_->findChildren<QAbstractState*>();
    for (QAbstractState* state : states) {
        if (!state_machine_->configuration().contains(state)) {
            state->deleteLater();
        }
    }
}

QString QOnlineTranslator::GetLanguageName(Language lang)
{
    switch (lang) {
    case Auto:
        return tr("Automatically detect");
    case Chinese:
        return tr("Chinese (Simplified)");
    case English:
        return tr("English");

    default:
        return {};
    }
}

QString QOnlineTranslator::GetLanguageCode(Language lang)
{
    return generic_language_codes.value(lang);
}

QOnlineTranslator::Language QOnlineTranslator::GetLanguage(const QLocale& locale)
{
    switch (locale.language()) {
    case QLocale::Chinese: {
        return Chinese;
    }
    default: {
        return English;
    }
    }
}

QOnlineTranslator::Language QOnlineTranslator::GetLanguage(const QString& langCode)
{
    return generic_language_codes.key(langCode, NoLanguage);
}

QList<QMediaContent> QOnlineTranslator::GenerateTextToSpeech(const QString& text, QString region)
{
    QList<QMediaContent> media;
    QString unparsedText = text;
    while (!unparsedText.isEmpty()) {
        const int split_index = QOnlineTranslator::GetSplitIndex(unparsedText, 200);
        QUrl api_url(QStringLiteral("https://translate.googleapis.com/translate_tts"));
        qDebug() << api_url;
        const QString query = QStringLiteral("ie=UTF-8&client=gtx&tl=%1&q=%2")
                                .arg(region, QString(QUrl::toPercentEncoding(unparsedText.left(split_index))));
        qDebug() << query;
        api_url.setQuery(query, QUrl::DecodedMode);
        qDebug() << api_url;
        media.append(api_url);
        unparsedText = unparsedText.mid(split_index);
    }
    return media;
}

// Returns engine-specific language code for translation
QString QOnlineTranslator::LanguageApiCode(Engine engine, Language lang)
{
    switch (engine) {
    case Google: {
        return google_language_codes.value(lang, generic_language_codes.value(lang));
    }
    case Bing: {
        return bing_language_codes.value(lang, generic_language_codes.value(lang));
    }
    case Baidu: {
        return baidu_language_codes.value(lang, generic_language_codes.value(lang));
    }
    }

    Q_UNREACHABLE();
}

// Parse language from response language code
QOnlineTranslator::Language QOnlineTranslator::GetLanguage(Engine engine, const QString& langCode)
{
    // Engine exceptions
    switch (engine) {
    case Google: {
        return google_language_codes.key(langCode, generic_language_codes.key(langCode, NoLanguage));
    }
    case Bing: {
        return bing_language_codes.key(langCode, generic_language_codes.key(langCode, NoLanguage));
    }
    case Baidu: {
        return baidu_language_codes.key(langCode, generic_language_codes.key(langCode, NoLanguage));
    }
    }

    Q_UNREACHABLE();
}

// 根据限制获取文本的拆分索引
int QOnlineTranslator::GetSplitIndex(const QString& untranslated_text, int limit)
{
    if (untranslated_text.size() < limit) {
        return limit;
    }

    int split_index = untranslated_text.lastIndexOf(QLatin1String(". "), limit - 1);
    if (split_index != -1) {
        return split_index + 1;
    }
    split_index = untranslated_text.lastIndexOf(' ', limit - 1);
    if (split_index != -1) {
        return split_index + 1;
    }
    split_index = untranslated_text.lastIndexOf('\n', limit - 1);
    if (split_index != -1) {
        return split_index + 1;
    }
    // Non-breaking space
    split_index = untranslated_text.lastIndexOf(0x00a0, limit - 1);
    if (split_index != -1) {
        return split_index + 1;
    }

    return limit;
}

bool QOnlineTranslator::IsContainsSpace(const QString& text)
{
    return std::any_of(text.cbegin(), text.cend(), [](QChar symbol) {
        return symbol.isSpace();
    });
}

void QOnlineTranslator::AddSpaceBetweenParts(QString& text)
{
    if (text.isEmpty())
        return;

    if (!text.back().isSpace()) {
        text.append(' ');
    }
}
