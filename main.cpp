#include "wordbase.h"
#include "ShareMemory.h"

//g++ -o Test runepool.cpp nodepool.cpp wordbase.cpp main.cpp
int main()
{
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
	/*
	objWordBase.Cut_Word("哪里见过你呀,朋友", objvecWord);
	
	printf("[Cut_Word]Word size=%d.\n", objvecWord.size());
	for(int i = 0; i < objvecWord.size(); i++)
	{
		printf("[Cut_Word]Word=%s.\n", objvecWord[i].c_str());
	}
	*/
	
	objvecWord.clear();
	
	objWordBase.Cut("09azAZ哪里见过你呀,朋友", objvecWord);
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
	
	/*
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
	int u2PoolSize = 10000;
	CHashTable<int> objHashTable;
	
	printf("[Main]1 Size=%d.\n", sizeof(objHashTable));
	
	size_t nArraySize = sizeof(_Hash_Table_Cell<int>) * u2PoolSize;
	char* pHashBase = new char[nArraySize];
	
	objHashTable.Init(pHashBase, u2PoolSize, NULL);
	
	printf("[Main]2 Size=%d nArraySize=%d.\n", sizeof(objHashTable), nArraySize);
	
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;
	
	printf("[main]a=08%08x, b=0x%08x, c=0x%08x.\n", &a, &b, &c);
	
	int nPos = 0;
	nPos = objHashTable.Add_Hash_Data("1", &a);
	printf("[Main]1 Pos=%d.\n", nPos);
	nPos = objHashTable.Add_Hash_Data("2", &b);
	printf("[Main]2 Pos=%d.\n", nPos);
	nPos = objHashTable.Add_Hash_Data("3", &c);
	printf("[Main]3 Pos=%d.\n", nPos);
	
	int* pT = objHashTable.Get_Hash_Box_Data("3");
	printf("[Main]key=3, value=%d,Addr=0x%08x.\n", *pT, pT);
	pT = objHashTable.Get_Hash_Box_Data("2");
	printf("[Main]key=2, value=%d,Addr=0x%08x.\n", *pT, pT);
	pT = objHashTable.Get_Hash_Box_Data("1");
	printf("[Main]key=1, value=%d,Addr=0x%08x.\n", *pT, pT);
	
	objHashTable.Del_Hash_Data("2");
	objHashTable.Add_Hash_Data("4", &d);
	pT = objHashTable.Get_Hash_Box_Data("4");
	printf("[Main]key=4, value=%d,Addr=0x%08x.\n", *pT, pT);	
	*/
	
	/*
	int nPoolSize = 1000;
	size_t stPoolSize = 1280 + sizeof(_RuneLinkNode)*nPoolSize 
								+ sizeof(_Hash_Table_Cell<_RuneLinkNode>) * MAIN_DICT_MAP_COUNT
								+ (sizeof(_Hash_Table_Cell<_RuneLinkNode>) * CHILD_DICT_MAP_COUNT) * (nPoolSize - 1);
	
	printf("[Main]stPoolSize=%d.\n", stPoolSize);
	char* pData = new char[stPoolSize];
	memset(pData, 0, stPoolSize);
	
	CNodePool objNodePool;
	size_t nSize = objNodePool.Init(nPoolSize, pData);
	printf("[Main]curr stPoolSize=%d.\n", nSize);
	
	delete[] pData;
	*/
		
	return 0;
}
