#ifndef RESULT_LINE_H_INCLUDED
#define RESULT_LINE_H_INCLUDED

#define RESULTS_HEADING (255)
#define RESULTS_SUBHEADING (127)
#define RESULTS_SUBSUBHEADING (100)
#define RESULTS_HELPHEADING (63)
#define RESULTS_HELPSUBHEADING (31)
#define RESULTS_HELPSUBSUBHEADING (15)
#define RESULTS_SPECIAL1 (7)
#define RESULTS_SPECIAL2 (3)
#define RESULTS_FULL_LINE (1)
#define RESULTS_NOT_FULL_LINE (0)
//Row type flags
#define RESULTS_NOTAROW (0)
#define RESULTS_ISAROW (1)
#define RESULS_HELPROW (2)
#define RESULTS_EVENROW (4)

//Must have windows.h as HWND type is used
#include <windows.h>

//=============================================================================
// Result Line Class - used for displaying results
//
// Holds Result Line info
//  rwnd:               respective windows control handle
//  result_flags        type of data heading/fulline
//  result_extraflags   reserverd
//  result_rowflags     type of row (table row)
class result_line {
private:
    HWND rwnd;
    int result_flags;
    int result_extraflags;
    int result_rowflags;
public:
    result_line(HWND, int, int);
    result_line(HWND, int, int, int);
    result_line();
    ~result_line();
    HWND result_lineGetHWND();
    void result_lineSetHWND(HWND);
    // Get Functions
    int result_lineGetFlags();
    int result_lineGetExtraFlags();
    int result_lineGetRowFlags();
    // Set Functions
    void result_lineSetFlags(int);
    void result_lineSetExtraFlags(int);
    void result_lineSetRowFlags(int);
};
//Default Constructor
result_line::result_line() {
    rwnd = (HWND)NULL;
    result_flags = 0;
    result_extraflags = 0;
    result_rowflags = 0;
}
// Constructor for result_line(HWND) and result_line(HWND, result_flag)
result_line::result_line(HWND r, int rf, int rxf) {
    rwnd = r;
    result_flags = rf;
    result_extraflags = rxf;
    result_rowflags = 0;
}
result_line::result_line(HWND r, int rf, int rxf, int rrf) {
    rwnd = r;
    result_flags = rf;
    result_extraflags = rxf;
    result_rowflags = rrf;
}
result_line::~result_line() {}
HWND result_line::result_lineGetHWND() { return rwnd; }
int result_line::result_lineGetFlags() { return result_flags; }
int result_line::result_lineGetExtraFlags() { return result_extraflags; }
int result_line::result_lineGetRowFlags() { return result_rowflags; }
void result_line::result_lineSetHWND(HWND r) { rwnd = r; }
void result_line::result_lineSetFlags(int rf) { result_flags = rf; }
void result_line::result_lineSetExtraFlags(int rxf) { result_extraflags = rxf; }
void result_line::result_lineSetRowFlags(int rrf) {
    result_rowflags = rrf;
}
#endif // RESULT_LINE_H_INCLUDED
