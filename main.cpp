#include <QCoreApplication>
#include <iostream>
#include <string>
#include <QDebug>
#include "manager.hpp"
#include "util.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Manager manager;
    manager.connect();

    // Init
    bool ok = true;
    QString error = "";
    bool force = false;
    bool add = false;
    bool download = false;
    bool downloadAndAdd = false;
    bool help = false;
    QString date1 = "";
    QString date2 = "";
    //
    if(ok && argc < 2)
    {
        ok = false;
        error = " not enough argument. Try typing help to get some help.";
    }
    //
    if(ok)
    {
        for(int i = 1 ; i < argc ; i++)
        {
            if(std::string(argv[i]) == "add")
            {
                add = true;
                if(std::string(argv[i+1]) == "from"
                   && std::string(argv[i+3]) == "to")
                {
                    date1 = argv[i+2];
                    date2 = argv[i+4];
                }
                else
                {
                    date1 = date2 = argv[i+1];
                }
            }
            else if(std::string(argv[i]) == "dl")
            {
                download = true;
                if(std::string(argv[i+1]) == "from"
                   && std::string(argv[i+3]) == "to")
                {
                    date1 = argv[i+2];
                    date2 = argv[i+4];
                }
                else
                {
                    date1 = date2 = argv[i+1];
                }
            }
            else if(std::string(argv[i]) == "dlnadd")
            {
                downloadAndAdd = true;
                if(std::string(argv[i+1]) == "from"
                   && std::string(argv[i+3]) == "to")
                {
                    date1 = argv[i+2];
                    date2 = argv[i+4];
                }
                else
                {
                    date1 = date2 = argv[i+1];
                }
            }
            else if(std::string(argv[i]) == "-f")
            {
                force = true;
            }
            else if(std::string(argv[i]) == "help")
            {
                help = true;
            }
        }
    }
    //
    if(ok)
    {
        if(add)
        {
            manager.add(date1, date2, force);
        }
        else if(download)
        {
            manager.download(date1, date2, force);
        }
        else if(downloadAndAdd)
        {
            manager.downloadAndAdd(date1, date2, force);
        }
        else if(help)
        {
            Util::showFile("help.txt");
        }
    }
    //
    if(!ok)
        Util::addMessage("ERROR : " + error);
    return 0;
}
