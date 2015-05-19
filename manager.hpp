#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QDate>

class Manager : public QObject
{
        Q_OBJECT
    public:
        Manager();
        ~Manager();
        void add(const QDate & date, const bool & forced);
        void add(const QString & dateFirststr,
                 const QString & dateLaststr,
                 const bool & force);
        void download(const QDate & date, const bool & force);
        void download(const QString & dateFirststr,
                      const QString & dateLaststr,
                      const bool & force);
        void downloadAndAdd(const QDate & date, const bool & force);
        void downloadAndAdd(const QString & dateFirststr,
                            const QString & dateLaststr,
                            const bool & force);
        bool connect();
    private:
        QSqlDatabase database;
        bool connected;
};

#endif // MANAGER_HPP
