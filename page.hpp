#ifndef page_hpp
#define page_hpp

class page
{
  public:
    int pageID;
    bool writtenTo;
    bool valid;
    int lastAccessTime;
    static int numPages;
    page();
};
#endif //page.hpp included