#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<regex>
#include<algorithm>

using namespace std;

struct Robot
{
    int px;
    int py;
    int vx;
    int vy;

    Robot(int px, int py, int vx, int vy) : px(px), py(py), vx(vx), vy(vy) { }

    void Move(int width, int height, int steps)
    {
        px = (int)(((int64_t)px + (int64_t)vx * (int64_t)steps) % (int64_t)width);
        py = (int)(((int64_t)py + (int64_t)vy * (int64_t)steps) % (int64_t)height);

        if(px < 0) px += width;
        if(py < 0) py += height;
    }
};

class Board
{
    vector<Robot> robots;
    int width;
    int height;

public:
    Board(int width, int height) : width(width), height(height) {}

    Board(const Board& board)
    {
        width = board.width;
        height = board.height;
        robots = vector<Robot>(board.robots);
    }

    void Read(FILE* file)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];

        regex robotRegex ("p=(-?\\d+),(-?\\d+) v=(-?\\d+),(-?\\d+)\\n?");
        smatch match;
   
        while(fgets(buffer, bufferSize, file) != NULL)
        {
            string token(buffer);
            if(!regex_match(token, match, robotRegex)) { continue; }
            int px = stoi(match[1]);
            int py = stoi(match[2]);
            int vx = stoi(match[3]);
            int vy = stoi(match[4]);

            robots.push_back(Robot(px, py, vx, vy));
        }
    }

    void Move(int steps)
    {
        for(auto& r : robots)
        {
            r.Move(width, height, steps);
        }
    }

    int64_t CalculateQuadrants()
    {
        vector<int64_t> quadrant(5, 0LL);

        for(auto r : robots)
        {
            quadrant[FindQuadrant(r)]++;
        }

        return quadrant[1] * quadrant[2] * quadrant[3] * quadrant[4];
    }

    void Print(FILE* file)
    {
        vector<string> printBoard(103, string(101, ' '));

        for(auto r : robots)
        {
            printBoard[r.py][r.px] = '#';
        }

        for(auto row : printBoard)
        {
            fprintf(file, "%s\n", row.c_str());
        }
    }

private:
    int FindQuadrant(Robot& robot)
    {
        if(robot.px < width/2 && robot.py < height/2) return 1;
        if(robot.px < width/2 && robot.py > height/2) return 2;
        if(robot.px > width/2 && robot.py < height/2) return 3;
        if(robot.px > width/2 && robot.py > height/2) return 4;

        return 0;
    }
};

int main()
{
    Board board1(101, 103);
   
    board1.Read(stdin);
    Board board2(board1);

    board1.Move(100);
    int64_t result = board1.CalculateQuadrants();
    printf("%lld\n", result);

    board2.Move(33);

    for(int i = 33; i <= 101*103; i+=101)
    {
        char fileName[256];
        sprintf(fileName, "%06d.out", i);

        FILE* out = fopen(fileName, "w+");
        board2.Print(out);
        fclose(out);

        board2.Move(101);
    }

    return 0;
}