#include <string.h>
#include <assert.h>

/**
* Function duplicates letters based on their index. Each index means how many times the letter needs to be duplicated.
* See test cases for examples.
*/
void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

char* accumulate(const char *word, const int length)
{
	const char dash = '-';
char* p;
p = (char*)malloc(length*length*sizeof(char) + (length+1));
int i,j;
for (i =0; i<length; i++)
{
    for (j=0;j<i+1;j++)
    {
		if(j == 0){
		append(p,toupper(word[i]));
		}
		else{
		append(p,tolower(word[i]));
		}
		
    }
	if (i != length-1) append(p,dash);
}
return p;
}


void test_cases()
{
	char* result = accumulate("abcd", strlen("abcd"));
	assert(strcmp(result, "A-Bb-Ccc-Dddd") == 0);

	result = accumulate("cwAt", strlen("cwAt"));
	assert(strcmp(result, "C-Ww-Aaa-Tttt") == 0);
}

int main(int argc, char *argv[])
{
	test_cases();
	return 0;
}