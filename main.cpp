/*
    Authors: Shane Laskowski + Michael Lingo
    Date Created: 11/30/2018
    Last Modification: 12/03/2018
    Title:  Page Replacement Alogrithm Simulations
    Purpose: This program simulates 3 page replacement algorithms.  FIFO, Random
             and LRU are simulated to compare their results against the same
             simulated stream of process memory requests.
*/

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <random>
#include <utility>
#include <math.h>
#include <time.h>   //for time functions
#include <stdlib.h> //for srand/rand functions
#include "page.hpp"
//add more libaries if needed

using namespace std;

const int MAXPAGESIZE = 8192;
const int MINPAGESIZE = 256;
const int MAXVIRTUALMEM = 134217728;
const int MAXNUMREFERENCES = 5000001;

void printSimulationData(int numberOfRef, long long duration, long numPageFaults, long numPageReplacements, long numFlushes);
bool isNumOfPowerTwo(int param); //checks if number is power of 2, such that there is an N such that (2^N == the number)
void fifoPages(const int addresses[], const int pageSize,
               const int maxPagesInMemory, const int numReferences); //fifo algorithm
void randomPages(const int addresses[], const int pageSize,
                 const int maxPagesInMemory, const int numReferences); //random algorithm
void leastRecentlyAccessed(const int addresses[], const int pageSize,
                           const int maxPagesInMemory, const int numReferences); //LRA algorithm

int main(int argc, char *argv[])
{
    cout << "Created By Shane Laskowski and Michael Lingo" << endl
         << endl;

    //process the command line arguments for this program
    //the 1st argument should be set page size( A number of power 2 (2^N) that is between 256 (2^8) and 8192 (2^13) )
    //the 2nd argument should be the total physical memory size of the main memory (is a number of power 2)
    int pageSize = 0;
    int totalPhysicalMemSize = 0;
    try
    {
        pageSize = stoi(argv[1]);
        totalPhysicalMemSize = stoi(argv[2]);

        if (pageSize < MINPAGESIZE || pageSize > MAXPAGESIZE || !isNumOfPowerTwo(pageSize))
            throw !!!true; //is this funny? refractor this line to make it funny
        if (!isNumOfPowerTwo(totalPhysicalMemSize))
            throw 420;
    }
    catch (bool e)
    {
        cerr << "page size is invalid," << endl
             << "range of page size is " << MINPAGESIZE << " to " << MAXPAGESIZE << endl
             << "page size must be a power of 2" << endl;
        return -1;
    }
    catch (int e)
    {
        cerr << "Physical Memory Size is invalid, it must be a power of 2" << endl;
        return -1;
    }
    catch (exception &e)
    {
        //cerr is like cout, but its console error output
        cerr << "An invalid argument was entered," << endl
             << "two arguments required:" << endl
             << "The size of the pages" << endl
             << "the total size of the physical main memory" << endl;
        return -1;
    }

    //determine what is the max number of pages, in the page table, that can be valid all at once (determined by user's input)
    int maxPagesInMemory = totalPhysicalMemSize / pageSize;

    //reads in the huge references.txt file into a data structure (the fstream library is used here)
    ifstream references;
    references.open("references.txt", ios::in);
    int *addresses = new int[MAXNUMREFERENCES];
    int numLines = 0;
    string in = "";
    while (getline(references, in))
    {
        addresses[numLines] = stoi(in);
        numLines++;
    }
    //this data is a list of logical addresses accessed by a program
    //for the right most digit(least significant), if even or 0 ==> read access, if odd ==> write access
    //For example, the value 1220149 means a write reference to memory location 1220148.
    //we can get away with this because minimum page size is 256 bytes

    //Now, start one of the algorithms, within a function that has arguments passeed to it, print results, reset any changes made to the data structures above
    cout << "FIFO Algorithm" << endl;
    fifoPages(addresses, pageSize, maxPagesInMemory, numLines);

    cout << "Random Algorithm" << endl;
    randomPages(addresses, pageSize, maxPagesInMemory, numLines);

    cout << "Least Recently Used Algorithm" << endl;
    leastRecentlyAccessed(addresses, pageSize, maxPagesInMemory, numLines);
    delete[] addresses;
    return 0;
}

//This algorithm is not mine, its amazing though
//http://javaexplorer03.blogspot.com/2016/01/how-to-check-number-is-power-of-two.html
//use bitwise & to the number with the number previous to it.
//**note if number is 0, then it will return true (incorrectly)
bool isNumOfPowerTwo(int numberToTest)
{
    if (((numberToTest & (numberToTest - 1)) == 0) && numberToTest != 0)
        return true;
    else
        return false;
}

/*
    prints out the resulting info of an algorithm
*/
void printSimulationData(int numberOfRef, long long duration, long numPageFaults, long numPageReplacements, long numFlushes)
{
    cout << "Number of Addresses Referenced: " << numberOfRef << endl;
    cout << "The algorithm took: " << duration << " microseconds" << endl;
    cout << "The Number Of Page Faults: " << numPageFaults << endl;
    cout << "The number of times a page was replaced: " << numPageReplacements << endl;
    cout << "The number of flushes: " << numFlushes << endl;
    cout << endl;
}

