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

//ÿ���ֵ�HMM��Ӧ��BEMS����
struct _RuneHMMInfo
{
	char   m_cUsed;    //0Ϊδʹ�ã�1Ϊʹ��
	int    m_nIndex;   //�ڴ�ص�λ��
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

//��Ӧ_RuneHMMInfo���ڴ��
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
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_RuneHMMInfo*  m_RuneHMMPoolList; //�ڴ���б�		
};

#endif
