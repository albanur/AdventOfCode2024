#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<algorithm>
#include<unordered_set>
#include<unordered_map>

using namespace std;

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }

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
    unordered_map<char, vector<Point>> antennas;
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

    int CountAntinodes()
    {
        unordered_set<Point, PointHash> antinodes;
        Point point;

        for(auto f : antennas)
        {
            vector<Point>& locations = f.second;

            for(int i = 0; i < locations.size(); i++)
            {
                for(int j = i+1; j < locations.size(); j++)
                {
                    point = NextAntinode(locations[i], locations[j]);
                    if (IsOnBoard(point)) { antinodes.insert(point); }

                    point = NextAntinode(locations[j], locations[i]);
                    if (IsOnBoard(point)) { antinodes.insert(point); }
                }
            }
        }

        return antinodes.size();
    }

    int CountExtendedAntinodes()
    {
        unordered_set<Point, PointHash> antinodes;
        Point point1, point2, point3;

        for(auto f : antennas)
        {
            vector<Point>& locations = f.second;

            if(locations.size() < 2)
            {
                continue;
            }

            for(int i = 0; i < locations.size(); i++)
            {
                antinodes.insert(locations[i]);

                for(int j = i+1; j < locations.size(); j++)
                {
                    point1 = locations[i];
                    point2 = locations[j];
                    point3 = NextAntinode(point1, point2);
                    while(IsOnBoard(point3))
                    {
                        antinodes.insert(point3);
                        point1 = point2;
                        point2 = point3;
                        point3 = NextAntinode(point1, point2);
                    }

                    point1 = locations[j];
                    point2 = locations[i];
                    point3 = NextAntinode(point1, point2);
                    while(IsOnBoard(point3))
                    {
                        antinodes.insert(point3);
                        point1 = point2;
                        point2 = point3;
                        point3 = NextAntinode(point1, point2);
                    }
                }
            }
        }

        return antinodes.size();
    }


private:
    void AddLine(const char* line)
    {
        int size = 0;
        for(int i = 0; line[i] != 0; i++)
        {
            if (line[i] == '.')
            {
                size++;
                continue;
            }
            else if (isalnum(line[i]))
            {
                size++;
                AddAntena(line[i], Point(i, height));
            }
            else
            {
                continue;
            }
        }

        width = max(width, size);
        if (size > 0) { height++; }
    }

    void AddAntena(char frequency, Point position)
    {
        antennas[frequency].push_back(position);
    }

    bool IsOnBoard(const Point& point)
    {
        return point.x >= 0 && point.x < width
            && point.y >= 0 && point.y < height;
    }

    Point NextAntinode(const Point& p1, const Point& p2)
    {
        return Point(p2.x + (p2.x-p1.x), p2.y + (p2.y-p1.y));
    }
};


int main()
{
    int sum = 0;
    int sum2 = 0;
    Board board;

    board.Read(stdin);

    sum = board.CountAntinodes();
    sum2 = board.CountExtendedAntinodes();

    printf("%i\n", sum);
    printf("%i\n", sum2);

    return 0;
}