//=============================================================================
//CS Terminology Tooltip/Help data
//
//Basically these are just character strings used by
//various tooltips that are explanations of acronyms
//
// pointers to character strings instead of strings are used as
// Windows uses LPTSTR.
//
//Note extren definitions appear in header csaterms.h
//=============================================================================

//Testing tooltip
char const* tooltip1(
                   "My tooltip test."
                   "More testing data."
                   );
//=============================================================================
// High Level Overview - Section header
char const* hlo_tt(
                   "The High-Level Overview detials how much an adult will"
                   " pay or receive overall."

                   "CS is calculated based upon an Annual amount."
                   "\n\nAs payemnt/receipt of CS will generally be monthly and"
                   " as payers/payees may have income/outgoings on other"
                   "schedules, payments are shown for"
                        "\n\tAnnual,"
                        "\n\tMonthly,"
                        "\n\t2 Weekly and"
                        "\n\tWeekly schedules."

                   "\n\nAs some calculations are based upon a Daily rate,"
                   " this value is also provided."

                   "\n\nFor a single case this will reflect actual payements."
                   " However, for multiple cases it could appear confusing"
                   " due to accumalation/reduction."

                   "\n\nFor example, an adult may have to pay CS in one case"
                   " whilst receiving CS in another. This section will only"
                   " show the result of the combination. of the cases."
                   );
//=============================================================================
// Case Level Overview - Section header
// TODO (Mike092015#1#): Look into only displaying High-Level Overview for multiple cases (or perhaps only the Case-Level)
char const* clo_tt(
                   "The Case-Level Overview details how much and adult will"
                   " pay or receive on per-case basis."

                   "\n\nNote! if there is just one case then this will be"
                   " the same as the High-Level Overview."

                   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                   // Note! currently this is identical to the Text for the
                   // high level overview.
                   // As such, changes to the High Level Overveiw should be
                   // reflected here or vice-versa
                   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                   "CS is calculated based upon an Annual amount."
                   "\n\nAs payemnt/receipt of CS will generally be monthly and"
                   " as payers/payees may have income/outgoings on other"
                   "schedules, payments are shown for"
                        "\n\tAnnual,"
                        "\n\tMonthly,"
                        "\n\t2 Weekly and"
                        "\n\tWeekly schedules."

                   "\n\nAs some calculations are based upon a Daily rate,"
                   " this value is also provided."

                   "\n\nFor a single case this will reflect actual payements."
                   " However, for multiple cases it could appear confusing"
                   " due to accumalation/reduction."

                   "\n\nFor example, an adult may have to pay CS in one case"
                   " whilst receiving CS in another. This section will only"
                   " show the result of the combination. of the cases."
                   );
//=============================================================================
// Detailed Informatio (Intermediate Values)
char const* dtl_tt(
                   "Detailed Informatio shows intermedaite values calculated"
                   " in order to produce the results."

                   "\n\nThese values are only shown if Show Calculations is"
                   " checked."

                   "\n\nA summary of the process is that the parent's Taxable "
                   " Income (ATI), after reductions, (SSA,RDCA and MCA) are"
                   " added to form the Combined Child Support Income (CCSI)."

                   "\n\nThe CCSI is applied to the Cost of Children tables, the"
                   " result is the Cost of the Children (COC)."

                   "\n\nThe Cost of each child (COTC) per parent is then"
                   " determined."
                   "\n\tThis factoring in the level of care and the percentage"
                   " of the CCSI (PIP) for the respective parent."
                   "\n\nThe resultant value, if positive, is that parent's CS"
                   " liability for that child."
                   "\n\nA negative value effectively becomes 0."
                   "\n\nIf parent's each have positive values (must be for "
                   "different children) the the amounts are offset."
                   "\n\nNote! This is a simplified overview. Some details"
                   " have not been covered. e.g. Distribution when a non-"
                   "parent carer is involved."
                   );
//=============================================================================
//Type (pays,receives or n/a)
char const* typ_tt(
                   "The type of the transaction detailed."

                   "\n\nThis can be Pays, Receives or N/A."
                   " N/A indicates that an amount of $0.00 is the result."

                   "\n\nNote that N/A may not reflect the actual situation."

                   " This is because other factors may be considered."

                   " \n\nFor example a low income, that results in a N/A, may be"
                   " due to a person being on benefits. In which case a"
                   " minimum rate may apply."
                   );

//=============================================================================
// Annually
char const* annually_tt(
                        "The amount that would be paid/received over a"
                        " full year."

                        );
