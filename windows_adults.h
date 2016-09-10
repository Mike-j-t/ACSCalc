#ifndef WINDOWS_ADULTS_H_INCLUDED
#define WINDOWS_ADULTS_H_INCLUDED

#include <windows.h>
#include <deque>
#include <cstring>
#include <string>

// Adults Section Windows Controls (Heading)
HWND wc_adults_label;
HWND wc_adults_hlp;
HWND wc_adults_hlp_tt;
HWND wc_adults_label_tt;
HWND wc_adults_addbutton;
HWND wc_adults_addbutton_tt;

// Class for holding windows control information per adult
// Note should really have memebers as private with
// corresponding getter/setter methods
class wc_adlts {
public:
    HWND wca_adlt_nmlabel;      //Name label pointer
    HWND wca_adlt_nmlabel_tt;   //Name Label ToolTip pointer
    HWND wca_adlt_nm;           //Name Input pointer
    HWND wca_adlt_nm_tt;        //Name Input ToolTip pointer
    HWND wca_adlt_atilabel;     //ATI Label pointer
    HWND wca_adlt_atilabel_tt;  //ATI Label ToolTip pointer
    HWND wca_adlt_ati;          //ATI Input pointer
    HWND wca_adlt_ati_tt;       //ATI Input ToolTip pointer
    HWND wca_adlt_dltb;         //Adult Delete Button pointer
    HWND wca_adlt_dltb_tt;      //Adult Delete Button ToolTip pointer
    wc_adlts();
    ~wc_adlts();
};
// wcadults connstructor creates all pointers as NULL
wc_adlts::wc_adlts() {
    wca_adlt_nmlabel=NULL;
    wca_adlt_nmlabel_tt=NULL;
    wca_adlt_nm=NULL;
    wca_adlt_nm_tt=NULL;
    wca_adlt_atilabel=NULL;
    wca_adlt_atilabel_tt=NULL;
    wca_adlt_ati=NULL;
    wca_adlt_ati_tt=NULL;
    wca_adlt_dltb=NULL;
    wca_adlt_dltb_tt=NULL;
}
// Destructor
wc_adlts::~wc_adlts() {
}

#endif // WINDOWS_ADULTS_H_INCLUDED
