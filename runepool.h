#ifndef _RUNEPOOL_H
#define _RUNEPOOL_H

#include <stdio.h>
#include <string.h>

#define MAX_SINGLE_RUNE 4

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
	bool operator == (_Rune& rhs) const
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
	
	_Rune& operator = (const _Rune& ar)
	{
		this->m_emType   = ar.m_emType;
		this->m_nRuneLen = ar.m_nRuneLen;
		this->m_nIndex   = ar.m_nIndex;    //记录在Pool中的下标
		this->m_nFlag    = ar.m_nFlag;     //转化为数字的标记
		if(ar.m_nRuneLen > 0)
		{ 
			memcpy(this->m_szRune, ar.m_szRune, ar.m_nRuneLen);	
		}
		return *this;
	}	
};

static ENUM_WORD_TYPE Get_Rune_From_String(const char* pWord, int nBegin, int nLen, _Rune* pRune)
{
	unsigned char cBegin = (unsigned char)pWord[nBegin];
	//printf("******0x%02x******\n", cBegin);
	if(cBegin < 0x80)
	{
		//当前是ascii字符集
		pRune->m_szRune[0] = (unsigned char)pWord[nBegin];
		pRune->m_nRuneLen  = 1;
		pRune->m_emType    = WORD_TYPE_ASCII;
		pRune->Set_Number();
		return pRune->m_emType;
	}
	else
	{
		//比较字符高4位
		char szTemp = cBegin >> 4;
		if(szTemp == 0xe && nLen - nBegin >= 3)
		{
			//为三字节utf8格式
			pRune->m_szRune[0] = (unsigned char)pWord[nBegin];
			pRune->m_szRune[1] = (unsigned char)pWord[nBegin + 1];
			pRune->m_szRune[2] = (unsigned char)pWord[nBegin + 2];
			pRune->m_nRuneLen = 3;
			pRune->m_emType   = WORD_TYPE_UTF8;
			pRune->Set_Number();
			return pRune->m_emType;
		}
		//else if(((char)(szTemp & 0xc) == 0xc) && (nLen - nBegin >= 2))
		else if(nLen - nBegin >= 2)
		{
			//为gbk编码
			pRune->m_szRune[0] = (unsigned char)pWord[nBegin];
			pRune->m_szRune[1] = (unsigned char)pWord[nBegin + 1];
			pRune->m_nRuneLen = 2;
			pRune->m_emType   = WORD_TYPE_GBK;
			pRune->Set_Number();
			return pRune->m_emType;
		}
		else
		{
			return pRune->m_emType;
		}
	}	
}

#endif
