#include "display.hpp"

#include <string>

#include <ncurses.h>

using std::string;

namespace display {

//===== [Objects] ===== 
//===== [Objects] ===== 
// [External objects]
// Nothing

// [Filescoped objects]
namespace {
	// Nothing
} // End of filescoped objects


//===== Prototypes =====
// [External prototypes]
// Nothing

// [Filescoped prototypes]
namespace {
	// Nothing
} // End of filescoped prototypes


// ===== Definitions of Function =====
// [External Functions]
void init()
{
	// Prepare ncurses
	initscr();   // Initialize screen environment
	noecho();    // Disable echo back
	cbreak();    // Do not use input buffer
	timeout(0); // To use non-blocking mode
	curs_set(0); // Do not display cursor
	refresh();   // Screen refresh
}

void deinit()
{
	endwin(); // Deinitialize screen environment
}

bool getKey(char &key)
{
	const int keyCode = getch();
	bool ret = false;

	if (keyCode != static_cast<int>(ERR)) {
		key = static_cast<char>(keyCode);
		ret = true;
	}

	return ret;
}

void putStr(const string &str)
{
	mvaddstr(static_cast<int>(0), static_cast<int>(0), str.c_str());
}

void erase()
{
	::erase();
}

void clear()
{
	::clear(); // Call clear() of ncurses
}

void refresh()
{
	::refresh(); // Call clear() of ncurses
}

// [Filescoped Functions]
namespace {
	// Nothing
} // End of filescoped functions

} // End of "namespace display"
