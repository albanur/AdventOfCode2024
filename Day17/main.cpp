#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<queue>
#include<regex>
#include<algorithm>

using namespace std;

typedef unsigned char ubyte;
typedef long long int64;

class Emulator
{
private:
    int64 registerA;
    int64 registerB;
    int64 registerC;
    vector<ubyte> program;
    string output;
    vector<ubyte> outputProgram;
    int instructionPointer;

public:
    Emulator() { }

    void Read(FILE* file)
    {
        const int bufferSize = 1024;
        char buffer[bufferSize];

        regex registerRegex ("Register [ABC]: (\\d+)\\n?");
        cmatch match;

        fgets(buffer, bufferSize, file);
        regex_match(buffer, match, registerRegex);
        registerA = stoll(match[1]);

        fgets(buffer, bufferSize, file);
        regex_match(buffer, match, registerRegex);
        registerB = stoll(match[1]);

        fgets(buffer, bufferSize, file);
        regex_match(buffer, match, registerRegex);
        registerC = stoll(match[1]);

        fgets(buffer, bufferSize, file);
        fgets(buffer, bufferSize, file);

        char* token = strtok(buffer, " ,");
        token = strtok(NULL, " ,");
        while (token != NULL)
        {
            program.push_back((ubyte)stoi(token));
            token = strtok(NULL, " ,");
        }
    }

    void RunProgram()
    {
        instructionPointer = 0;
        while(ProcessOperation()) { }
    }

    void RunProgram(int64 startA)
    {
        registerA = startA;
        registerB = registerC = 0LL;
        instructionPointer = 0;
        output.clear();
        outputProgram.clear();
        while(ProcessOperation()) { }
    }

    void PrintOutput(FILE* file)
    {
        printf("%s\n", output.c_str());
    }

    int64 RevertProgram()
    {
        queue<int64> startA;
        startA.push(0LL);
        queue<int64> nextA;

        for(int position = program.size() - 1; position >= 0; position--)
        {
            while(!startA.empty())
            {
                int64 prefixA = startA.front();
                startA.pop();

                for(int64 i = 0LL; i < 8LL; i++)
                {
                    int64 currentA = (prefixA << 3) + i;
                    RunProgram(currentA);

                    if(outputProgram[0] == program[position])
                    {
                        nextA.push(currentA);
                    }
                }
            }

            while(!nextA.empty())
            {
                int64 value = nextA.front();
                nextA.pop();
                startA.push(value);
            }
        }

        return startA.front();
    }

private:
    bool ProcessOperation()
    {
        if(instructionPointer >= program.size()) { return false; }

        switch(program[instructionPointer])
        {
            case 0: adv(); break;
            case 1: bxl(); break;
            case 2: bst(); break;
            case 3: jnz(); break;
            case 4: bxc(); break;
            case 5: out(); break;
            case 6: bdv(); break;
            case 7: cdv(); break;
        }

        instructionPointer+=2;
        return true;
    }

    void adv()
    {
        int64 operand = GetComboOperand();

        registerA = operand >= 63
            ? 0
            : registerA / (1 << operand);
    }

    void bxl()
    {
        int64 operand = GetLiteralOperand();
        registerB ^= operand;
    }

    void bst()
    {
        int64 operand = GetComboOperand();
        registerB = operand % 8;
    }

    void jnz()
    {
        int64 operand = GetLiteralOperand();

        if(registerA != 0)
        {
            instructionPointer = operand;
            instructionPointer -= 2;
        }
    }

    void bxc()
    {
        registerB ^= registerC;
    }

    void out()
    {
        int64 operand = GetComboOperand();

        if(!output.empty())
        {
            output.push_back(',');
        }
        output.push_back('0' + (operand % 8));
        outputProgram.push_back(operand % 8);
    }

    void bdv()
    {
        int64 operand = GetComboOperand();

        registerB = operand >= 63
            ? 0
            : registerA / (1 << operand);
    }

    void cdv()
    {
        int64 operand = GetComboOperand();

        registerC = operand >= 63
            ? 0
            : registerA / (1 << operand);
    }

    int64 GetLiteralOperand()
    {
        return program[instructionPointer+1];
    }

    int64 GetComboOperand()
    {
        switch(program[instructionPointer+1])
        {
            case 0:
            case 1:
            case 2:
            case 3:
                return program[instructionPointer+1];
            case 4:
                return registerA;
            case 5:
                return registerB;
            case 6:
                return registerC;
            default:
                return -1;
        }
    }
};

int main()
{
    Emulator emulator;

    emulator.Read(stdin);
    emulator.RunProgram();
    emulator.PrintOutput(stdout);

    int64 startA = emulator.RevertProgram();
    printf("%lld\n", startA);
    emulator.RunProgram(startA);
    emulator.PrintOutput(stdout);

    return 0;
}