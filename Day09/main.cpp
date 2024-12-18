#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<algorithm>
#include<list>

using namespace std;

struct DiscFragment
{
    int StartPosition;
    int Length;
    int Id;

    DiscFragment(int start, int length, int id) : StartPosition(start), Length(length), Id(id) {}

    bool IsFree()
    {
        return Id < 0;
    }

    bool IsOccupied()
    {
        return Id >= 0;
    }
};

class DiscMap
{
private:
    list<DiscFragment> map;

public:
    DiscMap() { }

    void ReadMap(FILE* file)
    {
        int id = 0;
        int size = 0;
        bool isFreeSlot = false;
        char c;

        while(isdigit(c = fgetc(file)))
        {
            int length = c - '0';

            if(isFreeSlot)
            {
                if(length > 0)
                {
                    map.push_back(DiscFragment(size, length, -1));
                }
            }
            else
            {
                if(length > 0)
                {
                    map.push_back(DiscFragment(size, length, id));
                }
                id++;
            }

            size += length;
            isFreeSlot = !isFreeSlot;
        }
    }

    void DoFileCompact()
    {
        list<DiscFragment>::iterator firstFree = map.begin();
        list<DiscFragment>::iterator lastOccupied = map.end();

        while(firstFree != map.end() && !firstFree->IsFree()) { firstFree++; }
        lastOccupied--;
        while(lastOccupied != map.begin() && !lastOccupied->IsOccupied()) { lastOccupied--; }

        while(firstFree != map.end())
        {
            if(firstFree->Length < lastOccupied->Length)
            {
                SplitAndMove(firstFree, lastOccupied);
            }
            else
            {
                Move(firstFree, lastOccupied);
            }
        }
    }

    void DoFileCompact2()
    {
        list<DiscFragment>::iterator lastProcessed = map.end();
        lastProcessed--;
        int lastId = lastProcessed->Id;

        while(lastProcessed != map.begin())
        {
            if(!lastProcessed->IsOccupied() || lastProcessed->Id > lastId)
            {
                lastProcessed--;
                continue;
            }

            list<DiscFragment>::iterator freeFragment =
                FindFirstFreeFragmentOfSize(lastProcessed->Length, lastProcessed->StartPosition);
            lastId = lastProcessed->Id;
            
            if(freeFragment == map.end()) { lastProcessed--; continue; }

            if(freeFragment->Length > lastProcessed->Length)
            {
                DiscFragment fragment(*freeFragment);
                fragment.Length = lastProcessed->Length;
                freeFragment->StartPosition += fragment.Length;
                freeFragment->Length -= fragment.Length;
                map.insert(freeFragment, fragment);
                freeFragment--;
            }

            freeFragment->Id = lastProcessed->Id;
            lastProcessed->Id = -1;
            lastProcessed--;
        }
    }

    int64_t CalculateChecksum()
    {
        int64_t checksum = 0LL;

        for(auto f : map)
        {
            checksum += CalculateFragmentShecksum(f);
        }

        return checksum;
    }

    void PrintMap()
    {
        for(auto f : map)
        {
            for(int i = 0; i < f.Length; i++)
            {
                printf("%d", f.Id);
            }
        }
        printf("\n");
    }

private:
    void SplitAndMove(list<DiscFragment>::iterator& firstFree, list<DiscFragment>::iterator& lastOccupied)
    {
        lastOccupied->Length -= firstFree->Length;
        firstFree->Id = lastOccupied->Id;

        while(firstFree != map.end() && !firstFree->IsFree()) { firstFree++; }
    }

    void Move(list<DiscFragment>::iterator& firstFree, list<DiscFragment>::iterator& lastOccupied)
    {
        DiscFragment fragment = *lastOccupied;
        list<DiscFragment>::iterator toEraseFrom = lastOccupied;
        list<DiscFragment>::iterator toEraseTo = lastOccupied;
        toEraseTo++;
        lastOccupied--;
        while(lastOccupied != map.begin() && !lastOccupied->IsOccupied())
        {
            lastOccupied--;
            toEraseFrom--;
        }

        map.erase(toEraseFrom, toEraseTo);

        if(firstFree->Length == fragment.Length)
        {
            firstFree->Id = fragment.Id;
            while(firstFree != map.end() && !firstFree->IsFree()) { firstFree++; }
        }
        else
        {
            fragment.StartPosition = firstFree->StartPosition;
            firstFree->StartPosition += fragment.Length;
            firstFree->Length -= fragment.Length;
            map.insert(firstFree, fragment);
        }
    }

    int64_t CalculateFragmentShecksum(DiscFragment& fragment)
    {
        if(fragment.IsFree())
        {
            return 0LL;;
        }

        int64_t checksum = 0LL;

        for(int i = 0; i < fragment.Length; i++)
        {
            checksum += (int64_t)(fragment.StartPosition + i) * (int64_t)fragment.Id;
            // printf("(%d, %d) -> %d\n",
            //     fragment.StartPosition + i,
            //     fragment.Id,
            //     (fragment.StartPosition + i)*fragment.Id);
        }

        return checksum;
    }

    list<DiscFragment>::iterator FindFirstFreeFragmentOfSize(int size, int maxPosition)
    {
        list<DiscFragment>::iterator it = map.begin();

        while(it != map.end() && it->StartPosition < maxPosition)
        {
            if(it->IsFree() && it->Length >= size)
            {
                return it;
            }
            
            it++;
        }

        return map.end();
    }
};

int main()
{
    uint64_t sum = 0;
    DiscMap discMap;

    discMap.ReadMap(stdin);
    //discMap.DoFileCompact();
    discMap.DoFileCompact2();
    sum = discMap.CalculateChecksum();
    //discMap.PrintMap();

    printf("%lld\n", sum);

    return 0;
}