#include <assert.h>
#include <stdbool.h>
#include <time.h>
/** 
* Function returns an array of 5 elements.
* Those 5 elements are created randomly in the range 1 - 49.
* Numbers can't repeat.
*/
int* Lotto_drawing()
{
    
	    int n=5,i;
    static int arr[50]={0};
  time_t t;
  srand((unsigned)time(&t));
  for(i=0;i<n;++i)
  {
    int r=rand()%48+1;
    if(!arr[r])
    {
    arr[i] = r;
	//printf("%d\n",arr[i]);
    }
    else
      i--;
    arr[r]=1;    
  }
return arr;
}

/* Please create test cases for this program. test_cases() function can return void, bool or int. */
bool test_cases()
{
	int* answer = Lotto_drawing();
    int i,n=5;
    bool judgement = true;
    for (i = 0; i < n; i++)
    {
        if(answer[i]< 1 || answer[i] > 49) judgement = false;
    }
	assert(judgement == true);

    int j;
    judgement = true;
    for(i = 0; i < n; i++)
    {
        for(j =0; j<n; j++)
        {
            if (answer[i] == answer[j] && i != j) judgement = false;
        }
    }
	assert(judgement == true);

}


int main()
{

test_cases();
}
