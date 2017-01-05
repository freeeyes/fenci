#include "wordbase.h"
#include "HMMDict.h"
#include "ShareMemory.h"

//g++ -o Test runepool.cpp nodepool.cpp wordbase.cpp main.cpp
int main()
{
	/*
	CWordBase objWordBase;
	
	int nPoolSize = 100;
	size_t stPoolSize = 1280 + sizeof(int) + sizeof(_RuneLinkNode)*nPoolSize 
								+ sizeof(_Hash_Table_Cell) * MAIN_DICT_MAP_COUNT + sizeof(_Hash_Table_Cell) * CHILD_DICT_MAP_COUNT * MAIN_DICT_MAP_COUNT
								+ (sizeof(_Hash_Table_Cell) * END_DICT_MAP_COUNT) * (nPoolSize - MAIN_DICT_MAP_COUNT - 1);	
	
	
	printf("[Main]stPoolSize=%d.\n", stPoolSize);
	
	//使用共享内存创建
	int nNodeCount = 100;
	shm_key obj_key = 30001;
	shm_id obj_shm_id;
	bool blCreate = true;
	char* pData = Open_Share_Memory_API(obj_key, stPoolSize, obj_shm_id, blCreate);
	//char* pData  = new char[stPoolSize];
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
  
  //测试HMM加载
  CHmmDict objHmmDict;
  size_t stPoolSize = HMM_DICT_POOL_SIZE * sizeof(_RuneHMMInfo) + HMM_HASH_SIZE * sizeof(_Hash_Table_Cell) * 4;
  stPoolSize += 1280;
 
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
  sprintf(szTestSentence, "哪里见过你呀,朋友");
  int nLen = strlen(szTestSentence);
  
  vector<string> objWordList;
  objHmmDict.Cut(szTestSentence, nLen, objWordList);
  //delete[] pData;
		
	return 0;
}
