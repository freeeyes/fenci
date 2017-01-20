#include "WordDict.h"

CWordInfoPool::CWordInfoPool()
{
	m_pCryptTable  = NULL;
	m_pBase        = NULL;
	m_WordInfoList = NULL;
	m_nPoolCount   = 0;
	m_nCurrIndex   = 0;
}

CWordInfoPool::~CWordInfoPool()
{
	Close();
}

//hash算法对应的加密字符串词单
void CWordInfoPool::prepareCryptTable()
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

void CWordInfoPool::Close()
{
	m_pCryptTable  = NULL;
	m_pBase        = NULL;	
	m_WordInfoList = NULL;
}

char* CWordInfoPool::GetCryptTable()
{
	return m_pCryptTable;
}

size_t CWordInfoPool::Init(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase     = pData;
	size_t nPos = 0;
	m_pCryptTable = pData;
	memset(m_pCryptTable, 0, 1280);
	prepareCryptTable();
	nPos += 1280;
	
	//printf("[CWordInfoPool::Init]nPos=%d.\n", nPos);
	m_WordInfoList = (_Word_Info* )&pData[nPos];
	nPos += sizeof(_Word_Info) * nPoolCount;
	
	for(int i = 0; i < nPoolCount; i++)
	{
		m_WordInfoList[i].Init();
		m_WordInfoList[i].Set_Index(i);
		//printf("[CWordInfoPool::Init](0)nPos=%d.\n", nPos);
	}
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;	
}

size_t CWordInfoPool::Load(int nPoolCount, char* pData)
{
	Close();
	
	m_pBase       = pData;
	size_t nPos   = 0;
	m_pCryptTable = pData;
	nPos += 1280;
	
	//printf("[CNodePool::Load]nPos=%d.\n", nPos);
	m_WordInfoList = (_Word_Info* )&pData[nPos];
	nPos += sizeof(_Word_Info) * nPoolCount;
	
	m_nPoolCount   = nPoolCount;
	m_nCurrIndex   = 0;	
	
	return nPos;		
}

_Word_Info* CWordInfoPool::Create()
{
	if(NULL == m_WordInfoList)
	{
		return NULL;
	}
	
	if(m_nCurrIndex >= m_nPoolCount - 1)
	{
		m_nCurrIndex = 0;
	}	
	
	if(m_WordInfoList[m_nCurrIndex].m_cUsed == 0)
	{
		//printf("[CWordInfoPool::Create]m_nCurrIndex=%d, nIndex=%d.\n", m_nCurrIndex, m_WordInfoList[m_nCurrIndex].Get_Index());
		m_WordInfoList[m_nCurrIndex].m_cUsed = 1;
		return &m_WordInfoList[m_nCurrIndex++];
	}
	else
	{
		//循环寻找空位
		for(int i = m_nCurrIndex + 1; i < m_nPoolCount; i++)
		{
			if(m_WordInfoList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				if(m_nCurrIndex > m_nPoolCount - 1)
				{
					m_nCurrIndex = 0;
				}
				m_WordInfoList[i].m_cUsed = 1;
				return &m_WordInfoList[i];				
			}
		}
		
		printf("[CWordInfoPool::Create]m_nCurrIndex=%d,m_nPoolCount=%d.\n", m_nCurrIndex, m_nPoolCount);
		int nStart = 0;
		//没找到，再重头开始找
		for(int i = nStart; i < m_nCurrIndex - 1; i++)
		{
			if(m_WordInfoList[i].m_cUsed == 0)
			{
				m_nCurrIndex = i + 1;
				m_WordInfoList[i].m_cUsed = 1;
				return &m_WordInfoList[i];				
			}			
		}
		
		//已经没有空位
		return NULL;
	}		
}

int CWordInfoPool::Get_Node_Offset(_Word_Info* pWordInfo)
{
	int nOffset = 0;
	if(NULL != pWordInfo)
	{
		nOffset = (int)((char* )pWordInfo - m_pBase);
	}
	
	return nOffset;
}
	
_Word_Info* CWordInfoPool::Get_NodeOffset_Ptr(int nOffset)
{
	return (_Word_Info* )(m_pBase + nOffset);
}

