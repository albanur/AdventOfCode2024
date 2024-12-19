#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<queue>
#include<unordered_set>
#include<unordered_map>
#include<regex>
#include<algorithm>

using namespace std;

class TowelMatcher
{
private:
    vector<string> towels;
    vector<string> patterns;

public:
    TowelMatcher() {}

    void Read(FILE* input)
    {
        const int bufferSize = 8192;
        char buffer[bufferSize];

        fgets(buffer, bufferSize, input);

        char* token = strtok(buffer, " ,\\n");
        while(token != NULL)
        {
            string towel;
            while(isalpha(*token))
            {
                towel.push_back(*token);
                token++;
            }
            towels.push_back(towel);
            token = strtok(NULL, " ,\\n");
        }

        fgets(buffer, bufferSize, input);

        while(fgets(buffer, bufferSize, input) != NULL)
        {
            if(strlen(buffer) > 0)
            {
                char* newLine = strchr(buffer,'\n');
                if(newLine != NULL) { *newLine = '\0'; }
                patterns.push_back(string(buffer));
            }
        }
    }

    int CountMatches()
    {
        int count = 0;
        for(auto& p : patterns)
        {
            if(DoesMatch(p.c_str()))
            {
                count++;
            }
        }
        return count;
    }

    int64_t CountAllMatches()
    {
        int64_t count = 0;
        for(auto& p : patterns)
        {
            count += CountAllMatches(p.c_str());
        }
        return count;
    }

private:
    bool DoesMatch(const char* pattern)
    {
        stack<const char*> subpatterns;
        subpatterns.push(pattern);

        while(!subpatterns.empty())
        {
            const char* subpattern = subpatterns.top();
            subpatterns.pop();

            for(auto& t : towels)
            {
                if(StartsWith(subpattern, t.c_str()))
                {
                    int len = strlen(t.c_str());
                    const char* next = subpattern + len;
                    if(*next == '\0') { return true; }
                    subpatterns.push(next);
                }
            }
        }

        return false;
    }

    int64_t CountAllMatches(const char* pattern)
    {
        int patternSize= strlen(pattern);
        vector<int64_t> matches(patternSize+1, 0LL);
        matches[patternSize] = 1LL;

        for(int i = patternSize-1; i >= 0; i--)
        {
            for(auto& t : towels)
            {
                if(StartsWith(pattern + i, t.c_str()))
                {
                    matches[i] += matches[i+t.size()];
                }
            }
        }

        return matches[0];
    }

    bool StartsWith(const char* value, const char* prefix)
    {
        while(*value != '\0' && *prefix != '\0')
        {
            if(*value != *prefix) { return false; }
            value++;
            prefix++;
        }

        return *prefix == '\0';
    }
};

int main()
{
    TowelMatcher matcher;
    int sum = 0;
    int64_t sum2 = 0;

    matcher.Read(stdin);
    sum = matcher.CountMatches();
    printf("%i\n", sum);

    sum2 = matcher.CountAllMatches();
    printf("%lld\n", sum2);
   
    return 0;
}