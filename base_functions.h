#ifndef _BASE
#define _BASE

#include <deque>
#include <string>

int sign(int x);

template<class T> void remove_it(std::deque<T>* base, T thing);

std::deque<std::string> split(std::string s, char seperator);

#endif // _BASE
