#include "wordbase.h"

CWordBase::CWordBase()
{
	m_pWordRoot = NULL;
	m_emType    = NULL;
}

CWordBase::~CWordBase()
{
	Close();
}

bool CWordBase::Init(const char* pDictFile, int nPoolSize, char* pData)
{
	string strLine;
	char   szLine[50] = {'\0'};
	
	_RuneLinkNode* pCurrTempNode = NULL;
	
	//初始化Rune内存池
	size_t nSize = m_objNodePool.Init(nPoolSize, pData);
	printf("[Main]curr stPoolSize=%d.\n", nSize);
	
	m_emType    = (int* )&pData[nSize];
	(*m_emType) = WORD_TYPE_ASCII;
	
	//初始化树
	m_pWordRoot = m_objNodePool.CreateRoot();
	
	ifstream ifs(pDictFile);
	while(!ifs.eof())
	{
		getline(ifs, strLine);
		pCurrTempNode = m_pWordRoot;
		
		//printf("[CWordBase::Init]Line Begin(%s).\n", strLine.c_str());
		int nLen = 0;
		sprintf(szLine, "%s", strLine.c_str());
		if(szLine[strLine.length() - 1] == 0x0D)
		{
#ifdef WIN32
			nLen = strlen(szLine) + 1;
#else
			nLen = strlen(szLine);
#endif 
		}
		else
		{
#ifdef WIN32
			nLen = strlen(szLine) + 2;
#else
			nLen = strlen(szLine) + 1;
#endif 			
		}
		
		_Rune objRune;
			
		for(int i = 0; i < nLen - 1;)
		{
			objRune.Clear();
			_Rune* pRune = &objRune;
			ENUM_WORD_TYPE emType = Get_Rune_From_String(szLine, i, nLen, pRune);
			if(emType != WORD_TYPE_UNKNOW)
			{
				i = i + pRune->m_nRuneLen;
				if((*m_emType) == WORD_TYPE_ASCII && emType != WORD_TYPE_ASCII)
				{
					//记录当前字符集
					(*m_emType) = emType;
					printf("[CWordBase::Init]m_emType=%d.\n", (*m_emType));
				}
			}
			else
			{
				printf("<ERROR WORD TYPE>");
				break;
			}	
			
			pCurrTempNode = Set_HashMap_Word_Tree(pCurrTempNode, pRune);
			if(NULL == pCurrTempNode)
			{
				return -1;
			}

			if(i == nLen - 1)
			{
				memcpy(pCurrTempNode->m_pWord, szLine, nLen - 1);
				//printf("[CWordBase::Init]m_pWord=%s.\n", pCurrTempNode->m_pWord);
			}					

		}
	}
	
	//展示链表内容
	//int nLayer = 0;
	//DisplayTempNodeList(m_pWordRoot, nLayer);
	
	return true;
}

bool CWordBase::Load(int nPoolSize, char* pData)
{
	//初始化Rune内存池
	size_t nSize = m_objNodePool.Load(nPoolSize, pData);
	printf("[Main]curr stPoolSize=%d.\n", nSize);
	
	m_emType = (int* )&pData[nSize];
	
	//初始化树
	m_pWordRoot = m_objNodePool.CreateRoot();	
}

void CWordBase::DisplayTempNodeList(_RuneLinkNode* pRuneNode, int nLayer)
{
	//递归显示所有子节点
	int nMapSize = (int)pRuneNode->m_hmapRuneNextMap.Get_Used_Count();
	//printf("[CWordBase::DisplayTempNodeList]pRuneNode=%d,size=%d.\n", nLayer, nMapSize);
	if(nMapSize > 0)
	{
		nLayer++;
		for(int i = 0; i < pRuneNode->m_hmapRuneNextMap.Get_Count(); i++)
		{
			int nOffset = pRuneNode->m_hmapRuneNextMap.Get_Index(i);
			if(nOffset > 0)
			{
				pRuneNode = m_objNodePool.Get_NodeOffset_Ptr(nOffset);
				if(pRuneNode->m_hmapRuneNextMap.Get_Used_Count() > 0)
				{
					_Rune* pRune = &pRuneNode->m_objRune;
					printf("<%d>", nLayer);
					pRune->DisPlay();
					printf("-->");				
				}
				
				if(strlen(pRuneNode->m_pWord) > 0 && pRuneNode->m_hmapRuneNextMap.Get_Count() > 0)
				{
					printf("1.Value=%s.\n", pRuneNode->m_pWord);
				}
				
				DisplayTempNodeList(pRuneNode, nLayer);				
			}		
		}
	}
	else
	{
		_Rune* pRune = &pRuneNode->m_objRune;
		printf("<%d>", nLayer);
		pRune->DisPlay();
		printf("-->");
		
		if(strlen(pRuneNode->m_pWord) > 0)
		{
			printf("2.Value=%s.\n", pRuneNode->m_pWord);
		}
	}
}

void CWordBase::Close()
{
	//删除词汇树	
}

