#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<queue>
#include<unordered_set>
#include<algorithm>

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
    vector<vector<unordered_multiset<Point, PointHash>>> peaks;

public:
    Board() { }

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

    void FindTrailheads()
    {
        queue<Point> processQueue;
        FillWithPeaks(processQueue);

        while(!processQueue.empty())
        {
            Point point = processQueue.front();
            processQueue.pop();

            char value = GetTile(point);
            unordered_multiset<Point, PointHash>* peaks = GetPeaks(point);

            Point neighbours[] = {
                Point(point, DirectionN), Point(point, DirectionE),
                Point(point, DirectionS), Point(point, DirectionW)
            };

            for (int i = 0; i < 4; i++)
            {
                Point neighbour = neighbours[i];
                if(!IsOnBoard(neighbour)) { continue; }
                if(GetTile(neighbour) != value-1) { continue; }

                unordered_multiset<Point, PointHash>* neighbourPeaks = GetPeaks(neighbour);
                if(neighbourPeaks->size() == 0)
                {
                    processQueue.push(neighbour);
                }
                neighbourPeaks->insert(peaks->begin(), peaks->end());
            }
        }
    }

    int64_t SumOfTrailHeads1()
    {
        int64_t sum = 0LL;
        Point point;

        for(int j = 0; j < board.size(); j++)
        {
            point.y = j;
            for(int i = 0; i < board[j].size(); i++)
            {
                point.x = i;
                if(GetTile(point) == '0')
                {
                    auto currentPeaks = GetPeaks(point);
                    unordered_set<Point, PointHash> distinctPeaks(currentPeaks->begin(), currentPeaks->end());
                    sum += distinctPeaks.size();
                }
            }
        }

        return sum;
    }

    int64_t SumOfTrailHeads2()
    {
        int64_t sum = 0LL;
        Point point;

        for(int j = 0; j < board.size(); j++)
        {
            point.y = j;
            for(int i = 0; i < board[j].size(); i++)
            {
                point.x = i;
                if(GetTile(point) == '0')
                {
                    sum += GetPeaks(point)->size();
                }
            }
        }

        return sum;
    }

private:
    void FillWithPeaks(queue<Point>& processQueue)
    {
        Point point;

        for(int j = 0; j < board.size(); j++)
        {
            point.y = j;
            for(int i = 0; i < board[j].size(); i++)
            {
                point.x = i;
                if(GetTile(point) == '9')
                {
                    processQueue.push(point);
                    GetPeaks(point)->insert(point);
                }
            }
        }
    }

    void AddLine(const char* line)
    {
        vector<char> row;

        for(int i = 0; line[i] != 0; i++)
        {
            if(isdigit(line[i]))
            {
                row.push_back(line[i]);
            }
        }

        if(row.size() > 0)
        {
            board.push_back(row);
            vector<unordered_multiset<Point, PointHash>> peaksRow(row.size());
            peaks.push_back(peaksRow);
        }
    }

    bool IsOnBoard(const Point& point)
    {
        return point.y >= 0 && point.y < (int)board.size()
            && point.x >= 0 && point.x < (int)board[point.y].size();
    }

    char GetTile(const Point& point)
    {
        return IsOnBoard(point)
            ? board[point.y][point.x]
            : '.';
    }

    unordered_multiset<Point, PointHash>* GetPeaks(const Point& point)
    {
        return IsOnBoard(point)
            ? &(peaks[point.y][point.x])
            : NULL;
    }
};

int main()
{
    Board board;
    int sum = 0;
    int sum2 = 0;

    board.Read(stdin);
    board.FindTrailheads();
    sum = board.SumOfTrailHeads1();
    sum2 = board.SumOfTrailHeads2();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}