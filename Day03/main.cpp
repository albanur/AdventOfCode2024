#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<map>
#include<algorithm>
#include<regex>

using namespace std;

int main()
{
    const int buffer_size = 1024;

    char buffer[buffer_size];
    string input;
    long long sum = 0;
    smatch fullMatch;
    smatch mulMatch;
    regex mulRegex ("mul\\((\\d{1,3}),(\\d{1,3})\\)");
    regex doRegex ("do\\(\\)");
    regex dontRegex ("don't\\(\\)");
    bool enabled = true;
    regex fullRegex ("mul\\(\\d{1,3},\\d{1,3}\\)|do\\(\\)|don't\\(\\)");

    memset(buffer, 0, buffer_size);

    while(fgets(buffer, buffer_size-1, stdin) != NULL)
    {
        input.append(buffer, strlen(buffer));
    }

    string s (input);
    while (regex_search (s,fullMatch,fullRegex))
    {
        string token = fullMatch[0].str();

        if(regex_match(token, doRegex))
        {
            enabled = true;
        }
        else if(regex_match(token, dontRegex))
        {
            enabled = false;
        }
        else if(enabled && regex_match(token, mulMatch, mulRegex))
        {
            int d1 = stoi(mulMatch[1]);
            int d2 = stoi(mulMatch[2]);
            sum += d1*d2;
        }

        s = fullMatch.suffix().str();
    }

    printf("%lld\n", sum);

    return 0;
}