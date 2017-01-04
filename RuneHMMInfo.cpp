#include "RuneHMMInfo.h"

CRuneHMMPool::CRuneHMMPool()
{
	m_pCryptTable     = NULL;
	m_pBase           = NULL;	
	m_RuneHMMPoolList = NULL;
}

CRuneHMMPool::~CRuneHMMPool()
{
	Close();
}

//hash算法对应的加密字符串词单
void CRuneHMMPool::prepareCryptTable()
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

void CRuneHMMPool::Close()
{
	m_pCryptTable  = NULL;
	m_pBase        = NULL;	
}

char* CRuneHMMPool::GetCryptTable()
{
	return m_pCryptTable;
}

size_t CRuneHMMPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	m_pCryptTable = pData;
	memset(m_pCryptTable, 0, 1280);
	prepareCryptTable();
	nPos += 1280;
	
	//printf("[CRuneHMMPool::Init]nPos=%d.\n", nPos);
	m_RuneHMMPoolList = (_RuneHMMInfo* )&pData[nPos];
	nPos += sizeof(_RuneHMMInfo) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_RuneHMMPoolList[i].Init();
		m_RuneHMMPoolList[i].Set_Index(i);
		//printf("[CRuneHMMPool::Init](0)nPos=%d.\n", nPos);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;	
}

size_t CRuneHMMPool::Load(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase       = pData;
	size_t nPos   = 0;
	m_pCryptTable = pData;
	nPos += 1280;
	
	printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_RuneHMMPoolList = (_RuneHMMInfo* )&pData[nPos];
	nPos += sizeof(_RuneHMMInfo) * nPoolCount;
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;		
}

_RuneHMMInfo* CRuneHMMPool::Create()
{
	if(NULL == m_RuneHMMPoolList)
	{
		return NULL;
	}
	
	if(m_nCurrIndex >= m_nPoolCount - 1)
	{
		m_nCurrIndex = 0;
	}	
	
	if(m_RuneHMMPoolList[m_nCurrIndex].m_cUsed == 0)
	{
		//printf("[CRuneHMMPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_RuneHMMPoolList[m_nCurrIndex].Get_Index());
		m_RuneHMMPoolList[m_nCurrIndex].m_cUsed = 1;
		return &m_RuneHMMPoolList[m_nCurrIndex++];
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_RuneHMMPoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				if(m_nCurrIndex == m_nPoolCount)
				{
					m_nCurrIndex = 0;
				}
				m_RuneHMMPoolList[i].m_cUsed = 1;
				return &m_RuneHMMPoolList[i];				
			}
		}
		
		printf("[CRuneHMMPool::Create]m_nCurrIndex=%d,m_nPoolCount=%d.\n", m_nCurrIndex, m_nPoolCount);
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < m_nCurrIndex - 1; i++)
		{
			if(m_RuneHMMPoolList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				m_RuneHMMPoolList[i].m_cUsed = 1;
				return &m_RuneHMMPoolList[i];				
			}			
		}
		
		//已经没有空位
		return NULL;
	}		
}

int CRuneHMMPool::Get_Node_Offset(_RuneHMMInfo* pRuneHMMInfo)
{
	int nOffset = 0;
	if(NULL != pRuneHMMInfo)
	{
		nOffset = (int)((char* )pRuneHMMInfo - m_pBase);
	}
	
	return nOffset;
}

_RuneHMMInfo* CRuneHMMPool::Get_NodeOffset_Ptr(int nOffset)
{
	return (_RuneHMMInfo* )(m_pBase + nOffset);
}

bool CRuneHMMPool::Delete(_RuneHMMInfo* pRuneHMMInfo)
{
	if(NULL == m_RuneHMMPoolList || NULL == pRuneHMMInfo)
	{
		return false;
	}	
	
	if(-1 == pRuneHMMInfo->Get_Index())
	{
		return false;
	}
	
	m_RuneHMMPoolList[pRuneHMMInfo->Get_Index()].m_cUsed = 0;
	return true;		
}
