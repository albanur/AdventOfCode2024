#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include<regex>

using namespace std;

class KeyLockMatcher
{
private:
    const int Height = 5;
    const int Widht = 5;
    vector<vector<int>> keys;
    vector<vector<int>> locks;

public:
    KeyLockMatcher() {}

    void Read(FILE* file)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];

        while(fgets(buffer, bufferSize, stdin) != NULL)
        {
            if(strlen(buffer) < 2) { continue; }

            bool isKey = (buffer[0] == '.');
            vector<int> item(Widht, 0);

            for(int i = 0; i < Height; i++)
            {
                fgets(buffer, bufferSize, stdin);
                for(int j = 0; j < Widht; j++)
                {
                    item[j] += (buffer[j] == '#') ? 1 : 0;
                }
            }

            fgets(buffer, bufferSize, stdin);

            if(isKey)
            {
                keys.push_back(item);
            }
            else
            {
                locks.push_back(item);
            }
        }
    }

    int64_t CountMatches()
    {
        int64_t result = 0LL;

        for(auto& l : locks)
        {
            for(auto& k : keys)
            {
                if(Matches(l, k)) { result++; }
            }
        }

        return result;
    }

private:
    bool Matches(vector<int>& lock, vector<int>& key)
    {
        for(int i = 0; i < Widht; i++)
        {
            if(lock[i] + key[i] > Height)
            {
                return false;
            }
        }

        return true;
    }
};

int main()
{
    KeyLockMatcher keyLockMatcher;
    int64_t result = 0LL;

    keyLockMatcher.Read(stdin);
    result = keyLockMatcher.CountMatches();

    printf("%lld\n", result);

    return 0;
}