bool CWordInfoPool::Delete(_Word_Info* pWordInfo)
{
	if(NULL == m_WordInfoList || NULL == pWordInfo)
	{
		return false;
	}	
	
	if(-1 == pWordInfo->Get_Index())
	{
		return false;
	}
	
	if(pWordInfo->Get_Index() >= m_nPoolCount || pWordInfo->Get_Index() < 0)
	{
		printf("[CWordInfoPool::Delete]Get_Index=%d is unvalid.\n", pWordInfo->Get_Index());
		return false;
	}
	
	m_WordInfoList[pWordInfo->Get_Index()].m_cUsed = 0;
	return true;	
}

//==================================================
CWordDict::CWordDict()
{
	m_nPoolSize = 0;
}

CWordDict::~CWordDict()
{
}

size_t CWordDict::Get_Mem_Size(int nPoolSize)
{
  size_t stPoolSize = nPoolSize * sizeof(_Word_Info) + nPoolSize * sizeof(_Hash_Table_Cell);
  stPoolSize += 1280;  //Hash字典
  stPoolSize += m_objHmmDict.Get_Mem_Size();
  m_nPoolSize = nPoolSize;
  return stPoolSize;	
}

bool CWordDict::Read_File_Line(ifstream& ifs, string& strLine)
{
	strLine = "";
	while(!ifs.eof())
	{
		getline(ifs, strLine);
 		if(StartsWith(strLine, "#") == true) 
 		{
    	continue;
    }
    else
    {
    	//printf("[CHmmDict::Read_File_Line]%s.\n", strLine.c_str());
    	return true;
    }
	}
	return false;	
}

bool CWordDict::Set_Dict_Hash_Table(vector<string>& objTempAttrList)
{
	_Rune objRune;
	char* pWord  = (char* )objTempAttrList[0].c_str();
	int nWordLen = strlen(pWord);
	char szPreWord[MAX_WORD_SIZE] = {'\0'};
	int nPrePos  = 0;
	int nLen     = 0;
	
	for(int i = 0; i < nWordLen;)
	{
		objRune.Clear();
		_Rune* pRune = &objRune;
		ENUM_WORD_TYPE emType = Get_Rune_From_String(pWord, i, nWordLen, pRune);
		if(emType != WORD_TYPE_UNKNOW)
		{
			i = i + pRune->m_nRuneLen;
			if(i < nWordLen)
			{
				//前置词
				memcpy(&szPreWord[nPrePos], pRune->m_szRune, pRune->m_nRuneLen);
				nPrePos += pRune->m_nRuneLen;
				szPreWord[nPrePos] = '\0';
				_Word_Info* pWordInfo = m_WordInfoPool.Create();
				if(NULL == pWordInfo)
				{
					printf("[CWordDict::Set_Dict_Hash_Table]pool is full.\n");
					return false;
				}
				sprintf(pWordInfo->m_szWord, "%s", szPreWord);
				pWordInfo->m_cType = PRE_WORD;
				
				//先查找hashmap里面是否已经存在
				if(-1 != m_hashDict.Get_Hash_Box_Data(pWordInfo->m_szWord))
				{
					//找到了，已经存在了，则不用添加前置词
					m_WordInfoPool.Delete(pWordInfo);
					//printf("[CWordDict::Set_Dict_Hash_Table]1 pWordInfo->m_szWord=%s is exist.\n", pWordInfo->m_szWord);
					continue;
				}
				
				int nNodeOffset = m_WordInfoPool.Get_Node_Offset(pWordInfo);
				if(-1 == m_hashDict.Add_Hash_Data(pWordInfo->m_szWord, nNodeOffset))
				{
					printf("[CWordDict::Set_Dict_Hash_Table]map is full(Used=%d)(All=%d).\n", m_hashDict.Get_Used_Count(), m_hashDict.Get_Count());
					return false;
				}
				//printf("[CWordDict::Set_Dict_Hash_Table](PRE_WORD)=%s.\n", pWordInfo->m_szWord);
			}
			else
			{
				//完整词
				memcpy(&szPreWord[nPrePos], pRune->m_szRune, pRune->m_nRuneLen);
				nPrePos += pRune->m_nRuneLen;
				szPreWord[nPrePos] = '\0';	
				_Word_Info* pWordInfo = m_WordInfoPool.Create();
				if(NULL == pWordInfo)
				{
					printf("[CWordDict::Set_Dict_Hash_Table]pool is full.\n");
					return false;
				}	
				sprintf(pWordInfo->m_szWord, "%s", szPreWord);
				pWordInfo->m_cType     = FULL_WORD;
				pWordInfo->m_nWordRote = (int)atoi(objTempAttrList[1].c_str());
				
				sprintf(pWordInfo->m_szWordSpeech, "%s", objTempAttrList[2].c_str());
				int nLen = (int)strlen(pWordInfo->m_szWordSpeech);
				if(pWordInfo->m_szWordSpeech[nLen - 1] == 0x0D)
				{
					pWordInfo->m_szWordSpeech[nLen - 1] = '\0';
				}

				//先查找hashmap里面是否已经存在
				int nPos = m_hashDict.Get_Hash_Box_Data(pWordInfo->m_szWord);
				if(-1 != nPos)
				{
					//找到了，已经存在了，则不用添加前置词
					m_WordInfoPool.Delete(pWordInfo);
					//printf("[CWordDict::Set_Dict_Hash_Table]2(%d) pWordInfo->m_szWord=%s is exist.\n", nPos, pWordInfo->m_szWord);
					continue;					
				}	
	
				int nNodeOffset = m_WordInfoPool.Get_Node_Offset(pWordInfo);
				if(-1 == m_hashDict.Add_Hash_Data(pWordInfo->m_szWord, nNodeOffset))
				{
					printf("[CWordDict::Set_Dict_Hash_Table]map is full(Used=%d)(All=%d).\n", m_hashDict.Get_Used_Count(), m_hashDict.Get_Count());
					return false;
				}							
			}
		}	
	}
	
	return true;
}

