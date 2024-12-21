#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<queue>
#include<map>
#include<algorithm>

using namespace std;

class NumericKeypad
{
public:
    map<string, int64_t> FindAllSequences(string& code)
    {
        vector<string> sequences;
        char currentKey = 'A';

        for(auto nextKey : code)
        {
            if(currentKey == nextKey)
            {
                sequences.back().push_back('A');
                continue;
            }

            sequences.push_back(FindSubsequence(currentKey, nextKey));

            currentKey = nextKey;
        }

        map<string, int64_t> result;
        for(auto s : sequences) { result[s]++; }

        return result;
    }

    int64_t CodeValue(string code)
    {
        int64_t value = 0LL;
        for(auto c : code)
        {
            if(isdigit(c))
            {
                value *= 10;
                value += c - '0';
            }
        }
        return value;
    }

private:
    string FindSubsequence(char from, char to)
    {
        string s;

        int startRow = GetRow(from);
        int endRow = GetRow(to);
        int startColumn = GetColumn(from);
        int endColumn = GetColumn(to);
        int rowDiff = endRow - startRow;
        int columnDiff = endColumn - startColumn;

        if(startColumn == 0 && endRow == 3)
        {
            s.append(abs(columnDiff), HorizontalDirection(columnDiff));
            s.append(abs(rowDiff), VerticalDirection(rowDiff));
        }
        else if(endColumn == 0 && startRow == 3)
        {
            s.append(abs(rowDiff), VerticalDirection(rowDiff));
            s.append(abs(columnDiff), HorizontalDirection(columnDiff));
        }
        else if (columnDiff < 0)
        {
            s.append(abs(columnDiff), HorizontalDirection(columnDiff));
            s.append(abs(rowDiff), VerticalDirection(rowDiff));
        }
        else
        {
            s.append(abs(rowDiff), VerticalDirection(rowDiff));
            s.append(abs(columnDiff), HorizontalDirection(columnDiff));
        }

        s.push_back('A');
        return s;
    }

    char HorizontalDirection(int diff)
    {
        return diff > 0 ? '>' : '<';
    }

    char VerticalDirection(int diff)
    {
        return diff > 0 ? 'v' : '^';
    }

    int GetRow(char c)
    {
        switch(c)
        {
            case '7':
            case '8':
            case '9':
                return 0;
            case '4':
            case '5':
            case '6':
                return 1;
            case '1':
            case '2':
            case '3':
                return 2;
            case '0':
            case 'A':
                return 3;
            default:
                return -1;
        }
    }

    int GetColumn(char c)
    {
        switch(c)
        {
            case '7':
            case '4':
            case '1':
                return 0;
            case '8':
            case '5':
            case '2':
            case '0':
                return 1;
            case '9':
            case '6':
            case '3':
            case 'A':
                return 2;
            default:
                return -1;
        }
    }
};

class DirectionalKeypad
{
public:
    map<string, int64_t> FindAllSequences(map<string, int64_t> sequences)
    {
        map<string, int64_t> result;

        for(auto s : sequences)
        {
            vector<string> n = NextSubsequences(s.first);
            for(auto ns : n)
            {
                result[ns] += s.second;
            }
        }

        return result;
    }

private:
    vector<string> NextSubsequences(const string& subsequence)
    {
        vector<string> result;
        char currentKey = 'A';

        for(char nextKey : subsequence)
        {
            if(currentKey == nextKey)
            {
                result.back().push_back('A');
                continue;
            }

            int startRow = GetRow(currentKey);
            int endRow = GetRow(nextKey);
            int startColumn = GetColumn(currentKey);
            int endColumn = GetColumn(nextKey);
            int rowDiff = endRow - startRow;
            int columnDiff = endColumn - startColumn;

            string s;

            if(startColumn == 0 && endRow == 0)
            {
                s.append(abs(columnDiff), HorizontalDirection(columnDiff));
                s.append(abs(rowDiff), VerticalDirection(rowDiff));
            }
            else if(endColumn == 0 && startRow == 0)
            {
                s.append(abs(rowDiff), VerticalDirection(rowDiff));
                s.append(abs(columnDiff), HorizontalDirection(columnDiff));
            }
            else if (columnDiff < 0)
            {
                s.append(abs(columnDiff), HorizontalDirection(columnDiff));
                s.append(abs(rowDiff), VerticalDirection(rowDiff));
            }
            else
            {
                s.append(abs(rowDiff), VerticalDirection(rowDiff));
                s.append(abs(columnDiff), HorizontalDirection(columnDiff));
            }

            s.push_back('A');
            result.push_back(s);

            currentKey = nextKey;
        }

        return result;
    }

    char HorizontalDirection(int diff)
    {
        return diff > 0 ? '>' : '<';
    }

    char VerticalDirection(int diff)
    {
        return diff > 0 ? 'v' : '^';
    }

    int GetRow(char c)
    {
        switch(c)
        {
            case '^':
            case 'A':
                return 0;
            case '<':
            case 'v':
            case '>':
                return 1;
            default:
                return -1;
        }
    }

    int GetColumn(char c)
    {
        switch(c)
        {
            case '<':
                return 0;
            case '^':
            case 'v':
                return 1;
            case 'A':
            case '>':
                return 2;
            default:
                return -1;
        }
    }
};

int main()
{
    const int bufferSize = 256;
    char buffer[bufferSize];
    vector<string> codes;
    NumericKeypad numericKeypad;
    DirectionalKeypad directionalKeypad;
    int64_t result = 0LL;
    int64_t result2 = 0LL;

    while(fgets(buffer, bufferSize, stdin) != NULL)
    {
        int len = strlen(buffer);
        if(strlen(buffer) > 1)
        {
            if(buffer[len-1] == '\n') { buffer[len-1] = '\0'; }
            codes.push_back(string(buffer));
        }
    }

    for(auto code : codes)
    {
        int64_t value = numericKeypad.CodeValue(code);

        printf("%s\n", code.c_str());

        map<string, int64_t> sequences = numericKeypad.FindAllSequences(code);
        for(int i = 0; i < 2; i++)
        {
            map<string, int64_t> nextSequences = directionalKeypad.FindAllSequences(sequences);
            sequences.clear();
            for(auto n : nextSequences) { sequences.insert(n); }
        }
        
        int64_t size = 0LL;
        for(auto r : sequences) { size += r.first.size() * r.second; }

        result += size * value;

        for(int i = 0; i < 23; i++)
        {
            map<string, int64_t> nextSequences = directionalKeypad.FindAllSequences(sequences);
            sequences.clear();
            for(auto n : nextSequences) { sequences.insert(n); }
        }

        size = 0LL;
        for(auto r : sequences) { size += r.first.size() * r.second; }

        result2 += size * value;
    }

    printf("%lld\n", result);
    printf("%lld\n", result2);

    return 0;
}