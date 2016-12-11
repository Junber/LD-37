#ifndef _BASE
#define _BASE

#include <deque>
#include <string>

int random(int x, int y);
void random_init();
int sign(int x);
float float_abs(float x);
template<class T> void remove_it(std::deque<T>* base, T thing);
std::deque<std::string> split(std::string s, char seperator);

#endif // _BASE
