#ifndef _NODEPOOL_H
#define _NODEPOOL_H

#include "runepool.h"
#ifdef __GNUC__  
#include <ext/hash_map>  
#else  
#include <hash_map>  
#endif  

#include <vector>
#include <string>

using namespace std;

namespace std  
{  
	using namespace __gnu_cxx;  
}  


//最大一个词的长度
#define MAX_WORD_LENGTH 50

//永久节点模型
struct _RuneLinkNode
{
	typedef hash_map<int, _RuneLinkNode*> hmapRuneNextMap;	
	
	hmapRuneNextMap            m_hmapRuneNextMap;
	_Rune*                     m_pRune;
	char                       m_pWord[MAX_WORD_LENGTH];
	int                        m_nPoolIndex;
	
	_RuneLinkNode()
	{
		m_nPoolIndex = 0;
		m_pRune      = NULL;
		memset(m_pWord, 0, MAX_WORD_LENGTH);
	}
	
	void Set_Index(int nIndex)
	{
		m_nPoolIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nPoolIndex;
	}	
};

class CNodePool
{
public:
	CNodePool();
	~CNodePool();
	
	void Init(int nPoolCount);
	void Close();
	
	_RuneLinkNode* Create();
	bool Delete(_RuneLinkNode* pNode);
	
private:
	struct _Node_Box
	{
		char          m_cUsed;            //0为未使用，1为使用
		_RuneLinkNode m_obj_RuneLinkNode; 
		
		_Node_Box()
		{
			m_cUsed = 0;
		}  
	};
	
private:
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Node_Box*     m_NodePoolList;		
};

#endif