void fifoPages(const int addresses[], const int pageSize,
               const int maxPagesInMemory, const int numReferences)
{
    queue<int> fifo;
    unordered_map<int, page *> physicalMemory;
    physicalMemory.reserve(maxPagesInMemory);
    page *virtualMem = new page[MAXVIRTUALMEM / pageSize];
    int toShift = log2(pageSize); //shifting is faster than dividing, know its a power of 2
    int pageAddress;
    bool isWrite;
    long numPageFaults = 0;
    long numPageReplacements = 0;
    long numFlushes = 0;
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < numReferences; i++)
    {
        isWrite = ((addresses[i] & 1) == 1);                          //check's the binary of the referenced address, if last bit is 1 ==> its a write
        pageAddress = addresses[i] >> toShift;                        //same as dividing by pageSize
        if (physicalMemory.find(pageAddress) == physicalMemory.end()) //if the reference is not in the physical memory
        {
            ++numPageFaults;

            if (physicalMemory.size() >= maxPagesInMemory) //the physical memory is overloaded, need a victim page to evict out of memory!!!
            {
                ++numPageReplacements;
                physicalMemory.at(fifo.front())->valid = false;

                if (physicalMemory.at(fifo.front())->writtenTo == true) //this page that is going to be deleted is dirty (written to), therefore a flush will occur
                    ++numFlushes;

                physicalMemory.erase(fifo.front());
                fifo.pop();
            }
            physicalMemory[pageAddress] = &virtualMem[pageAddress];
            physicalMemory[pageAddress]->valid = true;
            fifo.push(pageAddress);
        }
        physicalMemory.at(pageAddress)->writtenTo = isWrite;
    }
    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    printSimulationData(numReferences, duration, numPageFaults, numPageReplacements, numFlushes);
    physicalMemory.clear();
    delete[] virtualMem;
}

void randomPages(const int addresses[], const int pageSize,
                 const int maxPagesInMemory, const int numReferences)
{
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rand(seed);
    page *virtualMem = new page[MAXVIRTUALMEM / pageSize];
    unordered_map<int, page *> physicalMemory;
    vector<int> pagesInMemory;
    physicalMemory.reserve(maxPagesInMemory);
    pagesInMemory.reserve(maxPagesInMemory);
    int toShift = log2(pageSize);
    int pageAddress;
    bool isWrite;
    long numPageFaults = 0;
    long numPageReplacements = 0;
    long numFlushes = 0;
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < numReferences; i++)
    {
        isWrite = ((addresses[i] & 1) == 1);
        pageAddress = addresses[i] >> toShift;
        if (physicalMemory.find(pageAddress) == physicalMemory.end()) //if reference is not in the physical memory
        {
            ++numPageFaults;

            if (physicalMemory.size() >= maxPagesInMemory) //physical memory is full (overflowed), need to remove a victim page
            {
                ++numPageReplacements;

                unsigned indexToRemove = rand() % pagesInMemory.size();
                physicalMemory.at(pagesInMemory[indexToRemove])->valid = false;

                if (physicalMemory.at(pagesInMemory[indexToRemove])->writtenTo == true)
                    ++numFlushes;

                physicalMemory.erase(pagesInMemory[indexToRemove]);
                iter_swap(pagesInMemory.begin() + indexToRemove, pagesInMemory.end() - 1);
                pagesInMemory.pop_back();
            }
            physicalMemory[pageAddress] = &virtualMem[pageAddress];
            physicalMemory[pageAddress]->valid = true;
            pagesInMemory.push_back(pageAddress);
        }
        physicalMemory.at(pageAddress)->writtenTo = isWrite;
    }
    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    printSimulationData(numReferences, duration, numPageFaults, numPageReplacements, numFlushes);
    physicalMemory.clear();
    pagesInMemory.clear();
    delete[] virtualMem;
}

void leastRecentlyAccessed(const int addresses[], const int pageSize,
                           const int maxPagesInMemory, const int numReferences)
{
    page *virtualMem = new page[MAXVIRTUALMEM / pageSize];
    unordered_map<int, page *> physicalMemory;
    vector<pair<int, int>> lastAccessTime; //first is accessTime, second is ID
    lastAccessTime.reserve(maxPagesInMemory);
    physicalMemory.reserve(maxPagesInMemory);
    int toShift = log2(pageSize);
    int pageAddress;
    bool isWrite;
    long numPageFaults = 0;
    long numPageReplacements = 0;
    long numFlushes = 0;
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < numReferences; i++)
    {
        isWrite = ((addresses[i] & 1) == 1); //odd or even?
        pageAddress = addresses[i] >> toShift;
        if (physicalMemory.find(pageAddress) == physicalMemory.end()) //page isn't in memory
        {
            ++numPageFaults;

            if (physicalMemory.size() >= maxPagesInMemory) //need to evict a page
            {
                ++numPageReplacements;

                int pageAddresstoRemove = lastAccessTime[lastAccessTime.size() - 1].second;
                lastAccessTime.pop_back();
                try
                {
                    if (physicalMemory.at(pageAddresstoRemove)->writtenTo == true)
                        ++numFlushes;
                }
                catch (out_of_range &e)
                {
                    cout << pageAddresstoRemove << endl;
                    exit(1);
                }
                physicalMemory.erase(pageAddresstoRemove);
                virtualMem[pageAddress].valid = false;
                virtualMem[pageAddress].writtenTo = false;
            }
            physicalMemory[pageAddress] = &virtualMem[pageAddress];
            physicalMemory[pageAddress]->valid = true;
            lastAccessTime.push_back(make_pair(i, pageAddress));
        }
        physicalMemory.at(pageAddress)->writtenTo = isWrite;
        auto it = find_if(lastAccessTime.begin(), lastAccessTime.end(),
                          [pageAddress](const pair<int, int> &p) {
                              return p.second == pageAddress;
                          });
        it->first = i; //first is last access time
        sort(lastAccessTime.begin(), lastAccessTime.end(),
             [](const pair<int, int> &a, const pair<int, int> &b) {
                 return a.first > b.first;
             }); //ensure lowest accessTime is last
    }
    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    printSimulationData(numReferences, duration, numPageFaults, numPageReplacements, numFlushes);
    physicalMemory.clear();
    lastAccessTime.clear();
    delete[] virtualMem;
}