#include "WordDict.h"
#include "ShareMemory.h"
#include "TextRank.h"

//g++ -o Test runepool.cpp nodepool.cpp wordbase.cpp main.cpp
int main()
{
	//测试词典前置词加载方式
	CWordDict objWordDict;
	
	int nPoolSize = 600000;
	shm_key obj_key = 30001;
	shm_id obj_shm_id;
	bool blCreate = true;	
	
	size_t stDict = objWordDict.Get_Mem_Size(nPoolSize);
	printf("[Main]stDict=%d.\n", stDict);
	char* pData = Open_Share_Memory_API(obj_key, stDict, obj_shm_id, blCreate);
	
	//char* pData = new char[stDict];
	if(NULL != pData)
	{
		if(blCreate == true)
		{	
			memset(pData, 0, stDict);
			printf("[main]ShareMemory first create.\n");
			objWordDict.Init("worddict.txt", "hmm_model.utf8", pData);
		}
		else
		{
			printf("[main]ShareMemory load.\n");
			objWordDict.Load(pData);
		}
	}
	
	vector<_Word_Param> objvecWord;
	
	char szSentence[500] = {'\0'};
	sprintf(szSentence, "漂浮在算法的海洋。寻找那一瞬间的存在。不畏惧惊涛骇浪。");
	printf("[Main]src=(%s).\n", szSentence);
	
	//算法中，已有词不用HMM，出现连续的字，用HMM补全
	int nHMMCount = 0;
	int nHMMSize  = 0;
	char szHMMSentence[500] = {'\0'};
	
	objvecWord.clear();
	objWordDict.Cut(szSentence, objvecWord, SELECT_WORD);
	for(int i = 0; i < objvecWord.size(); i++)
	{
		if(i != objvecWord.size() - 1)
		{
			if(objvecWord[i].m_cType == FULL_RUNE)
			{
				printf("[Rune]<Len=%d,S=%d,Word=%s>\n",
					objvecWord[i].m_sWordSize, 
					objvecWord[i].m_nSentenceID,				 
					objvecWord[i].m_szWord);
			}
			else if(objvecWord[i].m_cType == FULL_WORD)
			{
				printf("[FW]<Speech=%s,Len=%d,S=%d,Word=%s>\n", 
					objvecWord[i].m_szWordSpeech, 
					objvecWord[i].m_sWordSize, 
					objvecWord[i].m_nSentenceID,
					objvecWord[i].m_szWord);
			}
			else
			{
				printf("[W]<Speech=%s,Len=%d,S=%d,Word=%s>\n", 
					objvecWord[i].m_szWordSpeech, 
					objvecWord[i].m_sWordSize, 
					objvecWord[i].m_nSentenceID,
					objvecWord[i].m_szWord);
			}
		}
		else
		{
			if(objvecWord[i].m_cType == FULL_RUNE)
			{
				printf("[Rune]<Len=%d,S=%d,Word=%s>\n",
						objvecWord[i].m_sWordSize, 
						objvecWord[i].m_nSentenceID,				 
						objvecWord[i].m_szWord);					
			}
			else if(objvecWord[i].m_cType == FULL_WORD)
			{
				printf("[FW]<Speech=%s,Len=%d,S=%d,Word=%s>\n", 
					objvecWord[i].m_szWordSpeech, 
					objvecWord[i].m_sWordSize, 
					objvecWord[i].m_nSentenceID,
					objvecWord[i].m_szWord);
			}
			else
			{
				printf("[W]<Speech=%s,Rote=%d,Word=%s>\n", 
					objvecWord[i].m_szWordSpeech, 
					objvecWord[i].m_nWordRote, 
					objvecWord[i].m_szWord);				
			}
		}
	}		
	
	//测试提炼摘要
	CTextRank objTextRank;
	vector<_Word> vecKeysList;
	objTextRank.Rank(objvecWord, vecKeysList, 3);
	
	printf("//========================\n");
	for(int i = 0; i < vecKeysList.size(); i++)
	{
		printf("[CTextRank::Rank]<%s == %f>.\n", vecKeysList[i].word.c_str(), vecKeysList[i].weight);
	}	
		
	return 0;
}
