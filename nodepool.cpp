#include "nodepool.h"

CNodePool::CNodePool()
{
	m_pCryptTable   = NULL;
	m_pBase         = NULL;
	m_NodePoolList  = NULL;
}

CNodePool::~CNodePool()
{
	Close();
}

void CNodePool::Close()
{
	m_pCryptTable  = NULL;
	m_pBase        = NULL;	
}

//hash算法对应的加密字符串词单
void CNodePool::prepareCryptTable()
{
  unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
  for(index1 = 0; index1 < 0x100; index1++)
  { 
    for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
    { 
      unsigned long temp1, temp2;
      seed = (seed * 125 + 3) % 0x2AAAAB;
      temp1 = (seed & 0xFFFF) << 0x10;
      seed = (seed * 125 + 3) % 0x2AAAAB;
      temp2 = (seed & 0xFFFF);
      if(index2 > 1280)
      {
      	printf("[prepareCryptTable]index2=%u.\n", (unsigned int)index2);
      }
      m_pCryptTable[index2] = (char)(temp1 | temp2); 
    } 
  } 		
}

size_t CNodePool::Init(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	m_pCryptTable = pData;
	memset(m_pCryptTable, 0, 1280);
	prepareCryptTable();
	nPos += 1280;
	
	printf("[CNodePool::Init]nPos=%d.\n", nPos);
	m_NodePoolList = (_RuneLinkNode* )&pData[nPos];
	nPos += sizeof(_RuneLinkNode) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		if(i == 0)
		{
			size_t nMapSize = sizeof(_Hash_Table_Cell) * MAIN_DICT_MAP_COUNT;
			m_NodePoolList[i].Init(&pData[nPos], MAIN_DICT_MAP_COUNT, m_pCryptTable);
			m_NodePoolList[i].Set_Index(i);
			nPos += nMapSize;
			//printf("[CNodePool::Init](0)nPos=%d.\n", nPos);
		}
		else if(i <= MAIN_DICT_MAP_COUNT)
		{
			size_t nMapSize = sizeof(_Hash_Table_Cell) * CHILD_DICT_MAP_COUNT;
			m_NodePoolList[i].Init(&pData[nPos], CHILD_DICT_MAP_COUNT, m_pCryptTable);
			m_NodePoolList[i].Set_Index(i);
			nPos += nMapSize;
		}
		else
		{
			size_t nMapSize = sizeof(_Hash_Table_Cell) * END_DICT_MAP_COUNT;
			m_NodePoolList[i - MAIN_DICT_MAP_COUNT + 1].Init(&pData[nPos], END_DICT_MAP_COUNT, m_pCryptTable);
			m_NodePoolList[i - MAIN_DICT_MAP_COUNT + 1].Set_Index(i);
			nPos += nMapSize;			
		}
		
		//printf("[CNodePool::Init](%d)nPos=%d.\n", i, nPos);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 1;	
	
	return nPos;
}

size_t CNodePool::Load(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	m_pCryptTable = pData;
	nPos += 1280;
	
	printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_NodePoolList = (_RuneLinkNode* )&pData[nPos];
	nPos += sizeof(_RuneLinkNode) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		if(i == 0)
		{
			size_t nMapSize = sizeof(_Hash_Table_Cell) * MAIN_DICT_MAP_COUNT;
			m_NodePoolList[i].Load(&pData[nPos], MAIN_DICT_MAP_COUNT, m_pCryptTable);
			//m_NodePoolList[i].Set_Index(i);
			nPos += nMapSize;
			//printf("[CNodePool::Init](0)nPos=%d.\n", nPos);
		}
		else if(i <= MAIN_DICT_MAP_COUNT)
		{
			size_t nMapSize = sizeof(_Hash_Table_Cell) * CHILD_DICT_MAP_COUNT;
			m_NodePoolList[i].Load(&pData[nPos], CHILD_DICT_MAP_COUNT, m_pCryptTable);
			//m_NodePoolList[i].Set_Index(i);
			nPos += nMapSize;
		}
		else
		{
			size_t nMapSize = sizeof(_Hash_Table_Cell) * END_DICT_MAP_COUNT;
			m_NodePoolList[i - MAIN_DICT_MAP_COUNT + 1].Load(&pData[nPos], END_DICT_MAP_COUNT, m_pCryptTable);
			//m_NodePoolList[i - MAIN_DICT_MAP_COUNT + 1].Set_Index(i);
			nPos += nMapSize;			
		}
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 1;	
	
	return nPos;	
}

_RuneLinkNode* CNodePool::CreateRoot()
{
	if(NULL == m_NodePoolList)
	{
		return NULL;
	}
	
	m_NodePoolList[0].m_cUsed = 1;
	return &m_NodePoolList[0];
}

_RuneLinkNode* CNodePool::Create(int nLayer)
{
	if(NULL == m_NodePoolList)
	{
		return NULL;
	}
	
	int nStart = 0;
	if(nLayer > 2)
	{
		//大于2层的，以10个节点为准
		m_nCurrIndex += MAIN_DICT_MAP_COUNT + 1;
	}
	
	if(m_NodePoolList[m_nCurrIndex].m_cUsed == 0)
	{
		m_NodePoolList[m_nCurrIndex].m_cUsed = 1;
		m_nCurrIndex++;
		if(m_nCurrIndex == m_nPoolCount)
		{
			m_nCurrIndex = 1;
		}
		m_NodePoolList[m_nCurrIndex].m_cUsed = 1;
		return &m_NodePoolList[m_nCurrIndex++];
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
					m_nCurrIndex = 1;
				}
				m_NodePoolList[i].m_cUsed = 1;
				return &m_NodePoolList[i];				
			}
		}
		
		int nStart = 1;
		if(nLayer > 2)
		{
			nStart += MAIN_DICT_MAP_COUNT;
		}
		//没找到，再重头开始找
		for(int i = nStart; i < m_nCurrIndex - 1; i++)
		{
			if(m_NodePoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i++;
				if(m_nCurrIndex == m_nPoolCount)
				{
					m_nCurrIndex = 1;
				}
				m_NodePoolList[i].m_cUsed = 1;
				return &m_NodePoolList[i];				
			}			
		}
		
		//已经没有空位
		return NULL;
	}	
}

int CNodePool::Get_Node_Offset(_RuneLinkNode* pRuneLinkNode)
{
	int nOffset = 0;
	if(NULL != pRuneLinkNode)
	{
		nOffset = (int)((char* )pRuneLinkNode - m_pBase);
	}
	
	return nOffset;
}

_RuneLinkNode* CNodePool::Get_NodeOffset_Ptr(int nOffset)
{
	return (_RuneLinkNode* )(m_pBase + nOffset);
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