_RuneLinkNode* CWordBase::Set_HashMap_Word_Tree(_RuneLinkNode* pRuneNode, _Rune* pRune)
{
	int nOfficeSet = pRuneNode->m_hmapRuneNextMap.Get_Hash_Box_Data((char* )pRune->m_szRune);
	if(nOfficeSet > 0)
	{
		_RuneLinkNode* pCurrRuneNode = m_objNodePool.Get_NodeOffset_Ptr(nOfficeSet);
		
		if(pCurrRuneNode->m_objRune == (*pRune))
		{
			//如果找到了，则返回当前节点
			return pCurrRuneNode;
		}
	}
	
	//如果没找到，则创建新的
	_RuneLinkNode* pNode = m_objNodePool.Create();
	if(NULL == pNode)
	{
		printf("[CWordBase::Set_HashMap_Word_Tree]node pool is empty.\n");
		return NULL;
	}
	
	int nNodeOffset = m_objNodePool.Get_Node_Offset(pNode);
	pNode->m_objRune = (*pRune);
	int nPos = pRuneNode->m_hmapRuneNextMap.Add_Hash_Data((char* )pRune->m_szRune, nNodeOffset);
	if(-1 == nPos)
	{
		printf("[CWordBase::Set_HashMap_Word_Tree]tree node child is full.\n");
		m_objNodePool.Delete(pNode);
		return NULL;		
	}
	return pNode;
}

ENUM_WORD_TYPE CWordBase::Get_Rune_From_String(const char* pWord, int nBegin, int nLen, _Rune* pRune)
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

_RuneLinkNode* CWordBase::Find(_Rune* pRune, _RuneLinkNode* pRuneLinkNode)
{
	if(pRuneLinkNode->m_hmapRuneNextMap.Get_Used_Count() == 0)
	{
		//找到了末尾
		if((*pRune) == pRuneLinkNode->m_objRune)
		{
			return pRuneLinkNode;
		}
		else
		{
			return NULL;
		}
	}
	
	//printf("[CWordBase::Find]pRune=%d, size=%d.\n", pRune->Get_Number(), pRuneLinkNode->m_hmapRuneNextMap.size());
	int nOfficeSet = pRuneLinkNode->m_hmapRuneNextMap.Get_Hash_Box_Data((char* )pRune->m_szRune);
	if(0 == nOfficeSet)
	{
		//没有找到当前字符
		return NULL;
	}
	else
	{
		return m_objNodePool.Get_NodeOffset_Ptr(nOfficeSet);	
	}
}

int CWordBase::Cut_Word(const char* pSentence, vector<string>& vecWord)
{
	int i    = 0;
	int nPos = 0;
	int nSentenceLen = strlen(pSentence);
	
	_RuneLinkNode* pPosRuneLinkNode = m_pWordRoot;
	
	for(int i = 0; i < nSentenceLen;)
	{
		_Rune objRune;
		
		//printf("[CWordBase::Cut]i=%d.\n", i);
		ENUM_WORD_TYPE emType = Get_Rune_From_String(pSentence, i, nSentenceLen, &objRune);
		if(emType != WORD_TYPE_UNKNOW)
		{
			i = i + objRune.m_nRuneLen;
			if(emType != WORD_TYPE_ASCII && emType != (*m_emType))
			{
				printf("[CWordBase::Cut_Word]Dictory and Sentence character set mismatching(emType=%d)(m_emType=%d).\n", emType, (*m_emType));
				return -1;
			}
		}
		else
		{
			printf("[CWordBase::Cut]<ERROR WORD TYPE>");
			break;
		}
		
		//printf("[CWordBase::Cut]");
		//objRune.DisPlay();
		//printf("\n");
		
		//printf("[CWordBase::Cut]pPosRuneLinkNode=0x%08x.\n", pPosRuneLinkNode);
		
		_RuneLinkNode* pCurrRuneLinkNode = Find(&objRune, pPosRuneLinkNode);
		if(NULL == pCurrRuneLinkNode)
		{
			//printf("[CWordBase::Cut]pCurrRuneLinkNode=NULL.\n");
			if(NULL != pPosRuneLinkNode && strlen(pPosRuneLinkNode->m_pWord) > 0)
			{
				string strWord = (string)pPosRuneLinkNode->m_pWord;
				vecWord.push_back(strWord);
			} 
			
			pPosRuneLinkNode = m_pWordRoot;
			//重新从词根开始查找
			pCurrRuneLinkNode = Find(&objRune, pPosRuneLinkNode);
			if(NULL != pCurrRuneLinkNode)
			{
				pPosRuneLinkNode = pCurrRuneLinkNode;
			}
		}
		else
		{
			if(i == nSentenceLen)
			{
				//已经找到末尾了，直接入库
				pPosRuneLinkNode = pCurrRuneLinkNode;
				string strWord = (string)pPosRuneLinkNode->m_pWord;
				vecWord.push_back(strWord);				
			}	
			else
			{
				pPosRuneLinkNode = pCurrRuneLinkNode;
			}
		}
	}
	
	return 0;
}