bool CWordDict::Init(const char* pFile, const char* pHMMFile, char* pData)
{
	size_t stSize = 0;
	stSize = m_WordInfoPool.Init(m_nPoolSize, pData);
	pData += stSize;
		
	m_hashDict.Init(pData, m_nPoolSize, m_WordInfoPool.GetCryptTable());
	pData += m_nPoolSize * sizeof(_Hash_Table_Cell);
	
	//读取HMM字典
	m_objHmmDict.Init(pHMMFile, pData);
	pData += m_objHmmDict.Get_Mem_Size();
	
	//读取配置文件，还原成数据结构
	ifstream ifs(pFile);
	string strLine;
	vector<string> objTempAttrList;
	
	//得到词典里面的所有词语
	int nLinePos = 0;
	while(true == Read_File_Line(ifs, strLine))
	{
		 if(strLine.length() == 0)
		 {
		 		printf("[CWordDict::Init]Map Count=%d.\n", m_hashDict.Get_Used_Count());
		 		return true;
		 }
		
		 objTempAttrList.clear();
		 Split(strLine, objTempAttrList, " ");
		 if(objTempAttrList.size() != 3)
		 {
		 		printf("[CWordDict::Init]word attr count error(Line=%d)(count=%d).\n", nLinePos, (int)objTempAttrList.size());
				return false;
		 }
		 else
		 {
		 		if(false == Set_Dict_Hash_Table(objTempAttrList))
		 		{
		 			printf("[CWordDict::Init]nLinePos=%d is error.\n", nLinePos);
		 			return false;
		 		}
		 		nLinePos++;
		 }
	}	
	
	return true;
}

bool CWordDict::Load(char* pData)
{
	size_t stSize = 0;
	stSize = m_WordInfoPool.Load(m_nPoolSize, pData);
	pData += stSize;
	//printf("[CWordDict::Load]stSize=%d.\n", stSize);
	
	m_hashDict.Load(pData, m_nPoolSize, m_WordInfoPool.GetCryptTable());
	pData += m_nPoolSize * sizeof(_Hash_Table_Cell);
	printf("[CWordDict::Load]Map Count=%d.\n", m_hashDict.Get_Used_Count());
	
	//加载HMM字典
	m_objHmmDict.Load(pData);
	pData += m_objHmmDict.Get_Mem_Size();	
	
	return true;
}

