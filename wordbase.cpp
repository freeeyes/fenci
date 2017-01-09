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

size_t CWordBase::Get_Mem_Size(int nPoolSize)
{
	size_t stPoolSize = 1280 + sizeof(int) + sizeof(_RuneLinkNode)*nPoolSize 
									+ sizeof(_Hash_Table_Cell) * MAIN_DICT_MAP_COUNT + sizeof(_Hash_Table_Cell) * CHILD_DICT_MAP_COUNT * MAIN_DICT_MAP_COUNT
									+ (sizeof(_Hash_Table_Cell) * END_DICT_MAP_COUNT) * (nPoolSize - MAIN_DICT_MAP_COUNT - 1);
									
	return stPoolSize;
}

bool CWordBase::Init(const char* pDictFile, int nPoolSize, char* pData)
{
	string strLine;
	char   szLine[50] = {'\0'};
	
	_RuneLinkNode* pCurrTempNode = NULL;
	
	//��ʼ��Rune�ڴ��
	size_t nSize = m_objNodePool.Init(nPoolSize, pData);
	printf("[CWordBase::Init]nPoolSize=%d, stPoolSize=%d.\n", nPoolSize, nSize);
	
	m_emType    = (int* )&pData[nSize];
	(*m_emType) = WORD_TYPE_ASCII;
	
	//��ʼ����
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
		
		//printf("[Init]szLine=%s.\n", szLine);
		
		int nLayer = 0;	
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
					//��¼��ǰ�ַ���
					(*m_emType) = emType;
					printf("[CWordBase::Init]m_emType=%d.\n", (*m_emType));
				}
			}
			else
			{
				printf("<ERROR WORD TYPE>");
				break;
			}	
			
			//printf("[CWordBase::Add_Word]i=%d.\n", i);
			pCurrTempNode = Set_HashMap_Word_Tree(pCurrTempNode, pRune, nLayer);
			if(NULL == pCurrTempNode)
			{
				return -1;
			}
			
			nLayer++;

			if(i == nLen - 1)
			{
				memcpy(pCurrTempNode->m_pWord, szLine, nLen - 1);
				//printf("[CWordBase::Init]m_pWord=%s.\n", pCurrTempNode->m_pWord);
			}					

		}
	}
	
	//չʾ��������
	int nLayer = 0;
	DisplayTempNodeList(m_pWordRoot, nLayer);
	
	return true;
}

bool CWordBase::Load(int nPoolSize, char* pData)
{
	//��ʼ��Rune�ڴ��
	size_t nSize = m_objNodePool.Load(nPoolSize, pData);
	printf("[Main]curr stPoolSize=%d.\n", nSize);
	
	m_emType = (int* )&pData[nSize];
	
	//��ʼ����
	m_pWordRoot = m_objNodePool.CreateRoot();	
	
	int nLayer = 0;
	DisplayTempNodeList(m_pWordRoot, nLayer);	
}

void CWordBase::DisplayTempNodeList(_RuneLinkNode* pRuneNode, int nLayer)
{
	//�ݹ���ʾ�����ӽڵ�
	int nMapSize = (int)pRuneNode->m_hmapRuneNextMap.Get_Used_Count();
	//printf("(nLayer=%d,size=%d)", nLayer, nMapSize);
	if(nMapSize > 0)
	{
		nLayer++;
		for(int i = 0; i < pRuneNode->m_hmapRuneNextMap.Get_Count(); i++)
		{
			int nOffset = pRuneNode->m_hmapRuneNextMap.Get_Index(i);
			if(nOffset > 0)
			{
				//printf("<i=%d>", i);
				_RuneLinkNode* pCurrRuneNode = m_objNodePool.Get_NodeOffset_Ptr(nOffset);
				if(pCurrRuneNode->m_hmapRuneNextMap.Get_Used_Count() > 0)
				{
					_Rune* pRune = &pRuneNode->m_objRune;
					printf("<%d>", nLayer);
					pRune->DisPlay();
					printf("-->");				
				}
				
				if(strlen(pRuneNode->m_pWord) > 0 && pRuneNode->m_hmapRuneNextMap.Get_Used_Count() > 0)
				{
					printf("1.Value=%s.\n", pRuneNode->m_pWord);
				}
				
				DisplayTempNodeList(pCurrRuneNode, nLayer);				
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
	//ɾ���ʻ���	
}

_RuneLinkNode* CWordBase::Set_HashMap_Word_Tree(_RuneLinkNode* pRuneNode, _Rune* pRune, int nLayer)
{
	//printf("[CWordBase::Set_HashMap_Word_Tree]pRuneNode=%d.\n", pRuneNode->m_nPoolIndex);
	int nOfficeSet = pRuneNode->m_hmapRuneNextMap.Get_Hash_Box_Data((char* )pRune->m_szRune);
	if(nOfficeSet > 0)
	{
		_RuneLinkNode* pCurrRuneNode = m_objNodePool.Get_NodeOffset_Ptr(nOfficeSet);
		
		if(pCurrRuneNode->m_objRune == (*pRune))
		{
			//����ҵ��ˣ��򷵻ص�ǰ�ڵ�
			return pCurrRuneNode;
		}
	}
	
	//���û�ҵ����򴴽��µ�
	_RuneLinkNode* pNode = m_objNodePool.Create(nLayer);
	//printf("[CWordBase::Set_HashMap_Word_Tree]pNode->m_nPoolIndex=%d.\n", pNode->m_nPoolIndex);
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

_RuneLinkNode* CWordBase::Find(_Rune* pRune, _RuneLinkNode* pRuneLinkNode)
{
	if(pRuneLinkNode->m_hmapRuneNextMap.Get_Used_Count() == 0)
	{
		//�ҵ���ĩβ
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
		//û���ҵ���ǰ�ַ�
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
			//���´Ӵʸ���ʼ����
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
				//�Ѿ��ҵ�ĩβ�ˣ�ֱ�����
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
			//���´Ӵʸ���ʼ����
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
				//�Ѿ��ҵ�ĩβ�ˣ�ֱ�����
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
	
	//�鿴��ǰ���Ѿ��ڴʿ���
	vector<string> vecWord;
	Cut(pWord, vecWord);
	if(vecWord.size() == 1 && strcmp(vecWord[0].c_str(), pWord) == 0)
	{
		printf("[CWordBase::Add_Word]word is exist.\n");
		return 0;
	}
	
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
	
	int nLayer = 0;	
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
				printf("[CWordBase::Add_Word]Dictory and Sentence character set mismatching(emType=%d)(m_emType=%d).\n", emType, (*m_emType));
				return -1;
			}
		}
		else
		{
			printf("<ERROR WORD TYPE>");
			break;
		}	
		
		pCurrTempNode = Set_HashMap_Word_Tree(pCurrTempNode, pRune, nLayer);
		if(NULL == pCurrTempNode)
		{
			return -1;
		}
		
		nLayer++;

		if(i == nLen - 1)
		{
			memcpy(pCurrTempNode->m_pWord, szLine, nLen - 1);
			//printf("[CWordBase::Init]m_pWord=%s.\n", pCurrTempNode->m_pWord);
		}
	}	
	
	//��ʾ���еĴʿ�
	//int nLayer = 0;
	//DisplayTempNodeList(m_pWordRoot, nLayer);	
	
	return 0;
}
