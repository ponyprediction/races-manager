#include "manager.hpp"
#include "util.hpp"
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <iostream>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QFileInfo>

Manager::Manager() :
    database(),
    connected(false)
{

}

Manager::~Manager()
{

}

void Manager::add(const QDate & date, const bool & forced)
{
    // Init
    bool ok = true;
    QString error = "";
    QString tableName;
    QString filePath;
    // Valid date ?
    if(ok
       && !(date <= QDate::currentDate().addDays(1)
            && date >= QDate(2010, 1, 1)))
    {
        ok = false;
        error = "invalid date";
    }
    // Connected ?
    if(ok && !connected)
    {
        ok = false;
        error = "not connected to any database";
    }
    // starting message
    if(ok)
    {
        Util::addMessage("Adding : " + date.toString("yyyy-MM-dd"));
    }
    //
    if(ok)
    {
        tableName = "day" + date.toString("yyyyMMdd");
        filePath = Util::getLineFromConf("pathToRawData")
                + "/" + tableName + ".csv";
    }
    // Drop table if asked
    if(ok && forced)
    {
        QSqlQuery query;
        QString statement = "DROP TABLE IF EXISTS " + tableName + ";";
        query.prepare(statement);
        if(!query.exec())
        {
            ok = false;
            error = "couldn't drop table (" + tableName + ") "
                    + database.lastError().text() + query.lastError().text();
        }
    }
    // Create table
    if(ok)
    {
        QSqlQuery query;
        QString statement = "";
        QStringList columns;
        QFile file(Util::getLineFromConf("pathToColumn") + "/columns.txt");
        if(!file.open(QIODevice::ReadOnly)) {
            ok = false;
            error = Util::getLineFromConf("pathToColumn")
                    + "/columns.txt " + file.errorString();
        }
        if(ok)
        {
            QTextStream in(&file);
            while(!in.atEnd())
            {
                columns.append(in.readLine());
            }
            file.close();
            statement = " CREATE TABLE ";
            statement += tableName + " ( ";
            for(int i = 0 ; i < columns.size() - 1 ; i++)
            {
                statement += " " + columns[i] + " text, ";
            }
            statement += " " + columns[columns.size() - 1] + " text );";
            query.prepare(statement);
            if(ok && !query.exec())
            {
                ok = false;
                error = "couldn't create table '" + tableName + "' : "
                        + database.lastError().text()
                        + query.lastError().text();
            }
        }
    }
    // Fill table
    if(ok)
    {
        QSqlQuery query;
        QString statement = "";
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly)) {
            ok = false;
            error = filePath +" "+ file.errorString();
        }
        QTextStream in(&file);
        in.setCodec("UTF-8");
        int lineNumber = 0;
        while(!in.atEnd())
        {
            lineNumber++;
            QString line = in.readLine();
            if(line[line.size()-1] == ';')
                line.remove(line.size()-1, 1);
            QStringList values = line.split(";");
            if(values.size() == Util::getLineFromConf("columnCount").toInt())
            {
                statement = " INSERT INTO " + tableName + " VALUES (";
                for(int i = 0 ; i < values.size() - 1 ; i++)
                {
                    statement += " '" + values[i] + "' , ";
                }
                statement += " '" + values[values.size()-1] + "' );";
                query.prepare(statement);
                if(!query.exec())
                {
                    Util::addMessage("ERROR: couldn't insert line "
                                     + QString::number(lineNumber)
                                     + " into " + tableName
                                     + database.lastError().text()
                                     + query.lastError().text());
                }
            }
            else
            {
                Util::addMessage("ERROR: Not the right count of value: "
                                 + QString::number(values.size())
                                 + " instead of "
                                 + Util::getLineFromConf("columnCount"));
            }
        }
        file.close();
    }
    // The end
    if(!ok)
        Util::addMessage("ERROR : " + error);
    else if(ok)
        Util::addMessage("Done adding");

}

void Manager::add(const QString & dateFirststr,
                  const QString & dateLaststr,
                  const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QDate dateFirst;
    QDate dateLast;
    // today | other day
    if(ok && dateFirststr == "today")
    {
        dateFirst = QDate::currentDate();
    }
    else if(ok)
    {
        dateFirst = QDate::fromString(dateFirststr, "yyyy-MM-dd");
    }
    if(ok && dateLaststr == "today")
    {
        dateLast = QDate::currentDate();
    }
    else if(ok)
    {
        dateLast = QDate::fromString(dateLaststr, "yyyy-MM-dd");
    }
    // Valid interval ?
    if(ok
       && dateFirst <= QDate::currentDate().addDays(1)
       && dateFirst >= QDate(2010, 1, 1)
       && dateLast <= QDate::currentDate().addDays(1)
       && dateLast >= QDate(2010, 1, 1)
       && dateFirst <= dateLast)
    {
        //ok
    }
    else if(ok)
    {
        ok = false;
        error = " not a valid interval";
    }
    //
    if(ok)
    {
        for(QDate date = dateFirst ; date <= dateLast ; date = date.addDays(1))
            add(date, force);
    }
    if(!ok)
    {
        Util::addMessage("ERROR : " + error);
    }
}

