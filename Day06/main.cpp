#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
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

DirectionType TurnRight(DirectionType direction)
{
    switch(direction)
    {
        case DirectionN: return DirectionE;
        case DirectionE: return DirectionS;
        case DirectionS: return DirectionW;
        case DirectionW: return DirectionN;
        default: return direction;
    }
};

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }

    Point(const Point& pt) : x(pt.x), y(pt.y) { }

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

struct State
{
    Point position;
    DirectionType direction;

    State(Point position, DirectionType direction) : position(position), direction(direction) { }

    State Next()
    {
        State state(*this);
        state.Move();
        return state;
    }

    void Move()
    {
        position.Move(direction);
    }

    void TurnRight()
    {
        switch(direction)
        {
            case DirectionN: direction=DirectionE; break;
            case DirectionE: direction=DirectionS; break;
            case DirectionS: direction=DirectionW; break;
            case DirectionW: direction=DirectionN; break;
            default: break;
        }
    }

    size_t Hash() const
    {
        return ((size_t)direction << 32) ^ ((size_t)position.x << 16) ^ (size_t)position.y;
    }
};

bool operator < (const State& s1, const State& s2)
{
    return (s1.position.x < s2.position.x)
        || (s1.position.x == s2.position.x && s1.position.y < s2.position.y)
        || (s1.position.x == s2.position.x && s1.position.y == s2.position.y && (int)s1.direction < (int)s2.direction);
}

bool operator == (const State& s1, const State& s2)
{
    return s1.position.x == s2.position.x
        && s1.position.y == s2.position.y
        && s1.direction == s2.direction;
}

struct StateHash {
   size_t operator()(const State &state) const
   {
        return state.Hash();
   }
};

class Board
{
private:
    vector<vector<char>> board;
    Point guardPosition;

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

    int CountGuardedTiles()
    {
        unordered_set<Point, PointHash> visited;

        State state(guardPosition, DirectionN);

        while(IsOnBoard(state.position))
        {
            visited.insert(state.position);
            
            State next = state.Next();
            while(GetTile(next.position) == '#')
            {
                state.TurnRight();
                next = state.Next();
            }

            state = next;
        }

        return visited.size();
    }

    int CountObstructionPlacements()
    {
        int sum = 0;
        unordered_set<Point, PointHash> positionVisited;
        unordered_set<State, StateHash> stateVisited;

        State state(guardPosition, DirectionN);

        while(IsOnBoard(state.position))
        {
            positionVisited.insert(state.position);
            stateVisited.insert(state);

            if(positionVisited.size() % 100 == 0)
            {
                printf("%d\n", (int)positionVisited.size());
                fflush(stdout);
            }

            State next = state.Next();
            if(GetTile(next.position) == '.')
            {
                if(positionVisited.count(next.position) == 0)
                {
                    if(DoesObstructionCausesLoop(state, stateVisited))
                    {
                        sum++;
                    }
                }
            }
            else
            {
                while(GetTile(next.position) == '#')
                {
                    state.TurnRight();
                    next = state.Next();
                }
            }
            state = next;
        }

        return sum;
    }

    int CountObstructionPlacements2()
    {
        int sum = 0;
        vector<Point> placements;

        GetPossibleObstructionPlacements(placements);
        for(int i = 0; i < placements.size(); i++)
        {
            bool loop = DoesObstructionCausesLoop(placements[i]);
            if(loop)
            {
                sum++;
            }
        }

        return sum;
    }

private:
    void AddLine(const char* line)
    {
        vector<char> row;

        for(int i = 0; line[i] != 0; i++)
        {
            switch(line[i])
            {
                case '^':
                    guardPosition = Point(i, board.size());
                    row.push_back('.');
                    break;
                case '#':
                case '.':
                    row.push_back(line[i]);
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

    bool DoesObstructionCausesLoop(State& state, unordered_set<State, StateHash> previouslyVisited)
    {
        State next = state.Next();
        Point obstruction = next.position;
        bool loop = false;

        SetTile(obstruction, '#');
        unordered_set<State, StateHash> recentlyVisited;

        while(IsOnBoard(state.position))
        {
            if(previouslyVisited.count(state) > 0 || recentlyVisited.count(state) > 0)
            {
                loop = true;
                break;
            }

            recentlyVisited.insert(state);
            
            State next = state.Next();
            while(GetTile(next.position) == '#')
            {
                state.TurnRight();
                next = state.Next();
            }

            state = next;
        }

        SetTile(obstruction, '.');
        return loop;
    }

    void GetPossibleObstructionPlacements(vector<Point>& placements)
    {
        placements.clear();

        unordered_set<Point, PointHash> visited;

        Point position = guardPosition;
        DirectionType direction = DirectionN;

        while(IsOnBoard(position))
        {
            visited.insert(position);
            
            Point next(position, direction);
            while(GetTile(next) == '#')
            {
                direction = TurnRight(direction);
                next = Point(position, direction);
            }

            position = next;
        }

        visited.erase(guardPosition);
        placements.resize(visited.size());
        copy(visited.begin(), visited.end(), placements.begin());
    }

    bool DoesObstructionCausesLoop(Point& obstruction)
    {
        if(GetTile(obstruction) == '#') { return false; }
        SetTile(obstruction, '#');

        bool loop = false;
        unordered_set<State, StateHash> visited;

        State state(guardPosition, DirectionN);

        while(IsOnBoard(state.position))
        {
            if(visited.count(state) > 0)
            {
                loop = true;
                break;
            }

            visited.insert(state);
            
            State next = state;
            next.Move();
            while(GetTile(next.position) == '#')
            {
                state.TurnRight();
                next = state;
                next.Move();
            }

            state = next;
        }

        SetTile(obstruction, '.');
        return loop;
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

    void SetTile(const Point& point, char value)
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
    sum = board.CountGuardedTiles();
    sum2 = board.CountObstructionPlacements();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}