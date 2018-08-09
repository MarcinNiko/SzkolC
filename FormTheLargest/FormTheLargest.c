#include <assert.h>
#include <math.h>

/**
* Given a number, return the maximum number that could be formed with digits of the number given.
* For example: number = 7389, return 9873
*/
int form_the_largest_number(int number)
{
	int count_digits = 0;
	
	int num = number;
	while (num != 0)
	{
		num /= 10;
		count_digits++;
	}

	char a[count_digits+1];

	sprintf(a,"%d",number);

	int i, j,output;
	char tmp;
	
		for (i = 0; i < strlen(a); i++)    
	{
		for (j = 0; j < strlen(a)-i-1; j++)          
		{
			if (a[j] < a[j+1])           
			{
				tmp = a[j+1];        
				a[j+1] = a[j];           
				a[j] = tmp;             
			}  
		}
	}
	output = atoi(a);
	
	return output;
}

void test_cases()
{
	int result = form_the_largest_number(213);
	assert(result == 321);

	result = form_the_largest_number(7389);
	assert(result == 9873);

	result = form_the_largest_number(63729);
	assert(result == 97632);

	result = form_the_largest_number(566797);
	assert(result == 977665);
}

int main()
{
	test_cases();
}
