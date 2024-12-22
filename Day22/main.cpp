#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<list>
#include<queue>
#include<unordered_map>
#include<algorithm>

using namespace std;

struct PriceSequence
{
    list<signed char> Sequence;

    PriceSequence() { }

    void NextPriceDiff(int priceDiff)
    {
        Sequence.push_back((signed char)priceDiff);
        if(Sequence.size() > 4)
        {
            Sequence.pop_front();
        }
    }

    bool IsValid()
    {
        return Sequence.size() == 4;
    }

    size_t Hash() const
    {
        size_t hash = 0;
        for(auto d : Sequence)
        {
            hash <<= 8;
            hash ^= d;
        }
        return hash;
    }
};

bool operator == (const PriceSequence& p1, const PriceSequence& p2)
{
    if (p1.Sequence.size() != p2.Sequence.size()) { return false; }

    list<signed char>::const_iterator it1 = p1.Sequence.begin();
    list<signed char>::const_iterator it2 = p2.Sequence.begin();

    while(it1 != p1.Sequence.end())
    {
        if(*it1 != *it2) { return false; }
        ++it1;
        ++it2;
    }
    return true;
}

struct PriceSequenceHash
{
   size_t operator()(const PriceSequence &priceSequence) const
   {
        return priceSequence.Hash();
   }
};


class SecretNumberPriceMarket
{
private:
    vector<int64_t> initialNumbers;

public:
    SecretNumberPriceMarket() { }

    void Read(FILE* file)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];

        while(fgets(buffer, bufferSize, stdin) != NULL)
        {
            int len = strlen(buffer);
            if(strlen(buffer) > 1 && isdigit(buffer[0]))
            {
                int64_t value = stoll(buffer);
                initialNumbers.push_back(value);
            }
        }
    }

    int64_t SumOfFinalNumbers()
    {
        int64_t result = 0LL;

        for(int64_t secret : initialNumbers)
        {
            for(int i = 0; i < 2000; i++)
            {
                secret = NextSecretNumber(secret);
            }
            result += secret;
        }

        return result;
    }

    int64_t FindBestSequencs()
    {
        unordered_map<PriceSequence, int64_t, PriceSequenceHash> result;

        for(int64_t secret : initialNumbers)
        {
            unordered_map<PriceSequence, int, PriceSequenceHash> r = GetSequenceMap(secret);
            for(auto s : r)
            {
                result[s.first] += s.second;
            }
        }

        int64_t best = 0LL;
        for(auto s : result)
        {
            best = max(best, s.second);
        }
        return best;
    }

private:
    unordered_map<PriceSequence, int, PriceSequenceHash> GetSequenceMap(int64_t secret)
    {
        int64_t currentSecret = secret;
        int currentPrice = GetPrice(currentSecret);
        PriceSequence sequence;
        unordered_map<PriceSequence, int, PriceSequenceHash> result;

        for(int i = 0; i < 2000; i++)
        {
            int64_t nextSecret = NextSecretNumber(currentSecret);
            int nextPrice = GetPrice(nextSecret);
            sequence.NextPriceDiff(nextPrice - currentPrice);

            if(sequence.IsValid() && result.count(sequence) == 0)
            {
                result[sequence] = nextPrice;
            }

            currentSecret = nextSecret;
            currentPrice = nextPrice;
        }

        return result;
    }

    int GetPrice(int64_t secret)
    {
        return secret % 10;
    }

    int64_t NextSecretNumber(int64_t secret)
    {
        secret = Prune(Mix(secret, secret * 64));
        secret = Prune(Mix(secret, secret / 32));
        secret = Prune(Mix(secret, secret * 2048));
        return secret;
    }

    int64_t Mix(int64_t v1, int64_t v2)
    {
        return v1 ^ v2;
    }

    int64_t Prune(int64_t v)
    {
        return v % 16777216;
    }
};

int main()
{
    SecretNumberPriceMarket market;
    int64_t result = 0LL;
    int64_t result2 = 0LL;

    market.Read(stdin);
    result = market.SumOfFinalNumbers();
    result2 = market.FindBestSequencs();

    printf("%lld\n", result);
    printf("%lld\n", result2);

    return 0;
}