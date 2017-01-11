#include "TextRank.h"

CTextRank::CTextRank()
{
}

CTextRank::~CTextRank()
{
}

bool CTextRank::Cut_Dict(vector<_Word_Param>& objvecWord)
{
	//����һ�������а��������й�����
	for(int i = 0; i < objvecWord.size(); i++)
	{
		//���˵���
		if(objvecWord[i].m_sWordSize <= 1)
		{
			continue;
		}
		
		for(int j = i + 1; j < objvecWord.size(); j++)
		{
			if(objvecWord[j].m_sWordSize <= 1)
			{
				continue;
			}
			
			//�������ʼ�¼���ֵ���
			string strkey = (string)objvecWord[i].m_szWord + (string)objvecWord[j].m_szWord;
			mapDictInfo::iterator f = m_mapDict.find(strkey);
			if(f != m_mapDict.end())
			{
				//�ڵ�ǰ�ֵ���
				_Dict_Info& obj_Dict_Info = (_Dict_Info& )f->second;
				obj_Dict_Info.nWeigth++;
			}
			else
			{
				//���ڵ�ǰ�ֵ��У�����¹�����
				_Dict_Info obj_Dict_Info;
				sprintf(obj_Dict_Info.szBeginWord, "%s", objvecWord[i].m_szWord);
				sprintf(obj_Dict_Info.szEndWord, "%s", objvecWord[j].m_szWord);
				obj_Dict_Info.nWeigth = 1;
				m_mapDict.insert(mapDictInfo::value_type(strkey, obj_Dict_Info));
			}
		}
	}
	
	return true;
}

bool CTextRank::Rank(vector<_Word_Param> objvecWord, vector<_Word>& vecWordList, int nTopCount)
{
	int nPos = 0;
	int nSentenceID = 1;
	
	m_mapDict.clear();
	vecWordList.clear();
	
	vector<_Word_Param> objvecSent;
	objvecSent.clear();
	
	//���ɹ����ʴʵ�
	for(int i = 0; i < (int)objvecWord.size(); i++)
	{
		/*
		printf("[CTextRank::Rank](%s)m_nSentenceID=%d,nSentenceID=%d.\n", 
			objvecWord[i].m_szWord,
			objvecWord[i].m_nSentenceID,
			nSentenceID);
		*/
		if(objvecWord[i].m_nSentenceID == nSentenceID)
		{
			objvecSent.push_back(objvecWord[i]);
		}
		else
		{
			Cut_Dict(objvecSent);
			objvecSent.clear();
			nSentenceID++;
			i--;
		}
	}
	//���һ��
	Cut_Dict(objvecSent);
	
	//����������еı�
	for(mapDictInfo::iterator b = m_mapDict.begin(); b != m_mapDict.end(); b++)
	{
		_Dict_Info& obj_Dict_Info = (_Dict_Info& )b->second;
		/*
		printf("[CTextRank::Rank]%s --> %s (%d).\n", 
						obj_Dict_Info.szBeginWord, 
						obj_Dict_Info.szEndWord,
						obj_Dict_Info.nWeigth);
		*/
		m_WordGraph.addEdge((string)obj_Dict_Info.szBeginWord, 
												(string)obj_Dict_Info.szEndWord,
												obj_Dict_Info.nWeigth);
	}
	
	//���㹲���ʾ���
	WordMap objWordMap;
	
	//����ժҪ
	m_WordGraph.rank(objWordMap, nTopCount);
	
	for(WordMap::iterator e = objWordMap.begin(); e != objWordMap.end(); e++)
	{
		//_Word& obj_Word = (_Word& )e->second;
		//printf("[CTextRank::Rank]<%s == %f>.\n", obj_Word.word.c_str(), obj_Word.weight);
		vecWordList.push_back(e->second);
	}
	
	//����
	std::sort(vecWordList.begin(), vecWordList.end(), SortByWord);
	vecWordList.resize(nTopCount);

	return true;
}
