#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<map>
#include<algorithm>

using namespace std;

int NumberOfDigits(uint64_t value)
{
    if(value == 0ULL)
    {
        return 1;
    }

    int digits = 0;
    while(value > 0ULL)
    {
        value /= 10ULL;
        digits++;
    }

    return digits;
}

uint64_t PowerOf10(int power)
{
    uint64_t result = 1ULL;
    while(power > 0)
    {
        result *= 10ULL;
        power--;
    }

    return result;
}

void TransformStone(uint64_t value, uint64_t& next1, uint64_t& next2)
{
    if(value == 0ULL)
    {
        next1 = 1ULL;
        next2 = UINT64_MAX;
        return;
    }
    
    int digits = NumberOfDigits(value);
    if(digits % 2 == 0)
    {
        uint64_t dividor = PowerOf10(digits/2);
        next1 = value / dividor;
        next2 = value % dividor;
        return;
    }

    next1 = value * 2024ULL;
    next2 = UINT64_MAX;
    return;
}

void TransformStones(map<uint64_t, uint64_t>** stones, int blinks)
{
    uint64_t next1, next2;

    for(int i = 0; i < blinks; i++)
    {
        map<uint64_t, uint64_t>* next = new map<uint64_t, uint64_t>();

        for(auto s : **stones)
        {
            TransformStone(s.first, next1, next2);

            if(next->count(next1) == 0)
            {
                next->insert(make_pair(next1, 0ULL));
            }
            next->at(next1) += s.second;

            if(next2 != UINT64_MAX)
            {
                if(next->count(next2) == 0)
                {
                    next->insert(make_pair(next2, 0ULL));
                }
                next->at(next2) += s.second;
            }
        }

        delete *stones;
        *stones = next;
    }
}

uint64_t CountStones(map<uint64_t, uint64_t>* stones)
{
    uint64_t sum = 0ULL;
    for(auto s : *stones)
    {
        sum += s.second;
    }
    return sum;
}

int main()
{
    uint64_t value;
    map<uint64_t, uint64_t>* stones = new map<uint64_t, uint64_t>();
    
    while(scanf("%llu", &value) != EOF)
    {
        stones->insert(make_pair(value, 1ULL));
    }

    TransformStones(&stones, 25);
    printf("%llu\n", CountStones(stones));

    TransformStones(&stones, 50);
    printf("%llu\n", CountStones(stones));

    delete stones;
    return 0;
}