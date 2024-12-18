#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<algorithm>
#include<unordered_set>
#include<unordered_map>
#include<queue>

using namespace std;

enum DirectionType : int
{
    DirectionN = 0,
    DirectionS = 1,
    DirectionW = 2,
    DirectionE = 3,
};

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }

    Point(const Point& pt, DirectionType direction) : x(pt.x), y(pt.y)
    {
        Move(direction);
    }

    void Move(DirectionType direction)
    {
        switch(direction)
        {
            case DirectionN:
                y--; break;
            case DirectionS:
                y++; break;
            case DirectionW:
                x--; break;
            case DirectionE:
                x++; break;
            default:
               break;
        }
    }

    size_t Hash() const
    {
        return ((size_t)x << 32) ^ (size_t)y;
    }
};

bool operator < (const Point& p1, const Point& p2)
{
    return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
}

bool operator == (const Point& p1, const Point& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

struct PointHash {
   size_t operator()(const Point &point) const
   {
        return point.Hash();
   }
};

class Board
{
private:
    vector<vector<char>> board;
    int width;
    int height;

public:
    Board()
    {
        width = height = 0;
    }

    void Read(FILE* input)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];
        memset(buffer, 0, bufferSize);

        while(fgets(buffer, bufferSize, input) != NULL)
        {
            AddLine(buffer);
        }
    }

    int64_t CountFenceCost()
    {
        int64_t cost = 0;
        unordered_set<Point, PointHash> visited;

        for(int j = 0; j < board.size(); j++)
        {
            for(int i = 0; i < board[j].size(); i++)
            {
                Point current = Point(i, j);
                if(visited.count(current) > 0) { continue; }

                queue<Point> processQueue;
                processQueue.push(current);
                char groupId = GetTile(current);
                int64_t groupArea = 0LL;
                int64_t groupPerimeter = 0LL;

                while(!processQueue.empty())
                {
                    current = processQueue.front();
                    processQueue.pop();

                    if(GetTile(current) == groupId)
                    {
                        if(visited.count(current) > 0) { continue; }

                        visited.insert(current);
                        groupArea++;

                        processQueue.push(Point(current, DirectionN));
                        processQueue.push(Point(current, DirectionE));
                        processQueue.push(Point(current, DirectionS));
                        processQueue.push(Point(current, DirectionW));
                    }
                    else
                    {
                        groupPerimeter++;
                    }
                }

                cost += groupArea * groupPerimeter;
            }
        }

        return cost;
    }

    int64_t CountBulkFenceCost()
    {
        int64_t cost = 0;
        unordered_set<Point, PointHash> visited;

        for(int j = 0; j < board.size(); j++)
        {
            for(int i = 0; i < board[j].size(); i++)
            {
                Point current = Point(i, j);
                if(visited.count(current) > 0) { continue; }

                queue<pair<Point,DirectionType>> processQueue;
                unordered_set<Point, PointHash> outerFenceN;
                unordered_set<Point, PointHash> outerFenceS;
                unordered_set<Point, PointHash> outerFenceE;
                unordered_set<Point, PointHash> outerFenceW;

                processQueue.push(make_pair(current, DirectionE));
                char groupId = GetTile(current);
                int64_t groupArea = 0LL;
                int64_t groupSides = 0LL;

                while(!processQueue.empty())
                {
                    pair<Point,DirectionType> item = processQueue.front();
                    current = item.first;
                    processQueue.pop();

                    if(GetTile(current) == groupId)
                    {
                        if(visited.count(current) > 0) { continue; }

                        visited.insert(current);
                        groupArea++;

                        processQueue.push(make_pair(Point(current, DirectionN), DirectionN));
                        processQueue.push(make_pair(Point(current, DirectionS), DirectionS));
                        processQueue.push(make_pair(Point(current, DirectionE), DirectionE));
                        processQueue.push(make_pair(Point(current, DirectionW), DirectionW));
                    }
                    else
                    {
                        switch(item.second)
                        {
                            case DirectionN:
                                outerFenceN.insert(current);
                                break;
                            case DirectionS:
                                outerFenceS.insert(current);
                                break;
                            case DirectionE:
                                outerFenceE.insert(current);
                                break;
                            case DirectionW:
                                outerFenceW.insert(current);
                                break;
                        }
                    }
                }

                while(outerFenceE.size() > 0)
                {
                    groupSides++;
                    current = *outerFenceE.begin();
                    outerFenceE.erase(current);

                    Point nextN = Point(current, DirectionN);
                    Point nextS = Point(current, DirectionS);

                    while(outerFenceE.count(nextN) > 0)
                    {
                        outerFenceE.erase(nextN);
                        nextN = Point(nextN, DirectionN);
                    }
                    while(outerFenceE.count(nextS) > 0)
                    {
                        outerFenceE.erase(nextS);
                        nextS = Point(nextS, DirectionS);
                    }
                }

                while(outerFenceW.size() > 0)
                {
                    groupSides++;
                    current = *outerFenceW.begin();
                    outerFenceW.erase(current);

                    Point nextN = Point(current, DirectionN);
                    Point nextS = Point(current, DirectionS);

                    while(outerFenceW.count(nextN) > 0)
                    {
                        outerFenceW.erase(nextN);
                        nextN = Point(nextN, DirectionN);
                    }
                    while(outerFenceW.count(nextS) > 0)
                    {
                        outerFenceW.erase(nextS);
                        nextS = Point(nextS, DirectionS);
                    }
                }

                while(outerFenceN.size() > 0)
                {
                    groupSides++;
                    current = *outerFenceN.begin();
                    outerFenceN.erase(current);

                    Point nextE = Point(current, DirectionE);
                    Point nextW = Point(current, DirectionW);

                    while(outerFenceN.count(nextE) > 0)
                    {
                        outerFenceN.erase(nextE);
                        nextE = Point(nextE, DirectionE);
                    }
                    while(outerFenceN.count(nextW) > 0)
                    {
                        outerFenceN.erase(nextW);
                        nextW = Point(nextW, DirectionW);
                    }
                }

                while(outerFenceS.size() > 0)
                {
                    groupSides++;
                    current = *outerFenceS.begin();
                    outerFenceS.erase(current);

                    Point nextE = Point(current, DirectionE);
                    Point nextW = Point(current, DirectionW);

                    while(outerFenceS.count(nextE) > 0)
                    {
                        outerFenceS.erase(nextE);
                        nextE = Point(nextE, DirectionE);
                    }
                    while(outerFenceS.count(nextW) > 0)
                    {
                        outerFenceS.erase(nextW);
                        nextW = Point(nextW, DirectionW);
                    }
                }

                cost += groupArea * groupSides;
            }
        }

        return cost;
    }

private:
    void AddLine(const char* line)
    {
        int size = 0;
        vector<char> row;

        for(int i = 0; line[i] != 0; i++)
        {
            if (isalnum(line[i]))
            {
                size++;
                row.push_back(line[i]);
            }
        }

        board.push_back(row);
        width = max(width, size);
        if (size > 0) { height++; }
    }

    bool IsOnBoard(const Point& point)
    {
        return point.x >= 0 && point.x < width
            && point.y >= 0 && point.y < height;
    }

    char GetTile(const Point& point)
    {
        return IsOnBoard(point)
            ? board[point.y][point.x]
            : '_';
    }
};


int main()
{
    int64_t sum = 0;
    int64_t sum2 = 0;
    Board board;

    board.Read(stdin);
    sum = board.CountFenceCost();
    sum2 = board.CountBulkFenceCost();

    printf("%lld\n", sum);
    printf("%lld\n", sum2);

    return 0;
}