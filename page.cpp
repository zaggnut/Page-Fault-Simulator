#include "page.hpp"

int page::numPages = 0;

page::page()
{
    this->pageID = numPages;
    numPages++;
    this->writtenTo = false;
    this->valid = false;
    this->lastAccessTime = 0;
}