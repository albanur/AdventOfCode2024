#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<map>
#include<algorithm>

using namespace std;

int main()
{
    char buffer[256];
    int v1, v2;
    int sum = 0;
    int sum2 = 0;
    vector<int> p1;
    vector<int> p2;
    map<int,int> m2;

    while(fgets(buffer, 256, stdin) != NULL)
    {
        if(strlen(buffer) > 1)
        {
            sscanf(buffer, "%i%i", &v1, &v2);
            p1.push_back(v1);
            p2.push_back(v2);

            if(m2.find(v1) == m2.end())
            {
                m2[v1] = 0;
            }
            if(m2.find(v2) == m2.end())
            {
                m2[v2] = 0;
            }
            m2[v2]++;
        }
    }

    sort(p1.begin(), p1.end());
    sort(p2.begin(), p2.end());

    for(int i = 0; i < p1.size(); i++)
    {
        int d = p1[i] - p2[i];
        d = d < 0 ? -d : d;
        sum += d;

        int v = p1[i];
        int m = m2[v];
        sum2 += v * m;
    }

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}