#include "wordbase.h"
#include "HMMDict.h"
#include "WordDict.h"
#include "ShareMemory.h"

//g++ -o Test runepool.cpp nodepool.cpp wordbase.cpp main.cpp
int main()
{
	//测试词典前置词加载方式
	CWordDict objWordDict;
	int nPoolSize = 1000000;
	shm_key obj_key = 30001;
	shm_id obj_shm_id;
	bool blCreate = true;	
	
	size_t stDict = objWordDict.Get_Mem_Size(nPoolSize);
	printf("[Main]stDict=%d.\n", stDict);
	//char* pData = Open_Share_Memory_API(obj_key, stDict, obj_shm_id, blCreate);
	
	char* pData = new char[stDict];
	memset(pData, 0, stDict);
	if(NULL != pData)
	{
		if(blCreate == true)
		{	
			objWordDict.Init("worddict.txt", pData);
		}
		else
		{
			objWordDict.Load(pData);
		}
	}
	
	/*
	//Tire树
	CWordBase objWordBase;
	
	int nPoolSize = 100000;
	size_t stPoolSize = objWordBase.Get_Mem_Size(nPoolSize);
	
	printf("[Main]stPoolSize=%d.\n", stPoolSize);
	
	//使用共享内存创建
	int nNodeCount = 100;
	shm_key obj_key = 30001;
	shm_id obj_shm_id;
	bool blCreate = true;
	//char* pData = Open_Share_Memory_API(obj_key, stPoolSize, obj_shm_id, blCreate);
	char* pData  = new char[stPoolSize];
	if(NULL != pData)
	{
		if(blCreate == true)
		{
			//第一次加载
			printf("[main]ShareMemory first create.\n");
			objWordBase.Init("./word.txt", nNodeCount, pData);
		}
		else
		{
			//直接加载共享内存
			printf("[main]ShareMemory load.\n");
			objWordBase.Load(nNodeCount, pData);
		}
	}
	
	vector<string> objvecWord;
	
	objvecWord.clear();
	
	objWordBase.Cut("哪里见过你呀,朋友", objvecWord);
	printf("[Cut]");
	for(int i = 0; i < objvecWord.size(); i++)
	{
		if(i != objvecWord.size() - 1)
		{
			printf("%s/", objvecWord[i].c_str());
		}
		else
		{
			printf("%s/\n", objvecWord[i].c_str());
		}
	}
	
	objWordBase.Add_Word("朋友");

  objvecWord.clear();

  objWordBase.Cut("哪里见过你呀,朋友", objvecWord);
  printf("[Cut]");
  for(int i = 0; i < objvecWord.size(); i++)
  {
    if(i != objvecWord.size() - 1)
    {
    	printf("%s/", objvecWord[i].c_str());
    }
    else
    {
    	printf("%s\n", objvecWord[i].c_str());
    }
  }	
  */
  
  /*
  //测试HMM加载
  CHmmDict objHmmDict;
  size_t stPoolSize = objHmmDict.Get_Mem_Size();
 
	shm_key obj_key = 30001;
	shm_id obj_shm_id;
	bool blCreate = true;
	char* pData = Open_Share_Memory_API(obj_key, stPoolSize, obj_shm_id, blCreate);
	  
  //char* pData = new char[stPoolSize];
  printf("[HMM]size=%d.\n", (int)stPoolSize);
  
  if(NULL != pData)
	{
		if(blCreate == true)
		{
  		objHmmDict.Init("hmm_model.utf8", pData);
		}  
  	else
  	{
  		objHmmDict.Load(pData);
  	}
  }
  else
  {
  	printf("[HMM]Create memory fail.\n");
  }
  
  //测试切分句子
  char szTestSentence[100]  = {'\0'};
  sprintf(szTestSentence, "哪里见过你朋友");
  int nLen = strlen(szTestSentence);
  
  vector<string> objWordList;
  objHmmDict.Cut(szTestSentence, nLen, objWordList);
  for(int i = 0; i < objWordList.size(); i++)
  {
    if(i != objWordList.size() - 1)
    {
    	printf("%s/", objWordList[i].c_str());
    }
    else
    {
    	printf("%s/\n", objWordList[i].c_str());
    }
  }	
  */  
  
 	//delete[] pData;		
	return 0;
}
