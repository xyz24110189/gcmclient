#ifndef __CBYTE_ARRAY_H__
#define __CBYTE_ARRAY_H__

#ifdef  _WIN32
#include <string>
#include <Winsock2.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#endif

namespace koal {

// 这个类目前实现基本的操作功能，后续有些接口需要再完善
// 后面可以考虑优化，有个初始长度的空间，这样就不用每操作一次append就申请一次空间
// 变长数据定义
class CByteArray
{
public:
	CByteArray() {
		m_pData = NULL;
		m_nSize = 0;
	}
	CByteArray(const CByteArray& src)  { 
		m_pData = NULL;
		m_nSize = 0;
		copy(src);
	}

	// 此处需要注意下
	// windows各模块都有自己的空间，无法保证申请释放的内存在同一个模块空间，需采用HeapAlloc/HeapFree接口指定
	// linux没有各个模块空间，申请的内存都在同一空间。	
	~CByteArray() {
		if(m_pData) {
#ifdef _WIN32
			HeapFree(GetProcessHeap(),0,m_pData);
			m_pData = NULL;
#else
            delete m_pData;
			m_pData = NULL;    
#endif	
		}
	}    

	unsigned long getSize() const { 
        return m_nSize; 
    }
	bool isEmpty() const { 
        return m_nSize == 0; 
    }

	// 有时外面会传入-1等参数
	void setSize(long nNewSize) {

		if(nNewSize < 0 ) {
            throw "InvalidArgExcption";
        }			

		if (nNewSize == 0) {
			if (NULL != m_pData) {
#ifdef _WIN32
				HeapFree(GetProcessHeap(),0,m_pData);
				m_pData = NULL;
#else
				delete m_pData;
                m_pData = NULL;
#endif					
			}
			m_nSize     = 0;
		} else {
            if (NULL == m_pData) {
                m_nSize = nNewSize;
#ifdef _WIN32
				m_pData = (unsigned char*) HeapAlloc(GetProcessHeap(),0, (m_nSize+1) * sizeof(unsigned char));
#else
				m_pData = (unsigned char*) malloc((m_nSize+1) * sizeof(unsigned char));
#endif							
                memset(m_pData, 0, (m_nSize+1) * sizeof(unsigned char));
            } else {
				// 新长度小于原有数据长度，原数据直接丢掉，重新开辟新长度空间
				if (nNewSize < m_nSize) {
#ifdef _WIN32
					HeapFree(GetProcessHeap(),0,m_pData);
					m_pData = NULL;
					m_nSize = nNewSize;
					m_pData = (unsigned char*) HeapAlloc(GetProcessHeap(),0, (m_nSize+1) * sizeof(unsigned char));
#else
					delete m_pData;
					m_pData = NULL;
					m_nSize = nNewSize;
					m_pData = (unsigned char*) malloc((m_nSize+1) * sizeof(unsigned char));
#endif							
					memset(m_pData, 0, (m_nSize+1) * sizeof(unsigned char));
				}
                unsigned long ulOldSize = m_nSize;
                m_nSize = nNewSize;

#ifdef _WIN32
				unsigned char * strTemp = (unsigned char*) HeapAlloc(GetProcessHeap(),0, (m_nSize+1) * sizeof(unsigned char));
#else
				unsigned char * strTemp = (unsigned char*) malloc((m_nSize+1) * sizeof(unsigned char));
#endif			
                memset(strTemp, 0, (m_nSize+1) * sizeof(unsigned char));
                memcpy(strTemp, m_pData, ulOldSize * sizeof(unsigned char));

#ifdef _WIN32
				HeapFree(GetProcessHeap(),0,m_pData);
				m_pData = NULL;
#else
				delete m_pData;
                m_pData = NULL;
#endif	
                m_pData = strTemp;
            }
		}
	}

	void RemoveAll() {
		setSize(0);
	}

	unsigned char getAt(unsigned long nIndex) const {
		if(nIndex >= m_nSize ) {
            throw "InvalidArgExcption";
        }
			
		return m_pData[nIndex]; 
	}

