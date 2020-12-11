#ifndef ERROR_H_20200313
#define ERROR_H_20200313

#include <string>
#include <map>

namespace koal{


#define ERR_SUCCESS         0

// modules
#define ERR_COMMON          0x01000000 
#define ERR_KPKISERVICE     0x02000000 
#define ERR_DEVSERVICE      0x03000000 
#define ERR_ENROLLSERVICE   0x04000000 
#define ERR_SIGNXSERVICE    0x05000000 
#define ERR_KMAILSERVICE    0x06000000 

#define ERR_KEYDRIVER       0x10000000

// thrift rpc接口对外提供的错误，根据业务情况，可以扩充这个错误码
enum rpcError {    
    rpcError_ok,                ///调用正常    
    rpcError_noSession,         ///session不存在    
    rpcError_loginInvalid,      ///登录状态已注销    
    rpcError_notifyExsited,     ///已经注册了Notify
    rpcError_msgTypeError,      ///消息类型错误    
    rpcError_jsonBodyError,     ///消息JsonBody无效/缺少参数    
    rpcError_logined,           ///app已经登录
    rpcError_notifyTimeOut,     ///超时

    rpcError_key_failed         = 0x0A000001,   //失败
    rpcError_key_notSupport     = 0x0A000003,   //不支持的服务
    rpcError_key_keyNotFound    = 0x0A00001B,   //密钥未发现
    rpcError_key_certNotFound   = 0x0A00001C,   //证书未发现
    rpcError_key_pinIncorrect   = 0x0A000024,   //PIN不正确 0x0A000026 PIN无效0x0A000027 PIN长度错误
    rpcError_key_pinLocked      = 0x0A000025,   //PIN被锁死
    rpcError_key_pinTypeInvalid = 0x0A00002A,   //PIN类型错误
    rpcError_key_appExisted     = 0x0A00002C,   //应用已经存在
    rpcError_key_userNotLogined = 0x0A00002D,   //用户没有登录
    rpcError_key_appNotExisted  = 0x0A00002E,   //应用不存在
    rpcError_key_maxContainer   = 0x0A000032,   //已达到最大可管理容器数
    rpcError_key_conNotExisted  = 0x0B000035,	//容器不存在
    rpcError_key_conExisted     = 0x0B000036	//容器已存在

};

#define SET_ERR_CODE(X, Y)			                \
	switch(Y) {						                \
	case rpcError_ok:{				                \
		X = rpcError_ok;			                \
		break;										\
	}												\
	case rpcError_noSession:{				        \
		X = rpcError_noSession;			            \
		 break;										\
	}	                                            \
    case rpcError_loginInvalid:{				    \
		X = rpcError_loginInvalid;			        \
		 break;										\
	}                                               \
    case rpcError_notifyExsited:{				    \
		X = rpcError_notifyExsited;			        \
		 break;										\
	}                                               \
    case rpcError_msgTypeError:{				    \
		X = rpcError_msgTypeError;			        \
		 break;										\
	}                                               \
    case rpcError_jsonBodyError:{				    \
		X = rpcError_jsonBodyError;			        \
		 break;										\
	}                                               \
    case rpcError_logined:{				            \
		X = rpcError_logined;			            \
		 break;										\
	}                                               \
    case rpcError_key_failed:{				        \
		X = rpcError_key_failed;			        \
		 break;										\
	}                                               \
    case rpcError_key_notSupport:{				    \
		X = rpcError_key_notSupport;			    \
		 break;										\
	}                                               \
    case rpcError_key_keyNotFound:{				    \
		X = rpcError_key_keyNotFound;			    \
		 break;										\
	}                                               \
    case rpcError_key_certNotFound:{				\
		X = rpcError_key_certNotFound;			    \
		 break;										\
	}                                               \
    case rpcError_key_pinIncorrect:{				\
		X = rpcError_key_pinIncorrect;			    \
		 break;										\
	}                                               \
    case rpcError_key_pinLocked:{				    \
		X = rpcError_key_pinLocked;			        \
		 break;										\
	}                                               \
    case rpcError_key_pinTypeInvalid:{				\
		X = rpcError_key_pinTypeInvalid;			\
		 break;										\
	}                                               \
    case rpcError_key_appExisted:{				    \
		X = rpcError_key_appExisted;			    \
		 break;										\
	}                                               \
    case rpcError_key_userNotLogined:{				\
		X = rpcError_key_userNotLogined;			\
		 break;										\
	}                                               \
    case rpcError_key_appNotExisted:{				\
		X = rpcError_key_appNotExisted;			    \
		 break;										\
	}                                               \
    case rpcError_key_maxContainer:{				\
		X = rpcError_key_maxContainer;			    \
		 break;										\
	}                                               \
    case rpcError_key_conNotExisted:{				\
		X = rpcError_key_conNotExisted;			    \
		 break;										\
	}                                               \
    case rpcError_key_conExisted:{				    \
		X = rpcError_key_conExisted;			    \
		 break;										\
	}                                               \
	default:{										\
		X = rpcError_key_failed;					\
        break;                                      \
	}												\
}

class errMgr{

public:   
	static errMgr &getInstance() {
		static errMgr err;
		return err;
	} 

    const char *getErrorString(unsigned long error) {
        return getInstance().mErrInfo[error];
    }

    bool isErrorExisted(unsigned long error) {
        std::map<unsigned long, const char*>::iterator it = mErrInfo.find(error);
        if(it != mErrInfo.end()) {
            return true;
        } else {
            return false;
        }
    }

private:
    errMgr() {
        mErrInfo[rpcError_ok]                   = "successful";
        mErrInfo[rpcError_noSession]            = "session is not exist, try login again";
        mErrInfo[rpcError_loginInvalid]         = "the status of login is invalid, try login again";
        mErrInfo[rpcError_notifyExsited]        = "notify is existed";
        mErrInfo[rpcError_msgTypeError]         = "msgType is error";
        mErrInfo[rpcError_jsonBodyError]        = "json body param invalid";
        mErrInfo[rpcError_logined]              = "logined";

        mErrInfo[rpcError_key_failed]           = "failed";
        mErrInfo[rpcError_key_notSupport]       = "key driver not support";
        mErrInfo[rpcError_key_keyNotFound]      = "key is not found";
        mErrInfo[rpcError_key_certNotFound]     = "certification is not found";
        mErrInfo[rpcError_key_pinIncorrect]     = "pin is incorrect";
        mErrInfo[rpcError_key_pinLocked]        = "pin is locked";
        mErrInfo[rpcError_key_pinTypeInvalid]   = "pinType is invalid";
        mErrInfo[rpcError_key_appExisted]       = "appname is existed";
        mErrInfo[rpcError_key_userNotLogined]   = "user is not login";
        mErrInfo[rpcError_key_appNotExisted]    = "appname is not existed";
        mErrInfo[rpcError_key_maxContainer]     = "Out of maximum of container";
        mErrInfo[rpcError_key_conNotExisted]    = "container is not existed";
        mErrInfo[rpcError_key_conExisted]       = "container is existed";
    }

    std::map<unsigned long, const char*> mErrInfo;
};




} // namespace koal

#endif