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
extern char const* ati_tt;
extern char const* ati_short;
extern char const* ati_long;
// SSA - Self Support Amount
extern char const* ssa_tt;
extern char const* ssa_short;
extern char const* ssa_long;
// Less SSA - ATI - SSA
extern char const* less_ssa_tt;
extern char const* less_ssa_short;
extern char const* less_ssa_long;
// RDCA - Relevant Dependant Children Amount
extern char const* rdca_tt;
extern char const* rdca_short;
extern char const* rdca_long;
// Less RDCA - ATI - SSA - RDCA
extern char const* less_rdca_tt;
extern char const* less_rdca_short;
extern char const* less_rdca_long;
// MCA - Multi-Case Allowance
extern char const* mca_tt;
extern char const* mca_short;
extern char const* mca_long;
// MCCAP - Multi-Case Cap
extern char const* mccap_tt;
extern char const* mccap_short;
extern char const* mccap_long;
// Less MCA - ATI - SSA - RDCA - MCA
extern char const* less_mca_tt;
extern char const* less_mca_short;
extern char const* less_mca_long;
// CSI - Child Support Income (ATI-SSA-RDCA-MCA)
extern char const* csi_tt;
extern char const* csi_short;
extern char const* csi_long;
// CCSI - Combined Child Support Income
extern char const* ccsi_tt;             // Combined Child Support Income
extern char const* ccsi_short;
extern char const* ccsi_long;

// COC - Cost of Children
extern char const* coc_tt;              // Cost of Children
extern char const* coc_short;
extern char const* coc_long;
//PIP - Parent's Income Percentage
extern char const* pip_tt;              // Parent's Income Percentage
extern char const* pip_short;
extern char const* pip_long;
// Liability
extern char const* liability_tt;        // CS Liability (annual rate of CS)
extern char const* liability_short;
extern char const* liability_long;
// Child Child's Name
extern char const* child_short;
extern char const* child_long;
// Nights of Care
extern char const* ncare_short;
extern char const* ncare_long;
// Percentage of Care
extern char const* pcare_short;
extern char const* pcare_long;
// Cost percetnage
extern char const* costp_short;
extern char const* costp_long;
// Actual Parent's CS Percentage
extern char const* apcsp_short;
extern char const* apcsp_long;
//Effective Parent's CS Percentage
extern char const* epcsp_short;
extern char const* epcsp_long;
// Cost of this child
extern char const* ctc_short;
extern char const* ctc_long;


#endif // CSATERMS_H_INCLUDED
