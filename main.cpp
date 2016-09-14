#define _WIN32_IE (0x0601)
#include <ctime>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <cmath>
#include <deque>
#include <array>

// Debugging output flags
// Setting to true enables debugging at the resepctive stage
// if flag starts with FNC_ then the stage is within that function
// DEBUG turns on ALL debugging
// CALCDEBUG turns on all debugging within the CS calculation i.e.
//      when the calculate button is clicked.
bool DEBUG=false;
bool CALCDEBUG = false;
bool FNC_ISPARENTINCHILDCASE = false;
bool FNC_DOPARENTSMATCH = false;
bool FNC_CALCULATECOSTOFCHILDREN = false;
bool FNC_CALCULATEINCOMEPERCENTAGES = false;
bool FNC_ADJUSTMCCAP = false;
bool FNC_APPLYMULTICASEAMOUNTS = false;
bool FNC_APPLYRDCAMOUNTS = true;
bool FNC_DETERMINEMULTICASECHILDREN = false;
bool FNC_DETERMINERELEVANTDEPENDANTS = false;
bool FNC_PROMOTEADULTSINTOFULLCASES = false;
bool FNC_MERGECHILDCASESINTOFULLCASES = false;
bool FNC_BUILDCHILDCASES = false;
bool FNC_BUILDRELATIONSHIPS = false;

// Custom/Program specific Headers
#include "csa.h"
#include "myresource.h"
#include "windows_children.h"   // Include Class wc_chldrn (gui controls)
#include "windows_adults.h"     // Include Class wc_adlts
#include "result_line.h"        // Include result_line class
#include "csaterms.h"

//
#define MIN_CHILDREN (1)
#define MIN_ADULTS (2)
#define LINES (55)
#define DESK_SZ_FACTOR (25)

#define ACSC_STD_CTLHGT (18)        //standard Control Height
#define ACSC_STD_GAP (5)            //standdard gap bewteen controls
#define ACSC_STD_SHORTCTLLEN (55)   //standard short control width
#define ACSC_STD_CTLLEN (70)        //standard control width (most)
#define ACSC_STD_NAMECTLLEN (120)   //standard name control width
#define ACSC_STD_LINEHGT (20)       //standard height of a row
#define ADSC_STD_HPHEADING (10)     //standard horizontal position for HEADING
#define ACSC_STD_HPHEADINGLEN (665)
#define ADCS_STD_HPSUBHEADING (20)   //
#define ADCS_STD_HPSUBSUBHEADING (30)

const COLORREF _GREEN_ = RGB(0,255,0);
const COLORREF _DARKGREEN_ = RGB(0,205,0);
const COLORREF _VERYDARKGREEN_ = RGB(0,155,0);
const COLORREF _LIGHTGREEN_ = RGB(100,255,100);;
const COLORREF _VERYLIGHTGREEN_ = RGB(200,255,200);
const COLORREF _EXTREMELIGHTGREEN_ = RGB(245,255,245);
const COLORREF _BLUE_ = RGB(0,0,255);
const COLORREF _DARKBLUE_ = RGB(0,0,205);
const COLORREF _VERYDARKBLUE_ = RGB(0,0,155);
const COLORREF _EXTREMEDARKBLUE_ = RGB(0,0,140);
const COLORREF _ULTRAEXTREMEDARKBLUE_ = RGB(0,0,130);
const COLORREF _LIGHTBLUE_ = RGB(100,100,255);
const COLORREF _VERYLIGHTBLUE_ = RGB(225,225,255);
const COLORREF _EXTREMELIGHTBLUE_ = RGB(240,240,255);
const COLORREF _ULTRAEXTREMELIGHTBLUE_ = RGB(250,250,255);
const COLORREF _RED_ = RGB(255,0,0);
const COLORREF _DARKRED_ = RGB(205,0,0);
const COLORREF _VERYDARKRED_ = RGB(155,0,0);
const COLORREF _LIGHTRED_ = RGB(255,100,100);
const COLORREF _VERYLIGHTRED_ = RGB(255,200,200);
const COLORREF _EXTREMELIGHTRED_ = RGB(255,245,245);
const COLORREF _WHITE_ = RGB(255,255,255);
const COLORREF _BLACK_ = RGB(0,0,0);
const COLORREF _CYAN_ = RGB(0,255,255);
const COLORREF _H1_ = _VERYDARKBLUE_;
const COLORREF _T1_ = _WHITE_;
const COLORREF _T2_ = _WHITE_;
const COLORREF _H2_ = _DARKBLUE_;
const COLORREF _ROWODD_ = _EXTREMELIGHTBLUE_;
const COLORREF _ROWEVEN_ = _VERYLIGHTBLUE_;

HWND g_htoolbar = NULL;
HINSTANCE ghInstance = NULL;
HINSTANCE CalculatingClassName = NULL;
HINSTANCE chInstance = NULL;
HWND hwnd;

// Windows Control Handles for single occurence controls
// Year Section
HWND wc_csyear;
HWND wc_csyear_hlp;
HWND wc_csyear_hlp_tt;
HWND wc_csyear_tt;
HWND wc_csyear_label;
HWND wc_csyear_label_tt;
HWND wc_csyear_notes;
HWND wc_csyear_notes_tt;

// Calculate Section
HWND wc_calculate_label;
HWND wc_calculate_label_tt;
HWND wc_calculate_hlp;
HWND wc_calculate_hlp_tt;
HWND wc_calculate_button;
HWND wc_calculate_button_tt;
HWND wc_calculate_chk_label;
HWND wc_calculate_chk_label_tt;
HWND wc_calculate_chkbox;
HWND wc_calculate_chkbox_tt;
//Results Section
HWND wc_results_label;
HWND wc_results_label_tt;
HWND wc_results_hlp;
HWND wc_results_hlp_tt;

// Windows controls (data input) related deques
std::deque<wc_adlts> wc_adults;      // deque of adult windows controls
std::deque<wc_chldrn> wc_children;   // deque of child windows controls
std::deque<HWND> results;           // deque of results windows control
std::deque<result_line> xresults;
std::deque<HWND> xresults_tt;       // Tooltip HWND's
std::deque<HWND> xresults_hb;       // Help Button HWND's
std::deque<char const*> xresults_hbtxt;

// Window structure variables
HDC hdc;
SCROLLINFO si;
TEXTMETRIC tmet;
RECT desktop, client;
HFONT SectionFont,LabelFont,InputFont,ToolTipFont;
SYSTEMTIME st;
TOOLINFO tt;
INITCOMMONCONTROLSEX icce;

//=============================================================================
// Global variables/constants

// Window dimensions
int ww, wh, wx, wy, xChar, xUpper, yChar;

// Data.Control positioning (vertical positions)
int vpos=10;                            // Current vetical position
int vposadd=ACSC_STD_LINEHGT;           // Amount to add per display row
//int vpos_after_year=0;                  // Current position of
// Adult to Child Relationship types
// i.e. An adult can be a parent, a non-parent carer, a Parent Abroad or
// a Deceased Parent (latter two are to cater for legislation specific
// to those special circumstances).
const std::string RELATIONSHIP_TYPES =
    "Parent,Other,Non-Parent Carer,Parent Abroad,Deceased Parent";
// Define a stack/queue to hold relationships
std::deque<std::string> rltype;
//  Child Support related deques
std::deque<cs_amounts> csyears;             // deque of child support amounts
// 1 element per year of CS data
std::deque<cs_adult> adults;                // deque of adults (initially 2)
std::deque<cs_child> children;              // deque of children
// (initially 1 but more can be added or removed)
std::deque<cs_relationship> relationships;  // deque of relationships (adult -> child e.g parent)
std::deque<cs_adultcase> adultcases;        // deque of adult cases
std::deque<cs_childcase> childcases;        // deque of child cases
std::deque<cs_fullcase> fullcases;          // deque of full cases (merged childcases)
// Filename of the file that contains the annual data (csamounts)
const char CS_FNAME[] = "csamounts.txt";
// Flag used to indicate of the csamounts (annual data) file was found
bool csamounts_found = false;
// Current year
int current_year=0;
signed int selected_year = -1;
bool calc_details = false;

// Function/Procedure declarations
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
bool BuildRelationShipTypes(std::string, std::deque<std::string> &);
signed int getYearData(std::deque<cs_amounts> &);
void BuildYearSection(HWND, int &, int);
void BuildAdultsSection(HWND, int &, int);
void BuildChildSection(HWND, int &, int);
void BuildCalcSection(HWND,int &,int );
void BuildResultsSection(HWND, int &, int);
void AddSectionLabel(HWND, HWND &, std::string,int &,int);
void AddSectionButton(HWND, HWND &, std::string, int, int, int);
void AddResultsLabel(HWND, HWND &, std::string, int &, int);
void AddItemLabel(HWND, HWND &, std::string, int, int, int);
void AddItemRichLabel(HWND, HWND &, std::string, int, int, int);
void AddItemLabelMny(HWND, HWND &, std::string, int, int, int);
void AddItemEditTXT(HWND, HWND &, std::string, int, int, int);
void AddItemEditNMB(HWND, HWND &, int, int,int, int);
void AddItemChkBox(HWND, HWND &, bool, int, int);
void AddRltDropDown(HWND, HWND &, int, int, int, int);
void AddItemButton(HWND, HWND &, std::string, int, int, int);
void ReBuildAll(HWND &, HWND &, HWND &, HWND &, HWND);
void AddNewAdult(std::deque<cs_child> &);
void AddNewChild();
std::string GetEditBoxData(HWND);
void DoCalculations(HWND);
void setyearnotes();
int chknmbofparents(signed int, signed int);
bool DoParentsMatch(int, int);
bool IsParentInChildCase(int, std::string);
bool AdultNameUsed(std::string);
bool ChildNameUsed(std::string);
void RemoveAdultFromChildren(signed in);
void preemptfill(HWND, DINT);
bool DoChecking(HWND);
void BuildRelationships();
void BuildChildCases();
void MergeChildCasesIntoFullCases();
void PromoteAdultsIntoFullCases();
void DetermineRelevantDependants();
void DetermineMultiCaseChildren();
void ApplyRDCAmounts();
void ZeroNonParentIncomes();
void ApplyMultiCaseAmounts();
void AdjustMCCap(int, std::string, int);
void CalculateCostOfChildren();
void CalculateIncomePercentages();
void AddToolTip(HWND, HWND &, LPTSTR);
void DisplayYearHelp(HWND);
void DisplayAdultHelp(HWND);
void DisplayChildHelp(HWND);
void DisplayCalculateHelp(HWND);
void KillWind(HWND);
void AddResultLine(HWND, result_line &, std::string, int, int &, int &, int, int, int, int);
WNDCLASSEX wincl;

