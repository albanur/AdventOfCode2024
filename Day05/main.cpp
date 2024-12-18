#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<algorithm>
#include<set>
#include<map>

using namespace std;

class PrintQueue
{
private:
    map<int, set<int>> rules;
    vector<vector<int>> printOrders;

public:
    PrintQueue() {}

    void Read(FILE* input)
    {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        memset(buffer, 0, bufferSize);

        while(fgets(buffer, bufferSize, input) != NULL)
        {
            if(strchr(buffer, '|') != NULL)
            {
                ParseRule(buffer);
            }
            else if (strchr(buffer, ',') != NULL)
            {
                ParseOrder(buffer);
            }
        }
    }

    int CountValidOrders()
    {
        int sum = 0;

        for(auto& order : printOrders)
        {
            if(IsValidOrder(order))
            {
                sum += order[order.size()/2];
            }
        }

        return sum;
    }

    int FixAndCountInvalidOrders()
    {
        int sum = 0;

        for(auto& order : printOrders)
        {
            pair<int, int> result = FindWrongOrder(order);
            if(result.first < 0) { continue; }

            while(result.first >= 0)
            {
                MoveItem(order, result.second, result.first);
                result = FindWrongOrder(order);
            }

            sum += order[order.size()/2];
        }

        return sum;
    }

private:
    void ParseRule(const char* rule)
    {
        int first, second;

        first = atoi(rule);
        second = atoi(strchr(rule, '|') + 1);

        rules[first].insert(second);
    }

    void ParseOrder(char* order)
    {
        vector<int> printOrder;
        char* token;

        token = strtok(order,",");
        while (token != NULL)
        {
            printOrder.push_back(atoi(token));
            token = strtok (NULL, ",");
        }

        printOrders.push_back(printOrder);
    }

    bool IsValidOrder(vector<int>& printOrder)
    {
        for(int i = 1; i < printOrder.size(); i++)
        {
            set<int>& rule = rules[printOrder[i]];

            for(int j = 0; j < i; j++)
            {
                if(rule.count(printOrder[j]) > 0)
                {
                    return false;
                }
            }
        }

        return true;
    }

    pair<int,int> FindWrongOrder(vector<int>& printOrder)
    {
        for(int i = 1; i < printOrder.size(); i++)
        {
            set<int>& rule = rules[printOrder[i]];

            for(int j = 0; j < i; j++)
            {
                if(rule.count(printOrder[j]) > 0)
                {
                    return make_pair(j, i);
                }
            }
        }

        return make_pair(-1, -1);
    }

    void MoveItem(vector<int>& printOrder, int from, int to)
    {
        if(from <= to)
        {
            return;
        }

        int t = printOrder[from];
        for(int i = from; i > to; i--)
        {
            printOrder[i] = printOrder[i-1];
        }
        printOrder[to] = t;
    }
};

int main()
{
    int sum = 0;
    int sum2 = 0;
    PrintQueue printQueue;

    printQueue.Read(stdin);
    sum = printQueue.CountValidOrders();
    sum2 = printQueue.FixAndCountInvalidOrders();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}