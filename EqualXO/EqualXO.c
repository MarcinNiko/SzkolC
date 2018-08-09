#include <string.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

/**
* Function checks if a string has the same amount of 'x' and 'o's. Function must be case insensitive.
*/
bool XO(const char *word, const int length)
{
	int i, diffrence;
    diffrence = 0;
for (i = 0; i < length; i++)
{
	// o's increment diffrence
	if ((char)tolower(word[i]) == 'o') diffrence++;
	//x's decrement diffrence
	if ((char)tolower(word[i]) == 'x') diffrence--;
}
	if (diffrence == 0) return true;
	return false;
}

void test_cases()
{
	bool answer = XO("ooxx", strlen("ooxx"));
	assert(answer == true);

	answer = XO("xooxx", strlen("xooxx"));
	assert(answer == false);

	answer = XO("ooxXm", strlen("ooxXm"));
	assert(answer == true);

	answer = XO("zzoo", strlen("zzoo"));
	assert(answer == false);
}

int main(int argc, char *argv[])
{
	test_cases();
	return 0;
}