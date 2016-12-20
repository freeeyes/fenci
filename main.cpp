#include "wordbase.h"

//g++ -o Test runepool.cpp nodepool.cpp wordbase.cpp main.cpp
int main()
{
	CWordBase objWordBase;
	
	objWordBase.Init("./word.txt", 1000);
	
	vector<string> objvecWord;
	objWordBase.Cut_Word("哪里见过你呀,朋友", objvecWord);
	
	printf("[Cut_Word]Word size=%d.\n", objvecWord.size());
	for(int i = 0; i < objvecWord.size(); i++)
	{
		printf("[Cut_Word]Word=%s.\n", objvecWord[i].c_str());
	}
	
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
		
	return 0;
}
