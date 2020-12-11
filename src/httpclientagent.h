#ifndef HTTPCLIENTAGENT_H
#define HTTPCLIENTAGENT_H

#include "commontype.h"
#include <memory>


namespace httplib
{
	class Client;
}

class UniqueAuthDatabase;

class httpClientAgent
{
public:
	static httpClientAgent *Instance();
    bool ReInitClient();
    bool GetST(const AppData &appData, std::string &strST);
    bool Logout();

private:
	httpClientAgent();
	~httpClientAgent();

protected:
    bool InitDB();
	bool InitClient();
	void DestroyClient();
    bool GetTGT(const AppData &appData, std::string &strTGT);
    bool VerifyLocalTGT(const AppData &appData, std::string &strTGT);
    bool GetP7Sign(std::string &p7Sign);

private:
    std::shared_ptr<UniqueAuthDatabase> m_uniqueDb;
	std::shared_ptr<httplib::Client> m_clientPtr;
    std::string m_strRandom;
};

#endif // HTTPCLIENTAGENT_H