void CWordDict::Get_Sentence_ID(const char* pWord, int& nSentenceID)
{
	unsigned int nKeyWord = 0;
	memcpy(&nKeyWord, pWord, sizeof(int));
	
	if(nKeyWord == 0x8280E3 || nKeyWord == 0x80002E)
	{
		nSentenceID = nSentenceID + 1;
	}
}

void CWordDict::HMM_Cut_Word(vector<_Word_Param>& vecTempWord, vector<_Word_Param>& vecWord, int nType)
{
	int nRunePos    = 0;
	int nRuneCount  = 0;
	int nSentenceID = 0;
	
	char szTemp[MAX_SENTENCE_SIZE] = {'\0'};
	
	for(int i = 0; i < (int)vecTempWord.size(); i++)
	{
		if(FULL_WORD == vecTempWord[i].m_cType)
		{
			//判断上一个字是否已经结束
			if(nRuneCount == 1)
			{
				if(SELECT_RUNE == nType)
				{
					_Word_Param obj_Word_Param;
		 			sprintf(obj_Word_Param.m_szWord, "%s", szTemp);		
		 			obj_Word_Param.m_cType       = FULL_RUNE;		
		 			obj_Word_Param.m_sWordSize   = 1;		
		 			obj_Word_Param.m_nSentenceID = nSentenceID;		
		 			vecWord.push_back(obj_Word_Param);		
				}
			}
			else if(nRuneCount > 1)
			{
				//进行马尔科夫分解
				vector<_Word_Param> vecHMMWord;
				m_objHmmDict.Cut(szTemp, nRunePos, nSentenceID, nType, vecHMMWord);
				for(int j = 0; j < (int)vecHMMWord.size(); j++)
				{
					if(FULL_WORD == vecHMMWord[j].m_cType)
					{
						//printf("[CWordDict::HMM_Cut_Word]szTemp=%s,HMM=%s.\n", szTemp, vecHMMWord[j].m_szWord);
						vecWord.push_back(vecHMMWord[j]);
					}
					else if(FULL_RUNE == vecHMMWord[j].m_cType && SELECT_RUNE == nType)
					{
						vecWord.push_back(vecHMMWord[j]);
					}
				}
			}
			
			//当前词直接进入最终结果
			vecWord.push_back(vecTempWord[i]);
			
			nRunePos   = 0;
			nRuneCount = 0;
		}
		else
		{
			if(nRunePos == 0)
			{
				nSentenceID = vecTempWord[i].m_nSentenceID;
			}
			
			if(nSentenceID != vecTempWord[i].m_nSentenceID)
			{
				//如果不在同一句子中
				//首先看是不是需要对上一句进行HMM
				if(nRuneCount == 1)
				{
					if(SELECT_RUNE == nType)
					{
						_Word_Param obj_Word_Param;
			 			sprintf(obj_Word_Param.m_szWord, "%s", szTemp);		
			 			obj_Word_Param.m_cType       = FULL_RUNE;		
			 			obj_Word_Param.m_sWordSize   = 1;		
			 			obj_Word_Param.m_nSentenceID = nSentenceID;		
			 			vecWord.push_back(obj_Word_Param);		
					}
				}
				else if(nRuneCount > 1)
				{
					//进行马尔科夫分解
					vector<_Word_Param> vecHMMWord;
					m_objHmmDict.Cut(szTemp, nRuneCount, nSentenceID, nType, vecHMMWord);
					for(int j = 0; j < (int)vecHMMWord.size(); j++)
					{
						if(FULL_WORD == vecHMMWord[j].m_cType)
						{
							vecWord.push_back(vecHMMWord[j]);
						}
						else if(FULL_RUNE == vecHMMWord[j].m_cType && SELECT_RUNE == nType)
						{
							vecWord.push_back(vecHMMWord[j]);
						}
					}
				}
				
				//重新开始
				nRunePos   = 0;
				nRuneCount = 0;
			}

			if(nRunePos + (int)strlen(vecTempWord[i].m_szWord) >= MAX_SENTENCE_SIZE)
			{
				continue;
			}
			
			int nRuneLen = (int)strlen(vecTempWord[i].m_szWord);
			memcpy(&szTemp[nRunePos], vecTempWord[i].m_szWord, nRuneLen);
			nRunePos += nRuneLen;
			szTemp[nRunePos] = '\0';
			nRuneCount++;			
		}
	}
}