int CWordBase::Cut(const char* pSentence, vector<string>& vecWord)
{
	int i    = 0;
	int nPos = 0;
	int nSentenceLen = strlen(pSentence);
	
	char szTemp[MAX_WORD_LENGTH] = {'\0'};
	
	_RuneLinkNode* pPosRuneLinkNode = m_pWordRoot;
	
	for(int i = 0; i < nSentenceLen;)
	{
		_Rune objRune;
		
		//printf("[CWordBase::Cut]i=%d.\n", i);
		ENUM_WORD_TYPE emType = Get_Rune_From_String(pSentence, i, nSentenceLen, &objRune);
		if(emType != WORD_TYPE_UNKNOW)
		{
			i = i + objRune.m_nRuneLen;
			if(emType != WORD_TYPE_ASCII && emType != (*m_emType))
			{
				printf("[CWordBase::Cut]Dictory and Sentence character set mismatching(emType=%d)(m_emType=%d).\n", emType, (*m_emType));
				return -1;
			}			
		}
		else
		{
			printf("[CWordBase::Cut]<ERROR WORD TYPE>");
			break;
		}
		
		//printf("[CWordBase::Cut]");
		//objRune.DisPlay();
		//printf("\n");
		
		//printf("[CWordBase::Cut]pPosRuneLinkNode=0x%08x.\n", pPosRuneLinkNode);
		
		_RuneLinkNode* pCurrRuneLinkNode = Find(&objRune, pPosRuneLinkNode);
		if(NULL == pCurrRuneLinkNode)
		{
			//printf("[CWordBase::Cut]pCurrRuneLinkNode=NULL.\n");
			if(NULL != pPosRuneLinkNode && strlen(pPosRuneLinkNode->m_pWord) > 0)
			{
				string strWord = (string)pPosRuneLinkNode->m_pWord;
				vecWord.push_back(strWord);
			} 
			
			pPosRuneLinkNode = m_pWordRoot;
			//重新从词根开始查找
			pCurrRuneLinkNode = Find(&objRune, pPosRuneLinkNode);
			if(NULL != pCurrRuneLinkNode)
			{
				pPosRuneLinkNode = pCurrRuneLinkNode;
			}
			else
			{
				memcpy(szTemp, objRune.m_szRune, objRune.m_nRuneLen);
				szTemp[objRune.m_nRuneLen] = '\0';
				vecWord.push_back((string)szTemp);
			}
		}
		else
		{
			//printf("[CWordBase::Cut]i=%d,nSentenceLen=%d.\n", i, nSentenceLen);
			if(i == nSentenceLen)
			{
				//已经找到末尾了，直接入库
				pPosRuneLinkNode = pCurrRuneLinkNode;
				string strWord = (string)pPosRuneLinkNode->m_pWord;
				vecWord.push_back(strWord);				
			}	
			else
			{
				pPosRuneLinkNode = pCurrRuneLinkNode;
			}
		}
	}	
	
	return 0;
}

int CWordBase::Add_Word(const char* pWord)
{
	char   szLine[50] = {'\0'};

	_RuneLinkNode* pCurrTempNode = NULL;	

	pCurrTempNode = m_pWordRoot;
	
	int nLen = 0;
	sprintf(szLine, "%s", pWord);
	if(szLine[strlen(pWord) - 1] == 0x0D)
	{
#ifdef WIN32
		nLen = strlen(szLine) + 1;
#else
		nLen = strlen(szLine);
#endif 
	}
	else
	{
#ifdef WIN32
		nLen = strlen(szLine) + 2;
#else
		nLen = strlen(szLine) + 1;
#endif 			
	}
	
	_Rune objRune;
	for(int i = 0; i < nLen - 1;)
	{
		objRune.Clear();
		
		_Rune* pRune = &objRune;
		ENUM_WORD_TYPE emType = Get_Rune_From_String(szLine, i, nLen, pRune);
		if(emType != WORD_TYPE_UNKNOW)
		{
			i = i + pRune->m_nRuneLen;
			if(emType != WORD_TYPE_ASCII && emType != (*m_emType))
			{
				printf("[CWordBase::Cut_Word]Dictory and Sentence character set mismatching(emType=%d)(m_emType=%d).\n", emType, (*m_emType));
				return -1;
			}
		}
		else
		{
			printf("<ERROR WORD TYPE>");
			break;
		}	
		
		pCurrTempNode = Set_HashMap_Word_Tree(pCurrTempNode, pRune);
		if(NULL == pCurrTempNode)
		{
			return -1;
		}

		if(i == nLen - 1)
		{
			memcpy(pCurrTempNode->m_pWord, szLine, nLen - 1);
			//printf("[CWordBase::Init]m_pWord=%s.\n", pCurrTempNode->m_pWord);
		}
	}	
	
	//显示所有的词库
	//int nLayer = 0;
	//DisplayTempNodeList(m_pWordRoot, nLayer);	
	
	return 0;
}
