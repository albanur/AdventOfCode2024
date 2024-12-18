#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<stack>
#include<algorithm>

using namespace std;

struct PartialEquation
{
    uint64_t Result;
    int Length;

    PartialEquation(uint64_t result, int length) : Result(result), Length(length) { }
};

class Calibration
{
private:
    uint64_t expectedResult;
    vector<uint64_t> values;

public:
    Calibration() { }

    bool Read(const char* line)
    {
        char* ptr;
        int length = strlen(line);
        if(length <= 1) { return false; }

        values.clear();
        
        expectedResult = strtoll(line, &ptr, 10);

        while(ptr < line + length)
        {
            while(!isdigit(*ptr) && ptr < line + length) { ptr++; }
            if(ptr >= line + length) { break; }
            
            long long value = strtoll(ptr, &ptr, 10);
            values.push_back(value);
        }
    }

    uint64_t GetCalibrationResult()
    {
        stack<PartialEquation> equations;
        equations.push(PartialEquation(expectedResult, values.size()));

        while(!equations.empty())
        {
            PartialEquation partialEquation = equations.top();
            equations.pop();

            if(partialEquation.Length == 1)
            {
                if(partialEquation.Result == values[0])
                {
                    return expectedResult;
                }
            }
            else if(partialEquation.Length == 2)
            {
                if(partialEquation.Result == values[0] + values[1]
                    || partialEquation.Result == values[0] * values[1])
                {
                    return expectedResult;
                }
            }
            else
            {
                int value = values[partialEquation.Length - 1];

                if(partialEquation.Result > value)
                {
                    equations.push(PartialEquation(partialEquation.Result - value, partialEquation.Length - 1));
                }

                if((partialEquation.Result % value) == 0)
                {
                    equations.push(PartialEquation(partialEquation.Result / value, partialEquation.Length - 1));
                }
            }
        }

        return 0LL;
    }

    uint64_t GetCalibrationResult2()
    {
        stack<PartialEquation> equations;
        equations.push(PartialEquation(expectedResult, values.size()));

        while(!equations.empty())
        {
            PartialEquation partialEquation = equations.top();
            equations.pop();

            if(partialEquation.Length == 1)
            {
                if(partialEquation.Result == values[0])
                {
                    return expectedResult;
                }
            }
            else
            {
                int value = values[partialEquation.Length - 1];

                if(partialEquation.Result > value)
                {
                    equations.push(PartialEquation(partialEquation.Result - value, partialEquation.Length - 1));
                }

                if((partialEquation.Result % value) == 0)
                {
                    equations.push(PartialEquation(partialEquation.Result / value, partialEquation.Length - 1));
                }

                uint64_t divider = GetConcatenationDivider(value);

                if((partialEquation.Result % divider) == value)
                {
                    equations.push(PartialEquation(partialEquation.Result / divider, partialEquation.Length - 1));
                }
            }
        }

        return 0LL;
    }

private:
    uint64_t GetConcatenationDivider(uint64_t value)
    {
        uint64_t divider = 10LL;
        while(divider <= value)
        {
            divider *= 10LL;
        }
    }
};

int main()
{
    const int bufferSize = 1024;
    char buffer[bufferSize];
    uint64_t sum = 0;
    uint64_t sum2 = 0;

    Calibration calibration;

    while(fgets(buffer, bufferSize, stdin) != NULL)
    {
        if(calibration.Read(buffer))
        {
            uint64_t result = calibration.GetCalibrationResult();
            sum += result;

            result = calibration.GetCalibrationResult2();
            sum2 += result;
        }
    }

    printf("%lld\n", sum);
    printf("%lld\n", sum2);

    return 0;
}