//=============================================================================
// Monthly
char const* monthly_tt(
                        "The amount that would be paid/received on a monthly"
                        " basis."
                        );
//=============================================================================
// 2 Weekly
char const* fortnightly_tt(
                           "The amount that would be paid/received on a"
                           " 2-weekly basis."
                           );
//=============================================================================
// Weekly
char const* weekly_tt(
                      "The amount that would be paid/received on a"
                      " weekly basis."
                      );
//=============================================================================
char const* daily_tt(
                     "The daily rate."
                     "\n\nNote! Daily payments/receipts would not occur. "
                     " Rather, the daily rate is used for some calculations."
                     );

//=============================================================================
// Adult
char const* adult_tt(
                     "Adult = The respective Adult."

                     "\n\nThe name, as either generated or input, of the adult."

                     "\n\nNote! only adults that are liable to pay or receive"
                     " Child Support will be displayed."

                     );

//=============================================================================
// Adjusted Taxable Income
char const* ati_short(
                      "ATI = Adjusted Taxable Income."
                      "\nTaxable income plus other amounts used for determining"
                      " Child Support."
                      );
char const* ati_long(
                     "ATI = Adjusted Taxable Income."
                     "\n\nThe Taxable Income plus, if relevant, the following :-"
                     "\n\tPre-sax superannuantion contributions."
                     "\n\tTotal net investment losses."
                     "\n\tTarget foreign income"
                     "\n\tReportable fringe benefits."
                     "\n\tSpecified tax-free benefits or pensions."

                     "\nAll the above should appear on the tax return."

                     "\nThe ATI used is from the last relevant tax return."

                     "\n\nA new tax return will result in a re-assessment;"
                     " assuming that it is for the current taxation period."

                     " Submitting late tax returns is beyond the scope of"
                     " this calculator."
                     "\n\nThe ATI is the basis of calculating Child Supoort."
                     "\n\nThe CS Guide section 2.4.4.10 Adjusted Taxable"
                     " Income may provide greater detail."
                     "\nThe guide may be found at :-"
                     "\n\thttp://guides.dss.gov.au/child-support-guide/2/4/4/10"
                     "\nNote! this address can change."
                     );
char const* ati_tt(
                   "ATI = Adjsuted Taxable Income."
                   "\n\nThe ATI is your Taxable Income plus, if relevant,"
                   " the following :-"

                   "\n\tPre-Tax Superannuation Contributions,"

                   "\n\tNet Investment losses,"

                   "\n\tFringe Benifits and"

                   "\n\tspecified free benefits."

                   "\n\nThese amounts will be included in the relevant"
                   " Tax return, again, if relevant."

                   "\n\nFrequently the ATI will be the Taxable Income."

                   "\n\nThe ATI is the base amount used for the"
                   " calculation of Child Support."
                   );
//=============================================================================
//Self-Support Amount
char const* ssa_short(
                      "SSA = Self Support Amount."
                      "\n\nThe SSA is an amount that is deemed sufficient for"
                      " an adult to support themself."
                      );
char const* ssa_long(
                     "SSA = Self Support Amount."
                     "\n\nThe SSA is an amount that is deemed sufficient for"
                     " an adult to support themself. "

                     "\n\nThe SSA is deducted from the ATI so the self-support"
                     " aspect reduces the amount considred available for"
                     " child support calculation."

                     "\n\nThe SSA is determined according to an ABS provided"
                     " figure (MTAWE) which is then annualised to form AMTAWE"
                     " (Annualised Male Total Average Weekly Earnings)."
                     " SSA is 1/3rd of AMTAWE and changes annually."

                     "\n\nNote! AMTAWE is also used in the detrmination of the"
                     " Cost of Children Tables."

                     "\n\nNote if ATI - SSA is negative then it is changed to 0."

                     "\n\nMore information regarding SSA can be obtained from"
                     " the CS Guide section 2.4.4.20 Other Components of"
                     " Child Support Income."

                     "\n\nThe CS Guide may be found at :-"

                     "\n\thttp://guides.dss.gov.au/child-support-guide/2/4/4/20"

                     "\nNote! this address can change."
                     );
