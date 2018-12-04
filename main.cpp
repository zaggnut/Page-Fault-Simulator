/*
    Authors: Shane Laskowski + Michael Lingo
    Date Created: 11/30/2018
    Last Modification: 12/03/2018
    Title:  Page Replacement Alogrithm Simulations
    Purpose:
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <time.h>   //for time functions
#include <stdlib.h> //for srand/rand functions
#include "page.hpp"
//add more libaries if needed

using namespace std;

const int MAXPAGESIZE = 8192;
const int MINPAGESIZE = 256;
const int MAXVIRTUALMEM = 134217728;
const int MAXNUMREFERENCES = 5000001;

bool isNumOfPowerTwo(int param);                                                                                //checks if number is power of 2, such that there is an N such that (2^N == the number)
void fifoPages(const int addresses[], const int pageSize, const int maxPagesInMemory, const int numReferences); //fifo algorithm

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
    //create a data structure for physical memory (initially empty)
    //need to be able to keep track of which frames of the physical memory are free

    //create a data structure for page table (all page table entries are invalid)
    //need to be able to keep track of page#, referenced frame, dirty or not, valid or not, etc....

    //read in the huge references.txt file into a data structure (can be done with ifstream)
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

    //repeat for the other two algorithms
    //note that LRU needs some additional data structures to figure out which is the least recently used (referenced) page

    //***NEED to add more skeletons for the other functions in this program.
    //***they will need parameters to do their jerbs
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

void fifoPages(const int addresses[], const int pageSize, const int maxPagesInMemory, const int numReferences)
{
    queue<int> fifo;
    unordered_map<int, page *> physicalMemory;
    page *virtualMem = new page[MAXVIRTUALMEM / pageSize];
    int pageAddress;
    bool isWrite;
    for (int i = 0; i < numReferences; i++)
    {
        isWrite = ((addresses[i] & 1) == 1);
        pageAddress = addresses[i] / pageSize;
        if (physicalMemory.find(pageAddress) == physicalMemory.end())
        {
            if (physicalMemory.size() >= maxPagesInMemory)
            {
                physicalMemory.at(fifo.front())->valid = false;
                physicalMemory.erase(fifo.front());
                fifo.pop();
            }
            physicalMemory[pageAddress] = &virtualMem[pageAddress];
            physicalMemory[pageAddress]->valid = true;
            fifo.push(pageAddress);
        }
        physicalMemory.at(pageAddress)->writtenTo = isWrite;
    }
    physicalMemory.clear();
    delete[] virtualMem;
}