/*  Make the class name into a global variable  */
char szClassName[ ] = "Advanced Child Support Calculator";

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{

    // Get annually changing CS data
    getYearData(csyears);

    // Build the relationship types deque
    if (BuildRelationShipTypes(RELATIONSHIP_TYPES, rltype)) csamounts_found = true;

    // Set the system time (st.wYear will be the year)
    GetSystemTime(&st);
    //cout << "System Time Extracted as " << st << endl;
    // Find the screen size (width/height)
    GetWindowRect(GetDesktopWindow(), &desktop);


    //HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    //WNDCLASSEX wincl;        /* Data structure for the windowclass */
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS ;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);
    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
    //wincl.hIcon = LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    //wincl.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    //wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);                 /* No menu */
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = CreateSolidBrush(_ULTRAEXTREMELIGHTBLUE_);
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl)) return 0;
    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               WS_EX_CONTROLPARENT,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               "Advanced Child Support Calculator - by Mike",       /* Title Text */
               WS_OVERLAPPEDWINDOW | WS_VSCROLL, /* default window */
               desktop.right/DESK_SZ_FACTOR,       /* Windows decides the position */
               desktop.bottom/DESK_SZ_FACTOR,       /* where the window ends up on the screen */
               ACSC_STD_HPHEADINGLEN + 50,                 /* The programs width */
               desktop.bottom - (desktop.bottom/DESK_SZ_FACTOR) * 2,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0) > 0)
    {
        // Note the IsDialogMessage function allows tabbing messages to be processed and
        // thus allow tabbing
        if (!IsDialogMessage(hwnd,&messages) )
        {
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
    }
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Parent/Main Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {


    case WM_CREATE:
    {

        // Define Fonts to be used (4)
        SectionFont = CreateFont (16, 6, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET,
                                  OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, "Arial");
        HDC Dc = GetDC (hwnd);
        SelectObject (Dc, SectionFont);
        SetBkMode (Dc, TRANSPARENT);

        LabelFont = CreateFont (15, 5, 0, 0, FW_MEDIUM, false, false, false, DEFAULT_CHARSET,
                                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, "Arial");
        SelectObject (Dc, LabelFont);
        SetBkMode (Dc, TRANSPARENT);

        InputFont = CreateFont (14, 6, 0, 0, FW_REGULAR, false, false, false, DEFAULT_CHARSET,
                                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, "Arial");
        SelectObject (Dc, InputFont);
        SetBkMode (Dc, TRANSPARENT);

        ToolTipFont = CreateFont (14, 8, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET,
                                  OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, "Arial");
        SelectObject (Dc, InputFont);
        SetBkMode (Dc, TRANSPARENT);

        // Get the system time (to extract the year later)
        GetClientRect(hwnd, &client);
        //GetSystemTime(&st);


        // Make it so that tool tips (common controls) can be displayed
        icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icce.dwICC = ICC_TAB_CLASSES;
        InitCommonControlsEx(&icce);

        if (!csamounts_found)
        {
            MessageBox(hwnd,"CS Amounts File Not Found - Ending","Error!", MB_OK | MB_ICONERROR);
            PostQuitMessage(0);
        }
        else
        {

            MessageBox(hwnd,"CS Amounts Found OK","OK So Far", MB_OK | MB_ICONINFORMATION);

            BuildYearSection(hwnd, vpos, vposadd);
            //vpos_after_year=vpos;

            // Extract font dimensions from the text metrics.
            if (DEBUG) std::cout << "\t Doing GetDC done\n";
            hdc = GetDC (hwnd);
            if (DEBUG) std::cout << "\t GetDC done - Doing GetTextMetrics\n";
            GetTextMetrics (hdc, &tmet);
            if (DEBUG) std::cout << "\t GetTextMetrics done - Setting Variables xChar xUpper and yChar\n";
            xChar = tmet.tmAveCharWidth;
            xUpper = (tmet.tmPitchAndFamily & 1 ? 3 : 2) * xChar/2;
            yChar = tmet.tmHeight + tmet.tmExternalLeading;
            if (DEBUG) std::cout << "\t Variables xChar xUpper and yChar set - Doing ReleaseDC\n";
            ReleaseDC (hwnd, hdc);
            if (DEBUG) std::cout << "\t ReleaseDC Done - Doing Build Adult Section\n";

            BuildAdultsSection(hwnd, vpos, vposadd);
            BuildChildSection(hwnd, vpos, vposadd);
            BuildCalcSection(hwnd, vpos, vposadd);
            BuildResultsSection(hwnd, vpos, vposadd);
        }
    }

    case WM_LBUTTONDOWN:
    {
        break;
    }
    case WM_KEYDOWN:
    {
        break;
    }
    case WM_SIZE:
    {
        wy = HIWORD(lParam);
        wx = LOWORD(lParam);
        si.cbSize = sizeof(si);
        si.fMask  = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = ((vpos / ACSC_STD_CTLHGT)+22) - 1;
        si.nPage = (wy / ACSC_STD_CTLHGT);
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        UpdateWindow(hwnd);
        //MessageBox(hwnd, "You changed the window size","Size Changed", MB_OK | MB_ICONINFORMATION);
        break;
    }
    case WM_VSCROLL:
    {
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        wy = si.nPos;

        switch (LOWORD(wParam))
        {
        case SB_TOP:
        {
            si.nPos = si.nMin;
            break;
        }
        case SB_BOTTOM:
        {
            si.nPos = si.nMax;
            break;
        }
        case SB_LINEUP:
        {
            si.nPos -= 1;
            break;
        }
        case SB_LINEDOWN:
        {
            si.nPos += 1;
            break;
        }
        case SB_PAGEUP:
        {
            si.nPos -= si.nPage;
            break;
        }
        case SB_PAGEDOWN:
        {
            si.nPos += si.nPage;
            break;
        }
        case SB_THUMBTRACK:
        {
            si.nPos = si.nTrackPos;
            break;
        }
        default:
        {
            break;
        }
        }
        si.fMask = SIF_POS;
        SetScrollInfo (hwnd, SB_VERT, &si, TRUE);
        GetScrollInfo (hwnd, SB_VERT, &si);
        // If the position has changed, scroll window and update it
        if (si.nPos != wy)
        {
            ScrollWindow(hwnd, 0, 19 * (wy - si.nPos), NULL, NULL);
            UpdateWindow (hwnd);
        }
        break;
    }
    case WM_DESTROY:
    {
        /* DestroyWindow(g_htoolbar); */
        PostQuitMessage (0);
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        HWND hwnd = (HWND)lParam;

        // On a per child basis set total care background colour
        // Red if the care for the child is not 365 in total
        // Green if OK (all care is allocated)
        for (signed int i = 0; i < (signed)wc_children.size(); i++)
        {
            if (wc_children[i].wcc_chld_care_total == hwnd)
            {
                if(children[i].GetTotalCare() != 0)
                {
                    //SetBkMode(hdc,TRANSPARENT);
                    SetBkColor(hdc,_LIGHTRED_);
                    return (LRESULT)( (HBRUSH)GetStockObject(HOLLOW_BRUSH) );
                }
                else
                {
                    //MessageBox(hwnd,"All Care Allocated","Care complete",MB_ICONINFORMATION);
                    SetBkColor(hdc,_LIGHTGREEN_);
                    return (LRESULT)( (HBRUSH)GetStockObject(HOLLOW_BRUSH) );
                }
            }
        }
        // Scan through all of the results entries applying approriate styles/colors
        // according to the flags as set in the result_line object
        for (signed int i = 0; i < (signed) xresults.size(); i++)
        {
            if (xresults[i].result_lineGetHWND() == hwnd)
            {
                // Heading
                switch (xresults[i].result_lineGetFlags())
                {
                case RESULTS_HEADING:
                {
                    SetBkColor(hdc,_VERYDARKBLUE_);
                    SetTextColor(hdc,_WHITE_);
                    //return (LRESULT)( (HBRUSH)GetStockObject(HOLLOW_BRUSH) );
                    break;
                }
                case RESULTS_HELPHEADING:
                {
                    SetBkColor(hdc,_VERYDARKGREEN_);
                    SetTextColor(hdc,_WHITE_);
                    break;
                }
                case RESULTS_HELPSUBHEADING:
                {
                    SetBkColor(hdc,_DARKGREEN_);
                    SetTextColor(hdc,_WHITE_);
                    break;
                }
                case RESULTS_HELPSUBSUBHEADING:
                {
                    SetBkColor(hdc,_VERYLIGHTGREEN_);
                    SetTextColor(hdc,_BLACK_);
                    break;
                }
                case RESULTS_SUBHEADING:
                {
                    SetBkColor(hdc,_DARKBLUE_);
                    SetTextColor(hdc,_WHITE_);
                    break;
                }
                case RESULTS_SUBSUBHEADING:
                {
                    SetBkColor(hdc,_BLUE_);
                    SetTextColor(hdc,_WHITE_);
                }
                }
                if(xresults[i].result_lineGetRowFlags() & RESULTS_ISAROW)
                {
                    if (xresults[i].result_lineGetRowFlags() & RESULTS_EVENROW )
                    {
                        SetBkColor(hdc,_ROWEVEN_);
                    }
                    else
                    {
                        SetBkColor(hdc,_ROWODD_);
                    }
                }
            }
        }
        if((wc_csyear_label == hwnd) |
                (wc_adults_label == hwnd) |
                (wc_children_label == hwnd) |
                (wc_calculate_label == hwnd) |
                (wc_results_label == hwnd) )
        {
            SetBkColor(hdc,_H1_);
            SetTextColor(hdc,_T1_);
            return (LRESULT) ((HBRUSH)GetStockObject(HOLLOW_BRUSH));
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_HELP_ABOUT:
        {
            int ret = IDOK;
            //int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
            if (ret == IDOK)
            {
                MessageBox(hwnd, "Dialog exited with IDOK.", "Notice", MB_OK | MB_ICONINFORMATION);
            }
            else if (ret == IDCANCEL)
            {
                MessageBox(hwnd, "Dialog exited with IDCANCEL.", "Notice", MB_OK | MB_ICONINFORMATION);
            }
            else if (ret == -1)
            {
                MessageBox(hwnd, "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        default:
        {
            break;
        }
            // Other menu commands...
        }
        switch (HIWORD(wParam))
        {
        // Checking for Dropdown selection changes (Year or Child/Adult Relationship)
        case CBN_SELENDOK:
        {
            // Year Has been changed
            if ((HWND)lParam == wc_csyear)
            {
                selected_year = (int)SendMessage(wc_csyear,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
                SendMessage(wc_csyear_notes,WM_SETTEXT,(WPARAM)0,(LPARAM)csyears[selected_year].getyearnotes().c_str());
            }
            // Child/Adult Relationship has been changed
            else
            {
                // Loop through children
                for (signed int i=0; i < (signed)wc_children.size(); i++)
                {
                    // Then through the relationships
                    for (signed int j=0; j < (signed)wc_children[i].wcc_chld_adlt_rlt.size(); j++)
                    {
                        // Check to see if this the ComboBox in question
                        if ((HWND)lParam == wc_children[i].wcc_chld_adlt_rlt[j])
                        {
                            // If so get the value of the currently selected item
                            LRESULT trlt = SendMessage(wc_children[i].wcc_chld_adlt_rlt[j],CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
                            // If the request was OK, then update the respective child
                            if (trlt != CB_ERR)
                            {
                                // Set the relationship type in the cs child deque
                                children[i].setrlt(j,trlt);
                                //Check the number of parents
                                if (chknmbofparents(i,j) > 2)
                                {
                                    std::string msg="The Child ";
                                    msg.append(children[i].GetChildsName());
                                    msg.append(" has more than 2 parents. The realtionship with Parent ");
                                    msg.append(adults[j].getaname());
                                    msg.append(" has been changed to Other. Please review this.");
                                    MessageBox(hwnd,msg.c_str(),"Child has too many Parents!",MB_OK | MB_ICONERROR);
                                    SendMessage(wc_children[i].wcc_chld_adlt_rlt[j],CB_SETCURSEL,(WPARAM)1,(LPARAM)0);
                                    children[i].setrlt(j,1);
                                    children[i].setcare(j,0);
                                    SendMessage(wc_children[i].wcc_chld_adlt_care[j],WM_SETTEXT,(WPARAM)0,(LPARAM)NumberToString(children[i].GetCare(j)).c_str());
                                }
                            }
                            if (children[i].getrlt(j) == 1 || children[i].getrlt(j) == 4)
                            {
                                children[i].setcare(j,0);
                                SendMessage(wc_children[i].wcc_chld_adlt_care[j],WM_SETTEXT,(WPARAM)0,(LPARAM)NumberToString(children[i].GetCare(j)).c_str());
                                SendMessage(wc_children[i].wcc_chld_adlt_care[j],EM_SETREADONLY,(WPARAM)true,(LPARAM)0);
                            }
                            else
                            {
                                SendMessage(wc_children[i].wcc_chld_adlt_care[j],EM_SETREADONLY,(WPARAM)false,(LPARAM)0);
                            }
                        }
                    }
                }
            }
            break;
        }
        case EN_CHANGE:
        {
            break;
        }
        case EN_KILLFOCUS:
        {
            // Adult Section related text boxes (Name and ATI)
            for (int i=0; i < (signed)wc_adults.size(); i++)
            {
                // ATI changed
                if ((HWND)lParam == wc_adults[i].wca_adlt_ati)
                {
                    adults[i].setati((int)StringToNumber(GetEditBoxData(wc_adults[i].wca_adlt_ati),0.0));
                }
                // Adult Name changed
                if ((HWND)lParam == wc_adults[i].wca_adlt_nm)
                {
                    adults[i].setaname(GetEditBoxData(wc_adults[i].wca_adlt_nm));
                    //MessageBox(hwnd,adults[i].getaname().c_str(),"Adults New Name!",MB_OK | MB_ICONINFORMATION);
                    // Apply name to children reltionship labels
                    for (int j=0; j < (signed)wc_children.size(); j++)
                    {
                        SendMessage((HWND)wc_children[j].wcc_chld_adlt_nmlabel[i],WM_SETTEXT,(WPARAM)0,(LPARAM)adults[i].getaname().c_str());
                    }
                }
            }
            // Child Section related text boxes (Name and Level of care)
            for (int i=0; i < (signed)wc_children.size(); i++)
            {
                // Childs Name changed
                if ((HWND) lParam == wc_children[i].wcc_chld_nm)
                {
                    children[i].setcname(GetEditBoxData(wc_children[i].wcc_chld_nm));
                    SendMessage(wc_children[i].wcc_chld_nm,WM_SETTEXT,(WPARAM)0,(LPARAM)children[i].GetChildsName().c_str());
                }
                // Delve into the child adult reltionships
                for (int j=0; j < (signed)wc_children[i].wcc_chld_adlt_care.size(); j++)
                {
                    if ((HWND) lParam == wc_children[i].wcc_chld_adlt_care[j])
                    {
                        //MessageBox(hwnd,GetEditBoxData((wc_children[i].wcc_chld_adlt_care[j])).c_str(),"New Care Level",MB_OK | MB_ICONINFORMATION);
                        children[i].setcare(j,(int)StringToNumber(GetEditBoxData(wc_children[i].wcc_chld_adlt_care[j]),0.0));
                        //Need to check level of care for an Non-Parent Carer as if under 35% then not allowed
                        std::string ct="Unallocated Care=";
                        ct.append(NumberToString(children[i].GetTotalCare()));
                        if(children[i].GetTotalCare() != 0)
                        {
                            SendMessage(wc_children[i].wcc_chld_care_total,WM_SETTEXT,(WPARAM)0,(LPARAM)ct.c_str());
                        }
                        else
                        {
                            ct = "ALL CARE ALLOCATED";
                            SendMessage(wc_children[i].wcc_chld_care_total,WM_SETTEXT,(WPARAM)0,(LPARAM)ct.c_str());
                        }
                        // If care is less than 128 (35%) then check for Non-Parent carer as NPC has to have at least 35% care
                        int childadult_care = (int) StringToNumber(GetEditBoxData(wc_children[i].wcc_chld_adlt_care[j]),0.0);
                        if (childadult_care < 128 && (children[i].getrlt(j)) == 2)
                        {
                            LRESULT trlt = SendMessage(wc_children[i].wcc_chld_adlt_rlt[j],CB_SETCURSEL,(WPARAM)1,(LPARAM)0);
                            children[i].setcare(j,0);
                            SendMessage(wc_children[i].wcc_chld_adlt_care[j],WM_SETTEXT,(WPARAM)0,(LPARAM)"0");
                            std::string ct="Unallocated Care=";
                            ct.append(NumberToString(children[i].GetTotalCare()));
                            SendMessage(wc_children[i].wcc_chld_care_total,WM_SETTEXT,(WPARAM)0,(LPARAM)ct.c_str());
                            children[i].setrlt(j,trlt);
                            std::string npclowcare = "Non-Parent Carer mus have 35% (128 nights) care or more.\n\n";
                            npclowcare.append("The realtionship has been changed to Other.\n");
                            npclowcare.append("Additionally, the care has been set to 0 (Other cannot have care)\n\n");
                            npclowcare.append("You should review the care for this child.");
                            MessageBox(hwnd,npclowcare.c_str(),"NPC Care Invalid",MB_ICONINFORMATION);
                        }
                    }
                }
            }
            break;
        }
        case BN_CLICKED:
        {
            if ((HWND)lParam == wc_csyear_hlp) DisplayYearHelp(hwnd);           // Year Section Help
            if ((HWND)lParam == wc_adults_hlp) DisplayAdultHelp(hwnd);          // Adult Section Help
            if ((HWND)lParam == wc_children_hlp) DisplayChildHelp(hwnd);        // Child Section Help
            if ((HWND)lParam == wc_calculate_hlp) DisplayCalculateHelp(hwnd);   // Calculate Section help

            // Calculation Button
            if ((HWND)lParam == wc_calculate_button)
            {
                DoCalculations(hwnd);      // Calculate
                ReBuildAll(wc_children_addbutton,
                           wc_children_label,
                           wc_adults_addbutton,
                           wc_adults_label,
                           hwnd
                           );
            }
            // Show Calculation details checkbox
            if ((HWND)lParam == wc_calculate_chkbox)
            {
                LRESULT ac = SendMessage(wc_calculate_chkbox,
                                         BM_GETCHECK,
                                         (WPARAM)0,
                                         (LPARAM)0
                                         );
                if (ac == BST_CHECKED) calc_details = true;
                else calc_details = false;
            }
            // Add a new child
            if ((HWND)lParam == wc_children_addbutton)
            {
                AddNewChild();
                ReBuildAll(wc_children_addbutton,
                           wc_children_label,
                           wc_adults_addbutton,
                           wc_adults_label,
                           hwnd
                           );
                si.nMax = ((vpos / 18)+22) -1;
                si.fMask = SIF_ALL;
                SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
                UpdateWindow(hwnd);
            }
            // Add a new Adult
            if ((HWND)lParam == wc_adults_addbutton)
            {
                AddNewAdult(children);
                ReBuildAll(wc_children_addbutton,
                           wc_children_label,
                           wc_adults_addbutton,
                           wc_adults_label,
                           hwnd
                           );
                si.nMax = ((vpos / 18)+22) - 1;
                si.fMask = SIF_ALL;
                SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
                UpdateWindow(hwnd);
            }
            else
            {
                // Delete an Adult
                for (unsigned int i=0; i < wc_adults.size(); i++)
                {
                    if ((HWND)lParam == wc_adults[i].wca_adlt_dltb )
                    {
                        adults.erase(adults.begin()+i);
                        RemoveAdultFromChildren(i);
                        ReBuildAll(wc_children_addbutton,
                                   wc_children_label,
                                   wc_adults_addbutton,
                                   wc_adults_label,
                                   hwnd
                                   );
                    }
                }
                // Delete a Child or detect age checked
                for (unsigned int i=0; i < wc_children.size(); i++)
                {
                    // Delete a child
                    if ((HWND)lParam == wc_children[i].wcc_chld_dltb)
                    {
                        children.erase(children.begin()+i);
                        ReBuildAll(wc_children_addbutton,
                                   wc_children_label,
                                   wc_adults_addbutton,
                                   wc_adults_label,
                                   hwnd
                                   );
                    }
                    // Set age checked status
                    if ((HWND)lParam == wc_children[i].wcc_chld_age)
                    {
                        LRESULT ac = SendMessage(wc_children[i].wcc_chld_age,
                                                 BM_GETCHECK,
                                                 (WPARAM)0,
                                                 (LPARAM)0
                                                 );
                        if (ac == BST_CHECKED) children[i].setteen(true);
                        else children[i].setteen(false);
                    }
                }
                // Results section help buttons i.e. display a message box with
                // the respective text as held in the associated (same index)
                // xresults_hbtxt.
                for (unsigned int i = 0; i < xresults_hb.size(); i++) {
                    if ((HWND)lParam == xresults_hb[i]) {
                        MessageBox(hwnd,xresults_hbtxt[i],
                                   "Help Info",
                                   MB_ICONINFORMATION
                                   );
                    }
                }
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }
    // All other messages
    default:
    {
        return DefWindowProc (hwnd, message, wParam, lParam);
        break;
    }
    }
    return 0;
}

//=======================================================================================
// Build deque of strings from comma-sperated list of relationship types
// Note only do so if the csamounts file has been found
//=======================================================================================
bool BuildRelationShipTypes(std::string rtyp, std::deque<std::string> &rtyp_str)
{
    if (!csamounts_found) return false;
    std::string thisFunction = __FUNCTION__;
    if (DEBUG) std::cout << "BldRltTyp_I001 Funct=" << thisFunction << " Started" << std::endl;
    if (DEBUG) std::cout << "         String=" << rtyp << std::endl;
    std::size_t loc, cnt=0;

    loc = rtyp.find(",");
    while (loc != std::string::npos)
    {
        if (DEBUG) std::cout << "BldRltTyp_I002 Funct=" << "\t\t current value of loc (location of comma)=" << loc << " npos (found/notfound) value=" << std::string::npos << "Reltionship String=" << rtyp << "\n";
        if (DEBUG) std::cout << "BldRltTyp_I003 Funct=" << "\t\tAdding Relationship=" << rtyp.substr(0,loc) << std::endl;
        rtyp_str.push_back(rtyp.substr(0,loc));
        rtyp = rtyp.substr(loc+1);
        loc = rtyp.find(",");
        cnt++;
        if (cnt > 10) break;
    }
    if (DEBUG) std::cout << "BldRltTyp_I004 Funct=" << thisFunction <<"\t\tAdding Last Relationship=" << rtyp << "\n";
    rtyp_str.push_back(rtyp);
    if(DEBUG) std::cout << "BldRltTyp_I005 Funct=" << thisFunction << " Completed" << std::endl;
    return true;
}

//=======================================================================================
// Function to Build the Year Selection Section
//=======================================================================================
void BuildYearSection(HWND hwnd, int &vp, int vpa)
{

    if (DEBUG) std::cout << "Function=BuildYearSection - Starting\n";

    // Get the annual data (csamounts.txt file) and populate the csyears deque(line an array)
    // each element of the deque is an instance of the cs_amounts class and there will be 1
    // such instance for each year in the csamounts file.
    if (DEBUG) std::cout << "Function=BuildYearSection - Calling GetYearData function\n";
    //getYearData(csyears);

    // Build the Section Heading/Label
    if (DEBUG) std::cout << "Function=BuildYearSection - Addind Section controls (label)\n";
    std::string label = "Step 1 - Select the Year in which the assessment starts";
    std::string csy_hlp="Clicking this button will display comprehensive help for this section";
    std::string csy_h = "This section is for the selection of the Year which results in many underlying amounts changing.";
    std::string csy_dd = "Select the Year in which the Assessment starts. ";
    std::string csy_n = "This shows the core values for the currently selected year.";

    AddItemButton(hwnd, wc_csyear_hlp,(std::string)"?",vp,600,15);
    AddToolTip(wc_csyear_hlp,wc_csyear_hlp_tt,(LPTSTR)csy_hlp.c_str());
    AddSectionLabel(hwnd, wc_csyear_label,label,vp,vpa);
    AddToolTip(wc_csyear_label,wc_csyear_label_tt,(LPTSTR)csy_h.c_str());

    // Create and populate the year selection combobox from the csyears deque
    if (DEBUG) std::cout << "Function=BuildYearSection - Building ComboBox\n";
    wc_csyear = CreateWindowEx((DWORD)NULL,"COMBOBOX",NULL,
                               WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 30,vp,60,400,
                               hwnd, NULL, ghInstance, NULL);
    SendMessage(wc_csyear,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    for (int i=0; i < (signed)csyears.size(); i++)
    {
        if (DEBUG) std::cout << "Function=BuildYearSection - Adding Year " << (i+1) << "(" << csyears[i].getyear()
                                 << ") of " << csyears.size() << "\n";
        SendMessage(wc_csyear,CB_ADDSTRING,(WPARAM)0,(LPARAM)NumberToString(csyears[i].getyear()).c_str());
        SendMessage(wc_csyear,CB_SETITEMHEIGHT,(WPARAM)-1,(LPARAM)15);
        SendMessage(wc_csyear,CB_SETITEMHEIGHT,(WPARAM)0,(LPARAM)15);
        if (selected_year == -1 && csyears[i].getyear() == (int)st.wYear)
        {
            if (DEBUG) std::cout << "Function=BuildYearSection - Setting Default Selection (Year "<< st.wYear << ")\n";
            SendMessage(wc_csyear,CB_SETCURSEL,(WPARAM)i,(LPARAM)NULL);
        }
        if (selected_year == i)
        {
            if (DEBUG) std::cout << "Function=BuildYearSection - Setting Previous Selection (Year "<< st.wYear << ")\n";
            SendMessage(wc_csyear,CB_SETCURSEL,(WPARAM)i,(LPARAM)NULL);
        }
        AddToolTip(wc_csyear,wc_csyear_tt,(LPTSTR)csy_dd.c_str());
    }
    // No match for the current year o set the selected year to the last year available
    if (selected_year == -1)
    {
        int t = (signed)csyears.size()-1;
        SendMessage(wc_csyear,CB_SETCURSEL,(WPARAM)t,(LPARAM)NULL);
    }

    if (DEBUG) std::cout << "Function=BuildYearSection - Adding Year Stats Note control\n";
    AddItemLabel(hwnd,wc_csyear_notes,(std::string)"",vp,100,500);
    AddToolTip(wc_csyear_notes,wc_csyear_notes_tt,(LPTSTR)csy_n.c_str());
    selected_year = (int) SendMessage(wc_csyear,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
    //if (DEBUG) cout << "Function=BuildYearSection - All Nearly Done Sending Message to set CSYEARS as per " << csyears[selected_year].getyearnotes().c_str() << "\n";
    if (DEBUG) std::cout << "\t\t selected year =" << selected_year << "\n";
    SendMessage(wc_csyear_notes,WM_SETTEXT,(WPARAM)0,(LPARAM)csyears[selected_year].getyearnotes().c_str());
    if (DEBUG) std::cout << "Function=BuildYearSection - All Done Showing WIndow\n";
    ShowWindow(wc_csyear,SW_SHOW);
    vp=vp+vpa;
    if (DEBUG) std::cout << "Function=BuildYearSection - Ending\n";
}

//=======================================================================================
// Build the Adult Section
//=======================================================================================

void BuildAdultsSection(HWND hwnd, int &vp, int vpa)
{

    if (DEBUG) std::cout << "Function=BuildAdultSection - Starting\n" <<  std::endl;

    // Define variables specific to this function
    std::string aname;

    // We must have a minimum of two adults so create enough to make two adults
    if (DEBUG) std::cout << "Function=BuildAdultSection - Building Initial Adults, currently " << adults.size()
                             << " exist\n";
    if (adults.size() < MIN_ADULTS)
    {
        for (int i=(0 + adults.size()); i < MIN_ADULTS; i++)
        {
            aname = "A";
            aname.append(NumberToString(i+1));
            adults.push_back(cs_adult(aname,0,i));
            wc_adults.push_back(wc_adlts());
        }
    }
    if (DEBUG) std::cout << "Function=BuildAdultSection - Equalsing adult windows controls(" << wc_adults.size()
                             << ") and cs adults(" << adults.size() << ")\n";
    if (wc_adults.size() < adults.size())
    {
        unsigned int diff = (adults.size() - wc_adults.size());
        for (unsigned int i=0; i < diff; i++)
        {
            wc_adults.push_back(wc_adlts());
        }
    }

    std::string label = "Step 2 - Add the Adults";

    std::string csa_hlp="Clicking this button will display comprehensive help for this section";
    std::string csa_h="This section is for specifying the Adults.";
    std::string csa_b="Click on this button to add another adult";
    std::string csa_anl="This shows the sequence number of the adult.";
    std::string csa_an="Input the Adults name if you wish to use a more meaningful name.";
    std::string csa_a="Enter the adults Adjusted Taxable Income (ATI).";
    std::string csa_d="Click on this button to delete this adult (note the first two adults cannot be deleted)";

    if (DEBUG) std::cout << "Function=BuildAdultSection - Building Adult section controls (label and Add Adult Button)\n";
    AddItemButton(hwnd, wc_adults_hlp,(std::string)"?",vp,600,15);
    AddToolTip(wc_adults_hlp,wc_adults_hlp_tt,(LPTSTR)csa_hlp.c_str());
    AddSectionLabel(hwnd,wc_adults_label,label,vp, vpa);
    AddToolTip(wc_adults_label,wc_adults_label_tt,(LPTSTR)csa_h.c_str());
    AddSectionButton(hwnd, wc_adults_addbutton,(std::string)"Add Adult",vp-vpa,474,100);
    AddToolTip(wc_adults_addbutton,wc_adults_addbutton_tt,(LPTSTR)csa_b.c_str());

    // Now build the section depending upon the number of adults that exist
    for (unsigned int i=0; i < adults.size(); i++)
    {
        if (DEBUG) std::cout << "Function=BuildAdultSection - Building Controls for Adult" << (i+1) << " of "
                                 << adults.size() << "\n";

        // Add the label for the adult (Adult 1, Adult 2, ....)
        aname="Adult ";
        aname.append(NumberToString(i+1));
        AddItemLabel(hwnd, wc_adults[i].wca_adlt_nmlabel, aname,vp,30,60);
        AddToolTip(wc_adults[i].wca_adlt_nmlabel,wc_adults[i].wca_adlt_nmlabel_tt,(LPTSTR)csa_anl.c_str());
        AddItemEditTXT(hwnd,wc_adults[i].wca_adlt_nm,adults[i].getaname(),vp,95,100);
        AddToolTip(wc_adults[i].wca_adlt_nm,wc_adults[i].wca_adlt_nm_tt,(LPTSTR)csa_an.c_str());
        AddItemLabel(hwnd,wc_adults[i].wca_adlt_atilabel,(std::string)"ATI",vp,205,30);
        AddItemEditNMB(hwnd,wc_adults[i].wca_adlt_ati,adults[i].getati(),vp,240,60);
        AddToolTip(wc_adults[i].wca_adlt_ati,wc_adults[i].wca_adlt_ati_tt,(LPTSTR)csa_a.c_str());
        if (i >= 2)
        {
            AddItemButton(hwnd, wc_adults[i].wca_adlt_dltb,(std::string)"X",vp,450,15);
            AddToolTip(wc_adults[i].wca_adlt_dltb,wc_adults[i].wca_adlt_dltb_tt,(LPTSTR)csa_d.c_str());
        }
        vp=vp+vpa;
    }
    if (DEBUG) std::cout << "Function=BuildAdultSection - Ending\n";
}

//=======================================================================================
// Function to Build the Child Section
//=======================================================================================
void BuildChildSection(HWND hwnd, int &vp, int vpa)
{

    if (DEBUG) std::cout << "Function=BuildChildSection - Starting\n";

    std::string cname,aname;

    if (DEBUG) std::cout << "Function=BuildChildSection - Building initial children, currently " << children.size()
                             << " are defined.\n";
    if (children.size() < MIN_CHILDREN)
    {
        for (int i=0; i < MIN_CHILDREN; i++)
        {
            if (DEBUG) std::cout << "Function=BuildChildSection - Building Child " << (i+1) << " of "
                                     << MIN_CHILDREN << "\n";
            cname = "C";
            cname.append(NumberToString(i+1));
            children.push_back(cs_child(cname,0,0));
            wc_children.push_back(wc_chldrn());
            for (signed int j=0; j < (signed)adults.size(); j++)
            {
                if (DEBUG) std::cout << "Function=BuildChildSection - Building Relationship" << (j+1) << " of "
                                         << adults.size() << ", for Child " << (i+1) << "\n";
                if (j >= children[i].rltsize())
                {
                    children[i].addcare(0);
                    if (j < 2 )
                    {
                        children[i].addrlt(0);
                    }
                    else
                    {
                        children[i].addrlt(1);
                    }
                }
            }
        }
    }

    if (DEBUG) std::cout << "Function=BuildChildSection - Equalising child windows conrtols(" << wc_children.size()
                             << ") with (" << children.size() << ") children.\n";
    if (wc_children.size() < children.size())
    {
        unsigned int diff = (children.size() - wc_children.size());
        for (unsigned int i=0; i < diff; i++)
        {
            wc_children.push_back(wc_chldrn());
            for (signed int j=0; j < (signed)adults.size(); j++)
            {
                if (j >= children[i].rltsize())
                {
                    children[i].addcare(0);
                    if (j < 2) children[i].addrlt(0);
                    else children[i].addrlt(1);
                }
            }
        }
    }

    if (DEBUG) std::cout << "Function=BuildChildSection - Adding Child Section controls (labe + add child button)\n";

    // Set ttoltip strings
    std::string label = "Step 3 - Add the Children";
    std::string csc_hlp="Clicking this button will display comprehensive help for this section";
    std::string csc_h="This section is for specifying Children and the relationship Adults have with the Children.";
    std::string csc_ab="Click on this button to add another Child";
    std::string csc_nl="This shows the sequence number of the Child";
    std::string csc_n="Input the name of the Child, if you wish to use a more meaningful name for the Child.";
    std::string csc_a="Tick this is the Child is 13 or older";
    std::string csc_d="Click on this Button to Delete this Child.";
    std::string csca_n="The Adult's name";
    std::string csca_r="Select the relationship of this Adult to the Child.";
    std::string csca_c="Input the number of Nights of Care this adult cares for the Child.";
    std::string csca_ct="This shows the unallocated number of Nights of Care.";

    AddItemButton(hwnd, wc_children_hlp,(std::string)"?",vp,600,15);
    AddToolTip(wc_children_hlp,wc_children_hlp_tt,(LPTSTR)csc_hlp.c_str());
    AddSectionLabel(hwnd, wc_children_label,label,vp,vpa);
    AddToolTip(wc_children_label, wc_children_label_tt,(LPTSTR)csc_h.c_str());
    AddSectionButton(hwnd, wc_children_addbutton,(std::string)"Add Child",vp-vpa,475,100);
    AddToolTip(wc_children_addbutton, wc_children_addbutton_tt,(LPTSTR)csc_ab.c_str());

    for (unsigned int i=0; i < children.size(); i++)
    {
        if (DEBUG) std::cout << "Function=BuildChildSection - Building Controls for Child " << (i+1)
                                 << " of " << children.size() <<  "\n";
        cname="Child ";
        cname.append(NumberToString(i + 1));
        AddItemLabel(hwnd, wc_children[i].wcc_chld_nmlabel,cname,vp,30,60);
        AddToolTip(wc_children[i].wcc_chld_nmlabel,wc_children[i].wcc_chld_nmlabel_tt,(LPTSTR)csc_nl.c_str());
        AddItemEditTXT(hwnd, wc_children[i].wcc_chld_nm,children[i].GetChildsName(),vp,95,100);
        AddToolTip(wc_children[i].wcc_chld_nm,wc_children[i].wcc_chld_nm_tt,(LPTSTR)csc_n.c_str());
        AddItemLabel(hwnd, wc_children[i].wcc_chld_agelabel,(std::string)"13+",vp,205,30);
        AddItemChkBox(hwnd, wc_children[i].wcc_chld_age,children[i].isteen(),vp,240);
        AddToolTip(wc_children[i].wcc_chld_age,wc_children[i].wcc_chld_age_tt,(LPTSTR)csc_a.c_str());
        if (i > 0)
        {
            AddItemButton(hwnd,wc_children[i].wcc_chld_dltb,(std::string)"X",vp,450,15);
            AddToolTip(wc_children[i].wcc_chld_dltb,wc_children[i].wcc_chld_dltb_tt,(LPTSTR)csc_d.c_str());
        }
        vp=vp+vpa;
        for (unsigned int j=0; j < adults.size(); j++)
        {
            if (DEBUG) std::cout << "Function=BuildChildSection - Building Adult Relationship controls for Adult "
                                     << (j+1) << " of " << adults.size() << " for Child " << (i+1) << "\n";
            aname = adults[j].getaname();
            wc_children[i].InitEmbedSet();
            AddItemLabel(hwnd, wc_children[i].wcc_chld_adlt_nmlabel[j],aname,vp,50,100);
            AddToolTip(wc_children[i].wcc_chld_adlt_nmlabel[j],wc_children[i].wcc_chld_adlt_nmlabel_tt[j],(LPTSTR)csca_n.c_str());
            AddRltDropDown(hwnd, wc_children[i].wcc_chld_adlt_rlt[j],children[i].getrlt(j),vp,155,140);
            AddToolTip(wc_children[i].wcc_chld_adlt_rlt[j],wc_children[i].wcc_chld_adlt_rlt_tt[j],(LPTSTR)csca_r.c_str());
            AddItemLabel(hwnd, wc_children[i].wcc_chld_adlt_carelabel[j],(std::string)"Nights Care",vp,305,80);
            AddItemEditNMB(hwnd, wc_children[i].wcc_chld_adlt_care[j],children[i].GetCare(j),vp,390,40);
            AddToolTip(wc_children[i].wcc_chld_adlt_care[j],wc_children[i].wcc_chld_adlt_care_tt[j],(LPTSTR)csca_c.c_str());
            std::string ct;
            if (j == (adults.size() - 1))
            {
                if(children[i].GetTotalCare() != 0 )
                {
                    ct = "Unallocated Care=";
                    ct.append(NumberToString(children[i].GetTotalCare()));
                }
                else
                {
                    ct = "ALL CARE ALLOCATED";
                }

                AddItemLabel(hwnd, wc_children[i].wcc_chld_care_total,ct,vp,475,150);
                AddToolTip(wc_children[i].wcc_chld_care_total,wc_children[i].wcc_chld_care_total_tt,(LPTSTR)csca_ct.c_str());
            }
            if (children[i].getrlt(j) == 1 || children[i].getrlt(j) == 4)
            {
                SendMessage(wc_children[i].wcc_chld_adlt_care[j],EM_SETREADONLY,(WPARAM)true,(LPARAM)0);
            }
            vp=vp+vpa;
        }
    }
    if (DEBUG) std::cout << "Function=BuildChildSection - Ending\n";
}
//=======================================================================================
// Function to Build the Calculation Section (enables the ability to proceed with calc)
//=======================================================================================
void BuildCalcSection(HWND hwnd,int &vp,int vpa)
{
    if (DEBUG) std::cout << "Function=BuildCalcSection - Starting\n";
    std::string label = "Step 4 - Calculate";
    std::string csc_hlp="Clicking this button will display comprehensive help for this section";
    std::string csc_h="This Section is for Initiating the Calculation.";
    std::string csc_cb="Click this button to initate the Calculations (after doing some checking)";
    std::string csc_sdl="Tick the checkbox to display intermediate calculation values";

    AddItemButton(hwnd, wc_calculate_hlp,(std::string)"?",vp,600,15);
    AddToolTip(wc_calculate_hlp,wc_calculate_hlp_tt,(LPTSTR)csc_hlp.c_str());
    AddSectionLabel(hwnd, wc_calculate_label,label,vp,vpa);
    AddToolTip(wc_calculate_label,wc_calculate_label_tt,(LPTSTR)csc_h.c_str());
    AddItemButton(hwnd, wc_calculate_button,(std::string)"Calculate",vp,30,100);
    AddToolTip(wc_calculate_button,wc_calculate_button_tt,(LPTSTR)csc_cb.c_str());
    AddItemLabel(hwnd, wc_calculate_chk_label,(std::string)"Show Details",vp,205,100);
    AddToolTip(wc_calculate_chk_label, wc_calculate_chk_label_tt,(LPTSTR)csc_sdl.c_str());
    AddItemChkBox(hwnd,wc_calculate_chkbox,calc_details,vp,310);
    AddToolTip(wc_calculate_chkbox,wc_calculate_chkbox_tt,(LPTSTR)csc_sdl.c_str());
    vp=vp+vpa;
    if (DEBUG) std::cout << "Function=BuildCalcSection - Ending\n";
}
//=======================================================================================
// Function to Build the Results Section
//=======================================================================================
void BuildResultsSection( HWND hwnd, int &vp, int vpa)
{

    int hp;
    // Note results deque being superceded by xresults deque
    for (int i=0; i < (signed)results.size(); i++)
    {
        KillWind(results[i]);
    }
    results.clear();
    //=============================================================================
    // Destroy Tooltips
    for (int i=0; i < (signed)xresults_tt.size(); i++) {
        KillWind(xresults_tt[i]);
    }
    xresults_tt.clear();
    //=============================================================================
    // Destroy Help Buttons
    for (int i=0; i < (signed)xresults_hb.size(); i++) {
        KillWind(xresults_hb[i]);
    }
    xresults_hb.clear();
    //=============================================================================
    // Destroy Help Button Text references
    xresults_hbtxt.clear();

    //=============================================================================
    // Destroy any existing windows conrols
    // handles to controls retrieved from xresults deque
    for (int i=0; i < (signed)xresults.size(); i++)
    {
        KillWind(xresults[i].result_lineGetHWND());
    }
    xresults.clear();

    if (DEBUG) std::cout << "Function-BuildResultsSection - Starting # of full cases=" << fullcases.size() << "\n";

    //Anything to do? (i.e. no fullcases then no results are possible)
    if ( fullcases.size() < 1) return;


    // Results Section heading
    std::string label = "Step 5 - Results for ";
    label.append( NumberToString(fullcases.size()));
    label.append(" Cases");
    hp = 10;
    xresults.push_back(result_line());
    AddResultLine(hwnd, xresults[xresults.size()-1], label,
                  RESULTS_HEADING,
                  vp,hp,
                  ACSC_STD_HPHEADINGLEN-hp,
                  RESULTS_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);

    //=============================================================================
    // Stage/report 1
    // =======
    //=============================================================================
    // Heading for High-Level Overview Data
    hp = 20;
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd, xresults[xresults.size() -1],
                  "High Level (Gross) Oveview of CS paid/received"
                  " (Note! Irrespective of Child Support Case).",
                  RESULTS_SUBHEADING,
                  vp,hp,
                  ACSC_STD_HPHEADINGLEN-hp,
                  RESULTS_FULL_LINE,
                  RESULTS_NOTAROW,vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)hlo_tt);

    //=============================================================================
    //Adult Name/Type/Annual/Monthly/Fortnightly/Weekly/Daily headings
    hp = 30;

    //=============================================================================
    //Adult label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "Adult",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_NAMECTLLEN,
                  RESULTS_NOT_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)adult_tt);

    //=============================================================================
    //Type label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "Type",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_SHORTCTLLEN,
                  RESULTS_NOT_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)typ_tt);

    //=============================================================================
    //Annually label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "Annually",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_CTLLEN,
                  RESULTS_NOT_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)annually_tt);

    //=============================================================================
    //Monhtly label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "Monthly",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_CTLLEN,
                  RESULTS_NOT_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)monthly_tt);

    //=============================================================================
    // 2-Weekly Label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "2-Weekly",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_CTLLEN,
                  RESULTS_NOT_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)fortnightly_tt);

    //=============================================================================
    //Weekly label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "Weekly",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_CTLLEN,
                  RESULTS_NOT_FULL_LINE,
                  RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)weekly_tt);

    //=============================================================================
    //Daily rate label
    xresults.push_back(result_line());
    xresults_tt.push_back((HWND)NULL);
    AddResultLine(hwnd,xresults[xresults.size() -1],
                  "Daily Rate",
                  RESULTS_SUBSUBHEADING,
                  vp,hp,
                  ACSC_STD_CTLLEN,
                  RESULTS_FULL_LINE,RESULTS_NOTAROW,
                  vpa);
    AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
               xresults_tt[xresults_tt.size() -1],
               (LPTSTR)daily_tt);


    //=============================================================================
    // Build the High level overview - irrespective of case
    for (int i=0; i < (signed)adults.size(); i++)
    {
        float totCS=0.00f;
        float dlyrate=0.00f;
        for (int j=0; j < (signed)fullcases.size(); j++)
        {
            for (int k=0; k < (signed)fullcases[j].GetfacaseSize(); k++)
            {
                if (fullcases[j].GetfacaseAdult(k) == i)
                {
                    totCS += (Round2dp(fullcases[j].GetfacasePays(k),0)
                              - Round2dp(fullcases[j].GetfacaseCSGets(k),0));
                }
            }
        }
        //curr_rslt = results.size();
        std::string xfertyp = "Pays";
        if (totCS < 0)
        {
            xfertyp = "Receives";
            totCS = 0 - totCS;
        }
        if ((int)totCS == 0)
        {
            xfertyp = "N/A";
        }
        dlyrate = Round2dp(totCS / 365.25,5);

        //=============================================================================
        // High Level Overview (Gross) Adult's Name value
        hp = 30;
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],adults[i].getaname(),
                      (i % 2),vp,hp,ACSC_STD_NAMECTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);

        // High Level Overview (Gross) Type (ie Pays, Receives or N/A) value
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],xfertyp,(i % 2),
                      vp,hp,ACSC_STD_SHORTCTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);

        // High Level Overview (Gross) Annual CS value
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      NumberToString(Round2dp(totCS,0)),(i % 2),
                      vp,hp,ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);

        // High Level Overview (Gross) Monthly CS value
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      NumberToString(Round2dp(totCS/12,2)),(i % 2),
                      vp,hp,ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);

        // High Level Overview (Gross) Fortnighly CS value
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      NumberToString(Round2dp(dlyrate * 14,2)),(i % 2),
                      vp,hp,ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);

        // High Level Overview (Gross) Weekly CS value
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      NumberToString(Round2dp(dlyrate * 7,2)),(i % 2),
                      vp,hp,ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);

        // High Level Overview (Gross) Daily CS value
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      NumberToString(Round2dp(dlyrate,5)),(i % 2),
                      vp,hp,ACSC_STD_CTLLEN,
                      RESULTS_FULL_LINE,
                      (RESULTS_ISAROW | ((i % 2) * RESULTS_EVENROW)),
                      vpa);
    }

    //=========================================================================
    // Stage/report 2 - Case level View
    // =======
    //
    //=========================================================================
    //Display Case Level View
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        std::string lbl="Case Level Overview of CS paid or received for Case ";
        lbl.append(NumberToString(i+1));
        lbl.append(" of ");
        lbl.append(NumberToString(fullcases.size()));
        hp = 20;

        //=====================================================================
        // Heading
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      lbl,
                      RESULTS_SUBHEADING,
                      vp,hp,
                      ACSC_STD_HPHEADINGLEN-hp,
                      RESULTS_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],(LPTSTR)clo_tt);

        //=============================================================================
        //Adult label - Case Level Overview
        hp = 30;
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "Adult",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_NAMECTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)adult_tt);

        //=====================================================================
        // Type label - Case Level Overvieww
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "Type",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_SHORTCTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)adult_tt);

        //=====================================================================
        // Annually Label - Case Level Overview
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "Annually",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)annually_tt);


        //=====================================================================
        // Monthly Label - Case Level Overview
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "Monthly",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)monthly_tt);

        //=====================================================================
        // 2-Weekly Label - Case Level Overview
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "2-Weekly",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)fortnightly_tt);

        //=====================================================================
        // Weekly Label - Case Level Overview
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "Weekly",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_CTLLEN,
                      RESULTS_NOT_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)weekly_tt);

        //=====================================================================
        // Daily Rate Label - Case Level Overview
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      "Daily Rate",
                      RESULTS_SUBSUBHEADING,
                      vp,hp,
                      ACSC_STD_CTLLEN,
                      RESULTS_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)daily_tt);

        // Prepare for looping through each case
        float CaseCS=0.00f;
        float CaseDlyRate=0.00f;
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {

            //=================================================================
            // Case Level Overview Values
            hp = 30;                        // Horizontal position per row
                                            // Note modified by AddResultsLine

            // Adult Name Value - Case Level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],
                          fullcases[i].GetfacaseAdultName(j),
                          0,
                          vp,hp,
                          ACSC_STD_NAMECTLLEN,
                          RESULTS_NOT_FULL_LINE,
                          (RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW))
                          ,vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)typ_tt);

            //=================================================================
            // Determine Type Pays, Receives or N/A (N/A for $0 result)
            // Case Level Overview
            CaseCS = (Round2dp(fullcases[i].GetfacasePays(j),0) -
                       Round2dp(fullcases[i].GetfacaseCSGets(j),0));
            std::string xfertyp = "Pays";
            if (CaseCS < 0)
            {
                xfertyp = "Receives";
                CaseCS = 0 - CaseCS;
            }
            if ((int)CaseCS == 0) {
                xfertyp = "N/A";
// TODO (Mike092015#1#): If $0 result then include min rate/fixed rate blurb

            }
            // Calculate the daily rate
            CaseDlyRate = Round2dp(CaseCS / 365.25,5);

            //=================================================================
            // Transaction type - Case Level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],
                          xfertyp,
                          0,
                          vp,hp,
                          ACSC_STD_SHORTCTLLEN,
                          RESULTS_NOT_FULL_LINE,
                          (RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW))
                          ,vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)typ_tt);


            //=================================================================
            // Annual rate - Case Level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],
                          NumberToString(Round2dp(CaseCS,0)),
                          0,
                          vp,hp,
                          ACSC_STD_CTLLEN,
                          RESULTS_NOT_FULL_LINE,
                          (RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW))
                          ,vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)annually_tt);

            //=================================================================
            // Monthly rate - Case Level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],
                          NumberToString(Round2dp(CaseCS/12,2)),
                          0,
                          vp,hp,
                          ACSC_STD_CTLLEN,
                          RESULTS_NOT_FULL_LINE,
                          (RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW)),
                          vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)monthly_tt);

            //=================================================================
            // 2-Weekly rate value - Case level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],
                          NumberToString(Round2dp(CaseDlyRate * 14,2)),
                          0,
                          vp,hp,
                          ACSC_STD_CTLLEN,
                          RESULTS_NOT_FULL_LINE,
                          (RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW)),
                          vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)fortnightly_tt);

            //=================================================================
            // Weekly Rate value - Case Level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],
                          NumberToString(Round2dp(CaseDlyRate * 7,2)),
                          0,
                          vp,hp,
                          ACSC_STD_CTLLEN,
                          RESULTS_NOT_FULL_LINE,
                          (RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW)),
                          vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)weekly_tt);

            //=================================================================
            // Daily rate - Case Level Overview
            xresults.push_back(result_line());
            xresults_tt.push_back((HWND)NULL);
            AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(Round2dp(CaseDlyRate,5)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_FULL_LINE,(RESULTS_ISAROW | ((j % 2) * RESULTS_EVENROW)),vpa);
            AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                       xresults_tt[xresults_tt.size() -1],
                       (LPTSTR)daily_tt);

        }
    }

    //=========================================================================
    // Stage/report 3 (and on) ONLY if show deatils has been checked
    // =======
    //
    //=========================================================================
    // Show Detailed information if Show details checkbox is selected.
    if (calc_details == true)
    {
        if (DEBUG) std::cout << "Function-BuildResultsSection - Calulation details requested\n";

        //Detailed Information - Section heading
        std::string lbl = "Detailed Information on a per case basis";
        hp = 20;
        xresults.push_back(result_line());
        xresults_tt.push_back((HWND)NULL);
        AddResultLine(hwnd,xresults[xresults.size() -1],
                      lbl,RESULTS_SUBHEADING,
                      vp,hp,
                      ACSC_STD_HPHEADINGLEN-hp,
                      RESULTS_FULL_LINE,
                      RESULTS_NOTAROW,
                      vpa);
        AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                   xresults_tt[xresults_tt.size() -1],
                   (LPTSTR)dtl_tt);

        // Loop though each Case
        for(int i=0; i < (signed)fullcases.size(); i++)
        {
            std::string lbl="Case ";
            lbl.append(NumberToString(i+1));
            lbl.append(" of ");
            lbl.append(NumberToString(fullcases.size()));
            // Add Adult Names to Case heading
            for (int j=0; j < (signed)fullcases[i].facase.size(); j++) {
                if (j == 0) { lbl.append(" for Adults");}
                if(j == ((signed)fullcases[i].facase.size() -1)) { lbl.append(" and"); }
                lbl.append(" ");
                lbl.append(fullcases[i].facase[j].GetAdultName());
            }
            hp = 30;
            //=================================================================
            // Current Case heading (no need for tooltip)
            xresults.push_back(result_line());
            AddResultLine(hwnd,xresults[xresults.size() -1],lbl,RESULTS_SUBHEADING,vp,hp,ACSC_STD_HPHEADINGLEN-hp,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);

            // loop through each adult (fulladultcase)
            for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
            {

                // 1st Line header
                hp = 40;
                //=============================================================
                //Adult Label - Case Detailed Information
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                AddResultLine(hwnd,xresults[xresults.size()-1],
                              "Adult",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_NAMECTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)adult_tt);
                //=============================================================
                //ATI Label - Case detailed Information
                // Prepare deques (mainly for HWND's)
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                xresults_hb.push_back((HWND)NULL);
                xresults_hbtxt.push_back(ati_long);
                // Add ATI Label
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "ATI",RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                // Add the ATI tooltip
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)ati_short);
                // Add the ATI help button
                AddItemButton(hwnd,xresults_hb[xresults_hb.size() -1],(std::string)"?",vp + 1,hp - 5,15);
                // Change horizontal position to consider the help button
                hp = hp + 15;

                //=============================================================
                //SSA Label - Case Detailed Information
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                xresults_hb.push_back((HWND)NULL);
                xresults_hbtxt.push_back(ssa_long);
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "SSA",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)ssa_short);
                AddItemButton(hwnd,xresults_hb[xresults_hb.size() -1],(std::string)"?",vp + 1,hp -5,15);
                hp = hp + 15;

                //=============================================================
                //Less SSSA Label - Case Detailed Information
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "Less SSA",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)less_ssa_tt);

                //=============================================================
                //RDCA Label - Case Detailed Information
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "RDCA",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)rdca_tt);

                //=============================================================
                //Less RDCA Label
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "Less RDCA",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)less_rdca_tt);


                //=============================================================
                // Case Detailed Information - 1st Group of labels and values
                hp = 40;
                //=============================================================
                //Adult Name - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              fullcases[i].GetfacaseAdultName(j),
                              0,
                              vp,hp,
                              ACSC_STD_NAMECTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);

                //=============================================================
                //ATI value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseATI(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);

                //=============================================================
                //SSA value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseSSA(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);
                //=============================================================
                //ATI Less SSA value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseATILessSSA(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);
                //=============================================================
                //RDCA value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseRDCCost(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);
                //=============================================================
                //ATI less SSA less RDCA value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseATILessRDCCost(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);

                //=============================================================
                // Case Detailed Information - 2nd group of labels and values
                hp = 165;       //indent past the adult name column
                //=============================================================
                //Multi-Case Allowance label - Case Detailed Information
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "MCA",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)mca_tt);

                //=============================================================
                //Multi-Case Cap Label - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "MC Cap",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
