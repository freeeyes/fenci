#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

const double MIN_DOUBLE = -3.14e+100;
const double MAX_DOUBLE = 3.14e+100;

#define MAX_WORD_SIZE   50
#define MAX_SPEECH_SIZE 5 

//词性 前置词 完整词 单字
enum
{
	NONE_WORD = 0,
	PRE_WORD,
	FULL_WORD,
	FULL_RUNE,
};

//字词解析结构信息
struct _Word_Param
{
	char  m_szWord[MAX_WORD_SIZE];           //词内容
	int   m_nWordRote;                       //当前IDF值
	char  m_szWordSpeech[MAX_SPEECH_SIZE];   //当前词性(名词，动词，形容词)
	char  m_cType;                           //当前词状态(单字，前置词，完整词)
	short m_sWordSize;                       //当前词的长度(一个汉字算一个)
	int   m_nSentenceID;                     //词所在当前句子的ID 
	
	_Word_Param()
	{
		m_szWord[0]       = '\0';
		m_nWordRote       = 0;
		m_szWordSpeech[0] = '\0';
		m_cType           = NONE_WORD;
		m_sWordSize       = 0;
		m_nSentenceID     = 0;
	}	
};

inline void Split(const string& src, vector<string>& res, const string& pattern, size_t maxsplit = string::npos) {
  res.clear();
  size_t Start = 0;
  size_t end = 0;
  string sub;
  while(Start < src.size()) 
  {
    end = src.find_first_of(pattern, Start);
    if(string::npos == end || res.size() >= maxsplit) 
    {
      sub = src.substr(Start);
      res.push_back(sub);
      return;
    }
    sub = src.substr(Start, end - Start);
    res.push_back(sub);
    Start = end + 1;
  }
  return;
}

inline bool StartsWith(const string& str, const string& prefix) {
  if(prefix.length() > str.length()) {
    return false;
  }
  return 0 == str.compare(0, prefix.length(), prefix);
}

#endif
