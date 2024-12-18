#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<queue>
#include<unordered_set>
#include<unordered_map>
#include<regex>
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
    int size;
    Point lastByte;

public:
    Board(int size)
    {
        board = vector<vector<TileType>>(size+1, vector<TileType>(size+1, Space));
        this->size = size+1;

        startPosition = Point(0,0);
        endPosition = Point(size, size);
    }

    bool Read(FILE* input, int maxLines)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];
        int lines = 0;

        while(fgets(buffer, bufferSize, input) != NULL)
        {
            AddLine(buffer);
            lines++;
            if(lines == maxLines) { return true; }
        }

        return false;
    }

    int64_t FindShortestPath()
    {
        unordered_set<Point, PointHash> visited;
        priority_queue<State> queue;

        queue.push(State(startPosition, 0));

        while(!queue.empty())
        {
            State state = queue.top();
            queue.pop();

            if(state.Location == endPosition) { return state.Cost; }
            if(visited.count(state.Location) > 0) { continue; }
            visited.insert(state.Location);

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

        return -1LL;
    }

    Point GetLastByte()
    {
        return lastByte;
    }

private:
    void AddLine(const char* line)
    {
        regex lineRegex("(\\d+),(\\d+)\\n?");
        cmatch match;

        if(regex_match(line, match, lineRegex))
        {
            lastByte.x = stoi(match[1]);
            lastByte.y = stoi(match[2]);
            SetTile(lastByte, Wall);
        }
    }

    bool IsOnBoard(const Point& point)
    {
        return point.y >= 0 && point.y < size
            && point.x >= 0 && point.x < size;
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
    Board board(70);
    int sum = 0;
    int sum2 = 0;

    board.Read(stdin, 1024);

    sum = board.FindShortestPath();
    printf("%i\n", sum);
    
    while(board.Read(stdin, 1))
    {
        sum2 = board.FindShortestPath();
        if(sum2 < 0LL)
        {
            Point lastByte = board.GetLastByte();
            printf("%i,%i\n", lastByte.x, lastByte.y);
            return 0;
        }
    }

    return 0;
}