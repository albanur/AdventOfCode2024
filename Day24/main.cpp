#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<queue>
#include<algorithm>
#include<regex>

using namespace std;

struct NodeLevel
{
    string Label;
    int Level;

    NodeLevel(string label, int level) : Label(label), Level(level) {}
};

bool operator < (const NodeLevel& l1, const NodeLevel& l2)
{
    return l1.Level > l2.Level;
}

enum OperationType
{
    OperationAND,
    OperationOR,
    OperationXOR,
};

struct Operation
{
    string Input1;
    string Input2;
    string Output;
    OperationType Type;

    Operation() : Input1(""), Input2(""), Output(""), Type(OperationAND) { }

    Operation(string in1, string in2, string out, OperationType type)
        : Input1(in1), Input2(in2), Output(out), Type(type) { }
};

class Calculator
{
private:
    unordered_set<string> nodes;
    unordered_map<string, bool> values;
    unordered_map<string, int> levels;
    unordered_multimap<int, string> nodesByLevel;

    unordered_map<string, Operation> operations;
    unordered_multimap<string, string> operationsByInput;

public:
    Calculator() {}

    void Read(FILE* file)
    {
        const int bufferSize = 256;
        char buffer[bufferSize];

        while(fgets(buffer, bufferSize, stdin) != NULL)
        {
            ReadLine(buffer);
        }
    }

    void DoCalculations()
    {
        for(int i = 0; i < 63; i++)
        {
            CalculateAndAssignLevel(i);
        }
    }

    vector<string> DoCalculations2()
    {
        vector<string> invalidNodes;
        priority_queue<NodeLevel> toCalculate;

        AssignInitialLevels();

        for(auto v : values)
        {
            auto range = operationsByInput.equal_range(v.first);
            for(auto it = range.first; it != range.second; ++it)
            {
                Operation& op = operations[it->second];
                if(CanCalculate(op))
                {
                    toCalculate.push(NodeLevel(op.Output, levels[it->first]));
                }
            }
        }

        while(!toCalculate.empty())
        {
            NodeLevel nl = toCalculate.top();
            toCalculate.pop();

            if(values.count(nl.Label) > 0) { continue; }

            Operation& op = operations[nl.Label];
            Calculate(op.Output);

            if(abs(nl.Level - levels[op.Input1]) > 1)
            {
                invalidNodes.push_back(op.Input1);
            }
            else if(abs(nl.Level - levels[op.Input2]) > 1)
            {
                invalidNodes.push_back(op.Input2);
            }
            else if (abs(levels[op.Input1] - levels[op.Input2]) > 1)
            {
                invalidNodes.push_back(op.Input1);
                invalidNodes.push_back(op.Input2);
            }

            if(levels.count(op.Output) == 0)
            {
                levels[op.Output] = max(levels[op.Input1], levels[op.Input2]);
            }
            else if (levels[op.Output] != max(levels[op.Input1], levels[op.Input2]))
            {
                invalidNodes.push_back(op.Output);
            }

            auto range = operationsByInput.equal_range(op.Output);
            for(auto it = range.first; it != range.second; ++it)
            {
                Operation& op = operations[it->second];
                if(CanCalculate(op))
                {
                    toCalculate.push(NodeLevel(op.Output, levels[it->first]));
                }
            }
        }

        return invalidNodes;
    }

    int64_t CalculateZ()
    {
        int64_t zValue = 0LL;
        
        const int bufferSize = 256;
        char buffer[bufferSize];

        for(int i = 0; i < 64; i++)
        {
            sprintf(buffer, "z%02d", i);
            string node(buffer);
            if(values.count(node) == 0) { break; }

            bool value = values[node];
            if(value) { zValue += (1LL << i); }
        }

        return zValue;
    }

    vector<string> FixCalculator()
    {
        vector<string> result;
        unordered_set<string> goodLabels;
        unordered_set<string> badLabels;
        unordered_set<string> levelLabels;

        for(int l = 0; l < 63; l++)
        {
            bool levelResult = PerformTestForLevel(l, goodLabels, badLabels);
            if(!levelResult)
            {
                vector<string> swap1 (badLabels.begin(), badLabels.end());
                vector<string> swap2;

                GetOperationIdsByLevel(l, levelLabels);
                swap2.insert(swap2.end(), levelLabels.begin(), levelLabels.end());

                vector<pair<string, string>> possibleSwaps;

                for(auto s1 : swap1)
                {
                    for(auto s2 : swap2)
                    {
                        if(s1 == s2) { continue; }

                        SwapOutputs(s1, s2);
                        if(ContainsLoop(s1) || ContainsLoop(s2))
                        {
                            SwapOutputs(s1, s2);
                            continue;
                        }

                        bool newResult = PerformTestForLevel(l, goodLabels, badLabels);
                        if(newResult)
                        {
                            possibleSwaps.push_back(make_pair(s1, s2));
                        }
                        SwapOutputs(s1, s2);
                    }
                }

                if(possibleSwaps.size() > 0)
                {
                    result.push_back(possibleSwaps[0].first);
                    result.push_back(possibleSwaps[0].second);
                    SwapOutputs(possibleSwaps[0].first, possibleSwaps[0].second);
                }
            }
        }

        return result;
    }

