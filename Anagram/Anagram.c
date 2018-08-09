#include <assert.h>
#include <string.h>
#include <stdbool.h>

/** 
* Function returns true if word_1 and word_2 are anagrams to each other. Otherwise false.
* Case sensitivity doesn't matter.
*/
bool check_if_anagram(const char* word_1, int length_1, const char* word_2, int length_2)
{
	int i, j, flag;
	if (length_1 == 0 || length_2 == 0) return false;
	//Case where length of the first word > length of the second word
	if (length_1 >= length_2){
		for (i = 0; i < length_1; i++)
		{
			if (word_1[i] == ' ') i++;
			for (j = 0; j < length_2; j++)
			{
				if (tolower(word_1[i]) == tolower(word_2[j])) flag = 1;					
			}
		if (flag == 0) return false; 
		flag = 0;
		}
	return true;
	}
		//Case where length of the first word > length of the second word
	else
	{
		for (i = 0; i < length_2; i++)
		{
			if (word_2[i] == ' ') i++;
			for (j = 0; j < length_1; j++)
			{
				if (word_2[i] == word_1[j]) flag = 1;					
			}
		if (flag == 0) return false; 
		flag = 0;
		}
	}
}

void test_cases()
{
	bool answer = check_if_anagram("LordVader", strlen("LordVader"), "VaderLord", strlen("VaderLord"));
	assert(answer == true);

	answer = check_if_anagram("silent", strlen("silent"), "listen", strlen("listen"));
	assert(answer == true);

	answer = check_if_anagram("funeral", strlen("funeral"), "real fun", strlen("real fun"));
	assert(answer == true);

	answer = check_if_anagram("Elon Musk", strlen("Elon Musk"), "Muskmelon", strlen("Muskmelon"));
	assert(answer == true);

	answer = check_if_anagram("Tieto", strlen("Tieto"), "Tietonator", strlen("Tietonator"));
	assert(answer == false);

	answer = check_if_anagram("Football", strlen("Football"), "Basketball", strlen("Basketball"));
	assert(answer == false);

	answer = check_if_anagram("F", strlen("F"), "", strlen(""));
	assert(answer == false);
}

int main()
{
	test_cases();
}