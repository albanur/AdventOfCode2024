#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<map>
#include<algorithm>

using namespace std;

int next(int current, int skip)
{
    current++;
    return current == skip ? current+1 : current;
}

bool doesSafelyIncrease(vector<int>& p, int skip)
{
    int i = next(-1, skip);
    
    for(int j = next(i, skip); j < p.size(); i=j, j=next(j, skip))
    {
        int diff = p[j]-p[i];
        if(diff < 1 || diff > 3)
        {
            return false;
        }
    }
    return true;
}

bool doesSafelyDecrease(vector<int>& p, int skip)
{
    int i = next(-1, skip);
    
    for(int j = next(i, skip); j < p.size(); i=j, j=next(j, skip))
    {
        int diff = p[i]-p[j];
        if(diff < 1 || diff > 3)
        {
            return false;
        }
    }
    return true;
}

bool isSafe(vector<int>& p)
{
    int skip = -1;
    {
        if(doesSafelyDecrease(p, skip) || doesSafelyIncrease(p, skip))
        {
            return true;
        }
    }

    return false;
}

bool isSafe2(vector<int>& p)
{
    for(int skip = -1; skip < (int)p.size(); skip++)
    {
        if(doesSafelyDecrease(p, skip) || doesSafelyIncrease(p, skip))
        {
            return true;
        }
    }

    return false;
}

int main()
{
    char buffer[256];
    int sum = 0;
    int sum2 = 0;
    vector<int> p;

    while(fgets(buffer, 256, stdin) != NULL)
    {
        if(strlen(buffer) <= 1)
        {
            continue;
        }

        p.clear();
        char* token = strtok(buffer, " ");
        while (token != NULL)
        {
            int value = atoi(token);
            p.push_back(value);
            token = strtok(NULL, " ");
        }

        sum += isSafe(p) ? 1 : 0;
        sum2 += isSafe2(p) ? 1 : 0;
    }

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}