    vector<string> FixCalculator2()
    {
        int failing = FailingTests();
        SwapOutputs("gjc", "qjj");
        failing = FailingTests();
        SwapOutputs("wmp", "z17");
        failing = FailingTests();
        SwapOutputs("gvm", "z26");
        failing = FailingTests();
        SwapOutputs("qsb", "z39");
        failing = FailingTests();

        vector<pair<string,string>> result;

        vector<string> allOutputs;
        for(auto& op : operations) { allOutputs.push_back(op.first); }
        
        while(failing > 0)
        {
            int bestScore = failing;
            pair<string,string> bestPair;

            for(auto it1 = allOutputs.begin(); it1!=allOutputs.end(); ++it1)
            {
                auto it2 = it1;
                for(++it2; it2!=allOutputs.end(); ++it2)
                {
                    SwapOutputs(*it1, *it2);
                    if(ContainsLoop(*it1) || ContainsLoop(*it2))
                    {
                        SwapOutputs(*it1, *it2);
                        continue;
                    }

                    int score = FailingTests(bestScore);
                    if(score < bestScore)
                    {
                        bestScore = score;
                        bestPair = make_pair(*it1, *it2);
                    }

                    SwapOutputs(*it1, *it2);
                }
            }

            if(bestScore == failing) { break; }

            failing = bestScore;
            result.push_back(bestPair);
            SwapOutputs(bestPair.first, bestPair.second);
        }

        return allOutputs;
    }

    void Print(char b)
    {
        string result;

        const int bufferSize = 256;
        char buffer[bufferSize];

        for(int i = 63; i >= 0; i--)
        {

            sprintf(buffer, "%c%02d", b, i);
            string node(buffer);
            if(values.count(node) == 0)
            {
                result.push_back(' ');
            }
            else
            {
                bool value = values[node];
                result.push_back(value ? '1' : '0');
            }
        }

        printf("%s\n", result.c_str());
    }

private:
    void ReadLine(const char* line)
    {
        regex valueRegex ("([a-z0-9]{3}): ([01])\\n?");
        regex operationRegex ("([a-z0-9]{3}) ([ANDXOR]{2,3}) ([a-z0-9]{3}) -> ([a-z0-9]{3})\\n?");
        cmatch valueMatch;
        cmatch operationMatch;

        if (regex_match(line, valueMatch, valueRegex))
        {
            string node(valueMatch[1]);
            bool value = valueMatch[2].str()[0] == '1';

            values[node] = value;
            nodes.insert(node);
        }
        else if (regex_match(line, operationMatch, operationRegex))
        {
            OperationType type;
            if(operationMatch[2].str()[0] == 'A') { type = OperationAND; }
            else if(operationMatch[2].str()[0] == 'O') { type = OperationOR; }
            else { type = OperationXOR; }

            Operation operation (
                string(operationMatch[1].str()),
                string(operationMatch[3].str()),
                string(operationMatch[4].str()),
                type
            );

            nodes.insert(operation.Input1);
            nodes.insert(operation.Input2);
            nodes.insert(operation.Output);

            operations.insert(make_pair(operation.Output, operation));
            operationsByInput.insert(make_pair(operation.Input1, operation.Output));
            operationsByInput.insert(make_pair(operation.Input2, operation.Output));
        }
    }

    void SwapOutputs(const string& label1, const string& label2)
    {
        Operation op1 = operations[label1];
        Operation op2 = operations[label2];

        operations.erase(op1.Output);
        operations.erase(op2.Output);
        RemoveOperationByInput(op1.Input1, op1.Output);
        RemoveOperationByInput(op1.Input2, op1.Output);
        RemoveOperationByInput(op2.Input1, op2.Output);
        RemoveOperationByInput(op2.Input2, op2.Output);

        op1.Output = label2;
        op2.Output = label1;

        operations.insert(make_pair(op1.Output, op1));
        operations.insert(make_pair(op2.Output, op2));
        operationsByInput.insert(make_pair(op1.Input1, op1.Output));
        operationsByInput.insert(make_pair(op1.Input2, op1.Output));
        operationsByInput.insert(make_pair(op2.Input1, op2.Output));
        operationsByInput.insert(make_pair(op2.Input2, op2.Output));
    }

