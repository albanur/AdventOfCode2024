#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<regex>
#include<algorithm>

using namespace std;

int64_t gcd(int64_t a, int64_t b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

int64_t CalculatePrizeCost(int64_t ax, int64_t ay, int64_t bx, int64_t by, int64_t px, int64_t py)
{
    int64_t x_gcd = gcd(ax, bx);
    int64_t y_gcd = gcd(ay, by);
   
    if(px % x_gcd != 0 || py % y_gcd != 0) { return 0LL; }
   
    int64_t result = INT64_MAX;
   
    for(int i = 0; i <= 100; i++)
    {
        int64_t rx = px - i*ax;
        int64_t ry = py - i*ay;
       
        if(rx < 0 || ry < 0) { break; }
       
        if(rx % bx != 0 || ry % by != 0) { continue; }
        int64_t j = rx / bx;
        int64_t k = ry / by;
       
        if(j == k && j <= 100)
        {
            result = min(result, 3*i + j);
            printf("%lld %lld %lld %lld %lld %lld -> %lld %lld %lld\n",
             ax, ay, bx, by, px, py, i, j, 3*i + j);
        }
    }
   
    return (result == INT64_MAX) ? 0LL : result;
}

int64_t CalculatePrizeCost3(int64_t ax, int64_t ay, int64_t bx, int64_t by, int64_t px, int64_t py)
{
    int64_t d = gcd(ax, ay);
    int64_t s1 = ay/d;
    int64_t s2 = -ax/d;
   
    int64_t c1 = s1*px + s2*py;
    int64_t c2 = s1*bx + s2*by;
   
    if(c1 % c2 != 0)
    {
        return 0LL;
    }
    int64_t ib = c1/c2;
   
    int64_t c3 = px - bx*ib;
    if(c3 % ax != 0)
    {
        return 0LL;
    }
    int64_t ia = c3/ax;
   
    return 3*ia + ib;

}

int main()
{
    int64_t ax, ay;
    int64_t bx, by;
    int64_t px, py;
    int64_t result = 0LL;
    int64_t result2 = 0LL;
   
    regex buttonARegex ("Button A: X\\+(\\d+), Y\\+(\\d+)\\n?");
    regex buttonBRegex ("Button B: X\\+(\\d+), Y\\+(\\d+)\\n?");
    regex prizeRegex ("Prize: X=(\\d+), Y=(\\d+)\\n?");
    smatch match;
   
    const int bufferSize = 256;
    char buffer[bufferSize];
   
    while(fgets(buffer, bufferSize, stdin) != NULL)
    {
        string token(buffer);
        if(!regex_match(token, match, buttonARegex)) { continue; }
        ax = stoll(match[1]);
        ay = stoll(match[2]);
       
        if(fgets(buffer, bufferSize, stdin) == NULL) { continue; }
        token = string(buffer);
        if(!regex_match(token, match, buttonBRegex)) { continue; }
        bx = stoll(match[1]);
        by = stoll(match[2]);
       
        if(fgets(buffer, bufferSize, stdin) == NULL) { continue; }
        token = string(buffer);
        if(!regex_match(token, match, prizeRegex)) { continue; }
        px = stoll(match[1]);
        py = stoll(match[2]);
       
        result += CalculatePrizeCost3(ax, ay, bx, by, px, py);
        result2 += CalculatePrizeCost3(ax, ay, bx, by, px + 10000000000000LL, py + 10000000000000LL);
    }
   
    printf("%lld\n", result);
    printf("%lld\n", result2);

    return 0;
}