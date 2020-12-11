#include "utils.h"
#include "commontype.h"
#include <fstream>
#include <QString>
#include <QFile>
#include <QNetworkInterface>

#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

#define WS " \t\n\r\f\v"

struct StringUtils_p {
    // trim from end of string (right)
    std::string &rtrim(std::string &s, const char *t = WS)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    std::string &ltrim(std::string &s, const char *t = WS)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (left & right)
    std::string &trim(std::string &s, const char *t = WS)
    {
        return ltrim(rtrim(s, t), t);
    }

    bool getValue(const char *key, const std::string &line, const std::string &sep, std::string &val)
    {
        size_t found = line.find(key);

        if (found == std::string::npos) {
            return false;
        }

        size_t pos = line.find(sep);
        if (pos == std::string::npos) {
            // 没找到分隔符
            return false;
        }
        val = line.substr(pos + sep.length());
        trim(val);
        return true;
    }
};

namespace GCM {
namespace Utils {

std::string getMacAddress()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback and isUp & isRunning MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack) &&
             (netInterface.flags() & QNetworkInterface::IsUp) &&
             (netInterface.flags() & QNetworkInterface::IsRunning))
        {
            QString strMacAddr = netInterface.hardwareAddress();
            if (0 != strMacAddr.compare("00:00:00:00:00:00"))
                return strMacAddr.toLocal8Bit().constData();
        }
    }
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
        {
            QString strMacAddr = netInterface.hardwareAddress();
            if (0 != strMacAddr.compare("00:00:00:00:00:00"))
                return strMacAddr.toLocal8Bit().constData();
        }

    }
    return "";
}

std::string getMachineID()
{
    QFile sysinfoFile(SYSTEM_INFO_FILE);
    if (!sysinfoFile.exists())
    {
        QFile machineFile(SYSTEM_MACHINE_ID);
        if (machineFile.exists())
        {
            std::string machineID;
            if (machineFile.open(QIODevice::ReadOnly))
            {
                machineID = machineFile.readAll().trimmed().constData();
                machineFile.close();
            }
            return machineID;
        }
        else
            return "";
    }

    StringUtils_p utils;
    GcmSysInfo sysInfo;
    std::string sep = ": ";

    std::ifstream is(SYSTEM_INFO_FILE);
    std::string line;
    while (getline(is, line)) {
        utils.getValue("产品名称", line, sep, sysInfo.productName);
        utils.getValue("产品型号", line, sep, sysInfo.productModel);
        utils.getValue("电磁", line, sep, sysInfo.shelterModel);
        utils.getValue("标识码", line, sep, sysInfo.ID);
        utils.getValue("生产者", line, sep, sysInfo.producer);
        utils.getValue("操作系统", line, sep, sysInfo.name);
        utils.getValue("系统版本", line, sep, sysInfo.release);
        utils.getValue("内核版本", line, sep, sysInfo.kernel);
        utils.getValue("系统位数", line, sep, sysInfo.bit);
        utils.getValue("I/O", line, sep, sysInfo.IO);
        utils.getValue("安全卡", line, sep, sysInfo.version);
        utils.getValue("固件版本", line, sep, sysInfo.biosVersion);
        utils.getValue("处理器", line, sep, sysInfo.CPU);
        utils.getValue("内存", line, sep, sysInfo.memory);
        utils.getValue("硬盘序列号", line, sep, sysInfo.hdSerial);
        utils.getValue("硬盘容量", line, sep, sysInfo.hdCapacity);
        utils.getValue("主板", line, sep, sysInfo.mainBoard);
    }
    return sysInfo.ID;
}

void Sleep(unsigned int ms)
{
#ifdef __linux__
    usleep(ms * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
#ifdef _WIN32
    ::Sleep(ms);
#endif
}

}
}

