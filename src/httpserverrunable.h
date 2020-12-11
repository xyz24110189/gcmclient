#ifndef HTTPSERVERRUNABLE_H
#define HTTPSERVERRUNABLE_H
#include "commontype.h"
#include <QtCore/QRunnable>
#include <memory>
#include <string>
#include <map>

namespace httplib
{
    class Server;
	struct Request;
	struct Response;
}

using namespace httplib;

class HttpServerRunable : public QRunnable
{
	enum HTTPSTATUS
	{
		GCM_SERVER_ERROR = 600
	};
public:
	void ShutDown();
	void DestroyServer();
    void SetErrorCode(ResErrCode errCode);

	static HttpServerRunable *Instance();

protected:
    void run();

    bool InitServer();
	void InitRouter();
    void InitErrorInfoMap();

	void DevInfoHandle(const Request *req, Response *res);
	void CertRequestHandle(const Request *req, Response *res);
	void CertInstallHandle(const Request *req, Response *res);
	void CertDeleteHandle(const Request *req, Response *res);
    void UnLockHandle(const Request *req, Response *res);
    void GetST(const Request *req, Response *res);
    void Logout(const Request *req, Response *res);

private:
	HttpServerRunable();
	~HttpServerRunable();

private:
	bool m_bExit;
    ResErrCode m_errCode;
    std::map<ResErrCode, std::string> m_errorInfoMap;
    std::shared_ptr<httplib::Server> m_serverPtr;
};

#endif // HTTPSERVERRUNABLE_H
