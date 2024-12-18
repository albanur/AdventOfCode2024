#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<map>
#include<algorithm>
#include<regex>

using namespace std;

enum DirectionType : int
{
    DirectionN = 0,
    DirectionS = 1,
    DirectionW = 2,
    DirectionE = 3,
    DirectionNW = 4,
    DirectionNE = 5,
    DirectionSW = 6,
    DirectionSE = 7,

    DirectionMin = 0,
    DirectionMax = 7,
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
            case DirectionNW:
                x--; y--; break;
            case DirectionNE:
                x++; y--; break;
            case DirectionSW:
                x--; y++; break;
            case DirectionSE:
                x++; y++; break;
            default:
               break;
        }
    }
};

class Board
{
private:
    vector<vector<char>> board;

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

    int CountXmas()
    {
        int sum = 0;
        Point start;

        for(int j = 0; j < board.size(); j++)
        {
            for(int i = 0; i < board[j].size(); i++)
            {
                start = Point(i, j);
                if(GetTile(start) != 'X') { continue; }

                for(DirectionType d = DirectionMin; d <= DirectionMax; d = (DirectionType)(d+1))
                {
                    if(IsXmas(start, d))
                    {
                        sum++;
                    }
                }
            }
        }

        return sum;
    }

    int CountX_Mas()
    {
        int sum = 0;

        for(int j = 1; j < board.size()-1; j++)
        {
            for(int i = 1; i < board[j].size()-1; i++)
            {
                if(IsMas(Point(i, j)))
                {
                    sum++;
                }
            }
        }

        return sum;
    }

private:
    void AddLine(const char* line)
    {
        vector<char> row;
        const char* ptr = line;

        while(*ptr != 0)
        {
            switch(*ptr)
            {
                case 'X':
                case 'M':
                case 'A':
                case 'S':
                    row.push_back(*ptr);
                default:
                    ptr++;
            }
        }

        if(row.size() > 0)
        {
            board.push_back(row);
        }
    }

    bool IsXmas(const Point& start, DirectionType direction)
    {
        Point point(start);

        if(GetTile(point) != 'X') { return false; }
        point.Move(direction);
        if(GetTile(point) != 'M') { return false; }
        point.Move(direction);
        if(GetTile(point) != 'A') { return false; }
        point.Move(direction);
        if(GetTile(point) != 'S') { return false; }

        return true;
    }

    bool IsMas(const Point& start)
    {
        if(GetTile(start) != 'A') { return false; }

        bool diag1 =
            (GetTile(Point(start, DirectionNE)) == 'M' && GetTile(Point(start, DirectionSW)) == 'S')
            ||
            (GetTile(Point(start, DirectionNE)) == 'S' && GetTile(Point(start, DirectionSW)) == 'M');

        bool diag2 =
            (GetTile(Point(start, DirectionNW)) == 'M' && GetTile(Point(start, DirectionSE)) == 'S')
            ||
            (GetTile(Point(start, DirectionNW)) == 'S' && GetTile(Point(start, DirectionSE)) == 'M');

        return diag1 && diag2;
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
            : '_';
    }
};

int main()
{
    Board board;
    int sum = 0;
    int sum2 = 0;

    board.Read(stdin);
    sum = board.CountXmas();
    sum2 = board.CountX_Mas();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}