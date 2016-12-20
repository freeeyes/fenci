#include "runepool.h"

CRunePool::CRunePool()
{
	m_nPoolCount   = 0;
	m_nCurrIndex   = 0;
	m_RunePoolList = NULL;
}

CRunePool::~CRunePool()
{
	Close();
}

void CRunePool::Close()
{
	if(NULL != m_RunePoolList)
	{
		delete[] m_RunePoolList; 
		m_RunePoolList = NULL;
	}
}	

void CRunePool::Init(int nPoolCount)
{
	Close();
	
	m_RunePoolList = new _Rune_Box[nPoolCount];
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_RunePoolList[i].m_objRune.Set_Index(i);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;
}

_Rune* CRunePool::Create()
{
	if(NULL == m_RunePoolList)
	{
		return NULL;
	}
	
	if(m_RunePoolList[m_nCurrIndex].m_cUsed == 0)
	{
		m_RunePoolList[m_nCurrIndex].m_cUsed = 1;
		m_nCurrIndex++;
		if(m_nCurrIndex == m_nPoolCount)
		{
			m_nCurrIndex = 0;
		}
		return &m_RunePoolList[m_nCurrIndex].m_objRune;
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_RunePoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i++;
				if(m_nCurrIndex == m_nPoolCount)
				{
					m_nCurrIndex = 0;
				}
				m_RunePoolList[i].m_cUsed = 1;
				return &m_RunePoolList[i].m_objRune;				
			}
		}
		
		//没找到，再重头开始找
		for(int i = 0; i < m_nCurrIndex - 1; i++)
		{
			if(m_RunePoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i++;
				if(m_nCurrIndex == m_nPoolCount)
				{
					m_nCurrIndex = 0;
				}
				m_RunePoolList[i].m_cUsed = 1;
				return &m_RunePoolList[i].m_objRune;				
			}			
		}
		
		//已经没有空位
		return NULL;
	}
}

bool CRunePool::Delete(_Rune* pRune)
{
	if(NULL == m_RunePoolList || NULL == pRune)
	{
		return false;
	}	
	
	if(-1 == pRune->Get_Index())
	{
		return false;
	}
	
	m_RunePoolList[pRune->Get_Index()].m_cUsed = 0;
	m_RunePoolList[pRune->Get_Index()].m_objRune.Clear();
	return true;
}
