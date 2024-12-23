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

class LanMap
{
private:
    unordered_map<string, unordered_set<string>> connections;

public:
    LanMap() { }

    void Read(FILE* file)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];

        while(fgets(buffer, bufferSize, stdin) != NULL)
        {
            int len = strlen(buffer);
            regex connectionRegex("([a-z]{2})-([a-z]{2})\\n?");
            cmatch match;

            if(len > 1 && regex_match(buffer, match, connectionRegex))
            {
                string node1(match[1]);
                string node2(match[2]);
                connections[node1].insert(node2);
                connections[node2].insert(node1);
            }
        }
    }

    int64_t CountSetsOf3WithT()
    {
        unordered_set<string> setsOf3;

        for(auto tc : connections)
        {
            if(tc.first[0] != 't') { continue; }

            vector<string> neighbours;
            for(string n : tc.second) { neighbours.push_back(n); }

            for(int i = 0; i < neighbours.size(); i++)
            {
                for(int j = i+1; j < neighbours.size(); j++)
                {
                    if(HasConnection(neighbours[i], neighbours[j]))
                    {
                        vector<string> group { tc.first, neighbours[i], neighbours[j] };
                        string id = UniqueGroupId(group);
                        setsOf3.insert(id);
                    }
                }
            }
        }

        return setsOf3.size();
    }

    string FindBiggestClique()
    {
        unordered_map<string, unordered_set<string>>* currentCliques = new unordered_map<string, unordered_set<string>>();
        unordered_map<string, unordered_set<string>>* nextCliques = NULL;

        for(auto c : connections) { currentCliques->insert(make_pair(c.first, unordered_set<string>({c.first}))); }

        while(true)
        {
            nextCliques = new unordered_map<string, unordered_set<string>>();

            for(auto c : *currentCliques)
            {
                TryToExtendClique(c.second, *nextCliques);
            }

            if(nextCliques->size() == 0)
            {
                delete nextCliques;
                break;
            }

            delete currentCliques;
            currentCliques = nextCliques;
            nextCliques = NULL;
        }

        string result = currentCliques->begin()->first;
        delete currentCliques;
        return result;
    }

private:
    void TryToExtendClique(unordered_set<string>& clique, unordered_map<string, unordered_set<string>>& nextCliques)
    {
        string node = MinNeighbourNode(clique);

        for(string candidat : connections[node])
        {
            if(clique.count(candidat) > 0) { continue; }

            int canBeAdded = true;
            for(string member : clique)
            {
                if(!HasConnection(candidat, member))
                {
                    canBeAdded = false;
                    break;
                }
            }

            if(!canBeAdded) { continue; }

            unordered_set<string> biggerClique(clique.begin(), clique.end());
            biggerClique.insert(candidat);
            nextCliques.insert(make_pair(UniqueGroupId(biggerClique), biggerClique));
        }
    }

    string MinNeighbourNode(unordered_set<string>& clique)
    {
        int min = INT32_MAX;
        string node;

        for(string n : clique)
        {
            if(connections[n].size() < min)
            {
                min = connections[n].size();
                node = n;
            }
        }

        return node;
    }

    bool HasConnection(string c1, string c2)
    {
        return connections[c1].count(c2) > 0;
    }

    string UniqueGroupId(unordered_set<string>& clique)
    {
        string result;

        vector<string> group (clique.begin(), clique.end());
        sort(group.begin(), group.end());

        for(auto s : group)
        {
            if(!result.empty()) { result.push_back(','); }
            result.append(s);
        }

        return result;
    }

    string UniqueGroupId(vector<string> group)
    {
        string result;

        sort(group.begin(), group.end());
        for(auto s : group)
        {
            if(!result.empty()) { result.push_back(','); }
            result.append(s);
        }

        return result;
    }
};

int main()
{
    LanMap lanMap;
    int64_t result = 0LL;
    string result2;

    lanMap.Read(stdin);
    result = lanMap.CountSetsOf3WithT();
    result2 = lanMap.FindBiggestClique();

    printf("%lld\n", result);
    printf("%s\n", result2.c_str());

    return 0;
}