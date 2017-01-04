#include "HMMDict.h"

CHmmDict::CHmmDict()
{
	
}

CHmmDict::~CHmmDict()
{
	
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

bool CHmmDict::LoadHashProb(const string& strLine, CHashTable& hbProp) 
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
      printf("[CHmmDict::LoadHashProb]pool is full.\n");
      return false;    	
    }
    
    memcpy(pRuneHMMInfo->m_szRune, (char* )objRuneList[0].c_str(), objRuneList[0].length());
    pRuneHMMInfo->m_dbProp = atof(objRuneList[1].c_str()); 
    
    int nOffSetPos = m_objRuneHMMPool.Get_Node_Offset(pRuneHMMInfo);
    
    hbProp.Add_Hash_Data(pRuneHMMInfo->m_szRune, nOffSetPos);
  }
  
  return true;
}

bool CHmmDict::Init(const char* pFile, char* pData)
{
	string strLine;
	
	//初始化内存池
	size_t stSize = 0;
	stSize = m_objRuneHMMPool.Init(HMM_DICT_POOL_SIZE, pData);
	printf("[CHmmDict::Init]m_objRuneHMMPool=%d.\n", (int)stSize);
	pData += stSize;
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
			 			m_dbTransProb[i][j]  = atof(objTempAttrList[i].c_str());
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
		printf("[CHmmDict::Init]m_hashMapB count=%d.\n", m_hashMapB.Get_Used_Count());
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
		printf("[CHmmDict::Init]m_hashMapE count=%d.\n", m_hashMapE.Get_Used_Count());
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
		printf("[CHmmDict::Init]m_hashMapM count=%d.\n", m_hashMapM.Get_Used_Count());
	}
	else
	{
		printf("[CHmmDict::Init]m_hashMapM read error.\n");
		return false;		
	}	
	
	//读取每个文字的M概率
	if(true == Read_File_Line(ifs, strLine))
	{
		LoadHashProb(strLine, m_hashMapS);
		printf("[CHmmDict::Init]m_hashMapS count=%d.\n", m_hashMapS.Get_Used_Count());
	}
	else
	{
		printf("[CHmmDict::Init]m_hashMapS read error.\n");
		return false;		
	}	
	
	return true;
}

