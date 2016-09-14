#ifndef CSATERMS_H_INCLUDED
#define CSATERMS_H_INCLUDED

#include <cstring>
//Result Section Headings
extern char const* tooltip1;
extern char const* hlo_tt;              // High level Overview (Gross)
extern char const* clo_tt;              // Case Level Overview
extern char const* dtl_tt;              // Detailed Information

//Result Overview Detail
extern char const* adult_tt;
extern char const* typ_tt;              // Type of transaction
extern char const* annually_tt;
extern char const* monthly_tt;
extern char const* fortnightly_tt;
extern char const* weekly_tt;
extern char const* daily_tt;

//Detailed Result (intermedaite values)
// ATI - Adjusted Taxable Income
extern char const* ati_tt;              // Adjusted taxable Income
extern char const* ati_short;
extern char const* ati_long;
extern char const* ssa_tt;              // Self-Support Amount
extern char const* ssa_short;
extern char const* ssa_long;
extern char const* less_ssa_tt;         // ATI less SSA
extern char const* less_ssa_short;
extern char const* less_ssa_long;
// RDCA
extern char const* rdca_tt;              // Relevant Dependant Child Amount
extern char const* rdca_short;
extern char const* rdca_long;
// Less RDCA
extern char const* less_rdca_tt;        // ATI less SSA less RDCA
extern char const* less_rdca_short;
extern char const* less_rdca_long;
// MCA
extern char const* mca_tt;              // Multi-Case Allowance
extern char const* mca_short;
extern char const* mca_long;
// MCCAP
extern char const* mccap_tt;            // Milti-Case Cap
extern char const* mccap_short;
extern char const* mccap_long;
// Less MCA
extern char const* less_mca_tt;         // Ati less SSA, RDCA and MCA (CSI)
extern char const* less_mca_short;
extern char const* less_mca_long;

extern char const* csi_tt;              // Child Support Income
extern char const* ccsi_tt;             // Combined Child Support Income
extern char const* coc_tt;              // Cost of Children
//PIP
extern char const* pip_tt;              // Parent's Income Percentage
extern char const* pip_short;
extern char const* pip_long;
extern char const* liability_tt;        // CS Liability (annual rate of CS)

#endif // CSATERMS_H_INCLUDED