char const* ssa_tt(
                   "SSA = Self Support Amount."

                   "\n\nThe SSA is an amount that is deemed sufficient to"
                   " provide for and adult."

                   "\n\nThe SSA is subtracted from the ATI for each parent"
                   " of the CS children in the case."

                   "\nThe SSA is set as 1/3rd AMTAWE. The AMTAWE is updated"
                   " annually."

                   "\n\nAMTAWE is  Annualised MTAWE. MTAWE is the Male Total"
                   " Average Weekly Earnings."

                   "\n\nMTAWE is provided by the ABS (Australian Bureau of"
                   " Statistics)"

                   "\n\nAMTAWE is also used in determining the income levels"
                   " of the Cost of Children Tables."

                   "\n\nNote! a negative value, after deduction, results in"
                   " $0."

                   );
//=============================================================================
//Less SSA (ATI-SSA)
char const* less_ssa_short(
                           "Less SSA = The amount after deducting the SSA from"
                           " the ATI."

                           "\n\nATI = Adjusted Taxable Income."
                           " SSA = Self Support Amount."
                           );
char const* less_ssa_long(
                          "Less SSA = The amount after deducting the SSA from"
                          " the ATI."

                          "\n\nATI = Adjusted Taxable Income."
                          " SSA = Self Support Amount."

                          "\n\nThis calculation reduces the amount that will"
                          " be used for calculation purposes in order to"
                          " set aside an amount deemed as being needed for"
                          " an adult to support themself."

                          "\n\nThis reduction is applied to all liable"
                          " parents."

                          "\n\nIf the result is negative then it is made"
                          " to be $0."
                          );
char const* less_ssa_tt(
                         "Less SSA = The amount after deducting the SSA from"
                         " the ATI."

                         "\n\nThus reducing the amount used for calculation"
                         " purposes"
                         " to account for the need for a parent to support"
                         " themself."

                         "\n\nNote! a negative value results in $0."

                         );

//=============================================================================
//RDCA tooltip text (Relevant Dependant Child Amount
char const* rdca_short(
                       "RDCA - Relevant Dependant Children Amount."
                       "\n\nAn amount that considers the support of Relevant"
                       " Dependant Children."
                       );
char const* rdca_long(
                      "RDCA = Relevant Dependant Children Amount."

                      "\n\nAn amount that considers the financial support "
                      " needs of Relevant Dependant Children (RDC)."

                      "\n\nAn RDC is a child (biological or adopted) of"
                      " only one of parent's who is not a child of any "
                      " of Child Support case."

                      "\n\nThe RDCA is determined by applying the respective"
                      " parent's ATI less SSA to the Cost of Children tables."

                      "\n\nThe RDCA is then deducted from the respective"
                      " parent's ATI less SSA (along with the Multi-Case"
                      " Allowance (MCA)) to form that parent's Child Support"
                      " Income (CSI)."

                      "\n\nRDCA can often be $0."
                      " It could be that there are no RDC's. It could be"
                      " that the respective parent's ATI less the SSA is $0."

                      "\n\nAdditional information may be obtained from the"
                      " CS Guide section 2.4.4.20 Other Components of"
                      " Child Support Income."

                      "\n\nThe CS Guide may be found at :-"

                     "\n\thttp://guides.dss.gov.au/child-support-guide/2/4/4/20"

                     "\nNote! this address can change."

                      );
char const* rdca_tt(
                     "RDCA = Relevant Dependant Children Amount."
                     "\n\nA relevant dependant child is a biological child of"
                     " one of the parent's (not both) who is not a child"
                     " of any other Child Support case."

                     "\n\nThe RDCA is used to ensure that such children"
                     " are supported."

                     "\nThe RDCA is deducted from the ATI (less the SSA)."

                     "\n\nRDCA is determined using the Cost of Children"
                     " tables BUT only using the adults ATI (less SSA)."

                     "\n\nRDCA may, and will often be 0. This can be a"
                     " result of no such children existing or if the"
                     " respective Parent's ATI (less SSA) is 0 or less."
                     "\n\nNote! a negative value, after deduction, results in $0."

                     );
//=============================================================================
//LessRDCA tooltip text (Relevant Dependant Child Amount)
char const* less_rdca_short(
                            "Less RDCA = ATI - SSA - RDCA. (i.e. Less SSA - RDCA)"
                            );
char const* less_rdca_long(
                           "Less RDCA = ATI - SSA - RDCA. (i.e. Less SSA - RDCA)"
                           "\n\nRDCA = Relevant Dependant Children Amount."
                           " ATI = Adjusted Taxable Income."
                           " SSA = Self Support Amount."

                           "This calculation factors in the legislated"
                           " requirement that a parent supports all of thier"
                           " children."

                           "\n\nNote in conjunction with MCA/MCCap."

                           "\n\nNote! a negative value, after deduction,"
                           " is made to be $0."
                           );