void Manager::download(const QDate & date, const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QFile output;
    QString url =
            "http://www.aspiturf.com/PTcoursejourdetinsertdatecsv.php?datejour="
            + date.toString("yyyy-MM-dd");
    QString filename = Util::getLineFromConf("pathToRawData") + "/day"
            + date.toString("yyyyMMdd") + ".csv";
    // Open file
    if(ok && !force && QFile::exists(filename))
    {
        ok = false;
        error = "the file already exists";
    }
    if(ok)
    {
        output.setFileName(filename);
        if (ok && !output.open(QIODevice::WriteOnly))
        {
            ok = false;
            error = "couldn't open " + QFileInfo(output).absoluteFilePath();
        }
    }
    // starting message
    if(ok)
    {
        Util::addMessage("Downloading : " + date.toString("yyyy-MM-dd"));
    }
    // Download
    if(ok)
    {
        QNetworkRequest request(url);
        QNetworkAccessManager manager;
        QNetworkReply * download = manager.get(request);
        QEventLoop eventLoop;
        QObject::connect(download, SIGNAL(finished()),
                         &eventLoop, SLOT(quit()));
        eventLoop.exec(); // wait until download is done
        output.write(download->readAll());
    }
    output.close();
    // End
    if(!ok)
        Util::addMessage("ERROR: " + error);
    else if(ok)
        Util::addMessage("Download finished");
}

void Manager::download(const QString & dateFirststr,
                       const QString & dateLaststr,
                       const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QDate dateFirst;
    QDate dateLast;
    // today | other day
    if(ok && dateFirststr == "today")
    {
        dateFirst = QDate::currentDate();
    }
    else if(ok)
    {
        dateFirst = QDate::fromString(dateFirststr, "yyyy-MM-dd");
    }
    if(ok && dateLaststr == "today")
    {
        dateLast = QDate::currentDate();
    }
    else if(ok)
    {
        dateLast = QDate::fromString(dateLaststr, "yyyy-MM-dd");
    }
    // Valid interval ?
    if(ok
       && dateFirst <= QDate::currentDate().addDays(1)
       && dateFirst >= QDate(2010, 1, 1)
       && dateLast <= QDate::currentDate().addDays(1)
       && dateLast >= QDate(2010, 1, 1)
       && dateFirst <= dateLast)
    {
        //ok
    }
    else if(ok)
    {
        ok = false;
        error = " not a valid interval";
    }
    //
    if(ok)
    {
        for(QDate date = dateFirst ; date <= dateLast ; date = date.addDays(1))
            download(date, force);
    }
    if(!ok)
    {
        Util::addMessage("ERROR : " + error);
    }
}

void Manager::downloadAndAdd(const QDate & date, const bool & force)
{
    download(date, force);
    add(date, force);
}

void Manager::downloadAndAdd(const QString & dateFirststr,
                             const QString & dateLaststr,
                             const bool & force)
{
    // Init
    bool ok = true;
    QString error = "";
    QDate dateFirst;
    QDate dateLast;
    // today | other day
    if(ok && dateFirststr == "today")
    {
        dateFirst = QDate::currentDate();
    }
    else if(ok)
    {
        dateFirst = QDate::fromString(dateFirststr, "yyyy-MM-dd");
    }
    if(ok && dateLaststr == "today")
    {
        dateLast = QDate::currentDate();
    }
    else if(ok)
    {
        dateLast = QDate::fromString(dateLaststr, "yyyy-MM-dd");
    }
    // Valid interval ?
    if(ok
       && dateFirst <= QDate::currentDate().addDays(1)
       && dateFirst >= QDate(2010, 1, 1)
       && dateLast <= QDate::currentDate().addDays(1)
       && dateLast >= QDate(2010, 1, 1)
       && dateFirst <= dateLast)
    {
        //ok
    }
    else if(ok)
    {
        ok = false;
        error = " not a valid interval";
    }
    //
    if(ok)
    {
        for(QDate date = dateFirst ; date <= dateLast ; date = date.addDays(1))
        {
            downloadAndAdd(date, force);
        }
    }
    if(!ok)
    {
        Util::addMessage("ERROR : " + error);
    }
}

bool Manager::connect()
{
    bool ok = true;
    QString error = "";
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName(Util::getLineFromConf("username"));
    database.setPassword(Util::getLineFromConf("password"));
    database.setDatabaseName(Util::getLineFromConf("databaseName"));
    if(ok && !database.open())
    {
        ok = false;
        error = "Couldn't connect to database ("
                + Util::getLineFromConf("databaseName") + ") "
                + database.lastError().text();
    }
    if(!ok)
    {
        std::cout << "ERROR: " + error.toStdString() << std::endl;
        database.close();
    }
    else if(ok)
    {
        std::cout << "Connected to database ("
                     + Util::getLineFromConf("databaseName").toStdString()
                     + ")"
                  << std::endl;
    }
    connected = ok;
    return ok;
}
