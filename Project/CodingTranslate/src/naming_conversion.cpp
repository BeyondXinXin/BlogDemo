#include "naming_conversion.h"

#include <QDebug>
#include <QRegularExpression>
#include <QStringList>

bool NamingConversion::SourceIsCode(const QString& str)
{
    return !str.contains("\n")
      && !str.contains(QRegExp("[\\x4e00-\\x9fa5]+"))
      && !str.contains(QRegExp(",.£¬¡£()"));
}

QString NamingConversion::ConversionSource(const QString& str)
{
    QString new_str;
    if (!str.contains(QRegExp("[a-z]"))) {
        new_str = str.toLower();
    } else {
        QStringList strs = str.split(QRegularExpression("(?=[A-Z)|(?<=[A-Z])"), QString::SkipEmptyParts);
        new_str = strs.join(" ").toLower();
    }

    new_str.replace("_", " ");
    new_str.replace("->", " ");
    new_str.replace(".", " ");
    new_str.replace("-", " ");
    new_str.replace("&", " ");
    new_str.replace("::", " :: ");

    new_str = new_str.simplified();
    return new_str;
}
