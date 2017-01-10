#ifndef _WORDDICT_H
#define _WORDDICT_H

#include <fstream>
#include "Common.h"
#include "runepool.h"
#include "HashTable.h"

//add by freeeyes
//��ֱ�ӵ�hash�滻tire���ķ�������ʡ�ֵ�ռ䣬Ч��һ��

#define MAX_WORD_SIZE   50
#define MAX_SPEECH_SIZE 5 

//���� ǰ�ô� ������ ����
enum
{
	NONE_WORD = 0,
	PRE_WORD,
	FULL_WORD,
	FULL_RUNE,
};

//�ʻ����ֵ��зֹ���
enum
{
	SELECT_RUNE = 0,
	SELECT_WORD,
};

//�ִʽ����ṹ��Ϣ
struct _Word_Param
{
	char  m_szWord[MAX_WORD_SIZE];
	int   m_nWordRote;
	char  m_szWordSpeech[MAX_SPEECH_SIZE];
	char  m_cType;
	
	_Word_Param()
	{
		m_szWord[0]       = '\0';
		m_nWordRote       = 0;
		m_szWordSpeech[0] = '\0';
		m_cType           = NONE_WORD;
	}	
};

//���������Ϣ
struct _Word_Info
{
	char  m_szWord[MAX_WORD_SIZE];
	int   m_nWordRote;
	char  m_szWordSpeech[MAX_SPEECH_SIZE];
	char  m_cType;
	int   m_nIndex;
	char  m_cUsed;                //0δʹ�� 1ʹ��
	
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
	char*          m_pBase;           //�����ڴ濪ʼ��ַ
	int            m_nPoolCount;
	int            m_nCurrIndex;
	_Word_Info*    m_WordInfoList;    //�ڴ���б�			
};

class CWordDict
{
public:
	CWordDict();
	~CWordDict();
	
	size_t Get_Mem_Size(int nPoolSize);
	bool Init(const char* pFile, char* pData);
	bool Load(char* pData);
	
	int Cut(const char* pSentence, vector<_Word_Param>& vecWord, int nType = SELECT_RUNE);
	int Cut_Rune(const char* pSentence, vector<_Word_Param>& vecWord);
	
private:
	bool Read_File_Line(ifstream& ifs, string& strLine);
	bool Set_Dict_Hash_Table(vector<string>& objTempAttrList);
	
private:
	CWordInfoPool m_WordInfoPool;
	int           m_nPoolSize;
	CHashTable    m_hashDict;
};

#endif