// TODO (Mike092015#1#): Add ToolTip for MCCAP

                //=============================================================
                //ATI - SSA - RDCA - MCA label - Case Detailed Information
                xresults.push_back(result_line());
                xresults_tt.push_back((HWND)NULL);
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              "Less MCA",
                              RESULTS_SUBSUBHEADING,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_FULL_LINE,
                              RESULTS_NOTAROW,
                              vpa);
                AddToolTip(xresults[xresults.size() -1].result_lineGetHWND(),
                           xresults_tt[xresults_tt.size() -1],
                           (LPTSTR)less_mca_tt);

                //=============================================================
                // Case Detailed Information - 2nd group values
                hp = 165;       // Indent past the adult column
                //=============================================================
                //MCA value - Case detaild Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseMCA(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);

                //=============================================================
                //MC CAP value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseMCCap(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_NOT_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);

                //=============================================================
                //ATI - SSA - RDCA - MCA value - Case Detailed Information
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],
                              NumberToString(fullcases[i].GetfacaseATILessMCA(j)),
                              0,
                              vp,hp,
                              ACSC_STD_CTLLEN,
                              RESULTS_FULL_LINE,
                              RESULTS_ISAROW,
                              vpa);


                //=============================================================
                // Case Detailed Information - 3rd group of labels and values
                hp = 165;       // Indent past the adult column
                //=============================================================
                // CSI - Case detailed Information
