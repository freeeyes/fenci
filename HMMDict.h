#ifndef _HMMDICT_H
#define _HMMDICT_H

#include <fstream>
#include "Common.h"
#include "HashTable.h"
#include "RuneHMMInfo.h"
#include "runepool.h"
#include <string>

using namespace std;

//HMM字典,以及HMM的实现
//add by freeeyes

#define HMM_DICT_POOL_SIZE 60000
#define HMM_HASH_SIZE      15000

class CHmmDict
{
public:
	CHmmDict();
	~CHmmDict();
	
	size_t Get_Mem_Size();
	bool Init(const char* pFile, char* pData);
	bool Load(char* pData);
	
	void Cut(const char* pData, int nLen, int nSentenceID, vector<_Word_Param>& objWordList);
	
private:
	bool Read_File_Line(ifstream& ifs, string& strLine);
	bool LoadHashProb(const string& strLine, CHashTable& hbProb);
	
	void Viterbi(const char* pData, int nLen, vector<_Rune>& objRuneList, vector<short>& objResList);
	void Sentance_To_Rune(const char* pData, int nLen, vector<_Rune>& objRuneList);
	double Get_Rune_Prob(_Rune objRune, CHashTable& objhashMap);
	
private:
	CRuneHMMPool m_objRuneHMMPool;
	double* m_dbStart;
	double* m_dbTransProb;
	CHashTable m_hashMapB;
	CHashTable m_hashMapE;
	CHashTable m_hashMapM;
	CHashTable m_hashMapS;
};

#endif
