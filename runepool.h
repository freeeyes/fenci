#ifndef _RUNEPOOL_H
#define _RUNEPOOL_H

#include <stdio.h>
#include <string.h>

#define MAX_SINGLE_RUNE 5

enum ENUM_WORD_TYPE
{
	WORD_TYPE_UNKNOW = 0,
	WORD_TYPE_ASCII,
	WORD_TYPE_GBK,
	WORD_TYPE_UTF8
};

struct _Rune
{
	unsigned char  m_szRune[MAX_SINGLE_RUNE];
	ENUM_WORD_TYPE m_emType;
	int            m_nRuneLen;
	int            m_nIndex;    //记录在Pool中的下标
	int            m_nFlag;     //转化为数字的标记 
	
	_Rune()
	{
		Clear();
	}
	
	void Clear()
	{
		memset(m_szRune, 0, MAX_SINGLE_RUNE);
		m_nRuneLen  = 0;
		m_nIndex    = -1;
		m_nFlag     = 0;
		m_emType    = WORD_TYPE_UNKNOW;		
	}
	
	int Get_Number()
	{

		return m_nFlag;
	}
	
	void Set_Number()
	{
		memcpy((char* )&m_nFlag, m_szRune, sizeof(int));
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}
	
	void DisPlay()
	{
		if(m_emType == WORD_TYPE_ASCII)
		{
			printf("[_Rune::DisPlay]<WORD_TYPE_ASCII>");
		}
		else if(m_emType == WORD_TYPE_GBK)
		{
			printf("[_Rune::DisPlay]<WORD_TYPE_GBK>");
		}
		else if(m_emType == WORD_TYPE_UTF8)
		{
			printf("[_Rune::DisPlay]<WORD_TYPE_UTF8>");
		}
		else
		{
			printf("[_Rune::DisPlay]<WORD_TYPE_UNKNOW>");
		}
		
		for(int i = 0; i < m_nRuneLen; i++)
		{
			printf("0x%02x ", (unsigned char)m_szRune[i]);
		}
		//printf("\n");
	}
	
	//重载比较运算符
	bool operator==(_Rune& rhs) const
	{
		if(rhs.m_nRuneLen != this->m_nRuneLen)
		{
			return false;
		}
		
		for(int i= 0; i < rhs.m_nRuneLen; i++)
		{
			if(this->m_szRune[i] != rhs.m_szRune[i])
			{
				return false;
			}
		}
		return true;
	}
};

class CRunePool
{
public:
	CRunePool();
	~CRunePool();
	
	void Init(int nPoolCount);
	void Close();
	
	_Rune* Create();
	bool   Delete(_Rune* pRune);
	
	size_t Create_Offset();
	bool Delete_Offset(size_t stRune);
	_Rune* Get_Offset(size_t stRune);
	
private:
	struct _Rune_Box
	{
		char  m_cUsed;   //0为未使用，1为使用
		_Rune m_objRune; 
		
		_Rune_Box()
		{
			m_cUsed = 0;
		}  
	};
	
private:
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Rune_Box*     m_RunePoolList;
};

#endif
