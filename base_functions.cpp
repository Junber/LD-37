#include "base_functions.h"
#include <algorithm>

class Object;
template void remove_it<Object*>(std::deque<Object*>*, Object*);

int sign(int x)
{
    return (x>0?1:(x<0?-1:0));
}

template<class T> void remove_it(std::deque<T>* base, T thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

std::deque<std::string> split(std::string s, char seperator)
{
    std::deque<std::string> ret;
    ret.push_back("");
    for (char c: s)
    {
        if (c == seperator)
        {
            ret.push_back("");
        }
        else
        {
            ret[ret.size()-1] += c;
        }
    }

    return ret;
}