char const* less_rdca_tt(
                    "Less RDCA = The amount after deducting the SSA "
                    "and the RDCA from the ATI."

                    "\n\nDeducting the RDCA reduces the amount used for"
                    " subsequent calculations, allowing this amount to"
                    " provide support for the children"
                    " who are not CS children, but are RDC's."

                    );
//=============================================================================
//Multi-Case Allowance tooltip
char const* mca_short(
                      "MCA = Multi-Case Allowance."

                      "\n\nAn amount that considers the support paid for"
                      " children in other cases."
                      );
char const* mca_long(
                     "MCA = Multi-Case Allowance."

                     "\n\nMCA is an allowance, deducted from a parent's"
                     " income in order to consider the Child Support paid"
                     " in other cases."

                     "\n\nMCA is determined by ascertaining the cost of all"
                     " CS children, from all cases, and then dividing the"
                     " result by the number of Multi-Case children."

                     "\n\nNote the above is a summary, the actual calculation"
                     " is more complex."

                     "\n\nAdditional information may be obtained from the"
                     " CS Guide section 2.4.4.20 Other Components of"
                     " Child Support Income."

                     "\n\nThe CS Guide may be found at :-"

                     "\n\thttp://guides.dss.gov.au/child-support-guide/2/4/4/20"

                     "\nNote! this address can change."
                     );
char const* mca_tt(
                   "MCA = Multi-Case Allowance."
                   "\n\nMCA is an allowance, deducted from a parent's ATI,"
                   " that ensures that a Parent's responsibility to"
                   " provide support for any children in other CS cases"
                   " is considered."

                   "\nMCA is determined by ascertaining the cost of all"
                   " CS children from all cases and then dividing"
                   " the result by the number of Multi-Case children."

                   "\n\nNote! this is a summary, the actual calculation is"
                   " more complicated."

                   );
//=============================================================================
// Multi-Case Cap tooltip text
// TODO (Mike092015#1#): Complete tooltip for MCCAP
char const* mccap_short(
                        "MCCAP = Multi-Case Cap."

                        "\n\nThe Multi-Case Cap ensures that no parent pays"
                        " more than if the children lived in the same household."
                        );
char const* mccap_long(
                       "MCCAP = Multi-Case Cap."

                       "\n\nThe Multi-Case Cap ensures that no parent pays"
                       " more than if the children lived in the same household."

                       "\n\nEither the MCA or the MCCap will be applied not both."
                       " The one that is applied is the one that has the highest"
                       " value (i.e. results in the lower amount.)"

                       "\n\nAdditional information may be obtained from the"
                       " CS Guide section 2.4.4.20 Other Components of"
                       " Child Support Income."

                       "\n\nThe CS Guide may be found at :-"

                       "\n\thttp://guides.dss.gov.au/child-support-guide/2/4/9#mccap"

                       "\nNote! this address can change."
                        );

char const* mccap_tt(
                     "MCCAP = Multi-Case Cap."
                     "\n\nThe Multi-Case Cap is an amount that is used to cap"
                     );
//=============================================================================
// LessMCA tooltip text (Multi-Case Allowance)
char const* less_mca_short(
                           "LessMCA = The amount after deducting SSA, RDCA and"
                           " MCA from the ATI. (i.e. Less RDCA - MCA)"

                           "\n\nSSA = Self Support Amount. RDCA = Relevant"
                           " Dependant Children Amount. MCA = Multi-Case Allowance."
                           );

char const* less_mca_long(
                          "LessMCA = The amount after deducting SSA, RDCA and"
                          " MCA from the ATI. (i.e. Less RDCA - MCA)"

                          "\n\nSSA = Self Support Amount. RDCA = Relevant"
                          " Dependant Children Amount. MCA = Multi-Case Allowance."

                          "\n\nThis amount accounts for all legislated support"
                          " requirements of a parent. The amount is subsequently"
                          " termed Child Support Income (CSI)."

                          "\n\nA negative value is changed to $0."
                           );
char const* less_mca_tt(
                        "LessMCA = The amount after deducting the SSA, the RDCA"
                        " and the MCA from the ATI."
                        "\n\nThis is the value, term Child Support Income (CSI)"
                        " that is subsequently used for this Parent."
                        "\n\nIt will always be 0 for a non-parent Carer as thier"
                        " income is not considered (i.e. a non-parent carer has no"
                        " legal responsibility to financially support a child that"
                        " is not thier own.)"
                        "\n\nThe two parent's CSI's are added to determine the "
                        " Combined Child Support Income."
                        "\n\nThe CSI is also used to deterime aparent's percentage"
                        " of the CCSI, this is known as the Parent's Income"
                        " Percentage (PIP)."
                        );
