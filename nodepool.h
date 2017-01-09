#ifndef _NODEPOOL_H
#define _NODEPOOL_H

#include "runepool.h"
#include "HashTable.h"

#include <vector>
#include <string>

using namespace std;

//���һ���ʵĳ���
#define MAX_WORD_LENGTH 30

//�ڵ����Ͳ�ͬ�Ķ���
#define MAIN_DICT_MAP_COUNT  10000
#define CHILD_DICT_MAP_COUNT 50
#define END_DICT_MAP_COUNT   5

//���ýڵ�ģ��
struct _RuneLinkNode
{
	CHashTable                 m_hmapRuneNextMap;
	_Rune                      m_objRune;
	char                       m_pWord[MAX_WORD_LENGTH];
	int                        m_nPoolIndex;
	char                       m_cUsed;            //0Ϊδʹ�ã�1Ϊʹ��
	
	_RuneLinkNode()
	{
		m_cUsed      = 0;
		m_nPoolIndex = 0;
		memset(m_pWord, 0, MAX_WORD_LENGTH);
	}
	
	void Init(char* pData, int nMapSize, char* pcryptTable)
	{
		m_cUsed      = 0;
		m_nPoolIndex = 0;
		memset(m_pWord, 0, MAX_WORD_LENGTH);		
		
		m_hmapRuneNextMap.Init(pData, nMapSize, pcryptTable);
	}
	
	void Load(char* pData, int nMapSize, char* pcryptTable)
	{		
		m_hmapRuneNextMap.Load(pData, nMapSize, pcryptTable);		
	}
	
	void Set_Index(int nIndex)
	{
		m_nPoolIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nPoolIndex;
	}	
	
	void Set_Used(char cUsed)
	{
		m_cUsed = cUsed;
	}
	
	char Get_Used()
	{
		return m_cUsed;
	}
};

class CNodePool
{
public:
	CNodePool();
	~CNodePool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	_RuneLinkNode* CreateRoot();
	_RuneLinkNode* Create(int nLayer);
	int Get_Node_Offset(_RuneLinkNode* pRuneLinkNode);
	_RuneLinkNode* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_RuneLinkNode* pNode);
	
private:
	void prepareCryptTable();	
	
private:
	char*          m_pCryptTable;
	char*          m_pBase;          //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_RuneLinkNode* m_NodePoolList; //���ڵ���м�ڵ��б�		
};

#endif
