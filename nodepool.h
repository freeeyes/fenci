#ifndef _NODEPOOL_H
#define _NODEPOOL_H

#include "runepool.h"
#include "HashTable.h"

#include <vector>
#include <string>

using namespace std;

//最大一个词的长度
#define MAX_WORD_LENGTH 50

//节点类型不同的定义
#define MAIN_DICT_MAP_COUNT  1000
#define CHILD_DICT_MAP_COUNT 50

//永久节点模型
struct _RuneLinkNode
{
	CHashTable                 m_hmapRuneNextMap;
	_Rune                      m_objRune;
	char                       m_pWord[MAX_WORD_LENGTH];
	int                        m_nPoolIndex;
	char                       m_cUsed;            //0为未使用，1为使用
	
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
	_RuneLinkNode* Create();
	int Get_Node_Offset(_RuneLinkNode* pRuneLinkNode);
	_RuneLinkNode* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_RuneLinkNode* pNode);
	
private:
	void prepareCryptTable();	
	
private:
	char*          m_pCryptTable;
	char*          m_pBase;          //整个内存开始地址
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_RuneLinkNode* m_NodePoolList;		
};

#endif
