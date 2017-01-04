#ifndef _RUNE_HMM_INFO
#define _RUNE_HMM_INFO

#include <stdio.h>
#include <string.h>

enum RUNE_POS
{
	RUNE_POS_B = 0,
	RUNE_POS_E,
	RUNE_POS_M,
	RUNE_POS_S,
	RUNE_POS_ALL,
};

//每个字的HMM对应的BEMS概率
struct _RuneHMMInfo
{
	char   m_cUsed;    //0为未使用，1为使用
	int    m_nIndex;   //内存池的位置
	char   m_szRune[4];
	double m_dbProp;
	
	_RuneHMMInfo()
	{
		memset(m_szRune, 0, 4);
		m_dbProp  = 0;
		m_nIndex  = 0;
		m_cUsed   = 0;
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}
};

//对应_RuneHMMInfo的内存池
class CRuneHMMPool
{
public:
	CRuneHMMPool();
	~CRuneHMMPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	char* GetCryptTable();
	
	_RuneHMMInfo* Create();
	int Get_Node_Offset(_RuneHMMInfo* pRuneHMMInfo);
	_RuneHMMInfo* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_RuneHMMInfo* pRuneHMMInfo);
	
private:
	void prepareCryptTable();	
	
private:
	char*          m_pCryptTable;
	char*          m_pBase;           //整个内存开始地址
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_RuneHMMInfo*  m_RuneHMMPoolList; //内存池列表		
};

#endif