int CWordDict::Cut_Rune(const char* pSentence, vector<_Word_Param>& vecWord, int nSentenceID, int nType)
{
	int i    = 0;
	int nPos = 0;
	int nSentenceLen = strlen(pSentence);
	
	//进行HMM分词
	//m_objHmmDict.Cut(pSentence, nSentenceLen, nSentenceID, nType, vecWord);
	for(int i = 0; i < nSentenceLen;)		
 	{		
 		_Rune       objRune;		
 		_Word_Param obj_Word_Param;		
 				
 		ENUM_WORD_TYPE emType = Get_Rune_From_String(pSentence, i, nSentenceLen, &objRune);		
 		if(emType != WORD_TYPE_UNKNOW)		
 		{		
 			sprintf(obj_Word_Param.m_szWord, "%s", objRune.m_szRune);		
 			obj_Word_Param.m_cType       = FULL_RUNE;		
 			obj_Word_Param.m_sWordSize   = 1;		
 			obj_Word_Param.m_nSentenceID = nSentenceID;		
 			vecWord.push_back(obj_Word_Param);					
 		}		
 		else		
 		{		
 			printf("[CWordBase::Cut_Rune]<ERROR WORD TYPE>");		
 			break;					
 		}			
 		i = i + objRune.m_nRuneLen;					
 	}		
}

