#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<stack>
#include<unordered_set>
#include<algorithm>

using namespace std;

enum MovementType : char
{
    DirectionN = '^',
    DirectionS = 'v',
    DirectionW = '<',
    DirectionE = '>',
};

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
    Box = 'O',
    Robot = '@',
    Space = '.',
    LeftBox = '[',
    RightBox = ']',
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

    int GetCoordinates()
    {
        return 100 * y + x;
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

class ScaledBoard;

class Board
{
friend class ScaledBoard;
private:
    vector<vector<TileType>> board;
    Point robotPosition;
    vector<MovementType> movements;

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

    void DoMovements()
    {
        for(auto m : movements)
        {
            DoMovement(m);
        }
    }

    int64_t GetBoxCoordinates()
    {
        int64_t result = 0LL;

        for(int j = 0; j < board.size(); j++)
        {
            for(int i = 0; i < board[j].size(); i++)
            {
                if(board[j][i] == Box)
                {
                    result += 100*j + i;
                }
            }
        }

        return result;
    }

private:
    void AddLine(const char* line)
    {
        if(*line == Wall)
        {
            AddBoardLine(line);
        }
        else
        {
            AddMovementLine(line);
        }
    }

    void AddBoardLine(const char* line)
    {
        vector<TileType> row;

        for(int i = 0; line[i] != 0; i++)
        {
            switch(line[i])
            {
                case Robot:
                    robotPosition = Point(i, board.size());
                    row.push_back(Space);
                    break;
                case Space:
                    row.push_back(Space);
                    break;
                case Wall:
                    row.push_back(Wall);
                    break;
                case Box:
                    row.push_back(Box);
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

    void AddMovementLine(const char* line)
    {
        for(int i = 0; line[i] != 0; i++)
        {
            switch(line[i])
            {
                case DirectionN:
                case DirectionS:
                case DirectionE:
                case DirectionW:
                    movements.push_back((MovementType)line[i]);
                    break;
                default:
                    break;
            }
        }
    }

    void DoMovement(MovementType movement)
    {
        Point nextSpace = FindSpace(robotPosition, movement);

        if(!IsOnBoard(nextSpace)) { return; }

        MovementType backMovement = OppositeMovement(movement);
        Point current = Point(nextSpace, backMovement);

        do
        {
            SetTile(nextSpace, GetTile(current));
            current.Move(backMovement);
            nextSpace.Move(backMovement);
        } while (nextSpace.x != robotPosition.x || nextSpace.y != robotPosition.y);
        
        robotPosition.Move(movement);
    }

    Point FindSpace(const Point& point, MovementType movement)
    {
        Point current = Point(point, movement);

        while(GetTile(current) == Box)
        {
            current.Move(movement);
        }

        TileType tile = GetTile(current);
        if(tile == Space) { return current; }
        else return Point(-1,-1);
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
            : Space;
    }

    void SetTile(const Point& point, TileType value)
    {
        if(IsOnBoard(point))
        {
            board[point.y][point.x] = value;
        }
    }
};

class ScaledBoard
{
private:
    vector<vector<TileType>> board;
    Point robotPosition;
    vector<MovementType> movements;

public:
    ScaledBoard() { }

    ScaledBoard(const Board& board)
    {
        movements = vector<MovementType>(board.movements);
        robotPosition = Point(2 * board.robotPosition.x, board.robotPosition.y);

        for(auto row : board.board)
        {
            vector<TileType> newRow;

            for(auto tile : row)
            {
                switch(tile)
                {
                    case Wall:
                    case Space:
                        newRow.push_back(tile);
                        newRow.push_back(tile);
                        break;
                    case Box:
                        newRow.push_back(LeftBox);
                        newRow.push_back(RightBox);
                        break;
                    default:
                        break;
                }
            }

            this->board.push_back(newRow);
        }

        SetTile(robotPosition, Robot);
    }

    void DoMovements()
    {
        //Print();

        for(auto m : movements)
        {
            switch(m)
            {
                case DirectionE:
                case DirectionW:
                    DoHorizontalMovement(m);
                    break;
                case DirectionN:
                case DirectionS:
                    DoVerticalMovement(m);
                    break;
                default:
                    break;
            }

            //printf("\nMovement %c\n", m);
            //Print();
        }

        //Print();
    }

    int64_t GetBoxCoordinates()
    {
        int64_t result = 0LL;

        for(int j = 0; j < board.size(); j++)
        {
            for(int i = 0; i < board[j].size(); i++)
            {
                if(board[j][i] == LeftBox)
                {
                    result += 100*j + i;
                }
            }
        }

        return result;
    }

    void Print()
    {
        for(auto row : board)
        {
            for(auto tile : row)
            {
                putchar(tile);
            }
            putchar('\n');
        }
    }

private:
    void DoHorizontalMovement(MovementType movement)
    {
        Point nextSpace = FindSpace(robotPosition, movement);

        if(!IsOnBoard(nextSpace)) { return; }

        MovementType backMovement = OppositeMovement(movement);
        Point current = Point(nextSpace, backMovement);

        do
        {
            SetTile(nextSpace, GetTile(current));
            current.Move(backMovement);
            nextSpace.Move(backMovement);
        } while (nextSpace.x != robotPosition.x || nextSpace.y != robotPosition.y);
        
        SetTile(robotPosition, Space);
        robotPosition.Move(movement);
        SetTile(robotPosition, Robot);
    }

    Point FindSpace(const Point& point, MovementType movement)
    {
        Point current = Point(point, movement);
        TileType tile = GetTile(current);

        while(tile == LeftBox || tile == RightBox)
        {
            current.Move(movement);
            tile = GetTile(current);
        }

        if(tile == Space) { return current; }
        else return Point(-1,-1);
    }

    void DoVerticalMovement(MovementType movement)
    {
        stack<Point> toBeMoved;
        unordered_set<Point, PointHash>* currentSet = new unordered_set<Point, PointHash>();
        unordered_set<Point, PointHash>* nextSet = NULL;

        toBeMoved.push(robotPosition);
        currentSet->insert(robotPosition);
        bool canMove = true;

        while(!currentSet->empty() && canMove)
        {
            nextSet = new unordered_set<Point, PointHash>();

            for(auto p : *currentSet)
            {
                Point next = Point(p, movement);
                TileType tile = GetTile(next);

                switch(tile)
                {
                    case Wall:
                        canMove = false;
                        break;
                    case Space:
                        break;
                    case LeftBox:
                        nextSet->insert(next);
                        nextSet->insert(Point(next.x+1, next.y));
                        break;
                    case RightBox:
                        nextSet->insert(Point(next.x-1, next.y));
                        nextSet->insert(next);
                        break;
                    default:
                        break;
                }
            }

            for(auto p : *nextSet)
            {
                toBeMoved.push(p);
            }

            delete currentSet;
            currentSet = nextSet;
            nextSet = NULL;
        }

        delete currentSet;
        currentSet = NULL;

        if(!canMove) { return; }

        while(!toBeMoved.empty())
        {
            Point from = toBeMoved.top();
            toBeMoved.pop();
            Point to = Point(from, movement);

            SetTile(to, GetTile(from));
            SetTile(from, Space);
        }

        robotPosition.Move(movement);
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
            : Space;
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
    ScaledBoard scaledBoard(board);

    board.DoMovements();
    sum = board.GetBoxCoordinates();

    scaledBoard.DoMovements();
    sum2 = scaledBoard.GetBoxCoordinates();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}