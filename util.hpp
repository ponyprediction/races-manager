#ifndef UTIL_HPP
#define UTIL_HPP

#include <QString>

class Util
{
    public:
        Util();
        ~Util();
        static QString getLineFromConf(const QString & id);
        static void addMessage(const QString & message);
        static void showFile(const QString & path);
};

#endif // UTIL_HPP
