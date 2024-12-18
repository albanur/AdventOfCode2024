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

MovementType TurnLeft(MovementType movement)
{
    switch(movement)
    {
        case DirectionN: return DirectionW;
        case DirectionS: return DirectionE;
        case DirectionE: return DirectionN;
        case DirectionW: return DirectionS;
        default: return movement;
    }
}

MovementType TurnRight(MovementType movement)
{
    switch(movement)
    {
        case DirectionN: return DirectionE;
        case DirectionS: return DirectionW;
        case DirectionE: return DirectionS;
        case DirectionW: return DirectionN;
        default: return movement;
    }
}

MovementType OppositeMovement(MovementType movement)
{
    switch(movement)
    {
        case DirectionN: return DirectionS;
        case DirectionS: return DirectionN;
        case DirectionE: return DirectionW;
        case DirectionW: return DirectionE;
        default: return movement;
    }
}

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

struct ReindeerLocation
{
    Point Position;
    MovementType Direction;

    ReindeerLocation(const Point& position, MovementType direction) : Position(position), Direction(direction) {}

    void MoveForward()
    {
        Position.Move(Direction);
    }

    void TurnLeftAndMove()
    {
        Direction = TurnLeft(Direction);
        Position.Move(Direction);
    }

    void TurnRightAndMove()
    {
        Direction = TurnRight(Direction);
        Position.Move(Direction);
    }

    Point GoBack()
    {
        Point result = Position;
        result.Move(OppositeMovement(Direction));
        return result;
    }

    size_t Hash() const
    {
        return ((size_t)Direction << 32) ^ ((size_t)Position.x << 16) ^ (size_t)Position.y;
    }
};

bool operator < (const ReindeerLocation& p1, const ReindeerLocation& p2)
{
    return (p1.Direction < p2.Direction)
        || (p1.Direction == p2.Direction && p1.Position < p2.Position);
}

bool operator == (const ReindeerLocation& p1, const ReindeerLocation& p2)
{
    return p1.Position == p2.Position && p1.Direction == p2.Direction;
}

struct ReindeerLocationHash {
   size_t operator()(const ReindeerLocation &position) const
   {
        return position.Hash();
   }
};

struct State
{
    ReindeerLocation Location;
    int64_t Cost;
    ReindeerLocation PreviousLocation;

    State(const ReindeerLocation& location, int64_t cost = 0)
        : Location(location), Cost(cost), PreviousLocation(location) { }

    State MoveForward()
    {
        State result(*this);

        result.PreviousLocation = result.Location;
        result.Location.MoveForward();
        result.Cost += 1;

        return result;
    }

    State MoveLeft()
    {
        State result(*this);

        result.PreviousLocation = result.Location;
        result.Location.TurnLeftAndMove();
        result.Cost += 1001;

        return result;
    }

    State MoveRight()
    {
        State result(*this);

        result.PreviousLocation = result.Location;
        result.Location.TurnRightAndMove();
        result.Cost += 1001;

        return result;
    }
};

bool operator < (const State& s1, const State& s2)
{
    return (s1.Cost > s2.Cost)
        || (s1.Cost == s2.Cost && s1.Location < s2.Location)
        || (s1.Cost == s2.Cost && s1.Location == s2.Location && s1.PreviousLocation < s2.PreviousLocation);
}

class Board
{
private:
    vector<vector<TileType>> board;
    Point startPosition;
    Point endPosition;

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

    int64_t FindShortestPath()
    {
        unordered_set<ReindeerLocation, ReindeerLocationHash> visited;
        priority_queue<State> queue;

        queue.push(State(ReindeerLocation(startPosition, DirectionE)));

        while(!queue.empty())
        {
            State state = queue.top();
            queue.pop();

            if(state.Location.Position == endPosition) { return state.Cost; }
            if(visited.count(state.Location) > 0) { continue; }
            visited.insert(state.Location);

            State nextStates[] = { state.MoveForward(), state.MoveLeft(), state.MoveRight() };
            for(int i = 0; i < 3; i++)
            {
                if(GetTile(nextStates[i].Location.Position) == Space)
                {
                    queue.push(nextStates[i]);
                }
            }
        }

        return -1LL;
    }

    int64_t CountTilesOnBestPaths()
    {
        unordered_set<ReindeerLocation, ReindeerLocationHash> visited;
        priority_queue<State> stateQueue;
        unordered_map<ReindeerLocation, int64_t, ReindeerLocationHash> costMap;
        unordered_map<ReindeerLocation, vector<ReindeerLocation>, ReindeerLocationHash> parentMap;
        queue<ReindeerLocation> processQueue;
        unordered_set<Point, PointHash> bestTiles;

        int64_t cost = INT64_MAX;

        stateQueue.push(State(ReindeerLocation(startPosition, DirectionE)));

        while(!stateQueue.empty())
        {
            State state = stateQueue.top();
            stateQueue.pop();

            if(state.Cost > cost) { break; }
            if(state.Location.Position == endPosition)
            {
                cost = state.Cost;
                visited.insert(state.Location);
                costMap[state.Location] = state.Cost;
                parentMap[state.Location].push_back(state.PreviousLocation);
                processQueue.push(state.Location);
                continue;
            }

            if(visited.count(state.Location) == 0)
            {
                visited.insert(state.Location);
                costMap[state.Location] = state.Cost;
                parentMap[state.Location].push_back(state.PreviousLocation);

                State nextStates[] = { state.MoveForward(), state.MoveLeft(), state.MoveRight() };
                for(int i = 0; i < 3; i++)
                {
                    if(GetTile(nextStates[i].Location.Position) == Space)
                    {
                        stateQueue.push(nextStates[i]);
                    }
                }
            }
            else if (costMap[state.Location] == state.Cost)
            {
                parentMap[state.Location].push_back(state.PreviousLocation);
            }
        }

        bestTiles.insert(endPosition);
        
        while(!processQueue.empty())
        {
            ReindeerLocation current = processQueue.front();
            processQueue.pop();

            for(auto p : parentMap[current])
            {
                bestTiles.insert(p.Position);
                if(!(current.Position == p.Position))
                {
                    processQueue.push(p);
                }
            }
        }

        return bestTiles.size();
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

    sum = board.FindShortestPath();
    sum2 = board.CountTilesOnBestPaths();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}