//=============================================================================
// CSI tooltip text (Child Support Income)
char const* csi_tt(
                   "CSI = Child Support Income."
                   "\n\nCSI is the result of substracting the deductables from"
                   " a parent's ATI."
                   "\n\n i.e. CSI = ATI - SSA - RDCA - MCA."
                   "\n\nWhen both parent's CSI's are determined they are added"
                   " to form the Combined Child Support Income."
                   "\n\nA Parent's Income Percentage (PIP) is also derived from"
                   " the CSI, PIP is the percentage of the CSI as part of the"
                   " CCSI."
                   );
//=============================================================================
// CCSI tooltip text (Combined Child Support Income)
// TODO (Mike092015#1#): Complte tooltip text for CCSI

char const* ccsi_tt(
                   "CCSI = Combined Child Support Income."
                   "\n\nCCSI is the total of both parent's Child Support Income."
                   "\n\nCCSI is ised in determening the Cost of Children (COC)."
                   );
//=============================================================================
// COC tooltip text (Cost f Children)
// TODO (Mike092015#1#): Complete tooltip text for CCSI

char const* coc_tt(
                   "COC = Cost ofChildren."
                   "The Cost of Children is determine by applying the Combined"
                   " Child Support Income (CCSI) to the Cost of Children tables."
                   "\n\nThe Cost of Children tables has 8 sets of figures, these"
                   "based upon permutatons of the childre's ages."
                   "\n\nThere are sets for 1,2 and 3 children of the two age"
                   " groups. The age groups being 1 for under 13's and another"
                   " for 13 and overs."
                   "There are two additonal sets for 2 or 3 children of mixed"
                   " age groups."
                   "\n\nIncome levels are also factored in. There are 5 income"
                   "levels. The income levels change annually and are all based"
                   " upon AMTAWE, which changes annually. "
                   "\n\nThe levels are:"
                   "\n\t0 to AMTAWE * 0.5"
                   "\n\t(0.5 * AMTAWE) + 1 to AMTAWE"
                   "\n\tAMTAWE + 1 to AMTAWE * 1.5"
                   "\n\t(AMTAWE * 1.5) + 1 to AMTAWE * 2"
                   "\n\t(AMTAWE * 2) + 1 to AMTAWE * 2.5"
                   "\n\nIncome that exceeds AMTAWE * 2.5 is discounted (i.e. "
                   "the CCSI is capped at 2.5 times AMTAWE."
                   "\n\nThe age sets and the income levels then form a matrix"
                   " each cell of the matrix has an associate percentage."
                   "\n\nThat percentage of the CCSI that is at that level"
                   "is added to other such amounts for other cells to"
                   " produce the Cost of the Children."
                   "\n\nNote! only 1 set of age groups would be used per"
                   "calculation, whilst multiple income levels may be used."
                   "\n\nNote! a maximum of 3 children are taken into account. If"
                   " there are more then the 3 oldest are used."
                   "\n\n"
                   );
//=============================================================================
// PIP tooltip text (Parent's Income Percentage)
// TODO (Mike092015#1#): complete tooltip text for PIP
char const* pip_short(
                      "PIP = Parent's Income Percentage."
                      );
char const* pip_long(
                     "PIP = Parent's Income Percentage."
                     "\n\nThis is the percentage of the Combined Child Support"
                     " Income (CCSI) that the respective parent's Child Support"
                     " Income (CSI) forms. i.e. CSI / CCS1 * 100."
                     "\n\nPIP less the Cost Pecentage, is used to determine the"
                     " CS liability for the respective child."
                     "\n\nNotes"
                     "\n\tIf PIP less the Cost Percentage is negative then there"
                     " is no CS liability."
                     "\n\tOther rules may apply that alter the liability."
                     );

char const* pip_tt(
                   "PIP = Parent's Income Percentage."
                   "\n\nPIP is the percentage of the Combined Child Support"
                   " Income (CCSI) that the parent's Child"
                   );
//=============================================================================
// Liability tooltip text (CS liability/Annual CS Amount)
// TODO (Mike092015#1#): Complete tooltip text for Liability

char const* liability_tt(
                   ""
                   );
