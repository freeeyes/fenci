#ifndef _TEXTRANK_H
#define _TEXTRANK_H

#include "Graph.h"
#include <algorithm>  

//ʵ��textRank�㷨
//add by freeeyes
#include <map>

using namespace std;

class CTextRank
{
public:
	CTextRank();
	~CTextRank();
	
	bool Rank(vector<_Word_Param> objvecWord, vector<_Word>& vecWordList, int nTopCount = 10);
	
private:
	void Filter_Word(vector<_Word_Param>& vecWordList, vector<_Word_Param>& objtagList);
	
private:
	struct _Dict_Info
	{
		char szBeginWord[MAX_WORD_SIZE];
		char szEndWord[MAX_WORD_SIZE];
		int nWeigth;
		
		_Dict_Info()
		{
			szBeginWord[0] = '\0';
			szEndWord[0]   = '\0';
			nWeigth        = 0;
		}
	};
	
	bool Cut_Dict(vector<_Word_Param>& objvecWord);
	
private:
	typedef map<string, _Dict_Info> mapDictInfo;
	mapDictInfo	m_mapDict;    //�����ʴʵ�
	WordGraph   m_WordGraph;  //�����ʾ���
};

#endif
