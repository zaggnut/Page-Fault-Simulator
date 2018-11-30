/*
    Authors: Shane Laskowski + Michael Lingo
    Date Created: 11/30/2018
    Last Modification: 11/30/2018
    Title:  Page Replacement Alogrithm Simulations
    Purpose:
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <time.h> //for time functions
#include <stdlib.h> //for srand/rand functions
//add more libaries if needed

using namespace std;

int main(int argc, char *argv[])
{
    //print our names Shane Laskowski and Michael Lingo

    //process the command line arguments for this program
        //the 1st argument should be set page size( A number of power 2 (2^N) that is between 256 (2^8) and 8192 (2^13) ) 
        //the 2nd argument should be the total physical memory size of the main memory (is a number of power 2)

    //determine what is the max number of pages, in the page table, that can be valid all at once (determined by user's input)

    //create a data structure for physical memory (initially empty)
        //need to be able to keep track of which frames of the physical memory are free

    //create a data structure for page table (all page table entries are invalid)
        //need to be able to keep track of page#, referenced frame, dirty or not, valid or not, etc....

    //read in the huge references.txt file into a data structure (can be done with ifstream)
        //this data is a list of logical addresses accessed by a program
        //for the right most digit(least significant), if even or 0 ==> read access, if odd ==> write access
        //For example, the value 1220149 means a write reference to memory location 1220148.
        //we can get away with this because minimum page size is 256 bytes


    //Now, start one of the algorithms, within a function that has arguments passeed to it, print results, reset any changes made to the data structures above
    
    //repeat for the other two algorithms
        //note that LRU needs some additional data structures to figure out which is the least recently used (referenced) page


    //***NEED to add more skeletons for the other functions in this program.

    return 0;
}