// TODO (Mike092015#1#): Addtooltips for CSI, CCSI, COC, PIP and Liability

                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"CSI",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);

                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"CCSI",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"COC",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"PIP",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Liability",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);


                hp = 165;
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].GetfacaseCSI(j)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_ISAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].GetCCSI()),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_ISAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].GetCOC()),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_ISAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].GetfacasePIP(j)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_ISAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].GetfacasePays(j)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_FULL_LINE,RESULTS_ISAROW,vpa);


                hp = 80;
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Per-Child values",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_HPHEADINGLEN-hp,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);
                hp = 90;
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Child",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_NAMECTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Care",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Care%",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Cost%",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Act PCS%",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Eff PCS%",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,RESULTS_NOTAROW,vpa);
                xresults.push_back(result_line());
                AddResultLine(hwnd,xresults[xresults.size() -1],"Child Cost",RESULTS_SUBSUBHEADING,vp,hp,ACSC_STD_CTLLEN,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);

                for (int k = 0; k < fullcases[i].facase[j].GetRltSize(); k++ )
                {

                    hp = 90;
                    //Child's name
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],fullcases[i].facase[j].GetRltChildsName(k),0,vp,hp,ACSC_STD_NAMECTLLEN,RESULTS_NOT_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);
                    //Level of Care
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].facase[j].GetRltCare(k)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);
                    //Care Percentage
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].facase[j].GetRltCarePercentage(k)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);
                    //CS Cost Percentage
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].facase[j].GetRltCostPercentage(k)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);
                    //Parents Actual CS Percentage
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].facase[j].GetRltPCSPC(k)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);
                    //Parents effective CS Pecentage (i.e if Actual is negative then 0)
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].facase[j].GetRltEffPCSPC(k)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_NOT_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);
                    //Cost of This Child
                    xresults.push_back(result_line());
                    AddResultLine(hwnd,xresults[xresults.size() -1],NumberToString(fullcases[i].facase[j].GetRltCostOfTheChild(k)),0,vp,hp,ACSC_STD_CTLLEN,RESULTS_FULL_LINE,(RESULTS_ISAROW | ((k % 2) * RESULTS_EVENROW)),vpa);

                }
            }
        }
        vp = vp + vpa;
        hp = 10;

        std::string terminology[38] = {
            "ATI=Adjusted Taxable Income.",
            "   Your taxable income plus, Pre-Tax Super Contributions, Reportable Fringe Benefits,",
            "   Net Investment Losses,,Target Foreign Income and Specified Tax-Free Pensions or ",
            "   benefits (these should all be on and as per the tax return).",
            "SSA=Self Support Amount.",
            "   An amount (1/3rd AMTAWE) deducted for the parent to support themself.",
            "   Note! this amount changes annually.",
            "AMTAWE=Annualised Male Total Average Weekly Earnings.",
            "   A figure based upon MTAWE as determined and issued by the ABA.",
            "Less SSA=the ATI-SSA.",
            "RDCA=Relevant Dependant Child(ren) Amount.",
            "   An amount deducted from the ATI to consider the support needs of a child or children ",
            "   that are a biological or adopted child of one of the parents.",
            "   The RDCA is determined using the same Cost of Children tables used for CS children ",
            "   BUT only the parent's income (less SSA) is used as the basis.",
            "Less RDCA=the ATI-SSA-RDCA",
            "MCA=Multi-Case Amount.",
            "   An amount deducted from the ATI, after RDC has been deducted, that takes into account",
            "   the CS paid to support children in other cases.",
            "   The MCA is calculated by determining the cost of ALL CS children from ALL cases using",
            "   only the respective parent's income. It is then divided by the number of child in all",
            "   cases and then multiplied by the number of multi-case children.",
            "MC Cap=Multi-Case Cap.",
            "   An amount, used as a cap, to ensure that a parent pays no more for a child than if the",
            "   child were living with the parent.",
            "CSI=Child Support Income.",
            "   The Parent's income less SSA, RDCA and MCA.",
            "CCSI=Combined Child Support Income.",
            "   The combined CSI's for both parents.",
            "COC=Cost of the Children.",
            "   The total cost of the CS children, as determined via the Cost of Children Tables, using"
            "   the Combined Child Support Income (CCSI)",
            "   The Cost of Children tables can be likend to Tax Brackets, however they are more complex",
            "   and as income increases the % of the income applied reduces.",
            "   With Tax there is just one path through the brackets for all incomes. The cost of children",
            "   tables have 8 paths dependant upon the number and age. That is a path each for 1,2 or 3 "
            "   under 13's, a path each for 1,2 or 3 13 or over's and a path each for 2 or 3 children of",
            "   mixed ages.",
            "   The actual income brackets, there are 5, are derived from AMTAWE."
        };


        lbl="Terminology";
        xresults.push_back(result_line());
        AddResultLine(hwnd,xresults[xresults.size() -1],lbl,RESULTS_HELPHEADING,vp,hp,ACSC_STD_HPHEADINGLEN-hp,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);

        for (int i=0;i < 38;i++)
        {
            hp = 20;
            xresults.push_back(result_line());
            if(terminology[i].substr(0,1) != " ")
            {
                hp=20;
                AddResultLine(hwnd,xresults[xresults.size() -1],terminology[i],RESULTS_HELPSUBHEADING,vp,hp,ACSC_STD_HPHEADINGLEN-hp,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);
            } else {
                hp=30;
                AddResultLine(hwnd,xresults[xresults.size() -1],terminology[i],RESULTS_HELPSUBSUBHEADING,vp,hp,ACSC_STD_HPHEADINGLEN-hp,RESULTS_FULL_LINE,RESULTS_NOTAROW,vpa);
            }
        }


        /*
        curr_rslt = results.size();
        results.push_back((HWND)NULL);
        lbl = "ATI=Adjusted Taxable Income, SSA=Self Support Amount, RDCA=Relevant Dependant Child Amount, MCA=Multi-Case Allowance, CSI=Child Support Income.";
        AddItemLabel(hwnd,results[curr_rslt+1],lbl,vp,70,990);
        vp += vpa;
        results.push_back((HWND)NULL);
        lbl = "ATI is the taxable income for last relevant year + reportable fringe benefits + target foriegn income + total net investment loss + tax free pensions/benefits + reportable";
        AddItemLabel(hwnd,results[curr_rslt+2],lbl,vp,85,975);
        vp += (vpa-5);
        results.push_back((HWND)NULL);
        lbl = "superannuation contributions (pre-tax salary sacrifice). All for the last relevant year (i.e. for the last tax year).";
        AddItemLabel(hwnd,results[curr_rslt+3],lbl,vp,100,960);
        vp += vpa;

        results.push_back((HWND)NULL);
        lbl = "SSA (Self Support Amount) is an amount, deducted from the ATI, that is deemed necessary for an adult to support themselves. It is 2/3rd of AMTAWE (Annualised Male Total Average Weekly Earnings). It is amended each year.";
        AddItemLabel(hwnd,results[curr_rslt+4],lbl,vp,85,975);
        vp += vpa;
        results.push_back((HWND)NULL);
        lbl = "RDCA (Relevant Dependant Children Amount)is the amount that is to be deducted in order to consider the support needs of relevant dependant children.";
        AddItemLabel(hwnd,results[curr_rslt+5],lbl,vp,85,975);
        vp += vpa;
        results.push_back((HWND)NULL);
        lbl = "A RDC (Relevant Dependant Child) is a child for whom only 1 of the CS parents is a biological or adoptive parent (i.e. a child from another parent) and for whom their is no CS case. RDC children have ";
        AddItemLabel(hwnd,results[curr_rslt+6],lbl,vp,85,975);
        vp += vpa-5;
        results.push_back((HWND)NULL);
        lbl = "the same right of financial support from their parents. Thier cost is determined using the same cost of children tables but only using the ATI of the 1 parent.";
        AddItemLabel(hwnd,results[curr_rslt+7],lbl,vp,100,960);
        vp += vpa;

        results.push_back((HWND)NULL);
        lbl = "Multi-Case is when more than one CS case is involved (i.e. other children and parent's are involved). The cost of the multi-case children is determined and accounted for by";
        AddItemLabel(hwnd,results[curr_rslt+8],lbl,vp,85,975);
        vp += vpa-5;
        results.push_back((HWND)NULL);
        lbl = "the multi-case allowance ina similar way to RDC children. However, to ensure that a parent doesn't pay more CS than if all the CS children lived in the same household the";
        AddItemLabel(hwnd,results[curr_rslt+9],lbl,vp,100,960);
        vp += vpa-5;
        results.push_back((HWND)NULL);
        lbl = "multi-case cap is determined and then applied if the CS liability exceeds the cap (i.e. if the liability matches the cap).";
        AddItemLabel(hwnd,results[curr_rslt+10],lbl,vp,100,960);
        vp += vpa;
        results.push_back((HWND)NULL);
        lbl = "CSI is the Child Support Income for the parent. It is the amount that will be used for calculation purposes (basically income less CS type deductables).";
        AddItemLabel(hwnd,results[curr_rslt+11],lbl,vp,100,960);
        vp += vpa;
        results.push_back((HWND) NULL);
        lbl = "CCSI is the Combined Child Support Income, which is the CSI of both parents added together.";
        AddItemLabel(hwnd,results[curr_rslt+12],lbl,vp,100,960);
        vp += vpa;
        results.push_back((HWND) NULL);
        lbl = "COC is the Cost of the Children. This is derived by applying the CCSI to the Cost of Children tables.";
        AddItemLabel(hwnd,results[curr_rslt+13],lbl,vp,100,960);
        vp += vpa;

        */
    }
    if (DEBUG) std::cout << "Function-BuildResultsSection - Ending\n";
}
//=======================================================================================
// Function to Rebuild all sections
//=======================================================================================
void ReBuildAll(HWND &wccab, HWND &wccl, HWND &wcaab, HWND &wcal,HWND hwnd)
{

    if (DEBUG) std::cout << "Function=ReBuild - Starting\n";
    HCURSOR h_wcursor = LoadCursor(NULL,IDC_WAIT);
    SetCursor(h_wcursor);


    // Delete Results Section Windows Objects as held in deque<result_line> called xresults
    // The result_line class has members rwnd (type HWND) (window handle)
    // Note! this will eventually replace the results deque (following loop) as
    // the other result_line members contain additional data that allows colouring via
    for (int i =0; i < (signed)xresults_tt.size(); i++)
    {
        KillWind(xresults_tt[i]);
    }
    // the message handling.
    for(int i = 0; i < (signed)xresults.size(); i++)
    {
        KillWind(xresults[i].result_lineGetHWND());
    }

    // Delete Window Objects, deque's in reverse creation order
    for(int i = 0; i < (signed)results.size(); i++)
    {
        KillWind(results[i]);
    }
    KillWind(wc_results_hlp_tt);
    KillWind(wc_results_hlp);
    KillWind(wc_results_label);
    KillWind(wc_results_label_tt);

    //MessageBox(hwnd,"Results DIsplay Removed: Results Deque Size=","Rebuild pt.1",MB_ICONINFORMATION);

    if (DEBUG) std::cout << "Function=ReBuildAll - Destroying Calc windows controls\n";
    //Calculation Section
    KillWind(wc_calculate_chkbox);
    KillWind(wc_calculate_chk_label);
    KillWind(wc_calculate_button);
    KillWind(wc_calculate_label);
    KillWind(wc_calculate_hlp);
    KillWind(wc_calculate_hlp_tt);

    // Children Section
    if (DEBUG) std::cout << "Function=ReBuildAll - Destroying Child windows controls\n";
    for (int i=wc_children.size()-1; i >= 0; i--)
    {
        for (int j=(wc_children[i].wcc_chld_adlt_nmlabel.size()-1); j >= 0; j--)
        {
            if (DEBUG) std::cout << "Function=ReBuildAll - Destroying windows controls for adult per child " << (j+1)
                                     << "and child " << (i+1) <<  "\n";
            if (j == (signed)(wc_children[i].wcc_chld_adlt_nmlabel.size()-1))
            {
                KillWind(wc_children[i].wcc_chld_care_total_tt);
                KillWind(wc_children[i].wcc_chld_care_total);
            }
            KillWind(wc_children[i].wcc_chld_adlt_care_tt[j]);
            KillWind(wc_children[i].wcc_chld_adlt_care[j]);
            KillWind(wc_children[i].wcc_chld_adlt_carelabel_tt[j]);
            KillWind(wc_children[i].wcc_chld_adlt_carelabel[j]);
            KillWind(wc_children[i].wcc_chld_adlt_rlt_tt[j]);
            KillWind(wc_children[i].wcc_chld_adlt_rlt[j]);
            KillWind(wc_children[i].wcc_chld_adlt_nmlabel_tt[j]);
            KillWind(wc_children[i].wcc_chld_adlt_nmlabel[j]);
        }
        if (DEBUG) std::cout << "Function=ReBuildAll - Destroying Windows controls for child " << (i+1) <<  "\n";
        wc_children[i].PopBackEmbedSet();
        if (i > 0)
        {
            KillWind(wc_children[i].wcc_chld_dltb_tt);
            KillWind(wc_children[i].wcc_chld_dltb);
        }
        KillWind(wc_children[i].wcc_chld_age_tt);
        KillWind(wc_children[i].wcc_chld_age);
        KillWind(wc_children[i].wcc_chld_agelabel_tt);
        KillWind(wc_children[i].wcc_chld_agelabel);
        KillWind(wc_children[i].wcc_chld_nm_tt);
        KillWind(wc_children[i].wcc_chld_nm);
        KillWind(wc_children[i].wcc_chld_nm_tt);
        KillWind(wc_children[i].wcc_chld_nmlabel);
    }

    if (DEBUG) std::cout << "Function=ReBuildAll - Destroying windows controls for Child Section\n";
    wc_children.clear();
    //wcc.erase(wcc.begin(),wcc.end());
    KillWind(wccab);
    KillWind(wc_children_hlp_tt);
    KillWind(wc_children_hlp);
    wccab=NULL;
    KillWind(wccl);
    wccl=NULL;

    //Adults Section
    for (int i=wc_adults.size()-1; i >= 0; i--)
    {
        if (DEBUG) std::cout << "Function=ReBuildAll - Destroying Adult windows controls for Adult " << (i+1) << "\n";
        KillWind(wc_adults[i].wca_adlt_ati);
        KillWind(wc_adults[i].wca_adlt_ati_tt);
        KillWind(wc_adults[i].wca_adlt_atilabel);
        KillWind(wc_adults[i].wca_adlt_nm);
        KillWind(wc_adults[i].wca_adlt_nm_tt);
        KillWind(wc_adults[i].wca_adlt_nmlabel);
        KillWind(wc_adults[i].wca_adlt_nmlabel_tt);
        if (i >= 2)
        {
            KillWind(wc_adults[i].wca_adlt_dltb_tt);
            KillWind(wc_adults[i].wca_adlt_dltb);
        }
    }
    if (DEBUG) std::cout << "Function=ReBuildAll - Destroying Adult Section windows controls\n";
    wc_adults.clear();
    //wca.erase(wca.begin(),wca.end());
    KillWind(wcaab);
    KillWind(wc_adults_hlp_tt);
    KillWind(wc_adults_hlp);
    wcaab=NULL;
    KillWind(wcal);
    wcal=NULL;
    KillWind(wc_csyear_label_tt);
    KillWind(wc_csyear_label);
    KillWind(wc_csyear_notes_tt);
    KillWind(wc_csyear_notes);
    KillWind(wc_csyear_tt);
    KillWind(wc_csyear_hlp_tt);
    KillWind(wc_csyear_hlp);
    KillWind(wc_csyear);
    ShowWindow(hwnd,SW_SHOW);

    //MessageBox(hwnd,"All Controls destroyed.","ACD",MB_ICONINFORMATION);

    // Reset the start position for the display of items by resetting vpos and also scrolling to the top of the window
    if (DEBUG) std::cout << "Function - ReBuildAll - Resetting start position of display";
    vpos=10;
    si.nPos = 0;
    si.fMask = SIF_POS;
    SetScrollInfo(hwnd,SB_VERT,&si,true);   // Note ignores the return code

    //MessageBox(hwnd,"Scroll complete.","Scrolled",MB_ICONINFORMATION);

    if (DEBUG) std::cout << "Function - ReBuildAll - Calling Build sections (Adult, Child & Calc)\n";
    SendMessage(hwnd, WM_SETREDRAW, FALSE, 0);
    BuildYearSection(hwnd, vpos, vposadd);
    BuildAdultsSection(hwnd, vpos, vposadd);
    BuildChildSection(hwnd, vpos, vposadd);
    BuildCalcSection(hwnd, vpos, vposadd);
    BuildResultsSection(hwnd, vpos, vposadd);
    //SendMessage(hwnd, WM_SETREDRAW, TRUE, 0);
    ShowWindow(hwnd,SW_SHOW);
    HCURSOR h_ncursor = LoadCursor(NULL,IDC_ARROW);
    SetCursor(h_ncursor);
    //MessageBox(hwnd,"Rebuild Complete","Rebuild Done",MB_ICONINFORMATION);
    if (DEBUG) std::cout << "Function=ReBuildAll - Ending\n";
}
//=======================================================================================
// Function to Add a Section Label
//=======================================================================================
void AddSectionLabel(HWND hwnd, HWND &lhwnd, std::string lbl, int &vp, int vpa)
{

    if (DEBUG) std::cout << "Function=AddSectionLabel - Starting\n";
    lhwnd = CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                           WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT,
                           10,vp+5,380,18, hwnd, NULL, ghInstance, NULL);
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)lbl.c_str());
    //ShowWindow(lhwnd,SW_SHOW);
    vp=vp+vpa+5;
    if (DEBUG) std::cout << "Function=AddSectionLabel - Ending\n";
}
//=======================================================================================
// Function to Add a Label (not to a deque)
//=======================================================================================
void AddItemLabel(HWND hwnd, HWND &lhwnd, std::string lbl, int vp, int hp, int hl)
{
    lhwnd = CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                           WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT,
                           hp,vp,hl,18, hwnd, NULL, ghInstance, NULL);
    if (DEBUG) std::cout << "Function=AddItemLabel - Starting" <<  std::endl;
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)LabelFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)lbl.c_str());
    //ShowWindow(lhwnd,SW_SHOW);
    // if (DEBUG) cout << "Function=AddItemLabel - Ending" <<  endl;
}
//=======================================================================================
// Function to Add a Monetary Label (not to a deque)
//=======================================================================================
void AddItemLabelMny(HWND hwnd, HWND &lhwnd, std::string lbl, int vp, int hp, int hl)
{
    lhwnd = CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                           WS_CHILD | WS_VISIBLE | ES_READONLY | ES_RIGHT,
                           hp,vp,hl,18, hwnd, NULL, ghInstance, NULL);
    if (DEBUG) std::cout << "Function=AddItemLabelMny - Starting" <<  std::endl;
    std::string mnylbl="$";
    mnylbl.append(lbl);
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)LabelFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)mnylbl.c_str());
    //ShowWindow(lhwnd,SW_SHOW);
    // if (DEBUG) cout << "Function=AddItemLabelMny - Ending" <<  endl;
}
//=======================================================================================
// Function to Add a Section Label (not to a deque)
//=======================================================================================
void AddResultsLabel(HWND hwnd, HWND &lhwnd, std::string lbl, int &vp, int vpa)
{
    lhwnd = CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                           WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT,
                           50,vp+5,380,18, hwnd, NULL, ghInstance, NULL);
    if (DEBUG) std::cout << "Function=AddResulsLabel - Starting" <<  std::endl;
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)lbl.c_str());
    //ShowWindow(lhwnd,SW_SHOW);
    vp+=vpa+5;
    if (DEBUG) std::cout << "Function=AddResultsLabel - Ending" <<  std::endl;
}
//=======================================================================================
// Function to Add an edititable text input field (not to a deque)
//=======================================================================================
void AddItemEditTXT(HWND hwnd, HWND &lhwnd, std::string txt, int vp, int hp, int hl)
{
    if (DEBUG) std::cout << "Function=AddItemeditTXT - Starting" << std::endl;

    lhwnd = CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                           WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_LEFT | WS_EX_CONTROLPARENT,
                           hp, vp, hl, 18, hwnd, NULL, ghInstance, NULL);
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)txt.c_str());
    //ShowWindow(lhwnd,SW_SHOW);
    if (DEBUG) std::cout << "Function=AddItemEditTXT - Ending" << std::endl;
}
//***************************************************************************************
// Function to Add an RICH edititable text input field (not to a deque)
//***************************************************************************************
void AddItemRichLabel(HWND hwnd, HWND &lhwnd, std::string txt, int vp, int hp, int hl)
{
    if (DEBUG) std::cout << "Function=AddItemRichEditTXT - Starting" << std::endl;

    lhwnd = CreateWindowEx(ES_SUNKEN,RICHEDIT_CLASS,NULL,
                           WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_LEFT,
                           hp, vp, hl, 18, hwnd, NULL, ghInstance, NULL);
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)txt.c_str());
    if (DEBUG) std::cout << "Function=AddItemRichEditTXT - Complete" << std::endl;
}
//=======================================================================================
// Function to Add an editable numeric input field (not to a deque)
//=======================================================================================
void AddItemEditNMB(HWND hwnd, HWND &lhwnd, int nmb, int vp, int hp, int hl)
{

    if (DEBUG) std::cout << "Function=AddItemEditNMB - Starting" << std::endl;

    lhwnd = CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                           WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER | WS_EX_CONTROLPARENT, hp, vp, hl, 18, hwnd, NULL, ghInstance, NULL);
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)NumberToString(nmb).c_str());
    //ShowWindow(lhwnd,SW_SHOW);
    if (DEBUG) std::cout << "Function=AddItemEditNMB - Ending" << std::endl;
}
//=======================================================================================
// Function to Add a checkbox
//=======================================================================================
void AddItemChkBox(HWND hwnd, HWND &lhwnd, bool sts, int vp, int hp)
{

    if (DEBUG) std::cout << "Function=AddItemChkBox - Starting" << std::endl;

    lhwnd = CreateWindowEx((DWORD)NULL,"BUTTON",NULL,
                           WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_EX_CONTROLPARENT,
                           hp, vp, 15,15, hwnd, NULL, ghInstance,NULL);
    if (sts) SendMessage(lhwnd,BM_SETCHECK,(WPARAM)BST_CHECKED,(LPARAM)0);
    else SendMessage(lhwnd,BM_SETCHECK,(WPARAM)BST_UNCHECKED,(LPARAM)0);
    //ShowWindow(lhwnd,SW_SHOW);
    if (DEBUG) std::cout << "Function=AddItemChkBox - Ending" << std::endl;
}
//=======================================================================================
// Function to Add a Result Line
// !!!!!!!!!!
// !! WARNING modifies vp (vertical position)
// !!!!!!!!!!
// Parameters
//      hwnd                The owning (main) window
//      resultline (by ref) A result_line object
//      txt                 The String to be displayed
//      flags               Flags if  a heading and the type of heading
//      MODIFIEDvp          Vertical position to place EDIT control (WARNING)
//                          nextvp will be added if the lineflag is RESULTS_FULL_LINE
//      hp                  Horizontal position of the EDIT control
//      hl                  Horizontal length of the EDIT control
//      lineflag            if RESULTS_FULL_LINE then vp will be modified to point to
//                          the next line
//      rowflag             An indicator, used in CONTROL_STATIC, to denote type of row.
//                          row here is if the line will be one of tabular type data.
//      nextvp              The amount to add to the vertical position (vp) in order to
//                           position control/s at the next line.
//=======================================================================================
void AddResultLine(HWND hwnd, result_line &resultline,std::string txt, int flags, int &MODIFIEDvp, int &MODIFIEDhp, int hl, int lineflag, int rowflag, int nextvp = 0)
{

    resultline.result_lineSetFlags(flags);          // Ensure result_line contains the flags
    resultline.result_lineSetRowFlags(rowflag);     // Ensure result_line contains the row flags

    // Create the windows EDIT READONLY control
    resultline.result_lineSetHWND(CreateWindowEx((DWORD)NULL,"EDIT",NULL,
                                  WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT,
                                  MODIFIEDhp,MODIFIEDvp,hl,ACSC_STD_CTLHGT, hwnd, NULL, ghInstance, NULL));

    // Set the appropraite font (Headings use Section Font)
    switch (resultline.result_lineGetFlags())
    {

    case RESULTS_HEADING:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
        break;
    }
    case RESULTS_SUBHEADING:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
        break;
    }
    case RESULTS_SUBSUBHEADING:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
    }
    case RESULTS_HELPHEADING:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
        break;
    }
    case RESULTS_HELPSUBHEADING:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
    }
    case RESULTS_HELPSUBSUBHEADING:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)SectionFont,(LPARAM)0);
        break;
    }
    default:
    {
        SendMessage(resultline.result_lineGetHWND(),WM_SETFONT,(WPARAM)LabelFont,(LPARAM)0);
    }
    }
    //Set The text to be displayed as passed
    SendMessage(resultline.result_lineGetHWND(),WM_SETTEXT,(WPARAM)NULL,(LPARAM)txt.c_str());
    ShowWindow(resultline.result_lineGetHWND(),SW_SHOW);

    // If fullline flag is set, ready vertical position (vp) for the next line
    if(lineflag == RESULTS_FULL_LINE)
    {
        MODIFIEDvp = MODIFIEDvp + nextvp;
    }
    MODIFIEDhp = MODIFIEDhp + hl + ACSC_STD_GAP;
    resultline.result_lineSetFlags(flags);
}
//=======================================================================================
// Function to Add a reltionship drop down selector
//=======================================================================================
void AddRltDropDown(HWND hwnd, HWND &lhwnd, int slct, int vp, int hp, int hl)
{

    if (DEBUG) std::cout << "Function=AddRltDropDown - Starting" << std::endl;

    lhwnd = CreateWindowEx((DWORD)NULL,"ComboBox",NULL,
                           WS_TABSTOP | WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_EX_CONTROLPARENT,
                           hp, vp,hl,400, hwnd, NULL, ghInstance,(HWND)NULL);

    for (int unsigned j=0; j < rltype.size(); j++)
    {
        SendMessage(lhwnd,CB_ADDSTRING,(WPARAM)0,(LPARAM)rltype[j].c_str());
    }
    if ((unsigned int)slct >= rltype.size() || slct < 0) slct = 0;
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    SendMessage(lhwnd,CB_SETCURSEL,(WPARAM)slct,(LPARAM)NULL);
    SendMessage(lhwnd,CB_SETITEMHEIGHT,(WPARAM)-1,(LPARAM)15);
    SendMessage(lhwnd,CB_SETITEMHEIGHT,(WPARAM)0,(LPARAM)15);
    if (DEBUG) std::cout << "Function=AddRltDropDown - Ending" << std::endl;
}
//=======================================================================================
// Function to Add a Section Button (will be Add Adult or Add Child)
//=======================================================================================
void AddSectionButton(HWND hwnd, HWND &lhwnd, std::string lbl, int vp, int hp,int hl)
{

    if (DEBUG) std::cout << "Function=AddSectionButton - Starting" << std::endl;
    lhwnd = CreateWindowEx((DWORD)NULL,"BUTTON",NULL,
                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER | WS_EX_CONTROLPARENT,
                           hp,vp,hl,18, hwnd, (HMENU)NULL, ghInstance,(HWND)NULL);
    SendMessage(lhwnd,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)lbl.c_str());
    if (DEBUG) std::cout << "Function=AddSectionButton - Ending" << std::endl;
}
//=======================================================================================
// Function to Add a Button
//=======================================================================================
void AddItemButton(HWND hwnd, HWND &lhwnd, std::string lbl, int vp, int hp, int hl)
{

    if (DEBUG) std::cout << "Function=AddItemButton - Starting" << std::endl;
    lhwnd = CreateWindowEx((DWORD)NULL,"BUTTON",NULL,
                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER | WS_EX_CONTROLPARENT,
                           hp,vp,hl,15, hwnd, (HMENU)NULL, ghInstance,(HWND)NULL);

    SendMessage(lhwnd,WM_SETFONT,(WPARAM)InputFont,(LPARAM)0);
    SendMessage(lhwnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)lbl.c_str());
    if (DEBUG) std::cout << "Function=AddItemButton - Ending" << std::endl;
}
//=======================================================================================
// Function to add a tooltip to a control (note shouldn't add one to edit controls)
//=======================================================================================
void AddToolTip(HWND lhwnd, HWND &thwnd, LPTSTR tip)
{

    thwnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_BALLOON,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           lhwnd, NULL, ghInstance, NULL);

    SetWindowPos(thwnd, HWND_TOPMOST,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    tt.cbSize = sizeof(TOOLINFO);
    tt.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    tt.hwnd = lhwnd;
    tt.hinst = ghInstance;
    tt.lpszText = tip;
    tt.uId = (UINT_PTR)lhwnd;
    GetClientRect(lhwnd, &tt.rect);
    SendMessage(thwnd,TTM_ADDTOOL,(WPARAM)0,(LPARAM)&tt);
    SendMessage(thwnd,TTM_SETMAXTIPWIDTH,(WPARAM)0,(LPARAM)600);
    SendMessage(thwnd,WM_SETFONT,(WPARAM)ToolTipFont,(LPARAM)0);
}
//=======================================================================================
// Function to Add a New Adult
//=======================================================================================
void AddNewAdult(std::deque<cs_child> &chldrn)
{
    if (DEBUG) std::cout << "Function=AddNewAdult - Starting" << std::endl;
    std::string aname = "A";
    aname.append(NumberToString(adults.size()+1));
    while (AdultNameUsed(aname))
    {
        //aname.append("_");
        aname.append(NumberToString(adults.size()+1));
    }
    adults.push_back(cs_adult(aname,0,adults.size()));
    for (signed int i=0; i < (signed)chldrn.size(); i++)
    {
        chldrn[i].addrlt(1);
        chldrn[i].addcare(0);
    }
    if (DEBUG) std::cout << "Function=AddNewAdult - Ending" << std::endl;
}

