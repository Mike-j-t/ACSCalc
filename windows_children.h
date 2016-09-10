#ifndef WINDOWS_CHILDREN_H_INCLUDED
#define WINDOWS_CHILDREN_H_INCLUDED

#include <deque>
#include <windows.h>
#include <string>
#include <cstring>

// Windows Handles for Child Section Heading
HWND wc_children_label;             // Label for Child's Name
HWND wc_children_label_tt;          // Tooltip for Label
HWND wc_children_hlp;               // Button for help
HWND wc_children_hlp_tt;            // Tooltip for help
HWND wc_children_addbutton;         // Add Button
HWND wc_children_addbutton_tt;      // Tooltip for Add Button

// Class for holding windows control information per child
// Note should really have memebers as private with
// corresponding getter/setter methods
class wc_chldrn {
    public:
        HWND wcc_chld_nmlabel;
        HWND wcc_chld_nmlabel_tt;
        HWND wcc_chld_nm;
        HWND wcc_chld_nm_tt;
        HWND wcc_chld_agelabel;
        HWND wcc_chld_agelabel_tt;
        HWND wcc_chld_age;
        HWND wcc_chld_age_tt;
        HWND wcc_chld_dltb;
        HWND wcc_chld_dltb_tt;
        HWND wcc_chld_care_total;
        HWND wcc_chld_care_total_tt;
        //deques for the child-adult relationship
        std::deque<HWND> wcc_chld_adlt_nmlabel;
        std::deque<HWND> wcc_chld_adlt_nmlabel_tt;
        std::deque<HWND> wcc_chld_adlt_rlt;
        std::deque<HWND> wcc_chld_adlt_rlt_tt;
        std::deque<HWND> wcc_chld_adlt_carelabel;
        std::deque<HWND> wcc_chld_adlt_carelabel_tt;
        std::deque<HWND> wcc_chld_adlt_care;
        std::deque<HWND> wcc_chld_adlt_care_tt;
        // Constructor/Destructor declarations
        wc_chldrn();
        ~wc_chldrn();
        // Method declarations
        void InitEmbedSet();
        void PopBackEmbedSet();
        void Setnmlabel(HWND, int);
        std::string Getnmlabel(HWND);
};
// Default Constructor
wc_chldrn::wc_chldrn() {
    wcc_chld_nmlabel = NULL;
    wcc_chld_nmlabel_tt = NULL;
    wcc_chld_nm = NULL;
    wcc_chld_nm_tt = NULL;
    wcc_chld_agelabel = NULL;
    wcc_chld_agelabel_tt = NULL;
    wcc_chld_age = NULL;
    wcc_chld_age_tt = NULL;
    wcc_chld_care_total = NULL;
    wcc_chld_care_total_tt = NULL;
}
// Default destructor
wc_chldrn::~wc_chldrn() {
}
// Initialise the embedded deques
// i.e. add an element to the respective deques
void wc_chldrn::InitEmbedSet() {
    wcc_chld_adlt_nmlabel.push_back((HWND)NULL);
    wcc_chld_adlt_nmlabel_tt.push_back((HWND)NULL);
    wcc_chld_adlt_rlt.push_back((HWND)NULL);
    wcc_chld_adlt_rlt_tt.push_back((HWND)NULL);
    wcc_chld_adlt_carelabel.push_back((HWND)NULL);
    wcc_chld_adlt_carelabel_tt.push_back((HWND)NULL);
    wcc_chld_adlt_care.push_back((HWND)NULL);
    wcc_chld_adlt_care_tt.push_back((HWND)NULL);
}

// Empty the respective deques
void wc_chldrn::PopBackEmbedSet() {
    while (!wcc_chld_adlt_nmlabel.empty()) {
        wcc_chld_adlt_nmlabel.back();
        wcc_chld_adlt_nmlabel.pop_back();
    }
    while (!wcc_chld_adlt_nmlabel_tt.empty()) {
        wcc_chld_adlt_nmlabel_tt.back();
        wcc_chld_adlt_nmlabel_tt.pop_back();
    }
    while (!wcc_chld_adlt_rlt.empty()) {
        wcc_chld_adlt_rlt.back();
        wcc_chld_adlt_rlt.pop_back();
    }
    while (!wcc_chld_adlt_rlt_tt.empty()) {
        wcc_chld_adlt_rlt_tt.back();
        wcc_chld_adlt_rlt_tt.pop_back();
    }
    while (!wcc_chld_adlt_carelabel.empty()) {
        wcc_chld_adlt_carelabel.back();
        wcc_chld_adlt_carelabel.pop_back();
    }
    while (!wcc_chld_adlt_carelabel_tt.empty()) {
        wcc_chld_adlt_carelabel_tt.back();
        wcc_chld_adlt_carelabel_tt.pop_back();
    }
    while (!wcc_chld_adlt_care.empty()) {
        wcc_chld_adlt_care.back();
        wcc_chld_adlt_care.pop_back();
    }
    while (!wcc_chld_adlt_care_tt.empty()) {
        wcc_chld_adlt_care_tt.back();
        wcc_chld_adlt_care_tt.pop_back();
    }
}

void wc_chldrn::Setnmlabel(HWND lhwnd, int ix) {
    wcc_chld_adlt_nmlabel[ix]=lhwnd;
}

#endif // WINDOWS_CHILDREN_H_INCLUDED