	void setAt(unsigned long nIndex, unsigned char newElement) {
		if(nIndex >= m_nSize ) {
            throw "InvalidArgExcption";
        }
			
		m_pData[nIndex] = newElement;
	}

	unsigned char& elementAt(unsigned long nIndex)
	{
		if(nIndex >= m_nSize ) {
            throw "InvalidArgExcption";
        }
			
		return m_pData[nIndex];
	}

	const unsigned char* getData() const {
		return (const unsigned char*)m_pData;
	}

	unsigned char* getData() {
		return (unsigned char*)m_pData;
	}

	std::string getDataString() const{
		std::string strRe = "";
		if (NULL != m_pData) {
			strRe = (char*)m_pData;
		}
		return strRe;
	}

	unsigned long append(void * pBuffer,unsigned long iSize) {
		unsigned long nOldSize = m_nSize;
		if ( iSize > 0 ) {
			setSize(m_nSize + iSize);
			memcpy(m_pData + nOldSize, pBuffer, iSize);
		}
		return nOldSize;
	}

	unsigned long append(std::string str) {
		unsigned long nOldSize = m_nSize;
		setSize(m_nSize + str.length()*sizeof(char));
		memcpy(m_pData + nOldSize, str.c_str(), str.length()*sizeof(char));
		return nOldSize;
	}

	unsigned long append(const char* str) {
		unsigned long nOldSize = m_nSize;
		setSize(m_nSize + strlen(str)*sizeof(char));
		memcpy(m_pData + nOldSize, str, strlen(str)*sizeof(char));
		return nOldSize;
	}

	unsigned long append(const CByteArray& src) {
		if(this == &src) {
           throw "InvalidArgExcption";
        }

		unsigned long nOldSize = m_nSize;
	    setSize(m_nSize + src.m_nSize);
		memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(unsigned char));

		return nOldSize;
	}

	CByteArray& assign (const char* s) {
		setSize(strlen(s));
		memcpy(m_pData, s, strlen(s) * sizeof(unsigned char));	
		return *this;		
	}

	CByteArray& assign (const char* s, unsigned long n) {
		setSize(n + 1);
		memcpy(m_pData, s, n * sizeof(unsigned char));	
		return *this;		
	}	

	void copy(const CByteArray& src) {
		if(this != &src) {
			setSize(src.m_nSize);
			memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(unsigned char));
		}
	}

    //找到cTarget，替换为cNewContent
	void replaceAstringAll(char* cTarget,const char* cNewContent) {
	}

    //然后替换cStart/cEnd之间的数据为cNewContent
	void replaceAstring(char* cStart, char* cEnd, const char* cNewContent) {
		//InsertAt();
	}

	unsigned char operator[](unsigned char nIndex) const {
		return getAt(nIndex);
	}
	unsigned char& operator[](unsigned char nIndex) {
		return elementAt(nIndex);
	}

	CByteArray& operator=(const CByteArray& baData) {
		copy(baData);
		return *this;
	}

	CByteArray& operator=(const char *pStr) {
		setSize(strlen(pStr));
		memcpy(m_pData, pStr, strlen(pStr) * sizeof(unsigned char));	
		return *this;
	}

    //将内容打印成Hex形式输出
	std::string toString() {
		std::string strOutput = "";
		std::string strSingle = "";
		unsigned char ch = 0;
		char cSingle[3];
		for(size_t index = 0; index<(size_t)(getSize()); index++) {
			ch = elementAt(index);
			memset(cSingle,0,sizeof(cSingle));
			sprintf(cSingle, "%02X",ch);
			strOutput += cSingle;
		}
		return strOutput;
	}

	void removeAt(unsigned long nIndex, unsigned long nCount = 1) {
	}
	void insertAt(unsigned long nIndex, unsigned char newElement, unsigned long nCount = 1) {
	}
	void insertAt(unsigned long nStartIndex, CByteArray* pNewArray) {
	}

protected:
	unsigned char* m_pData;    // the actual array of data
	unsigned long m_nSize;     // size of elements

};

} //namespace koal

#endif //__CBYTE_ARRAY_H__