#ifndef _WORDDICT_H
#define _WORDDICT_H

#include <fstream>
#include "HMMDict.h"

//add by freeeyes
//用直接的hash替换tire树的方法，节省字典空间，效率一样

//词或者字的切分规则
enum
{
	SELECT_RUNE = 0,
	SELECT_WORD,
};

//词语相关信息
struct _Word_Info
{
	char  m_szWord[MAX_WORD_SIZE];
	int   m_nWordRote;
	char  m_szWordSpeech[MAX_SPEECH_SIZE];
	char  m_cType;
	int   m_nIndex;
	char  m_cUsed;                //0未使用 1使用
	
	_Word_Info()
	{
		Init();
	}
	
	void Init()
	{
		memset(m_szWord, 0, MAX_WORD_SIZE);
		memset(m_szWordSpeech, 0, MAX_SPEECH_SIZE);
		m_nWordRote = 0;
		m_nIndex    = 0;
		m_cUsed     = 0;	
		m_cType     = NONE_WORD;	
	}
	
	void Set_Index(int nIndex)
	{
		m_nIndex = nIndex;
	}
	
	int Get_Index()
	{
		return m_nIndex;
	}	
};

class CWordInfoPool
{
public:
	CWordInfoPool();
	~CWordInfoPool();
	
	size_t Init(int nPoolCount, char* pData);
	size_t Load(int nPoolCount, char* pData);
	void   Close();
	
	char* GetCryptTable();
	
	_Word_Info* Create();
	int Get_Node_Offset(_Word_Info* pWordInfo);
	_Word_Info* Get_NodeOffset_Ptr(int nOffset);
	bool Delete(_Word_Info* pWordInfo);
	
private:
	void prepareCryptTable();	
	
private:
	char*          m_pCryptTable;
	char*          m_pBase;           //整个内存开始地址
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Word_Info*    m_WordInfoList;    //内存池列表			
};

class CWordDict
{
public:
	CWordDict();
	~CWordDict();
	
	size_t Get_Mem_Size(int nPoolSize);
	bool Init(const char* pFile, const char* pHMMFile, char* pData);
	bool Load(char* pData);
	
	int Cut(const char* pSentence, vector<_Word_Param>& vecWord, int nType = SELECT_RUNE);
	int Cut_Rune(const char* pSentence, vector<_Word_Param>& vecWord, int nSentenceID = 1);
	bool Add_Word(const char* pWord, int nRote, const char* pWordSpeech);
	
private:
	bool Read_File_Line(ifstream& ifs, string& strLine);
	bool Set_Dict_Hash_Table(vector<string>& objTempAttrList);
	void Get_Sentence_ID(const char* pWord, int& nSentenceID);
	
private:
	CWordInfoPool m_WordInfoPool;
	int           m_nPoolSize;
	CHashTable    m_hashDict;
	CHmmDict      m_objHmmDict;
};

#endif
