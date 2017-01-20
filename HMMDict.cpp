#include "HMMDict.h"

CHmmDict::CHmmDict()
{
	m_dbStart     = NULL;
	m_dbTransProb = NULL;
}

CHmmDict::~CHmmDict()
{
	
}

size_t CHmmDict::Get_Mem_Size()
{
  size_t stPoolSize = HMM_DICT_POOL_SIZE * sizeof(_RuneHMMInfo) + HMM_HASH_SIZE * sizeof(_Hash_Table_Cell) * 4;
  stPoolSize += 1280;  //Hash字典
  stPoolSize += sizeof(double)*RUNE_POS_ALL;
  stPoolSize += sizeof(double)*RUNE_POS_ALL*RUNE_POS_ALL;
  return stPoolSize;
}

bool CHmmDict::Read_File_Line(ifstream& ifs, string& strLine)
{
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

bool CHmmDict::LoadHashProb(const string& strLine, CHashTable& hbProb) 
{
  if (strLine.empty()) 
  {
    return false;
  }
  vector<string> objTempList, objRuneList;
  Split(strLine, objTempList, ",");
  for (size_t i = 0; i < objTempList.size(); i++) 
  {
    Split(objTempList[i], objRuneList, ":");
    if (2 != objRuneList.size())
    {
      printf("[CHmmDict::LoadHashProb]emitProb illegal.\n");
      return false;
    }
    
    //将数据插入到HashMap里面去
    _RuneHMMInfo* pRuneHMMInfo = m_objRuneHMMPool.Create();
    if(NULL == pRuneHMMInfo)
    {
      printf("[CHmmDict::LoadHashProb](%d)pool is full.\n", i);
      return false;    	
    }
    else
    {
    	//printf("[CHmmDict::LoadHashProb]Index=%d.\n", pRuneHMMInfo->Get_Index());
    }
    
    memcpy(pRuneHMMInfo->m_szRune, (char* )objRuneList[0].c_str(), objRuneList[0].length());
    pRuneHMMInfo->m_dbProb = atof(objRuneList[1].c_str()); 
    
    int nOffSetPos = m_objRuneHMMPool.Get_Node_Offset(pRuneHMMInfo);
    
    hbProb.Add_Hash_Data(pRuneHMMInfo->m_szRune, nOffSetPos);
  }
  
  return true;
}

bool CHmmDict::Init(const char* pFile, char* pData)
{
	string strLine;
	
	//初始化内存池
	size_t stSize = 0;
	stSize = m_objRuneHMMPool.Init(HMM_DICT_POOL_SIZE, pData);
	//printf("[CHmmDict::Init]m_objRuneHMMPool=%d.\n", (int)stSize);
	pData += stSize;
	
	//固定数组
	m_dbStart     = (double* )pData;
	pData += sizeof(double)*RUNE_POS_ALL;
	m_dbTransProb = (double* )pData;
	pData += sizeof(double)*RUNE_POS_ALL*RUNE_POS_ALL;
	
	//Hash数组
	m_hashMapB.Init(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);
	m_hashMapE.Init(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);
	m_hashMapM.Init(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);
	m_hashMapS.Init(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);
	printf("[CHmmDict::Init]stSize=%d.\n", (int)stSize + HMM_HASH_SIZE * sizeof(_Hash_Table_Cell) * 4);
	
	//读取配置文件，还原成数据结构
	ifstream ifs(pFile);
	vector<string> objTempAttrList;
	
	//得到所有文字的初始化的BEMS概率表
	if(true == Read_File_Line(ifs, strLine))
	{
		 objTempAttrList.clear();
		 Split(strLine, objTempAttrList, " ");
		 if(objTempAttrList.size() != RUNE_POS_ALL)
		 {
		 		printf("[CHmmDict::Init]m_dbStart count error(%d).\n", (int)objTempAttrList.size());
				return false;
		 }
		 else
		 {
		 		for(int i = 0; i < RUNE_POS_ALL; i++)
		 		{
		 			m_dbStart[i]  = atof(objTempAttrList[i].c_str());
		 		}
		 		printf("[CHmmDict::Init]m_dbStart Read OK.\n");
		 }
	}
	else
	{
		printf("[CHmmDict::Init]m_dbStart read error.\n");
		return false;
	}
	
	//得到所有字根对应BEMS的概率总表
	for(int i = 0; i < RUNE_POS_ALL; i++)
	{
		if(true == Read_File_Line(ifs, strLine))
		{
			 Split(strLine, objTempAttrList, " ");
			 if(objTempAttrList.size() != RUNE_POS_ALL)
			 {
			 		printf("[CHmmDict::Init]m_dbTransProb count error(%d).\n", (int)objTempAttrList.size());
					return false;
			 }
			 else
			 {
			 		for(int j = 0; j < RUNE_POS_ALL; j++)
			 		{
			 			m_dbTransProb[j + i*RUNE_POS_ALL]  = atof(objTempAttrList[j].c_str());
			 		}
			 }
		}
		else
		{
			printf("[CHmmDict::Init]m_dbTransProb read error.\n");
			return false;
		}	
	}
	printf("[CHmmDict::Init]m_dbTransProb Read OK.\n");
	
	//读取每个文字的B概率
	if(true == Read_File_Line(ifs, strLine))
	{
		LoadHashProb(strLine, m_hashMapB);
		printf("[CHmmDict::Init]m_hashMapB count=%d OK.\n", m_hashMapB.Get_Used_Count());
	}
	else
	{
		printf("[CHmmDict::Init]m_hashMapB read error.\n");
		return false;		
	}
	
	//读取每个文字的E概率
	if(true == Read_File_Line(ifs, strLine))
	{
		LoadHashProb(strLine, m_hashMapE);
		printf("[CHmmDict::Init]m_hashMapE count=%d OK.\n", m_hashMapE.Get_Used_Count());
	}
	else
	{
		printf("[CHmmDict::Init]m_hashMapE read error.\n");
		return false;		
	}
	
	//读取每个文字的M概率
	if(true == Read_File_Line(ifs, strLine))
	{
		LoadHashProb(strLine, m_hashMapM);
		printf("[CHmmDict::Init]m_hashMapM count=%d OK.\n", m_hashMapM.Get_Used_Count());
	}
	else
	{
		printf("[CHmmDict::Init]m_hashMapM read error.\n");
		return false;		
	}	
	
	//读取每个文字的S概率
	if(true == Read_File_Line(ifs, strLine))
	{
		LoadHashProb(strLine, m_hashMapS);
		printf("[CHmmDict::Init]m_hashMapS count=%d OK.\n", m_hashMapS.Get_Used_Count());
	}
	else
	{
		printf("[CHmmDict::Init]m_hashMapS read error.\n");
		return false;		
	}	
	
	return true;
}

bool CHmmDict::Load(char* pData)
{
	size_t stSize = 0;
	stSize = m_objRuneHMMPool.Load(HMM_DICT_POOL_SIZE, pData);
	pData += stSize;
	
	m_dbStart     = (double* )pData;
	pData += sizeof(double)*RUNE_POS_ALL;
	m_dbTransProb = (double* )pData;
	pData += sizeof(double)*RUNE_POS_ALL*RUNE_POS_ALL;	
	
	m_hashMapB.Load(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);	
	m_hashMapE.Load(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);	
	m_hashMapM.Load(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);
	m_hashMapS.Load(pData, HMM_HASH_SIZE, m_objRuneHMMPool.GetCryptTable());
	pData += HMM_HASH_SIZE * sizeof(_Hash_Table_Cell);	
	
	printf("[CHmmDict::Load]m_hashMapB count=%d OK.\n", m_hashMapB.Get_Used_Count());
	printf("[CHmmDict::Load]m_hashMapB count=%d OK.\n", m_hashMapE.Get_Used_Count());
	printf("[CHmmDict::Load]m_hashMapB count=%d OK.\n", m_hashMapM.Get_Used_Count());
	printf("[CHmmDict::Load]m_hashMapB count=%d OK.\n", m_hashMapS.Get_Used_Count());
	
	return true;
}

void CHmmDict::Sentance_To_Rune(const char* pData, int nLen, vector<_Rune>& objRuneList)
{
	_Rune objRune;
	for(int i = 0; i < nLen;)
	{
		ENUM_WORD_TYPE emType = Get_Rune_From_String(pData, i, nLen, &objRune);
		if(emType != WORD_TYPE_UNKNOW)
		{
			i = i + objRune.m_nRuneLen;
			objRuneList.push_back(objRune);
		}
		else
		{
			printf("[CWordBase::Cut]<ERROR WORD TYPE>");
			break;
		}		
	}	
}

double CHmmDict::Get_Rune_Prob(_Rune objRune, CHashTable& objhashMap)
{
	double dbRuneProb = MIN_DOUBLE;
	int nOffset = objhashMap.Get_Hash_Box_Data((const char* )objRune.m_szRune);
	//printf("[CHmmDict::Get_Rune_Prob]nOffset=%d.\n", nOffset);
	_RuneHMMInfo* pRuneHMMInfo = m_objRuneHMMPool.Get_NodeOffset_Ptr(nOffset);
	if(NULL != pRuneHMMInfo)
	{
		dbRuneProb = pRuneHMMInfo->m_dbProb;
	}
	
	//printf("[CHmmDict::Get_Rune_Prob]dbRuneProb=%f.\n", dbRuneProb);
	return dbRuneProb;
}

void CHmmDict::Viterbi(const char* pData, int nLen, vector<_Rune>& objRuneList, vector<short>& objResList)
{
	//首先把句子拆成一个个字
	Sentance_To_Rune(pData, nLen, objRuneList);
	//printf("[CHmmDict::Viterbi]objRuneList Count=%d.\n", objRuneList.size());
	
	int nRowCount    = (int)objRuneList.size();
	int nColCount    = (int)RUNE_POS_ALL;
	int nMatrixCount = nRowCount * nColCount;
	
	//马尔科夫模型
	//组建显式矩阵(字权重矩阵)
	vector<double> objWeightMatrix(nMatrixCount);
	
	//组建隐式矩阵(字状态矩阵)
	vector<short> objStatusMatrix(nMatrixCount);
	
	//显式矩阵第一行，第一个字初始状态的概率，对应BEMS
	//第一个字的概率为 初始概率 + 字的初始概率
  for (int nCol = 0; nCol < nColCount; nCol++) 
  {
  	if(nCol == RUNE_POS_B)
  	{
    	objWeightMatrix[0 + nCol * nRowCount] = m_dbStart[nCol] + Get_Rune_Prob(objRuneList[0], m_hashMapB);
    	objStatusMatrix[0 + nCol * nRowCount] = -1;
    }
    else if(nCol == RUNE_POS_E)
    {
    	objWeightMatrix[0 + nCol * nRowCount] = m_dbStart[nCol] + Get_Rune_Prob(objRuneList[0], m_hashMapE);
    	objStatusMatrix[0 + nCol * nRowCount] = -1;    	
    }
    else if(nCol == RUNE_POS_M)
    {
    	objWeightMatrix[0 + nCol * nRowCount] = m_dbStart[nCol] + Get_Rune_Prob(objRuneList[0], m_hashMapM);
    	objStatusMatrix[0 + nCol * nRowCount] = -1;    	
    }
    else
    {
    	objWeightMatrix[0 + nCol * nRowCount] = m_dbStart[nCol] + Get_Rune_Prob(objRuneList[0], m_hashMapS);
    	objStatusMatrix[0 + nCol * nRowCount] = -1;    	
    }            
  }	
	
	//填充矩阵的其余部分(按字的顺序)
	for(int nRow = 1; nRow < nRowCount; nRow++)
	{
		for (int nCol = 0; nCol < nColCount; nCol++) 
		{
			int nCurrPos = nRow + nCol*nRowCount;
			objWeightMatrix[nCurrPos] = MIN_DOUBLE;
			objStatusMatrix[nCurrPos] = RUNE_POS_E;
			double dbCurrProb = MIN_DOUBLE;
			if(nCol == RUNE_POS_B)
			{
				dbCurrProb = Get_Rune_Prob(objRuneList[nRow], m_hashMapB);
			}
			else if(nCol == RUNE_POS_E)
			{
				dbCurrProb = Get_Rune_Prob(objRuneList[nRow], m_hashMapE);
			}
			else if(nCol == RUNE_POS_M)
			{
				dbCurrProb = Get_Rune_Prob(objRuneList[nRow], m_hashMapM);
			}
			else
			{
				dbCurrProb = Get_Rune_Prob(objRuneList[nRow], m_hashMapS);
			}
			
			//寻找和上一个字的对应关系，取概率最高的关系作为当前概率(算法核心)
			for (int nPreCol = 0; nPreCol < nColCount; nPreCol++)
			{
				int nOldPos = nRow - 1 + nPreCol * nRowCount;
				double dbTemp = objWeightMatrix[nOldPos] + m_dbTransProb[nCol + nPreCol*nColCount] + dbCurrProb;
				if (dbTemp > objWeightMatrix[nCurrPos]) 
				{
					objWeightMatrix[nCurrPos] = dbTemp;
					objStatusMatrix[nCurrPos] = nPreCol;
				}
			}
		}
	}
	
	//打印隐式矩阵
	/*
	printf("=============(Status Matrix)=================\n");
	for(int nRow = 0; nRow < nRowCount; nRow++)
	{
		for (int nCol = 0; nCol < nColCount; nCol++) 
		{
			int nCurrPos = nRow + nCol*nRowCount;
			printf("%d	", objStatusMatrix[nCurrPos]);	
		}
		printf("\n");
	}
	printf("=============(Status Matrix)=================\n");
	*/
	//获得最后末尾的S和E，因为末尾的字只可能是这两个状态之一
	double dbEndE = objWeightMatrix[nRowCount - 1 + RUNE_POS_E*nRowCount];
	double dbEndS = objWeightMatrix[nRowCount - 1 + RUNE_POS_S*nRowCount];
	short sStat = RUNE_POS_E;
	if(dbEndE < dbEndS)
	{
		sStat = RUNE_POS_S;
	}
	
	objResList.resize(nRowCount);
	for(int i = nRowCount - 1; i >= 0; i--)
	{
		objResList[i] = sStat;
		sStat       = objStatusMatrix[i + sStat*nRowCount];
	}
	
	/*
	for(int i = 0; i < nRowCount; i++)
	{
		if(objResList[i] == RUNE_POS_B)
		{
			printf("B");
		}
		else if(objResList[i] == RUNE_POS_E)
		{
			printf("E");
		}
		else if(objResList[i] == RUNE_POS_M)
		{
			printf("M");
		}
		else
		{
			printf("S");
		}		
	}
	printf("\n");
	*/
}

void CHmmDict::Cut(const char* pData, int nLen, int nSentenceID, int nType, vector<_Word_Param>& objWordList)
{
	vector<_Rune> objRuneList;
	vector<short> objResList;
	
	Viterbi(pData, nLen, objRuneList, objResList);
	
	char szTemp[100] = {'\0'};
	int  nTempLen    = 0;
	//整合输出成词
	for(int i = 0; i < (int)objRuneList.size(); i++)
	{
		if(objResList[i] == RUNE_POS_B)
		{
			nTempLen = 0;
			memcpy(&szTemp[nTempLen], objRuneList[i].m_szRune, objRuneList[i].m_nRuneLen);
			nTempLen += objRuneList[i].m_nRuneLen;
		}
		else if(objResList[i] == RUNE_POS_E)
		{
			_Word_Param obj_Word_Param;
			memcpy(&szTemp[nTempLen], objRuneList[i].m_szRune, objRuneList[i].m_nRuneLen);
			nTempLen += objRuneList[i].m_nRuneLen;
			szTemp[nTempLen] = '\0';
			
			sprintf(obj_Word_Param.m_szWord, "%s", szTemp);
			sprintf(obj_Word_Param.m_szWordSpeech, "hmm");
			obj_Word_Param.m_cType       = FULL_WORD;
			obj_Word_Param.m_sWordSize   = nTempLen; 
			obj_Word_Param.m_nSentenceID = nSentenceID;
			objWordList.push_back(obj_Word_Param);
		}
		else if(objResList[i] == RUNE_POS_M)
		{
			memcpy(&szTemp[nTempLen], objRuneList[i].m_szRune, objRuneList[i].m_nRuneLen);
			nTempLen += objRuneList[i].m_nRuneLen;			
		}
		else
		{
			_Word_Param obj_Word_Param;
			nTempLen = 0;
			memcpy(&szTemp[nTempLen], objRuneList[i].m_szRune, objRuneList[i].m_nRuneLen);
			nTempLen += objRuneList[i].m_nRuneLen;
			szTemp[nTempLen] = '\0';
			
			if(nType == SELECT_RUNE)
			{
				sprintf(obj_Word_Param.m_szWord, "%s", szTemp);
				sprintf(obj_Word_Param.m_szWordSpeech, "hmm");
				obj_Word_Param.m_cType       = FULL_RUNE;
				obj_Word_Param.m_sWordSize   = nTempLen; 
				obj_Word_Param.m_nSentenceID = nSentenceID;
				objWordList.push_back(obj_Word_Param);
			}
		}
	}
}