    bool ContainsLoop(const string& startLabel)
    {
        queue<string> processingQueue;

        if(operations.count(startLabel) > 0)
        {
            Operation& op = operations[startLabel];
            processingQueue.push(op.Input1);
            processingQueue.push(op.Input2);
        }

        while(!processingQueue.empty())
        {
            string label = processingQueue.front();
            processingQueue.pop();

            if(label == startLabel) { return true; }

            if(operations.count(label) > 0)
            {
                Operation& op = operations[label];
                processingQueue.push(op.Input1);
                processingQueue.push(op.Input2);
            }
        }

        return false;
    }

    void RemoveOperationByInput(string& input, string& output)
    {
        auto range = operationsByInput.equal_range(input);

        auto it = range.first;
        while(it != range.second)
        {
            if(it->second == output) { break; }
            ++it;
        }

        if(it->second == output)
        {
            operationsByInput.erase(it);
        }
    }

    void ReCalculate(unordered_set<string>& inputs, unordered_set<string>& affectedLabels)
    {
        affectedLabels.clear();

        queue<string> toRecalculate;
        vector<string> ids;

        for(auto i : inputs)
        {
            GetOperationIdsWithInput(i, ids);
            for(string id : ids) { toRecalculate.push(id); }
        }

        while(!toRecalculate.empty())
        {
            string label = toRecalculate.front();
            toRecalculate.pop();

            bool value = Calculate(label);
            if(values[label] != value)
            {
                values[label] = value;
                affectedLabels.insert(label);
                GetOperationIdsWithInput(label, ids);
                for(string id : ids) { toRecalculate.push(id); }
            }
        }
    }