//=======================================================================================
// Function to Add a New Child
//=======================================================================================
void AddNewChild()
{
    if (DEBUG) std::cout << "Function=AddNewChild - Starting" << std::endl;
    std::string cname = "C";
    cname.append(NumberToString(children.size()+1));
    while (ChildNameUsed(cname))
    {
        cname.append(NumberToString(children.size()));
    }
    children.push_back(cs_child(cname,0,children.size()));
    for (signed int i=0; i < (signed)adults.size(); i++)
    {
        children[children.size()-1].addcare(0);
        if (i < 2) children[children.size()-1].addrlt(0);
        else children[children.size()-1].addrlt(1);
    }
    if (DEBUG) std::cout << "Function=AddNewChild - Ending" << std::endl;
}
//=======================================================================================
// Function to get editbox contents as a string
//=======================================================================================
std::string GetEditBoxData(HWND eb)
{
    // if (DEBUG) cout << "Function=GetEditBoxData - Starting\n";
    int irslt=0;
    std::string s ="";
    irslt = SendMessage(eb,EM_LINELENGTH,(WPARAM)0,(LPARAM)0);
    if (DEBUG) std::cout << "   >>>GetEditBoxData - EM_LINELENGTH returned " << irslt <<"<<<\n";
    TCHAR crslt[256];
    *reinterpret_cast<WORD *>(crslt) = 256;
    SendMessage(eb,EM_GETLINE,(WPARAM)0,(LPARAM) crslt);
    crslt[irslt] = '\0';
    s = (std::string)crslt;
    if (DEBUG) std::cout << "Function=GetEditBoxData - Ending Data retrieved=>>>" << s <<"<<<\n";
    return s;
}
//=======================================================================================
// Function to extract the annual date from a file (csamounts.txt) into instances of
// the cs_amounts class, which builds man of the underlying values that are based on
// the MTAWE (e.g. cost of children tables)
//=======================================================================================
signed int getYearData(std::deque<cs_amounts> &years)
{
    // if (DEBUG) cout << "Function=getYearData - Starting\n";
    std::ifstream ifs;
    std::string fline;
    std::string wline;
    std::string curr;
    size_t loc;
    int yr=0;
    int amtawe=0;
    int pps=0;
    int fa=0;
    int mincs=0;
    int item = 0;
    signed int eod = -1;
    signed int ldir = -1;
    char buffer[MAX_PATH];
    char eolc[] = "\0";
    char eodc[] = "\\";
    GetModuleFileName(NULL,buffer,sizeof(buffer));
    for (int i=0; i<(signed int)sizeof(buffer); i++)
    {
        if (buffer[i] == eolc[0])
        {
            eod = i;
            break;
        }
    }
    for (int i=eod; i > 0; i--)
    {
        if (buffer[i] == eodc[0])
        {
            ldir=i;
            break;
        }
    }
    if ((ldir + sizeof(CS_FNAME)) > sizeof(buffer)) return -2;
    for (signed int i=0; i < (signed int)sizeof(CS_FNAME); i++)
    {
        buffer[ldir+i+1] = CS_FNAME[i];
    }
    ifs.open(buffer,std::ios::in);
    if (ifs.fail())
    {
        // Unable to open the file
        // if (DEBUG) cout << "Function=getYearData - Ending NOT OK -1 (unable to open file)\n";
        return -1;
    }
    else
    {
        while (! ifs.eof())
        {
            getline(ifs,fline);
            if (fline.substr(0,1) == "#") continue;
            wline = fline;
            loc = wline.find(",");
            item = 0;
            while (loc!= std::string::npos)
            {
                curr = wline.substr(0,loc);
                wline = wline.substr(loc+1);
                loc = wline.find(",");
                switch (item)
                {
                case 0:
                    yr = StringToNumber(curr,0);
                    break;
                case 1:
                    amtawe = StringToNumber(curr,0);
                    break;
                case 2:
                    pps = StringToNumber(curr,0);
                    break;
                case 3:
                    fa = StringToNumber(curr,0);
                    break;
                case 4:
                    mincs = StringToNumber(curr,0);
                    break;
                }
                item++;
            }
            if (item == 4 && wline.find(",") == std::string::npos)
            {
                mincs = StringToNumber(wline,0);
                years.push_back(cs_amounts(yr,amtawe,pps,fa,mincs));
            }
            else
            {
                // Ouch we don't have the fifth item so file is unuseable
                return -3;
            }
            item = 0;
        }
        ifs.close();
        // if (DEBUG) cout << "Function=getYearData - Ending OK\n";
        csamounts_found = true;
        return 0;
    }
    // Unable to open the file
    // if (DEBUG) cout << "Function=getYearData - Ending NOT OK -1 (unable to open the file)\n";
    return -1;
}
//=======================================================================================
// Function to check the number of parents a child has
//=======================================================================================
int chknmbofparents(int chld, int rlt)
{
    // if (DEBUG) cout << "Function=chknmbofparentrs - Starting\n";
    int parentcount = 0;
    for (int i=0; i < (signed int)rltype.size(); i++)
    {
        if ((rltype[i]=="Parent") | (rltype[i]=="Parent Abroad") | (rltype[i]=="Deceased Parent"))
        {
            parentcount+=children[chld].parenttypecount(i);
        }
    }
    // if (DEBUG) cout << "Function=chknmbofparents - Ending\n";
    return parentcount;
}
//=======================================================================================
// Function to check if an Adults Name has already been used
//=======================================================================================
bool AdultNameUsed(std::string n)
{
    for (int i=0; i < (signed int)adults.size(); i++)
    {
        if (n == adults[i].getaname()) return true;
    }
    return false;
}
//=======================================================================================
// Function to check if a Childs Name has already been used
//=======================================================================================
bool ChildNameUsed(std::string n)
{
    for (int i=0; i < (signed int)children.size(); i++)
    {
        if (n == children[i].GetChildsName()) return true;
    }
    return false;
}
//=======================================================================================
// Function to Remove an Adult from all Children
//=======================================================================================
void RemoveAdultFromChildren(signed int aid)
{
    for (int i=0; i < (signed int)children.size(); i++)
    {
        children[i].rmvrlt(aid);
    }
}
//=======================================================================================
// Function to pre-emptively fill the last care field for a child
//=======================================================================================
void preemptfill(HWND h, DINT d)
{
    // if (DEBUG) cout << "Function=preemptfill - Starting\n";
    int tc=0;
    int csz=children[d.aint].caresz();
    int tv= children[d.aint].GetCare(d.bint);
    for (int i=0; i < csz; i++)
    {
        tc+=children[d.aint].GetCare(i);
    }
    if (tc > 365)
    {
        int nv = tv - (tc-365);
        children[d.aint].setcare(d.bint,nv);
        SendMessage(wc_children[d.aint].wcc_chld_adlt_care[d.bint],
                    WM_SETTEXT,
                    (WPARAM)0,
                    (LPARAM)NumberToString(nv).c_str());
        MessageBeep(MB_ICONEXCLAMATION);
    }
    tv = children[d.aint].GetCare(d.bint);
    if (d.bint == (csz-2) && (tc < 365))
    {
        int nv = 365 - tc;
        children[d.aint].setcare(d.bint+1,nv);
        SendMessage(wc_children[d.aint].wcc_chld_adlt_care[d.bint+1],
                    WM_SETTEXT,
                    (WPARAM)0,
                    (LPARAM)NumberToString(nv).c_str());
        MessageBeep(MB_ICONEXCLAMATION);
    }
    // if (DEBUG) cout << "Function=preemptfill - Ending\n";
}
//=======================================================================================
//=======================================================================================
// Function to perform the calculations
//=======================================================================================
//=======================================================================================
void DoCalculations(HWND hwnd)
{

    //ApplyEditValues();
    if (DEBUG | CALCDEBUG) std::cout << "Function=DoCalculations - Started" << std::endl;
    if (!DoChecking(hwnd)) return;
    if (DEBUG | CALCDEBUG) for (int i=0; i < (signed) adults.size(); i++) std::cout << adults[i].Display();

    // Prepare deques
    relationships.clear();
    childcases.clear();
    adultcases.clear();

    // Call the functions to progressively create the objects and to then fully populate the objects
    BuildRelationships();               // Build relationship objects (i.e. 1 per adult child combination)
    BuildChildCases();                  // Build Child cases (i.e. where a child has at least 2 CS adults)
    MergeChildCasesIntoFullCases();     // Merge the child cases into full cases
    PromoteAdultsIntoFullCases();       // Apply the appropriate adults to the fullcases
    DetermineRelevantDependants();      // update the fullcase adults with any RDC children
    DetermineMultiCaseChildren();       // update the fullcase adults with Multi-Case Children
    ApplyRDCAmounts();                  // Apply Relevant Dependant Child Amounts where applicable
    ApplyMultiCaseAmounts();            // Apply the multicase amounts - Multi Case allowance and Multi Case Cap
    CalculateIncomePercentages();       // Calculate each parents income percentage (share of CCSI)
    CalculateCostOfChildren();          // Calculate the cost of children and use the result CS to pay receive

    // Just checking
    if (DEBUG | CALCDEBUG) std::cout << "Number of Full Cases=" << fullcases.size() << std::endl;
    if (DEBUG | CALCDEBUG)
    {
        for (int i=0; i < (signed)fullcases.size(); i++)
        {
            std::cout << "Case=" << i << std::endl;
            std::cout << fullcases[i].Display();
            for (int j=0; j < (signed)fullcases[i].GetfccaseSize(); j++)
            {
                int cc = fullcases[i].Getfccase(j);
                std::cout << "\tChild=" << childcases[cc].GetChildName() << std::endl;
                for (int k=0; k < childcases[cc].GetACSize(); k++)
                {
                    int ac = childcases[cc].GetAdultCase(k);
                    std::cout << "\t\tAdult=" << adultcases[ac].GetAdultName() << std::endl;
                    std::cout << "\t\t\tRlt=" << rltype[adultcases[ac].GetRlTyp()];
                    std::cout << "\tCare%=" << adultcases[ac].GetCarepc();
                    std::cout << "\tCost%=" << adultcases[ac].GetCostpc();
                    std::cout << std::endl;
                }

            }
            // Checking that facases have been added and populated.
            for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
            {
                std::cout << "\tAdult=" << fullcases[i].GetfacaseAdultName(j)
                          << "\tATI=" << fullcases[i].GetfacaseATI(j)
                          << "\tSSA=" << fullcases[i].GetfacaseSSA(j)
                          << "\tATI-SSA=" << fullcases[i].GetfacaseATILessSSA(j)
                          << "\n\t\tRDC Minors=" << fullcases[i].GetfacaseRDCMinors(j)
                          << "\tRDC Teens=" << fullcases[i].GetfacaseRDCTeens(j)
                          << "\tMC Minors=" << fullcases[i].GetfacaseMCMinors(j)
                          << "\tMC Teens=" << fullcases[i].GetfacaseMCTeens(j)
                          << "\n\t\tRDC Cost=" << fullcases[i].GetfacaseRDCCost(j)
                          << "\tATI-RDC=" << fullcases[i].GetfacaseATILessRDCCost(j)
                          << "\n\t\tMCA="<< fullcases[i].GetfacaseMCA(j)
                          << "\tATI-MCA="<< fullcases[i].GetfacaseATILessMCA(j)
                          << "\n\t\tPIP="<< fullcases[i].GetfacasePIP(j)
                          << "\n\t\tCHILD SUPPORT INCOME FOR ADULT=$" << fullcases[i].GetfacaseCSI(j)
                          << std::endl;
                for (int k=0; k < (signed)fullcases[i].GetAdultsRltSize(j); k++)
                {
                    std::cout << "\t\tChild=" << fullcases[i].GetAdultsRltChildsName(j,k) << " Relate="
                              << rltype[fullcases[i].GetAdultsRltRType(j,k)]
                              << " 13+=" << fullcases[i].GetAdultsRltAge(j,k)
                              << "\n\t\t\tCare=" << fullcases[i].GetAdultsRltCare(j,k)
                              << "\tCare %=" << fullcases[i].GetAdultsRltCarePercentage(j,k)
                              << "\tCost %=" << fullcases[i].GetAdultsRltCostPercentage(j,k)
                              << "\n\t\t\tCOTC=" << fullcases[i].GetAdultsRltCostOfTheChild(j,k)
                              << "\tMC Cap=$" << fullcases[i].GetAdultsRltMultiCaseCap(j,k)
                              << "\tPCS%=" << fullcases[i].GetAdultsRltPCSPC(j,k)
                              << "\tTot Rcv Care%="<< fullcases[i].GetAdultsRltRcvTotCare(j,k)
                              << std::endl;
                }
                for (int k=0; k < (signed)fullcases[i].GetAdultsMCMinorsSize(j); k++)
                {
                    std::cout << "\t\tMC Minor=" << fullcases[i].GetAdultsMCMinorsChildsName(j,k)
                              << "\tMC Cost=" << fullcases[i].GetAdultsMCMinorsMCCost(j,k)
                              << std::endl;
                }
                for (int k=0; k < (signed)fullcases[i].GetAdultsMCTeensSize(j); k++)
                {
                    std::cout << "\t\tMC Teen=" << fullcases[i].GetAdultsMCTeensChildsName(j,k)
                              << "\tMC Cost=" << fullcases[i].GetAdultsMCTeensMCCost(j,k)
                              << std::endl;
                }
                for (int k=0; k < (signed)fullcases[i].GetfccaseSize(); k++)
                {
                    std::cout << childcases[fullcases[i].Getfccase(k)].Display() << std::endl;
                }
                std::cout << "\t\tCS TO PAY=$" << fullcases[i].GetfacasePays(j) << " for Adult "
                          << fullcases[i].GetfacaseAdultName(j) << " (note! not offest)" << std::endl;
                std::cout << "\t\tCS TO GET=$" << fullcases[i].GetfacaseCSGets(j) << " for Adult "
                          << fullcases[i].GetfacaseAdultName(j) << " (note! not offset)"  << std::endl;
            }
            std::cout << "\n Combined Child Support Income =" << fullcases[i].GetCCSI();
            std::cout << "\n Cost of Children              =" << fullcases[i].GetCOC();
            std::cout << std::endl;
        }
    }
}
//=======================================================================================
// Function to check the input
//=======================================================================================
bool DoChecking(HWND hwnd)
{

    std::deque<std::string> emsg; // Create deque for error messages (empty at this stage)

    if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Preparing adult type deques" << std::endl;
    // Prepare adult type indicators (rtp=parent, rtpa=Parent Abroad, rtdp=Deceased Parent
    std::deque<int> rtp, rtpa, rtdp, ro, rnpc, pcount;
    for (int i=0; i < (signed)adults.size(); i++)
    {
        rtp.push_back(0);
        rtpa.push_back(0);
        rtdp.push_back(0);
    }

    if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Checking Levls of care" << std::endl;
    //check levels of care for the children
    for (int i=0; i < (signed)children.size(); i++)
    {
        if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Child " << (i+1) << " of " << children.size() << std::endl;
        pcount.push_back(0);
        if (!children[i].care365())
        {
            if (DEBUG) std::cout << "Function-DoChecking - Level of care is not 365  for child " << (i+1) << std::endl;
            std::string e="ACSC-0010E - The level of care for child Child";
            e.append(NumberToString(i+1));
            e.append("(");
            e.append(children[i].GetChildsName());
            e.append(") does not total 365. \n\tPlease Amend the Nights Care for the child to total 365.\n");
            emsg.push_back(e);
        }
        // Whilst processing the children also get counts of parent types and the parent count
        for (int j=0; j < (signed)adults.size(); j++)
        {
            if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Retrieving Adult types and parent counts for adult"
                                                 << (j+1) <<  " of " << adults.size() << std::endl;
            switch (children[i].getrlt(j))
            {
            case 0: // Parent
                rtp[j]+=1;
                pcount[i]+=1;
                break;
            case 1: // Other
                ro[j]+=1;
                break;
            case 2: // non-parent carer
                rnpc[j]+=1;
                break;
            case 3: // Parent Abroad
                rtpa[j]+=1;
                pcount[i]+=1;
                break;
            case 4: // Deceased Parent
                rtdp[j]+=1;
                pcount[i]+=1;
                break;
            default:
                break;
            }
        }
    }

    // check for children having too many parents (shouldn't happen due to checks being made after changing rlts)
    for (int i=0; i < (signed)pcount.size(); i++)
    {
        if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Checking Parent counts for child " << (i+1) << " of "
                                             << pcount.size() <<  std::endl;
        if (pcount[i] > 2)
        {
            std::string e="ACSC-0020E - The number of parents for child Child";
            e.append(NumberToString(i+1));
            e.append("(");
            e.append(children[i].GetChildsName());
            e.append(") is greater then 2.\n\tPlease Review the parents defined for this child.");
            emsg.push_back(e);
        }
    }

    // check for ambiguous parent types (i.e. Parent, Parent Abroad and Decease Parent cannot be used
    // for the same parent
    for (int i=0; i < (signed)adults.size(); i++)
    {
        if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Checking Parent ambiguities for Parent" << (i+1) << " of "
                                             << adults.size() << "\n";
        if (DEBUG | CALCDEBUG) std::cout << "         Parent=" << rtp[i] << " Parent Abroad=" << rtpa[i] << " Deceased Parent="
                                             << rtdp[i] << "\n";
        if ((rtp[i] > 0 && (rtpa[i] > 0 || rtdp[i] > 0)) || (rtpa[i] > 0 && rtdp[i] > 0)  )
        {
            std::string e="ACSC-0030E - The Relationships for Adult";
            e.append(NumberToString(i+1));
            e.append("(");
            e.append(adults[i].getaname());
            e.append(") are amibguous. \n\tAn adult can only be one of;");
            e.append("\n\t\tParent, Parent Abroad or Deceased Parent throughout.");
            e.append(" Please amend.");
            emsg.push_back(e);
        }
    }

    // Report on erros, if any
    if (DEBUG | CALCDEBUG) std::cout << "Function-DoDoChecking - Reporting on " << emsg.size() << " errors." << std::endl;
    if (emsg.size() > 0 )
    {
        std::string mstring="";
        for (int i=0; i < (signed)emsg.size(); i++)
        {
            mstring.append(emsg[i]);
            mstring.append("\n");
        }
        MessageBox(hwnd,mstring.c_str(),"Validation Errors!",MB_OK | MB_ICONERROR);
        if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Ending with errors detected" << std::endl;
        emsg.clear();
        return false;
    }
    if (DEBUG | CALCDEBUG) std::cout << "Function-DoChecking - Ending with no errors detected" << std::endl;
    emsg.clear();
    return true;
}
//=======================================================================================
// Function to Build the relationships objects
// a reltionships exists for every child/adult combination
//=======================================================================================
void BuildRelationships()
{
    // Build Relationship Objects
    if (DEBUG | CALCDEBUG | FNC_BUILDRELATIONSHIPS) std::cout << "Function-BuildRelationships - Starting" << std::endl;
    for (int i=0; i < (signed)children.size(); i++)
    {
        for (int j=0; j < children[i].rltsize(); j++)
        {
            relationships.push_back(cs_relationship(i,j,children[i].getrlt(j),children[i].GetCare(j),
                                                    children[i].GetChildsName()));
        }
    }

    if (DEBUG | CALCDEBUG | FNC_BUILDRELATIONSHIPS) std::cout << "Function-BuildRelationships - Ending" << std::endl;
}
//void ConstructChildCases() {
//    if (DEBUG) std::cout << "Function-ConstructChildCases - Starting" << std:endl;
//    for (int i=0; i < (signed)relationships.size(); i++) {
//    }
//}
//=======================================================================================
// Function to Build the Child Case objects and also the undrelying AdultCase objects
// Child cases being an intermediate
//=======================================================================================
void BuildChildCases()
{

    if (DEBUG | CALCDEBUG | FNC_BUILDCHILDCASES) std::cout << "Function-BuildChildCases - Starting" << std::endl;

    // for each child build a child case if the liable parents + recipients totals 2 or more
    for (int i=0; i < (signed)children.size(); i++)
    {
        int liable_parents = 0;
        int recipients = 0;
        for (int j=0; j < children[i].rltsize(); j++)
        {
            switch (children[i].getrlt(j))
            {
            case 0:
                liable_parents+=1;
                break;
            case 2:
                recipients+=1;
                break;
            case 3:
                liable_parents+=1;
                break;
            default:
                break;
            }
        }
        if (DEBUG | CALCDEBUG | FNC_BUILDCHILDCASES) std::cout << "Function-BuildChildCases - Child=" << i <<
                    " LiableParents=" << liable_parents <<  " recipients=" << recipients << std::endl ;

        // check for the number of liable parents and recipients and if 2 or more create a child case
        if (liable_parents >=1 && (liable_parents + recipients) >=2)
        {
            if (DEBUG | CALCDEBUG | FNC_BUILDCHILDCASES) std::cout << "Function-BuildChildCases - Creating ChildCase\n";
            int ix = childcases.size();
            childcases.push_back(cs_childcase(i, children));
            for (int j=0; j < (signed)relationships.size(); j++)
            {
                if ( relationships[j].GetChildsName() != children[i].GetChildsName()) continue;
                if ( rltype[relationships[j].GetRelationshipType()] == "Other") continue;
                adultcases.push_back(cs_adultcase(relationships[j].GetAdult(),j,adults,relationships));
                childcases[ix].AddAdultCase( adultcases.size()-1, adultcases);
            }
            if (DEBUG | CALCDEBUG | FNC_BUILDCHILDCASES) std::cout << "\nResult of ChildCase Build \n" <<
                        childcases[ix].Display();
        }
    }

    if (DEBUG | CALCDEBUG | FNC_BUILDCHILDCASES) std::cout << "Function-BuildChildCases - Ending\n";

}
//=======================================================================================
// Function to Merge childcases into fullcases.
// That is that CS children with the same parents (parent, parent abroad or deceased parent)
// will be placed into the same case.
//=======================================================================================
void MergeChildCasesIntoFullCases()
{

    if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "Function-MergeChildCasesIntoFullCases - Starting #of chidlcases=" << childcases.size() << std::endl;
    fullcases.clear();
    if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tNumber of Childs cases=" << childcases.size();
    for (int i=0; i < (signed)childcases.size(); i++)
    {
        if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tCurrent iteration (Child Case) = " << i << std::endl;
        if (childcases[i].IsMerged())
        {
            if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tSkipped as already merged." << std::endl;
            continue;
        }
        if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tCreating fullcase for iteration=" << i << "\n";
        fullcases.push_back(cs_fullcase(i));
        childcases[i].SetMerged();
        if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tChecking for other child cases with the same parent as the current case" << std::endl;
        for (int j=0; j < (signed)childcases.size(); j++)
        {
            if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tOther Child going to be child " << j << std::endl;
            if ((j != i) && !childcases[j].IsMerged())
            {
                if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tChecking Child Case " << j << " (against main case " << i << ")" << std::endl;
                if (DoParentsMatch(i,j))
                {
                    fullcases[fullcases.size()-1].AddChildCase(j);
                    childcases[j].SetMerged();
                }
                else if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tSkip CCase " << j << " (against main case " << i << ") No Parents Match" << std::endl;
            }
            else if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "\tSkip CCase " << j << " (against main case " << i << ") Merged or Main case" << std::endl;
        }
    }
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        int csteen_cnt = 0;
        int csminor_cnt = 0;
        for (int j=0; j < (signed)fullcases[i].GetfccaseSize(); j++)
        {
            if (children[fullcases[i].Getfccase(j)].isteen()) csteen_cnt+=1;
            else csminor_cnt +=1;
        }
        fullcases[i].SetChildCounts(csminor_cnt,csteen_cnt);
    }
    if (DEBUG | CALCDEBUG | FNC_MERGECHILDCASESINTOFULLCASES) std::cout << "Function-MergeChildCasesIntoFullCases - Ending #of fulcases=" << fullcases.size() <<  std::endl;
}
//=======================================================================================
// Function to promote adults into fullcases
// That is that the adults, as obtained by looking at the child cases refrenced in the full case
// and then going through the adultcases referenced in the childcase to get the repsective
// adultcase and then using this to access the underlying adult. If the adult already exists
// as a fullcase adult (facase deque), then it is not added again.
//=======================================================================================
void PromoteAdultsIntoFullCases()
{
    if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES) std::cout << "Function-PromoteAdultsIntoFullCases - Starting\n # of FullCases=" << fullcases.size() << std::endl;
    // Loop through each fullcase
    for (int i=0; i < (signed) fullcases.size(); i++)
    {
        if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES) std::cout << "\tCurrent FullCase index=" << i << "\t# of Full Child cases="
                    << fullcases[i].GetfccaseSize() << std::endl;
        // loop through the full child cases
        for (int j=0; j < (signed)fullcases[i].GetfccaseSize(); j++)
        {
            if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES) std::cout << "\tCurrent Full Child Case index=" << j << std::endl;
            //This child case
            int cc = fullcases[i].Getfccase(j);
            if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES) std::cout << "\tPoints to child index=" << cc << std::endl;
            // loop through the adult cases of the child case pointed to by this full child case
            for (int k=0; k < childcases[cc].GetACSize(); k++)
            {
                // Now see if this adult is already definied
                bool facase_exists = false;
                int ac = childcases[cc].GetAC(k);
                int curr_facase = -1;
                // Check to see if the adult already exists
                for (int l=0; l < fullcases[i].GetfacaseSize(); l++)
                {
                    if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES)
                    {
                        std::cout << "\tComparing Potential Adult=" << adultcases[ac].GetAdult() << " with existing Adult="
                                  << fullcases[i].GetfacaseAdult(l) << std::endl;
                    }
                    if (fullcases[i].GetfacaseAdult(l) == adultcases[ac].GetAdult())
                    {
                        curr_facase = l;
                        facase_exists = true;
                    }
                }
                // As the adult doesn't exist yet add the adult
                if (!facase_exists)
                {
                    int ati = 0;
                    curr_facase = fullcases[i].GetfacaseSize();
                    if (rltype[relationships[adultcases[ac].GetRlt()].GetRelationshipType()] != "Non-Parent Carer")
                    {
                        ati = adults[adultcases[ac].GetAdult()].getati() ;
                    }
                    //int ati = adults[adultcases[ac].GetAdult()].getati();
                    int ssa = csyears[selected_year].getssa();
                    fullcases[i].AddFullAdultCase(adultcases[ac].GetAdult(), adultcases[ac].GetAdultName(),ati,ssa);
                }
                // Get the relationship for this adultcase and add it as a full relationship
                int curr_rlt = adultcases[ac].GetRlt();
                std::string cnm = relationships[curr_rlt].GetChildsName();
                int cref = relationships[curr_rlt].getchild();
                int aref = relationships[curr_rlt].GetAdult();
                int rtype = relationships[curr_rlt].GetRelationshipType();
                int care = relationships[curr_rlt].GetCare();
                int carepc = relationships[curr_rlt].GetCarePercentage();
                int costpc = relationships[curr_rlt].GetCostPercentage();
                int teen = children[cref].isteen();
                if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES)
                {
                    std::cout << "\t\tAdding Full Relationship for Adult " << adultcases[ac].GetAdultName();
                    std::cout << " into case " << i;
                    std::cout << " as RLT " << fullcases[i].GetAdultsRltSize(curr_facase);
                    std::cout << "\n\t\t\tChild=" << cnm << " idx=" << cref << "\t Adult=" << adults[aref].getaname();
                    std::cout << "\n\t\t\tTeen=" << BoolToString(teen) << "\tRelType=" << rltype[rtype];
                    std::cout << "\n\t\t\tCare=" << care << "\tCare%=" << carepc << "\tCost%=" << costpc << std::endl;
                }
                fullcases[i].AddFullRelationshipToAdult(curr_facase, cref, cnm, aref, rtype, care, carepc, costpc, teen);
                if ( rltype[rtype] == "Parent Abroad") fullcases[i].SetParentAbroad();
                if ( rltype[rtype] == "Non-Parent Carer") fullcases[i].SetNonParentCarer();
            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_PROMOTEADULTSINTOFULLCASES) std::cout << "Funtion-PromoteAdultsIntoFullCases - Ending" << std::endl;
}
//=======================================================================================
// Function to Determine Relevant Dependant Children
//=======================================================================================
void DetermineRelevantDependants()
{
    if (DEBUG | CALCDEBUG | FNC_DETERMINERELEVANTDEPENDANTS) std::cout << "Function-DetermineRelevantDependants - Starting" << std::endl;
    // Loop through fullcases
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        if (DEBUG | CALCDEBUG | FNC_DETERMINERELEVANTDEPENDANTS) std::cout << "\tCurrent Case=" << i+1 << std::endl;
        // loop through the adults
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            if (DEBUG | CALCDEBUG | FNC_DETERMINERELEVANTDEPENDANTS) std::cout << "\t\tCurrent Adult=" << j+1 << std::endl;
            int adlt = fullcases[i].GetfacaseAdult(j);
            // Loop through the relationships
            for (int k=0; k < (signed)relationships.size(); k++)
            {
                int radlt = relationships[k].GetAdult();
                std::string rtype = rltype[relationships[k].GetRelationshipType()];
                // Relationship must be for this adult and only if the relationship type is parent or parent abroad
                if ((radlt == adlt) && (rtype == "Parent" || rtype == "Parent Abroad"))
                {
                    int parent_count = 1;
                    // If a valid relationship then get the child
                    int rchld = relationships[k].getchild();
                    // Now go through the relationships checking for this child, a parent, but not this parent
                    // and if so increment the parent count
                    for (int l=0; l < (signed)relationships.size(); l++)
                    {
                        int crchld = relationships[l].getchild();
                        int cradlt = relationships[l].GetAdult();
                        std::string crtype = rltype[relationships[l].GetRelationshipType()];
                        if ((crchld == rchld) && (radlt != cradlt) && (crtype == "Parent" || crtype == "Parent Abroad"
                                || crtype == "Non-Parent Carer"))
                        {
                            parent_count +=1;
                        }
                    }
                    // See if we have a relevant dependant child and if so increment
                    if (parent_count == 1)
                    {
                        if (children[rchld].isteen()) fullcases[i].AddRelevantDependantTeen(j);
                        else fullcases[i].AddRelevantDependantMinor(j);
                        if (DEBUG | CALCDEBUG | FNC_DETERMINERELEVANTDEPENDANTS) std::cout << "RDC found for parent." << std::endl;
                    }
                }
            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_DETERMINERELEVANTDEPENDANTS) std::cout << "Function-DetermineRelevantDependants - Ending" << std::endl;
}
//=======================================================================================
// Function to check if the parents of one child case match the parents of the other
// childcase.
//=======================================================================================
void DetermineMultiCaseChildren()
{
    if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "Function-DetermineMultiCaseChildren - Starting" << std::endl;
    // Loop through the full cases
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        // Loop through the adult cases
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            // Get this adult
            int adlt = fullcases[i].GetfacaseAdult(j);
            // adult must be parent not non-parent carer so check this is the case
            bool ok_rtyp = false;
            for (int jj=0; jj < (signed)fullcases[i].GetAdultsRltSize(j); jj++)
            {
                int rtyp = fullcases[i].GetAdultsRltRType(j,jj);
                if (rltype[rtyp] == "Parent" || rltype[rtyp] == "Parent Abroad") ok_rtyp = true;
            }
            // skip if not a parent
            if (ok_rtyp == false) continue;
            // Loop through other cases
            if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "\tAdult " << fullcases[i].GetfacaseAdultName(j) << " could be MC parent." << std::endl;
            for (int k=0; k < (signed)fullcases.size(); k++)
            {
                // skip this case
                if (k==i) continue;
                // Loop through the adults of the other case
                for (int l=0; l < (signed)fullcases[k].GetfacaseSize(); l++)
                {
                    int sadlt = fullcases[k].GetfacaseAdult(l);
                    // Do the adults of the main case and this other case match if not skip
                    if (adlt != sadlt) continue;
                    // Loop through the relationships in the other case
                    if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "\tAdult " << fullcases[i].GetfacaseAdultName(j) << " in another Case." << std::endl;
                    for (int m=0; m < (signed)fullcases[k].GetAdultsRltSize(l); m++)
                    {
                        std::string srtyp = rltype[fullcases[k].GetAdultsRltRType(l,m)];
                        bool steen = fullcases[k].GetAdultsRltAge(l,m);
                        // Check if the adult in the other case is a parent
                        if (srtyp == "Parent" || srtyp == "Parent Abroad")
                        {
                            if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "\tAdult " << fullcases[i].GetfacaseAdultName(j)
                                        <<" is parent in another case so adding MC child" << std::endl;
                            if (steen) fullcases[i].AddMultiCaseTeen(j);
                            else fullcases[i].AddMultiCaseMinor(j);
                            if (steen)
                            {
                                fullcases[i].AddMCTeenRltToAdult(j,
                                                                 fullcases[k].GetAdultsRltChild(l,m),
                                                                 fullcases[k].GetAdultsRltChildsName(l,m),
                                                                 fullcases[k].GetAdultsRltAdult(l,m),
                                                                 fullcases[k].GetAdultsRltRType(l,m),
                                                                 fullcases[k].GetAdultsRltCare(l,m),
                                                                 fullcases[k].GetAdultsRltCarePercentage(l,m),
                                                                 fullcases[k].GetAdultsRltCostPercentage(l,m));
                                if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "\n\t"
                                            << fullcases[i].AdultsMCTeensDisplay(j,fullcases[i].GetAdultsMCTeensSize(j)-1);
                            }
                            else
                            {
                                fullcases[i].AddMCMinorRltToAdult(j,
                                                                  fullcases[k].GetAdultsRltChild(l,m),
                                                                  fullcases[k].GetAdultsRltChildsName(l,m),
                                                                  fullcases[k].GetAdultsRltAdult(l,m),
                                                                  fullcases[k].GetAdultsRltRType(l,m),
                                                                  fullcases[k].GetAdultsRltCare(l,m),
                                                                  fullcases[k].GetAdultsRltCarePercentage(l,m),
                                                                  fullcases[k].GetAdultsRltCostPercentage(l,m));
                                if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "\n\t"
                                            << fullcases[i].AdultsMCMinorsDisplay(j,fullcases[i].GetAdultsMCMinorsSize(j)-1);
                            }
                        }
                    }
                }
            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_DETERMINEMULTICASECHILDREN) std::cout << "Function-DetermineMultiCaseChildren - Ending" << std::endl;
}
//=======================================================================================
// Function to calculate and apply Cost of Relevant Dependant Children
// Note that this includes applying the costs even if there aren't any
// (i.e. if no RDC's RDC Cost is 0 however this applies a 0 reduction to the ATI-RDC amount)
//=======================================================================================
void ApplyRDCAmounts()
{
    if (DEBUG | CALCDEBUG | FNC_APPLYRDCAMOUNTS)
    {
       std::cout <<
                 "Function=ApplyRDCAmounts - Starting" << std::endl;
    }
    // Loop thought the cases
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            // Get RDC numbers
            int rdc_m=fullcases[i].GetfacaseRDCMinors(j);
            int rdc_t=fullcases[i].GetfacaseRDCTeens(j);
            // any RDC's then calculate the RDC amount
            fullcases[i].SetfacaseRDCCost(j,csyears[selected_year].calccostofchild(rdc_m,rdc_t,fullcases[i].GetfacaseATILessSSA(j)));

            if ( DEBUG | CALCDEBUG | FNC_APPLYRDCAMOUNTS)
            {
                std::cout <<
                    "Applying RDC counts for Case " << (i+1) <<
                    " Adult=" << fullcases[i].GetfacaseAdultName(j) <<
                    "\n\tMinors RDC=" << rdc_m << "\tTeens RDC=" << rdc_t <<
                    "\n\tRDC Cost=" << fullcases[i].GetfacaseRDCCost(j) << std::endl;
            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_APPLYRDCAMOUNTS)
    {
        std::cout <<
                  "Function=ApplyRDCAmounts - Ending" << std::endl;
    }
}
//=======================================================================================
// Function to Calculate and apply Multi-Case Allowances, to then set the multi-costs
// and then the Combined Child Support Income
//=======================================================================================