int CWordDict::Cut(const char* pSentence, vector<_Word_Param>& vecWord, int nType)
{
	int i           = 0;
	int nPos        = 0;
	int nWordSize   = 0;
	int nSentenceID = 1;
	int nSentenceLen = strlen(pSentence);
	
	vector<_Word_Param> vecTempWord;
	
	vecWord.clear();
	
	char szTempWord[MAX_WORD_SIZE];
	memset(szTempWord, 0, MAX_WORD_SIZE);
	
	for(int i = 0; i < nSentenceLen;)
	{
		_Rune       objRune;
		_Word_Param obj_Word_Param;
		
		ENUM_WORD_TYPE emType = Get_Rune_From_String(pSentence, i, nSentenceLen, &objRune);
		if(emType != WORD_TYPE_UNKNOW)
		{
			memcpy(&szTempWord[nPos], &pSentence[i], objRune.m_nRuneLen);
			nPos += objRune.m_nRuneLen;
			szTempWord[nPos] = '\0';
			nWordSize++;
			
			//开始在词库中寻找
			int nOffSet = m_hashDict.Get_Hash_Box_Data(szTempWord);
			if(nOffSet == -1)
			{
				//已经没有后续词了，将这个单词完整的入库
				if(nPos - objRune.m_nRuneLen > 0)
				{
					//如果是一个词，去掉最后一个字，因为这个字是下一个词的词头
					szTempWord[nPos - objRune.m_nRuneLen] = '\0';
					nOffSet = m_hashDict.Get_Hash_Box_Data(szTempWord);
					_Word_Info* pWordInfo = m_WordInfoPool.Get_NodeOffset_Ptr(nOffSet);
					if(pWordInfo->m_cType == FULL_WORD)
					{
						//是整词
						sprintf(obj_Word_Param.m_szWord, "%s", szTempWord);
						sprintf(obj_Word_Param.m_szWordSpeech, "%s", pWordInfo->m_szWordSpeech);
						obj_Word_Param.m_nWordRote   = pWordInfo->m_nWordRote;
						obj_Word_Param.m_cType       = pWordInfo->m_cType;
						obj_Word_Param.m_sWordSize   = --nWordSize;
						obj_Word_Param.m_nSentenceID = nSentenceID;
						vecTempWord.push_back(obj_Word_Param);
					}
					else
					{
						//不是整个词，需要把整个词拆分成单字
						Cut_Rune(szTempWord, vecTempWord, nSentenceID, nType);
					}
					
					nPos      = 0;
					nWordSize = 1;
					memcpy(&szTempWord[nPos], &pSentence[i], objRune.m_nRuneLen);
					nPos += objRune.m_nRuneLen;
					szTempWord[nPos] = '\0';	
					//再单独寻找一下这个词头，看看是不是单子还是词的一部分
					nOffSet = m_hashDict.Get_Hash_Box_Data(szTempWord);
					if(nOffSet == -1)
					{
						//字
						sprintf(obj_Word_Param.m_szWord, "%s", szTempWord);
						obj_Word_Param.m_cType = FULL_RUNE;
						obj_Word_Param.m_sWordSize   = 1;
						obj_Word_Param.m_nSentenceID = nSentenceID;							
						vecTempWord.push_back(obj_Word_Param);
						
						//判断是否为句子终止符
						Get_Sentence_ID(szTempWord, nSentenceID);						
						
						nPos      = 0;	
						nWordSize = 0;				
					}		
				}
				else
				{
					//如果是单个字，则直接入库
					sprintf(obj_Word_Param.m_szWord, "%s", szTempWord);
					obj_Word_Param.m_cType = FULL_RUNE;	
					obj_Word_Param.m_sWordSize   = 1;
					obj_Word_Param.m_nSentenceID = nSentenceID;											
					vecTempWord.push_back(obj_Word_Param);
					
					//判断是否为句子终止符
					Get_Sentence_ID(szTempWord, nSentenceID);						
					
					nPos      = 0;
					nWordSize = 0;
				}
			}
			
			i = i + objRune.m_nRuneLen;
			if(i == nSentenceLen && nPos > 0)
			{
				//如果是末尾了，判断是单字还是词语
				int nCurrPos = nPos - objRune.m_nRuneLen;
				if(nCurrPos == 0)
				{
					//字
					sprintf(obj_Word_Param.m_szWord, "%s", szTempWord);
					obj_Word_Param.m_cType       = FULL_RUNE;
					obj_Word_Param.m_sWordSize   = 1;
					obj_Word_Param.m_nSentenceID = nSentenceID;													
					vecTempWord.push_back(obj_Word_Param);
					nPos      = 0;
					nWordSize = 0;
				}
				else
				{
					//词
					nOffSet = m_hashDict.Get_Hash_Box_Data(szTempWord);
					_Word_Info* pWordInfo = m_WordInfoPool.Get_NodeOffset_Ptr(nOffSet);
					sprintf(obj_Word_Param.m_szWord, "%s", szTempWord);
					sprintf(obj_Word_Param.m_szWordSpeech, "%s", pWordInfo->m_szWordSpeech);
					obj_Word_Param.m_nWordRote   = pWordInfo->m_nWordRote;
					obj_Word_Param.m_cType       = pWordInfo->m_cType;	
					obj_Word_Param.m_sWordSize   = 1;
					obj_Word_Param.m_nSentenceID = nSentenceID;							
					vecTempWord.push_back(obj_Word_Param);
					nPos      = 0;	
					nWordSize = 0;				
				}				
			}
		}
		else
		{
			printf("[CWordBase::Cut]<ERROR WORD TYPE>");
			break;			
		}		
	}
	
	//最后进行统一的HMM筛选
	HMM_Cut_Word(vecTempWord, vecWord, nType);
	
	return 0;
}

bool CWordDict::Add_Word(const char* pWord, int nRote, const char* pWordSpeech)
{
	vector<string> objTempAttrList;
	
	char szRote[10] = {'\0'};
	sprintf(szRote, "%d", nRote);
	objTempAttrList.push_back((string)pWord);
	objTempAttrList.push_back((string)szRote);
	objTempAttrList.push_back((string)pWordSpeech);
	
	if(false == Set_Dict_Hash_Table(objTempAttrList))
	{
		printf("[CWordDict::Add_Word]Add fail.\n");
		return false;
	}	
	
	return true;
}