    bool PerformTestForLevel(int level, unordered_set<string>& goodLabels, unordered_set<string>& badLabels)
    {
        string label = GetLabel('z', level);
        if(nodes.count(label) == 0) { return true; }

        bool result = true;
        unordered_set<string> affectedLabels;
        goodLabels.clear();
        badLabels.clear();

        if(CanPerformPerform1Plus0Test(level))
        {
            bool result1Plus0 = Perform1Plus0Test(level, affectedLabels);
            if(result1Plus0)
            {
                goodLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            else
            {
                badLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            result = result && result1Plus0;
        }

        if(CanPerformPerform1Plus1Test(level))
        {
            bool result1Plus1 = Perform1Plus1Test(level, affectedLabels);
            if(result1Plus1)
            {
                goodLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            else
            {
                badLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            result = result && result1Plus1;
        }

        if(CanPerformPerform1Plus1Test(level+1))
        {
            bool result1Plus1 = Perform1Plus1Test(level+1, affectedLabels);
            if(result1Plus1)
            {
                goodLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            else
            {
                badLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            result = result && result1Plus1;
        }

        if(CanPerformPerform11Plus01Test(level))
        {
            bool result11Plus01 = Perform11Plus01Test(level, affectedLabels);
            if(result11Plus01)
            {
                goodLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            else
            {
                badLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            result = result && result11Plus01;
        }

        if(CanPerformPerform11Plus01Test(level+1))
        {
            bool result11Plus01 = Perform11Plus01Test(level+1, affectedLabels);
            if(result11Plus01)
            {
                goodLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            else
            {
                badLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            result = result && result11Plus01;
        }

        if(CanPerformPerform11Plus01Test(level+2))
        {
            bool result11Plus01 = Perform11Plus01Test(level+2, affectedLabels);
            if(result11Plus01)
            {
                goodLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            else
            {
                badLabels.insert(affectedLabels.begin(), affectedLabels.end());
            }
            result = result && result11Plus01;
        }

        return result;
    }

    int FailingTests(int max = INT32_MAX)
    {
        int result = 0;
        unordered_set<string> affectedLabels;

        for(int l = 0; l < 63; l++)
        {
            string label = GetLabel('z', l);
            if(nodes.count(label) == 0) { continue; }

            if(CanPerformPerform1Plus0Test(l))
            {
                bool result1Plus0 = Perform1Plus0Test(l, affectedLabels);
                if(!result1Plus0) { result++; printf("L %d Test 1+0\n", l); }
            }

            if(CanPerformPerform1Plus1Test(l))
            {
                bool result1Plus1 = Perform1Plus1Test(l, affectedLabels);
                if(!result1Plus1) { result++; printf("L %d Test 1+1\n", l); }
            }

            if(CanPerformPerform11Plus01Test(l))
            {
                bool result11Plus01 = Perform11Plus01Test(l, affectedLabels);
                if(!result11Plus01) { result++; printf("L %d Test 11+01\n", l); }
            }

            if(result > max) { return result; }
        }
        
        return result;
    }

    void ZeroAllValues()
    {
        for(auto n : nodes)
        {
            values[n] = false;
        }

        // unordered_set<string> inputs;
        // unordered_set<string> affectedLabels;

        // for(int l = 0; l < 63; l++)
        // {
        //     string xLabel = GetLabel('x', l);
        //     if(nodes.count(xLabel) > 0 && values[xLabel] == true)
        //     {
        //         values[xLabel] = false;
        //         inputs.insert(xLabel);
        //     }

        //     string yLabel = GetLabel('y', l);
        //     if(nodes.count(yLabel) > 0 && values[yLabel] == true)
        //     {
        //         values[yLabel] = false;
        //         inputs.insert(yLabel);
        //     }
        // }

        // ReCalculate(inputs, affectedLabels);
    }

    bool CanPerformPerform1Plus0Test(int level)
    {
        string xLabel = GetLabel('x', level);
        string zLabel = GetLabel('z', level);
        return (nodes.count(xLabel) > 0 && nodes.count(zLabel) > 0);
    }

    bool Perform1Plus0Test(int level, unordered_set<string>& affectedLabels)
    {
        unordered_set<string> inputs;
        bool result = true;

        ZeroAllValues();

        string xLabel = GetLabel('x', level);
        values[xLabel] = true;
        inputs.insert(xLabel);

        affectedLabels.clear();
        ReCalculate(inputs, affectedLabels);

        for(int l = 0; l < 63; l++)
        {
            string label = GetLabel('z', l);
            if(nodes.count(label) == 0) { continue; }

            if(values[label] != (l == level))
            {
                result = false;
                break;
            }
        }

        return result;
    }

    bool CanPerformPerform1Plus1Test(int level)
    {
        string xLabel = GetLabel('x', level-1);
        string zLabel = GetLabel('z', level);
        return (level >= 1 && nodes.count(xLabel) > 0 && nodes.count(zLabel) > 0);
    }

    bool Perform1Plus1Test(int level, unordered_set<string>& affectedLabels)
    {
        unordered_set<string> inputs;
        bool result = true;

        ZeroAllValues();

        string xLabel = GetLabel('x', level-1);
        values[xLabel] = true;
        inputs.insert(xLabel);

        string yLabel = GetLabel('y', level-1);
        values[yLabel] = true;
        inputs.insert(yLabel);

        affectedLabels.clear();
        ReCalculate(inputs, affectedLabels);

        for(int l = 0; l < 63; l++)
        {
            string label = GetLabel('z', l);
            if(nodes.count(label) == 0) { continue; }

            if(values[label] != (l == level))
            {
                result = false;
                break;
            }
        }

        return result;
    }

    bool CanPerformPerform11Plus01Test(int level)
    {
        string xLabel = GetLabel('x', level-2);
        string zLabel = GetLabel('z', level);
        return (level >= 2 && nodes.count(xLabel) > 0 && nodes.count(zLabel) > 0);
    }

    bool Perform11Plus01Test(int level, unordered_set<string>& affectedLabels)
    {
        unordered_set<string> inputs;
        bool result = true;

        ZeroAllValues();

        string xLabel1 = GetLabel('x', level-1);
        values[xLabel1] = true;
        inputs.insert(xLabel1);

        string xLabel2 = GetLabel('x', level-2);
        values[xLabel2] = true;
        inputs.insert(xLabel2);

        string yLabel = GetLabel('y', level-2);
        values[yLabel] = true;
        inputs.insert(yLabel);

        affectedLabels.clear();
        ReCalculate(inputs, affectedLabels);

        for(int l = 0; l < 63; l++)
        {
            string label = GetLabel('z', l);
            if(nodes.count(label) == 0) { continue; }

            if(values[label] != (l == level))
            {
                result = false;
                break;
            }
        }

        return result;
    }

    void CalculateAndAssignLevel(int level)
    {
        stack<string> toCalculate;
        string label = GetLabel('z', level);
        if(nodes.count(label) > 0) { toCalculate.push(label); }

        while(!toCalculate.empty())
        {
            string node = toCalculate.top();
            toCalculate.pop();

            if(values.count(node) > 0) { continue; }

            const Operation& operation = operations[node];
            if(CanCalculate(operation))
            {
                bool value = Calculate(node);
                values[operation.Output] = value;
                AssignLevel(operation.Output, level);
            }
            else
            {
                toCalculate.push(operation.Output);
                toCalculate.push(operation.Input2);
                toCalculate.push(operation.Input1);
            }
        }
    }

    bool CanCalculate(const Operation& operation)
    {
        return values.count(operation.Input1) > 0 && values.count(operation.Input2) > 0;
    }

    bool Calculate(const string& label)
    {
        Operation& operation = operations[label];
        if(!CanCalculate(operation)) { return false; }

        switch(operation.Type)
        {
            case OperationAND:
                return values[operation.Input1] && values[operation.Input2];
            case OperationOR:
                return values[operation.Input1] || values[operation.Input2];
            case OperationXOR:
                return values[operation.Input1] ^ values[operation.Input2];
            default:
                return false;
        }
    }

    void AssignInitialLevels()
    {
        for(auto n : nodes)
        {
            int level = GetLevel(n);
            if(level != -1)
            {
                levels[n] = level;
            }
        }
    }

    int GetLevel(string label)
    {
        char c = label[0];
        return (c != 'x' && c != 'y' && c != 'z')
            ? -1
            : stoi(label.substr(1, 2));
    }

    string GetLabel(char b, int i)
    {
        string result;
        result.push_back(b);
        result.push_back('0' + ((i/10)%10));
        result.push_back('0' + (i%10));
        return result;
    }

    void AssignLevel(const string& label, int level)
    {
        levels[label] = level;
        nodesByLevel.insert(make_pair(level, label));
    }

    void GetOperationIdsWithInput(const string& input, vector<string>& result)
    {
        result.clear();

        auto range = operationsByInput.equal_range(input);
        for(auto it = range.first; it != range.second; ++it)
        {
            result.push_back(it->second);
        }
    }

    void GetOperationIdsByLevel(int level, unordered_set<string>& result)
    {
        result.clear();

        auto range = nodesByLevel.equal_range(level);
        for(auto it = range.first; it != range.second; ++it)
        {
            result.insert(it->second);
        }
    }

    bool ValidateData()
    {
        unordered_set<string> usedNodes;
        vector<string> errors;

        for(auto& o : operations)
        {
            Operation& op = o.second;
            usedNodes.insert(op.Output);
            usedNodes.insert(op.Input1);
            usedNodes.insert(op.Input2);

            if(o.first != op.Output)
            {
                errors.push_back("Mismatch: operation key with operation: " + o.first + "/" + op.Output);
            }

            auto range = operationsByInput.equal_range(op.Input1);
            int c = count_if(range.first, range.second, [&op](auto& pair) { return pair.second == op.Output; });
            if(c != 1)
            {
                errors.push_back("Invalid number of operationsByInput: " + op.Input1 + "/" + op.Output + "/" + to_string(c));
            }

            range = operationsByInput.equal_range(op.Input2);
            c = count_if(range.first, range.second, [&op](auto& pair) { return pair.second == op.Output; });
            if(c != 1)
            {
                errors.push_back("Invalid number of operationsByInput: " + op.Input2 + "/" + op.Output + "/" + to_string(c));
            }
        }

        for(auto& o : operationsByInput)
        {
            if(operations.count(o.second) != 1)
            {
                errors.push_back("Missing operation with key " + o.second);
                continue;
            }

            Operation& op = operations[o.second];
            if(op.Input1 != o.first && op.Input2 != o.first)
            {
                errors.push_back("Operation does not have input: " + o.first + "/" + o.second);
            }
        }

        if(usedNodes.size() != nodes.size())
        {
            errors.push_back("Wrong number of nodes");
        }

        return errors.size() == 0;
    }
};

int main()
{
    Calculator calculator;
    int64_t result = 0LL;
    vector<string> result2;

    calculator.Read(stdin);
    calculator.DoCalculations();
    result = calculator.CalculateZ();
    printf("%lld\n", result);

    result2 = calculator.FixCalculator();
    sort(result2.begin(), result2.end());
    for(int i = 0; i < result2.size(); i++)
    {
        if(i != 0) { printf(","); }
        printf("%s", result2[i].c_str());
    }
    printf("\n");

    return 0;
}