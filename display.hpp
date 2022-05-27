#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <string>

namespace display {

void init();
void deinit();
bool getKey(char &key);
void putStr(const std::string &str);
void erase();
void clear();
void refresh();

} // End of "namespace display"

#endif // End of "#ifndef DISPLAY_HPP"

