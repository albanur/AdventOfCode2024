#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<queue>
#include<unordered_set>
#include<unordered_map>
#include<map>
#include<algorithm>

using namespace std;

enum MovementType : char
{
    DirectionN = '^',
    DirectionS = 'v',
    DirectionW = '<',
    DirectionE = '>',
};

enum TileType : char
{
    Wall = '#',
    Start = 'S',
    End = 'E',
    Space = '.',
};

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }

    Point(const Point& pt) : x(pt.x), y(pt.y) { }

    Point(const Point& pt, MovementType direction) : x(pt.x), y(pt.y)
    {
        Move(direction);
    }

    void Move(MovementType direction)
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

    int Distance(const Point& pt)
    {
        return abs(x-pt.x) + abs(y-pt.y);
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

struct State
{
    Point Location;
    int64_t Cost;

    State(const Point& location, int64_t cost = 0) : Location(location), Cost(cost) { }
};

bool operator < (const State& s1, const State& s2)
{
    return (s1.Cost > s2.Cost)
        || (s1.Cost == s2.Cost && s1.Location < s2.Location);
}

class Board
{
private:
    vector<vector<TileType>> board;
    Point startPosition;
    Point endPosition;
    unordered_map<Point, int, PointHash> distanceFromStart;
    unordered_map<int, vector<Point>> cheatMap;

public:
    Board() { }

    void Read(FILE* input)
    {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        memset(buffer, 0, bufferSize);

        while(fgets(buffer, bufferSize, input) != NULL)
        {
            AddLine(buffer);
        }
    }

    int FindShortestPath()
    {
        priority_queue<State> queue;
        queue.push(State(startPosition, 0));

        while(!queue.empty())
        {
            State state = queue.top();
            queue.pop();

            if(state.Location == endPosition)
            {
                distanceFromStart[state.Location] = state.Cost;
                return state.Cost;
            }

            if(distanceFromStart.count(state.Location) > 0) { continue; }
            distanceFromStart[state.Location] = state.Cost;

            Point nextLocations[] =
            {
                Point(state.Location, DirectionN),
                Point(state.Location, DirectionE),
                Point(state.Location, DirectionS),
                Point(state.Location, DirectionW),
            };

            for(int i = 0; i < 4; i++)
            {
                if(GetTile(nextLocations[i]) == Space)
                {
                    queue.push(State(nextLocations[i], state.Cost+1));
                }
            }
        }

        return distanceFromStart[endPosition];
    }

    void CalculateCheatMap()
    {
        cheatMap.clear();
        Point current;

        for(int y = 1; y < board.size()-1; y++)
        {
            current.y = y;
            for(int x = 1; x < board[y].size()-1; x++)
            {
                current.x = x;
                if(GetTile(current) != Wall) { continue; }

                Point nextLocations[] =
                {
                    Point(current, DirectionN),
                    Point(current, DirectionE),
                    Point(current, DirectionS),
                    Point(current, DirectionW),
                };

                for(int i = 0; i < 4; i++)
                {
                    if(GetTile(nextLocations[i]) != Space) { continue; }
                    for(int j = i+1; j < 4; j++)
                    {
                        if(GetTile(nextLocations[j]) != Space) { continue; }

                        int saving = abs(distanceFromStart[nextLocations[i]] - distanceFromStart[nextLocations[j]]) - 2;
                        cheatMap[saving].push_back(current);
                    }
                }
            }
        }
    }

    void CalculateExtendedCheatMap()
    {
        cheatMap.clear();
        Point current;
        Point next;

        for(int y = 1; y < board.size()-1; y++)
        {
            current.y = y;
            for(int x = 1; x < board[y].size()-1; x++)
            {
                current.x = x;
                if(GetTile(current) != Space || distanceFromStart.count(current) == 0) { continue; }

                for(int j = max(1, y-20); j < min((int)board.size()-1, y+21); j++)
                {
                    next.y = j;
                    for(int i = max(1, x-20); i < min((int)board[j].size()-1, x+21); i++)
                    {
                        next.x = i;
                        if(current == next || current.Distance(next) > 20 || GetTile(next) != Space || distanceFromStart.count(next) == 0) { continue; }

                        int saving = abs(distanceFromStart[current] - distanceFromStart[next]) - current.Distance(next);
                        cheatMap[saving].push_back(current);
                    }
                }
            }
        }
    }


    int CountCheats(int min)
    {
        int result = 0;

        for(auto& cm : cheatMap)
        {
            if(cm.first >= min)
            {
                result += cm.second.size();
            }
        }

        return result;
    }

private:
    void AddLine(const char* line)
    {
        vector<TileType> row;

        for(int i = 0; line[i] != 0; i++)
        {
            switch(line[i])
            {
                case Wall:
                    row.push_back(Wall);
                    break;
                case Space:
                    row.push_back(Space);
                    break;
                case Start:
                    startPosition = Point(i, board.size());
                    row.push_back(Space);
                    break;
                case End:
                    endPosition = Point(i, board.size());
                    row.push_back(Space);
                    break;
                default:
                    break;
            }
        }

        if(row.size() > 0)
        {
            board.push_back(row);
        }
    }

    bool IsOnBoard(const Point& point)
    {
        return point.y >= 0 && point.y < (int)board.size()
            && point.x >= 0 && point.x < (int)board[point.y].size();
    }

    TileType GetTile(const Point& point)
    {
        return IsOnBoard(point)
            ? board[point.y][point.x]
            : Wall;
    }

    void SetTile(const Point& point, TileType value)
    {
        if(IsOnBoard(point))
        {
            board[point.y][point.x] = value;
        }
    }
};

int main()
{
    Board board;
    int sum = 0;
    int sum2 = 0;

    board.Read(stdin);

    board.FindShortestPath();
    board.CalculateCheatMap();
    sum = board.CountCheats(100);

    board.CalculateExtendedCheatMap();
    sum2 = board.CountCheats(100)/2;

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}