void ApplyMultiCaseAmounts()
{
    // Loop through fullcases
    if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
    {
        std::cout <<
                  "Function-ApplyMultiCaseAmounts - Starting" << std::endl;
    }
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        // Loop through adults
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            int cschld_cnt = fullcases[i].GetfccaseSize();
            int mcteen_cnt = fullcases[i].GetAdultsMCTeensSize(j);
            int mcminor_cnt = fullcases[i].GetAdultsMCMinorsSize(j);
            int totchld = cschld_cnt + mcteen_cnt + mcminor_cnt;
            int atilessrdc = fullcases[i].GetfacaseATILessRDCCost(j);
            int mccost=0;
            // Work out Multi-case allowance  and multi-case cap for Teens
            for (int k=0; k < mcteen_cnt; k++)
            {
                int this_mccost = csyears[selected_year].calccostofchild(0,totchld,atilessrdc);
                if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << "\n\tMC Cost This Child=" <<
                              this_mccost << "\t(before div by tot children)";
                }
                mccost+=this_mccost;
                this_mccost = Round2dp((float)this_mccost/(float)totchld,0);
                if(DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              "\tMC Cost This Child=" << this_mccost << " Acuumulated=" << mccost << std::endl;
                }
                if(DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS )
                {
                    std::cout << std::endl <<
                              "\tRetrieved Cost%=" << fullcases[i].GetAdultsMCTeensCostPercentage(j,k);
                }
                //int this_mccap = Round((this_mccost * (100-fullcases[i].GetAdultsMCTeensCostPercentage(j,k))) / 100);
                float base_mccap = (float) (this_mccost *
                                            ((float) (100 - fullcases[i].GetAdultsMCTeensCostPercentage(j,k)) / (float) 100) );
                int this_mccap = Round2dp(base_mccap,0);
                if(DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl << "\tMC Cap=" << this_mccap << std::endl;
                }
                fullcases[i].SetAdultsMCTeensValues(j,k,this_mccost, this_mccap);
                if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              fullcases[i].AdultsMCTeensDisplay(j,k);
                }
                AdjustMCCap(i,fullcases[i].GetAdultsMCTeensChildsName(j,k),this_mccap);
                if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl << fullcases[i].AdultsMCTeensDisplay(j,k);
                }
            }
            // Work out Mulit-case allowance and multi-case cap for Minors
            for (int k=0; k < mcminor_cnt; k++)
            {
                int this_mccost = csyears[selected_year].calccostofchild(totchld,0,atilessrdc);
                if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              "\tMC Cost This Child=" << this_mccost << "\t(before div by tot children)";
                }
                mccost+=this_mccost;
                this_mccost = Round2dp((float)this_mccost/(float)totchld,0);
                if(DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              "\tMC Cost This Child=" << this_mccost << " Acuumulated=" << mccost << "\n";
                }
                if(DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              "\tRetrieved Cost%=" << fullcases[i].GetAdultsMCMinorsCostPercentage(j,k);
                }
                //int this_mccap = Round((this_mccost * (100-fullcases[i].GetAdultsMCMinorsCostPercentage(j,k))) / 100);
                float base_mccap = (float) (this_mccost * ((float) (100 - fullcases[i].GetAdultsMCMinorsCostPercentage(j,k)) / (float) 100) );
                int this_mccap = Round2dp(base_mccap,0);
                // fullcases[i].SetfacaseMCCap(j,this_mccap);
                if(DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              "\tMC Cap=" << this_mccap << std::endl;
                }
                fullcases[i].SetAdultsMCMinorsValues(j,k,this_mccost,this_mccap);
                if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              fullcases[i].AdultsMCMinorsDisplay(j,k);
                }
                AdjustMCCap(i,fullcases[i].GetAdultsMCMinorsChildsName(j,k),this_mccap);
                if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
                {
                    std::cout << std::endl <<
                              fullcases[i].AdultsMCMinorsDisplay(j,k);
                }
            }
            fullcases[i].SetfacaseMCCost(j,Round(mccost/totchld));
            fullcases[i].IncreaseCombinedCSIncome(fullcases[i].GetfacaseCSI(j), csyears[selected_year]);
        }
    }
    if (DEBUG | CALCDEBUG | FNC_APPLYMULTICASEAMOUNTS)
    {
        std::cout <<
                  "Function-ApplyMultiCAseAmounts - Ending" << std::endl;
    }
}
//=======================================================================================
// Function to make adjustments for the multicase cap, that is to apply the cap from the
// case in which it is worked out to the other case that the child is in.
//=======================================================================================
void AdjustMCCap(int fcase, std::string cname,int mcap)
{
    if (DEBUG | CALCDEBUG | FNC_ADJUSTMCCAP) std::cout << "Fundtion-AdjustMCCap Starting - Parms Case=" << fcase << " MC Childs Name=" << cname <<
                " MC Cap=" << mcap << "\n";
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        if (i == fcase) continue;
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); i++)
        {
            for (int k=0; k < (signed)fullcases[i].GetAdultsRltSize(j); k++)
            {
                if (DEBUG | CALCDEBUG | FNC_ADJUSTMCCAP)
                {
                    std::cout << "Checking if MC Childs Name(" << cname << ") is current Childs Name(" <<
                              fullcases[i].GetAdultsRltChildsName(j,k) << std::endl;
                }
                if (fullcases[i].GetAdultsRltChildsName(j,k) == cname) fullcases[i].SetAdultsRltMCCap(j,k,mcap);

            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_ADJUSTMCCAP)
    {
        std::cout << "Function-AdjustMCCap Ending" << std::endl;
    }
}
//=======================================================================================
// Function to apply the Parents Income Percentages (PIP)
// PIP is the percentage of the combined child support income the parent pays. This
// then being used in addition to the cost percentage (care reduction) to determine the
// portion of the cost of the child a parent pays in CS.
//=======================================================================================
void CalculateIncomePercentages()
{
    if (DEBUG | CALCDEBUG | FNC_CALCULATEINCOMEPERCENTAGES) std::cout << "Function-CalculateIncomePercentages - Starting" << std::endl;
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            if (fullcases[i].GetfacaseCSI(j) > 0)
            {
                if (DEBUG | CALCDEBUG | FNC_CALCULATEINCOMEPERCENTAGES) std::cout << "\tSetting PIP for Parent " << j << " CSI=" << fullcases[i].GetfacaseCSI(j)
                            << " CCSI=" << fullcases[i].GetCCSI() << std::endl;
                fullcases[i].SetfacasePIP(j, Round2dp((((float)fullcases[i].GetfacaseCSI(j) / (float)fullcases[i].GetCCSI()) * 100.000f),2));
            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_CALCULATEINCOMEPERCENTAGES) std::cout << "Function-CalculateIncomePercentages - Ending" << std::endl;
}
//=======================================================================================
// Function to calculate the Cost of Children and apply that cost to the relationship
// record for the adult/child relationship.
//=======================================================================================
void CalculateCostOfChildren()
{

    if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN )
    {
        std::cout << "Function CalculateCostOfChilrden - Starting" << std::endl;
    }
    // Loop through Fullcases
    for (int i=0; i < (signed)fullcases.size(); i++)
    {
        // Get the total cost of the children
        float coc = (float)fullcases[i].GetCOC();
        // Get the cost of one CS child
        int costperchild = (int)Round2dp(coc / fullcases[i].GetfccaseSize(),0);
        fullcases[i].SetCostPerChild(costperchild);
        if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN )
        {
            std::cout << "Cost per CS child=" << costperchild << std::endl;
        }
        // For each full adult case
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            // for each CS child
            for (int k=0; k < (signed)fullcases[i].GetAdultsRltSize(j); k++)
            {
                // Get the relationship type for this adult/child
                std::string rlt = rltype[fullcases[i].GetAdultsRltRType(j,k)];
                // Get the care percentage
                int carepc = fullcases[i].GetAdultsRltCarePercentage(j,k);
                // Calculate the Parent's Income Percentage and store it in the adult/child relationship
                float pcsp = Round2dp(fullcases[i].GetfacasePIP(j) - (float)fullcases[i].GetAdultsRltCostPercentage(j,k),2);
                fullcases[i].SetAdultsRltPCSPC(j,k,pcsp);
                // Calculate the cost of this child (0 as the default)
                float cotc=0;
                fullcases[i].SetAdultsRltPreAdjCOTC(j,k,(int)Round2dp(cotc,0));
                fullcases[i].SetAdultsRltCOTC(j,k,(int)Round2dp(cotc,0));
                if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                {
                    std::cout << "\t\tParents CS%=" <<
                              fullcases[i].GetAdultsRltPCSPC(j,k) << "\tFor parent "
                              << fullcases[i].GetfacaseAdultName(j)
                              << "\tfor child " << fullcases[i].GetAdultsRltChildsName(j,k) << std::endl;
                }
                // To pay the parents CS percentage must be positive and the level of care 65% or less
                // (over 65% care results in the parent not having to pay any CS)
                if (pcsp > 0 && carepc <= 65)
                {
                    // Work out the cost of this child (i.e. parents CS percentage of the cost per a child)
                    cotc = Round2dp(((float)costperchild * pcsp) / 100.0f,0);
                    fullcases[i].SetAdultsRltPreAdjCOTC(j,k,(int)cotc);
                    // If applicable make the parent abroad adjustment
                    if (fullcases[i].IsParentAbroadApplicable() && rlt == "Parent Abroad")
                    {
                        cotc = Round2dp((float)cotc / (float)2.0f,0);
                    }
                    // Apply the MultiCase Cap if applicable
                    int mccap = fullcases[i].GetAdultsRltMultiCaseCap(j,k);
                    if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN )
                    {
                        std::cout << "\tApplying Multicase Cap for Adult=" <<
                                  fullcases[i].GetfacaseAdultName(j) <<
                                  " Child=" << fullcases[i].GetAdultsRltChildsName(j,k) <<
                                  "\n\t\tCOTC=" << cotc << " MC Cap=" << mccap << std::endl;
                    }
                    if (mccap > 0 && cotc > mccap) cotc = mccap;
                }
                // Store the cost of this child
                fullcases[i].SetAdultsRltCOTC(j,k,(int)Round2dp(cotc,0));
                // Accumulate the total CS cost for this adult
                fullcases[i].AddfacaseCSCost(j,Round2dp(cotc,0));
                if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN )
                {
                    std::cout << "\t\tCost Child " <<
                              fullcases[i].GetAdultsRltChildsName(j,k) << " =" <<
                              fullcases[i].GetAdultsRltCostOfTheChild(j,k) << "\tFor Parent " <<
                              fullcases[i].GetfacaseAdultName(j) << "\tChild " <<
                              fullcases[i].GetAdultsRltChildsName(j,k) << std::endl;
                }
                // Accumulate the total care of the recipients
                if (((rlt == "Non-Parent Carer" && carepc >= 35) || (rlt == "Parent" && pcsp <= 0)))
                {
                    fullcases[i].AddAdultsRltRcvTotCare(j,k,fullcases[i].GetAdultsRltCostPercentage(j,k));
                    for (int l=0; l < (signed)childcases.size(); l++)
                    {
                        if (childcases[l].IsThisChildName(fullcases[i].GetAdultsRltChildsName(j,k)))
                        {
                            childcases[l].AddTotalRecipientCarePercent(fullcases[i].GetAdultsRltCostPercentage(j,k));
                        }
                    }
                }
            }
        }
        // Determine the payment distribution
        // For each full adultcase(fulladulscase=j) for this case(case=i)
        if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
        {
            std::cout << "Payment Ditsribution Started" << std::endl;
        }
        for (int j=0; j < (signed)fullcases[i].GetfacaseSize(); j++)
        {
            // for each adult/child relationship for this fuladultcase in this case, (relationship=k [effectively child])
            if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
            {
                std::cout <<
                          "Current Case is case " << i+1 <<
                          " Current Adult is " << fullcases[i].GetfacaseAdultName(j) <<
                          std::endl;
            }

            for (int k=0; k < (signed)fullcases[i].GetAdultsRltSize(j); k++)
            {
                if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                {
                    std::cout <<
                              "\tCurrent Child is " << fullcases[i].GetAdultsRltChildsName(j,k) <<
                              " Relationship is " << rltype[fullcases[i].GetAdultsRltRType(j,k)]  << std::endl;
                }
                int rcvtotcare = 0;
                for (int l=0; l < (signed)childcases.size(); l++)
                {
                    if (childcases[l].IsThisChildName(fullcases[i].GetAdultsRltChildsName(j,k)))
                    {
                        rcvtotcare = childcases[l].GetTotalRecipientCarePercent();
                    }
                    if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                    {
                        std::cout << "\t\tChildCase=" << l <<
                                  " For Child=" << childcases[l].GetChildName() <<
                                  " RCVTOTCARE is now=" << rcvtotcare << std::endl;
                    }
                }
                std::string rlt = rltype[fullcases[i].GetAdultsRltRType(j,k)];
                int costp = fullcases[i].GetAdultsRltCostPercentage(j,k);
                int carep = fullcases[i].GetAdultsRltCarePercentage(j,k);
                float pcsp = fullcases[i].GetAdultsRltPCSPC(j,k);
                float totcotc = 0;
                if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                {
                    std::cout << "\t\t\t Cost%=" << costp <<
                              " Care%=" << carep << "PCS%=" << pcsp <<
                              " Total Cost Of This Child set to=" << totcotc << std::endl;

                }
                if ((pcsp > 0.0f) || ((rlt == "Non-Parent Carer" && carep < 35)))
                {
                    if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                    {
                        std::cout << "\t\t\tSKIPPING AS either PCS% is > 0 or NPC amd less than 35% Care. PCS%=" <<
                                  pcsp << " RLT=" << rlt <<std::endl;
                    }
                    continue;
                }
                for (int l=0; l < (signed)fullcases[i].GetfacaseSize(); l++)
                {
                    for (int m=0; m < (signed)fullcases[i].GetAdultsRltSize(l); m++)
                    {
                        if (fullcases[i].GetAdultsRltChildsName(j,k) != fullcases[i].GetAdultsRltChildsName(l,m)) continue;
                        if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                        {
                            std::cout << std::endl <<
                                      "\t\tThis Cost of the child=" << fullcases[i].GetAdultsRltCostOfTheChild(l,m);
                        }
                        totcotc = Round2dp(totcotc + (float) fullcases[i].GetAdultsRltCostOfTheChild(l,m),0);
                    }
                }
                if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                {
                    std::cout << std::endl <<
                              "\t\t\tTotal Cost of the child=" << totcotc << "\tCost%=" <<
                              costp << "\tTotal Recipient Care%=" << rcvtotcare << std::endl;
                }
                float gets = (int)(Round2dp((float)totcotc / (float)rcvtotcare * (float)costp,0));
                if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN)
                {
                    std::cout << std::endl <<
                              "\t\t\t\tCS Liability for this child=" << gets;
                }
                fullcases[i].AddfacaseCSGets(j,(int)gets);
            }
        }
    }
    if (DEBUG | CALCDEBUG | FNC_CALCULATECOSTOFCHILDREN )
    {
        std::cout << "Function CalculateCostOfChilrden - Ennding" << std::endl;
    }
}
//=======================================================================================
// Function to check if the parents of one child case matches the parents of the other
// childcase.
//=======================================================================================
bool DoParentsMatch(int ccase1, int ccase2)
{
    if (DEBUG | CALCDEBUG | FNC_DOPARENTSMATCH )
    {
        std::cout << "Function - Do Parentsmatch - Starting" << std::endl;
    }
    int pmatch = 0;
    for (int i=0; i < childcases[ccase1].GetACSize(); i++)
    {
        int acase = childcases[ccase1].GetAC(i);
        std::string aname = adultcases[acase].GetAdultName();
        std::string artyp = rltype[adultcases[acase].GetRlTyp()];
        if ((artyp != "Parent") && (artyp != "Parent Abroad") && (artyp != "Deceased Parent"))
        {
            continue;
        }
        if (DEBUG | CALCDEBUG | FNC_DOPARENTSMATCH)
        {
            std::cout << "Case1 Adult Case being checked is" << acase << std::endl << adultcases[acase].Display() << std::endl;
        }
        if (IsParentInChildCase(ccase2, aname))
        {
            pmatch+=1;
        }
    }
    if (DEBUG | CALCDEBUG | FNC_DOPARENTSMATCH )
    {
        std::cout << "Function - Do Parentsmatch - Ending" << std::endl;
    }
    if (pmatch < 2) return false;
    return true;
}
//=======================================================================================
// Function to check if the parent is a parent in a child case
//=======================================================================================
bool IsParentInChildCase(int ccase, std::string parent)
{
    // Loop through the adult cases
    if (DEBUG | CALCDEBUG | FNC_ISPARENTINCHILDCASE)
    {
        std::cout << "\t\t\tLooking for Parent " << parent << "in childcase " << ccase << std::endl;
    }
    for (int i=0; i < childcases[ccase].GetACSize(); i++)
    {
        int acase = childcases[ccase].GetAC(i);
        std::string aname = adultcases[acase].GetAdultName();
        std::string artyp = rltype[adultcases[acase].GetRlTyp()];

        if (DEBUG | CALCDEBUG | FNC_ISPARENTINCHILDCASE)
        {
            std::cout << "Case2 Adult Case being checked is" <<
                      acase << "\n" << adultcases[acase].Display() << std::endl;
        }
        if (DEBUG | CALCDEBUG | FNC_ISPARENTINCHILDCASE)
        {
            std::cout << "\t\t\t\tCurrently checking parent " <<
                      aname << " with relationship as " << artyp << std::endl;
        }
        if ( aname == parent && (artyp == "Parent" || artyp == "Parent Abroad" || artyp == "Deceased Parent"))
        {
            return true;
        }
    }
    return false;
}
//=======================================================================================
// Function to display the help text associated with the Year Section
//=======================================================================================
void DisplayYearHelp(HWND hwnd)
{
    std::string yh="The Year Section\r\n";
    yh.append("\r\nThis section allows the selection of a single value from a dropdown list.");
    yh.append(" The list is a list of the available years as defined in the file csamounts.txt. This file");
    yh.append(" is located in the same directory in which the program itself resides in.");
    yh.append(" \r\nThe file layout is described within the file as comments. Comments are lines that start with #.");
    yh.append(" The file should be amended before January 1st each year with the gazetted values for the new year,");
    yh.append(" to ensure that accurate calculations can be performed for assessments that start in the new year.");
    yh.append("\r\nEach year has a number of values(5 inclduing the year) that are changed annually that affect");
    yh.append("the calculation of Child Support. The values are :-");

    yh.append("\r\n\r\nYear is simply the year for which the following 4 values applies");

    yh.append("\r\n\r\nAMTAWE (Annualised Male Total Average Weekly Earning)");
    yh.append(" is an amount that reflects the average income according to figures produced by the ABS.");
    yh.append(" This amount is used as the basis for a number of values.\r\nThe self support amount (SSA) which is ");
    yh.append(" an amount that is deemed to be sufficient to support oneself. Under a formula based assessment, ");
    yh.append(" this amount is deducted from a persons taxable income, thus ensuring that the parent has sufficient ");
    yh.append("money to support themself. The SSA is 1/3rd of the AMTAWE.");
    yh.append("\r\nAMTAWE is also the basis for the income bands to which set percentages are applied when determining ");
    yh.append("the cost of children.\r\nThe bands are determined by multiplying AMTAWE by 0.5, 1.0, 1.5, 2.0 and 2.5.");
    yh.append("\r\n2.5 times AMTAWE is the maximum income that affects the cost of children, and thus AMTAWE also ");
    yh.append("determines the income cap (note for CS children the income applied is the parents ");
    yh.append(" combined income) (also note that the income is after deductables such as SSA)");
    yh.append("\r\nThe default income to be applied if a parent has not submitted a tax return for two years or more ");
    yh.append("is also based on AMTAWE and is 2/3rd of AMTAWE");

    yh.append("\r\n\r\nPPS (Parenting Payment Single) ");
    yh.append("is used in determining whether a Fixed Assessment will be applied.");
    yh.append(" i.e. if the taxable income is below this value then a Fixed Assessment may apply.");

    yh.append("\r\n\r\nMinimum Rate of Child Support");
    yh.append(" is the annual rate of Child Support paid where a child support formula produces an ");
    yh.append("annual rate below the minimum annual rate (i.e. this amount), and the fixed annual rate of ");
    yh.append("child support does not apply and the level of care for a child is less than 14% and ");
    yh.append("there is not an assessment made in accordance with a change of assessment decision, ");
    yh.append("a court order or a child support agreement.");
    yh.append("\r\nThe minimum rate is per case for up to 3 cases, after which the amount for 3 cases ");
    yh.append("is shared equally between cases.");

    yh.append("\r\n\r\nFixed Assessment Rate");
    yh.append(" is the annual rate of Child Support for those who are not on income");
    yh.append(" support and whose taxable income or income estimate is below PPS ");
    yh.append(" (if not shown to be genuinely low) and the parent has less than 35% care of the child. ");
    yh.append("\r\nIt is paid on a per child basis for up to three children, after which the amount for 3 children ");
    yh.append(" is shared between the children, which ca result in it being shared between carers.");

    yh.append("\r\n\r\nTo the right of the Year selector is information about the currently selected values as ");
    yh.append("described above.");
    yh.append("\r\n\r\nNote that the above is only a guide and may not be fully comprehensive.");
    MessageBox(hwnd,yh.c_str(),"Help for the Year Section!",MB_OK | MB_ICONINFORMATION);
}
//=======================================================================================
// Function to display the help text associated with the Adults Section
//=======================================================================================
void DisplayAdultHelp(HWND hwnd)
{
    std::string ah="The Adult Section\r\n";
    ah.append("\r\nThis section is for providing the adults to be considered in the calculation.");

    ah.append("For each adult there are two fields that can be supplied. The adults name and ");
    ah.append("the adults ATI (Adjusted Taxable Income).");
    ah.append("\r\n\r\nIn addition to the input fields, there is a button, named Add Adult that allows additional adults");
    ah.append(" to be added. If there are more than two adults, a button with an X is to the right, ");
    ah.append("Clicking this will remove that adult.");

    ah.append("\r\n\r\nDefault names are provided and can be used, however these names can be changed to more ");
    ah.append("meaningful names if desired.");

    ah.append("\r\n\r\nThe Adjusted Taxable Income (ATI) must be accurate for an accurate result.");
    ah.append(" An exception is if an adult is not a living parent to a child for whom CS is liable to be paid.");
    ah.append(" In such cases supplying an ATI other than $0 will automatically be deemed as $0.");

    ah.append("\r\n\r\nThe adults that should be added are :-");
    ah.append("\r\n\r\n   Parents of the children for whom CS may be paid.");
    ah.append("\r\n\r\n   Non-Parent carers (i.e. adults who are not parents of the child or children but who have care of");
    ah.append(" the child or children. If the care a non-parent provides is less than 35% (128 nights) then they ");
    ah.append("will not be eligible for payment of CS. If the child or chldren are in welfare care, then they are ");
    ah.append(" not CS children.");
    ah.append("\r\n\r\n   A deceased parent of the child or children should be given if the childor children are in ");
    ah.append("the care of a non-parent carer.");
    ah.append("\r\n\r\n   If a parent has children from another relationship and CS is liable, then the other parent");
    ah.append(" of that other relationship should be given (this is a multi-case scenario)");

    ah.append("\r\n\r\nNote that the above is only a guide and may not be fully comprehensive.");
    MessageBox(hwnd,ah.c_str(),"Help for the Adult Section!",MB_OK | MB_ICONINFORMATION);
}
//=======================================================================================
// Function to display the help text associated with the Children Section
//=======================================================================================
void DisplayChildHelp(HWND hwnd)
{
    std::string ch="The Child Section\r\n";
    ch.append("\r\nThis section is for providing information about children and the relationships they have");
    ch.append(" with the adults.");

    ch.append("\r\nFor each child there are two input fields along with another two fields for each adult that exists. ");
    ch.append("The first field is for the child's name, for which a default name is given, this may be changed ");
    ch.append("if desired. \r\nThe second field is a checkbox, that if ticked marks the child as 13 or over. If a child");
    ch.append(" is 13 or older then a different cost of children calculation is used.");
    ch.append("\r\n\rAfter the fields for a child's name and age there are two fields per adult each on a separate row.");
    ch.append(" The first field is for the relationship between the child and the respective adult. The second field ");
    ch.append("is for specifying the number of nights care that adult has for the child.");

    ch.append("\r\n\r\nThe relationship is selected from a dropdown list of 5 types of relationship as follows.");
    ch.append("\r\n   Parent - This indictaes if the parent is a parent or adoptee of the child and if not one the the ");
    ch.append("more specific types of parent.");
    ch.append("\r\n   Other - This indicates no primary relationship and is the defualt for all but the first two adults.");
    ch.append("\r\n   Non-Parent Carer - This indicates that the adult is not a parent but has care of the child and may");
    ch.append(" therefore be entitled to CS.");
    ch.append("\r\n   Parent Abroad - This indicates a parent of the child who is not a permenant resident in Australia");
    ch.append(" but is a permenant resident of another country.");
    ch.append("\r\n   Deceased Parent - This indicates a deceased parent, however it need only be used if the child");
    ch.append(" is cared for by a non-parent carer, as in such a scenario, a slightly different means of calculation");
    ch.append(" is used.");

    ch.append("\r\n\r\nThe nights care field may or may not be available for input. This is dependant upon the type ");
    ch.append(" of relationship. Input is disabled if the relationship is either Other or Deceased Parent.");
    ch.append("\r\nAlongside the last adult for the child is a counter of the total nights care not allocated. For");
    ch.append(" calculation to proceed, this must be 0 for each child. If any are not 0 then a message will be");
    ch.append(" displayed indicating the issue.");

    ch.append("\r\n\r\nChildren to include are the children of the relationship for which CS is sought.");
    ch.append("\r\nOther children from other reltionships should also be included if the child is a biological ");
    ch.append(" child of one of the parents of the CS children.");

    ch.append("\r\nIf the other relationship is ongoing then the other parent would not be included ");
    ch.append("or specified as other (this latter situation could arise for very complex cases).");
    ch.append("\r\nIn this situation the child is a relevant dependant child and the calculation takes into ");
    ch.append("consideration the duty of the parent to support that child and thus the cost in doing so.");
    ch.append("\r\nThis cost is calculated using the same method as for CS children, other than that only ");
    ch.append("single parent's income is used indetermining this cost.");

    ch.append("If the other relationship is not ongoing, the other parent should be included as this would then be ");
    ch.append("a multi-case scenario for which special considerations are made. These are a multi-case allowance and");
    ch.append(" a multi-case cap. The multi-case allowance is to take into consideration the fact that CS will be");
    ch.append("paid for the child. The multi-case cap ensures that the CS paid does not exceed the cost if all the ");
    ch.append("children of other cases lived full time with the parent.");
    ch.append("The method of calculating the cost of multi-case children is the same as for CS children.");

    ch.append("\r\n\r\nNote that the above is only a guide and may not be fully comprehensive.");
    MessageBox(hwnd,ch.c_str(),"Help for the Child Section!",MB_OK | MB_ICONINFORMATION);
}
//=======================================================================================
// Function to display the help text associated with the Calculate Section
//=======================================================================================
void DisplayCalculateHelp(HWND hwnd)
{
    std::string ch="The Calculate Section\r\n";
    ch.append("\r\n\r\nThis section is for initiating the calculation after entering the information in ");
    ch.append("the previous section.");
    ch.append("\r\nYou can simpy click on the button to initiate calculation or alternately you can click on the");
    ch.append("show details checkbox, this will perform the same calculation but will provide more comprehensive ");
    ch.append("information.");

    ch.append("\r\nIt should be noted that checks are made prior to peforming the calculation. If such checks ");
    ch.append("result in anomalies being found, then messages will be displayed and the calculation will not ");
    ch.append("proceed. The anomalies have to be corrected and then the calculation re-attempted.");
    ch.append("\r\n\r\nChecks are made for the total care for each child being 365 nights.");
    ch.append("\r\n\r\nChecks are made for the total number of parents, 2 being the maximum.");
    ch.append("\r\n\r\nChecks are also made for amiguous reltionships such as a parent being not abroad or deceased ");
    ch.append("for one child but not another.");
    ch.append("\r\n\r\nThe messages detail the error and where the error has been detected");

    MessageBox(hwnd,ch.c_str(),"Help for the Calculate Section!",MB_OK | MB_ICONINFORMATION);
}
//=======================================================================================
// Function to kill a window or control only if the handle has been set
//=======================================================================================
void KillWind(HWND hwnd)
{
    if (hwnd != NULL) DestroyWindow(hwnd);
}
