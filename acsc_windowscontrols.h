#ifndef ACSC_WINDOWSCONTROLS_H_INCLUDED
#define ACSC_WINDOWSCONTROLS_H_INCLUDED

class wc_chldrn
{
public:
    HWND wcc_chld_nmlabel;
    HWND wcc_chld_nm;
    HWND wcc_chld_agelabel;
    HWND wcc_chld_age;
    HWND wcc_chld_dltb;
    HWND wcc_chld_care_total;
    deque<HWND> wcc_chld_adlt_nmlabel;
    deque<HWND> wcc_chld_adlt_rlt;
    deque<HWND> wcc_chld_adlt_carelabel;
    deque<HWND> wcc_chld_adlt_care;
    wc_chldrn();
    ~wc_chldrn();
    void InitEmbedSet();
    void PopBackEmbedSet();
    void Setnmlabel(HWND, int);
};
wc_chldrn::wc_chldrn()
{
    wcc_chld_nmlabel = NULL;
    wcc_chld_nm = NULL;
    wcc_chld_agelabel = NULL;
    wcc_chld_age = NULL;
    wcc_chld_care_total = NULL;
}
wc_chldrn::~wc_chldrn()
{
}
void wc_chldrn::InitEmbedSet()
{
    wcc_chld_adlt_nmlabel.push_back((HWND)NULL);
    wcc_chld_adlt_rlt.push_back((HWND)NULL);
    wcc_chld_adlt_carelabel.push_back((HWND)NULL);
    wcc_chld_adlt_care.push_back((HWND)NULL);
}

void wc_chldrn::PopBackEmbedSet()
{
    while (!wcc_chld_adlt_nmlabel.empty())
    {
        wcc_chld_adlt_nmlabel.back();
        wcc_chld_adlt_nmlabel.pop_back();
    }
    while (!wcc_chld_adlt_rlt.empty())
    {
        wcc_chld_adlt_rlt.back();
        wcc_chld_adlt_rlt.pop_back();
    }
    while (!wcc_chld_adlt_carelabel.empty())
    {
        wcc_chld_adlt_carelabel.back();
        wcc_chld_adlt_carelabel.pop_back();
    }
    while (!wcc_chld_adlt_care.empty())
    {
        wcc_chld_adlt_care.back();
        wcc_chld_adlt_care.pop_back();
    }
}

void wc_chldrn::Setnmlabel(HWND lhwnd, int ix)
{
    wcc_chld_adlt_nmlabel[ix]=lhwnd;
}


#endif // ACSC_WINDOWSCONTROLS_H_INCLUDED
