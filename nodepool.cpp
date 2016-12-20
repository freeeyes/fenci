#include "nodepool.h"

CNodePool::CNodePool()
{
	m_NodePoolList = NULL;
}

CNodePool::~CNodePool()
{
	Close();
}

void CNodePool::Close()
{
	if(NULL != m_NodePoolList)
	{
		delete[] m_NodePoolList; 
		m_NodePoolList = NULL;
	}	
}

void CNodePool::Init(int nPoolCount)
{
	Close();
	
	m_NodePoolList = new _Node_Box[nPoolCount];
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_NodePoolList[i].m_obj_RuneLinkNode.Set_Index(i);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
}

_RuneLinkNode* CNodePool::Create()
{
	if(NULL == m_NodePoolList)
	{
		return NULL;
	}
	
	if(m_NodePoolList[m_nCurrIndex].m_cUsed == 0)
	{
		m_NodePoolList[m_nCurrIndex].m_cUsed = 1;
		m_nCurrIndex++;
		if(m_nCurrIndex == m_nPoolCount)
		{
			m_nCurrIndex = 0;
		}
		return &m_NodePoolList[m_nCurrIndex].m_obj_RuneLinkNode;
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_NodePoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i++;
				if(m_nCurrIndex == m_nPoolCount)
				{
					m_nCurrIndex = 0;
				}
				m_NodePoolList[i].m_cUsed = 1;
				return &m_NodePoolList[i].m_obj_RuneLinkNode;				
			}
		}
		
		//没找到，再重头开始找
		for(int i = 0; i < m_nCurrIndex - 1; i++)
		{
			if(m_NodePoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i++;
				if(m_nCurrIndex == m_nPoolCount)
				{
					m_nCurrIndex = 0;
				}
				m_NodePoolList[i].m_cUsed = 1;
				return &m_NodePoolList[i].m_obj_RuneLinkNode;				
			}			
		}
		
		//已经没有空位
		return NULL;
	}	
}

bool CNodePool::Delete(_RuneLinkNode* pNode)
{
	if(NULL == m_NodePoolList || NULL == pNode)
	{
		return false;
	}	
	
	if(-1 == pNode->Get_Index())
	{
		return false;
	}
	
	m_NodePoolList[pNode->Get_Index()].m_cUsed = 0;
	return true;	
}
