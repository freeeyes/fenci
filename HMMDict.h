#ifndef _HMMDICT_H
#define _HMMDICT_H

#include <fstream>
#include "Common.h"
#include "HashTable.h"
#include "RuneHMMInfo.h"
#include <string>

using namespace std;

//HMM字典,以及HMM的实现
//add by freeeyes

#define HMM_DICT_POOL_SIZE 120000
#define HMM_HASH_SIZE      30000

class CHmmDict
{
public:
	CHmmDict();
	~CHmmDict();
	
	bool Init(const char* pFile, char* pData);
	
private:
	bool Read_File_Line(ifstream& ifs, string& strLine);
	bool LoadHashProb(const string& strLine, CHashTable& hbProp);
	
private:
	CRuneHMMPool m_objRuneHMMPool;
	double m_dbStart[RUNE_POS_ALL];
	double m_dbTransProb[RUNE_POS_ALL][RUNE_POS_ALL];
	CHashTable m_hashMapB;
	CHashTable m_hashMapE;
	CHashTable m_hashMapM;
	CHashTable m_hashMapS;
};

#endif
