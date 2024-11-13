#ifndef NAMINGCONVERSION_H
#define NAMINGCONVERSION_H

#include <QString>

class NamingConversion
{
    enum SourceType
    {
        Text,
        Code
    };

public:
    static bool SourceIsCode(const QString& str);
    static QString ConversionSource(const QString& str);
};

#endif // NAMINGCONVERSION_H
