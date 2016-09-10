#ifndef FILE_CSA_READ
#define FILE_CSA_READ
const int csa_test=0;
//===============================================================================================
// DINT a class holding two integers
//===============================================================================================
class DINT {
    public:
        DINT(int, int);
        DINT();
        ~DINT();
        int aint;
        int bint;
};
DINT::DINT(int a, int b) {
    aint = a;
    bint = b;
}
DINT::DINT() {
    aint=0;
    bint=0;
}
DINT::~DINT() {
}

//===============================================================================================
// RTYPES a class to hold counts of the type of relationships
//===============================================================================================
class RTYPES {
    public:
        RTYPES(int, int, int, int, int);
        RTYPES();
        ~RTYPES();
        int getliables();
        int parents, others, nonparentcarers, parentsabroad, deceasedparents;
        int liableparents;
};
RTYPES::RTYPES(int p, int o, int npc, int pa, int dp) {
    parents = p;
    others = o;
    nonparentcarers = npc;
    parentsabroad = pa;
    deceasedparents=dp;
    liableparents = p + pa;
}
RTYPES::RTYPES() {
    parents = 0;
    others = 0;
    nonparentcarers = 0;
    parentsabroad = 0;
    deceasedparents = 0;
    liableparents = 0;
}
RTYPES::~RTYPES() {
}
int RTYPES::getliables() {
    return parents + parentsabroad;
}
//==============================================================================================
// Function to convert a number to a string
template <typename T>
std::string NumberToString( T Number) {
    std::stringstream ss;
    ss << Number;
    return ss.str();
}
std::string BoolToString(bool b) {
    if (b) return "Y";
    return "N";
}

//==============================================================================================
// Function to convert a string to a number
template <typename T>
T StringToNumber ( const std::string &Text, T defValue = T() ) {

    std::stringstream ss;
    for ( std::string::const_iterator i=Text.begin(); i!=Text.end(); ++i )
        if ( isdigit(*i) || *i=='e' || *i=='-' || *i=='+' || *i=='.' )
            ss << *i;
    T result;
    return ss >> result ? result : defValue;
}

//==============================================================================================
// Function to round a floating point number
int Round (float fp) {
    return ((int)((fp * 100.00f)+0.5f)) / 100;
}
//==============================================================================================
//function to round to a floating point number to ? decimal places
float Round2dp (float fp, int dp) {
    if (dp  >= 0) {
        float dpf = pow(10,(float)dp);
        float add = 0.5f / dpf;
        fp = fp + add;
        //fp = fp += add;
        return (float)((int)(fp * dpf) / dpf);
    }
    return fp;
}



//==============================================================================================
// Annualised Male Total Average Weekly Earnings Split multipliers
// These values are used to multiply the AMTAWE in order to ascertain
// the amounts at which differing rates of CS are paid. Similar to
// tax brackets.
// These shouldn't need to be changed, unless the legislation itself
// is changed.
//===============================================================================================
const float amtawe_splits[5] = { 0.5, 1.0, 1.5, 2.0, 2.5 };

//===============================================================================================
// Class for CS amounts.
// As child support depends upon core amounts that are changed annually
// An instance when constructed will take these core values and then calculate
// other valuse.
// When creating an instance the following are required as parameters :-
// 1) The Year (CS is calculated based upon the amounts for the year in which the assesment start)
//    Note that there may be requirements to calculate CS for other years.
// 2) The AMTAWE (Annualised Male Total Average Weekly Earnigs)
//    The AMTAWE is used to detrmine the Self Support Amount and is the basis for the cost of children
//    tables (6 bands based upon 5 amounts (see amtawe_splits above) with 8 classes of children)
//    classes are 1,2 or 3 minors (under 13), 1,2 or 3 teens and 2 or 3 of mixed  age groups.
// 3) pps (Parenting Payment Single), this is used to determine the level at which fixed assessments
//    may apply
// 4) fa (Fixed Assessment) annual amount (amount of CS per child for up to 3 children if a parent
//    is not on income support and records a taxable income lower than pps)
// 5) CS Minimum Rate, rate that is paid per case for up to 3 cases if the parent has less than
//    14% care and is on income support.
//===============================================================================================
class cs_amounts {
    public:
        cs_amounts(int cs_year, int cs_amtawe, int cs_pps, int cs_fa, int cs_min_rate);
        ~cs_amounts();
        int getyear();
        int getamtawe();
        int getpps();
        int getfa();
        int getminrate();
        int getssa();
        int calccostofchild(int minors, int teens, int income);
        std::string getyearnotes();
    protected:
        void finishConstruct();
        int year;                       // Assessment year
        int amtawe;                     // Annualised Male Total Average Weekly Earnings
        int pps;                        // Parenting Payment Single (max normal)
        int fa;                         // Fixed Annual rate for fixed assessments
        int minrate;                    // Min Annual Rate of CS
        int ssa;                        // Self Support Amount
        float coct_band_lower[5];
        float coct_band_upper[5];
        float coct_stg_prevadd[8][5];     // Cost of Children Table previous costs array
        std::string coct_rate_str;           // Cost of Children Table percentage multipliers string
        std::string m1, m2, m3, t1, t2, t3, c2, c3;  // Intermediate strings combined to make above
        float coct_rates[8][5];         // Cost of Children Table percent multipliers (extracted)
};

// cs_amounts class constructor
// Initially set according to annual values but then calls finishConstruct to
// generated derived values e.g. SSA is 1/3 cs_amtawe.
cs_amounts::cs_amounts(int cs_year, int cs_amtawe, int cs_pps, int cs_fa, int cs_min_rate) {
    year = cs_year;
    amtawe = cs_amtawe;
    pps = cs_pps;
    fa = cs_fa;
    minrate = cs_min_rate;
    finishConstruct();

}
// Finish the contruction by building derrived values
void cs_amounts::finishConstruct() {

    // Calculate the SSA (Self Support Amount)
    // SSA is 1/3rd of AMTAWE rounded to 0dp
    // Note!!! must use float to perform calculation
    ssa = (int)Round2dp((float)amtawe / (float)3,0);


    // Build Cost Of Children Tables
    // =============================
    // Cost of Children tables consists of 8 subtables each used accoring to
    // the number and age of the children for whom the cost is being determined.
    // Children are 1 of two ages under 13 OR 13 and over.
    // Three subtables exist for each age group each subtable for 1, 2 or 3 children of
    // that age group.
    // Two subtables exist for combined age groups (cannot be combined if only 1 child) so
    // one for 2 and the other for 3 children with combined age groups
    //
    // For each cell (table box) there are specific percentages for income levels for the
    // specific child combination. There are 5 income levels, hence 5 percentage values
    // per child combination.
    // Income levels are derived from AMTAWE starting at .5 times, subsequent 4 levels
    // are each .5 higher i.e. .5, 1, 1.5, 2 and finally 2.5 (cap applies at 2.5)

    // set cost percentages string (long winded but should make updates easier to handle)
    m1 = "0.17,0.15,0.12,0.10,0.07";    // 1 minor (under 13)
    m2 = "0.24,0.23,0.20,0.18,0.10";    // 2 minors
    m3 = "0.27,0.26,0.25,0.24,0.18";    // 3 minors
    t1 = "0.23,0.22,0.12,0.10,0.09";    // 1 teen (13+)
    t2 = "0.29,0.28,0.25,0.20,0.13";    // 2 teens
    t3 = "0.32,0.31,0.30,0.29,0.20";    // 3 teens
    c2 = "0.265,0.255,0.225,0.19,0.115";// 2 combined (1 teen, 1 minor)
    c3 = "0.295,0.285,0.275,0.265,0.19";// 3 combined 1 teen, 1 minor plus 1 of either

    // make single string to allow the coct_rates array to be populated in one go
    coct_rate_str = m1 + "," + m2 + "," + m3 + "," + t1 + "," + t2 + "," + t3 + "," + c2 + "," + c3;

    // create a working copy of the rates string (will be progressively demolished)
    std::string wrk = coct_rate_str;
    // string to hold current string extract
    std::string curr;
    // holder for the location that find returns
    size_t loc;
    // do the initial find
    loc = wrk.find(",");

    // Get ready to build the Cost Of Children Table data
    int i = 0;
    int agetype = 0;
    int band = 0;
    while (loc!=std::string::npos) {;
        // extract the current value from the Cost of Children Rates
        curr = wrk.substr(0,loc);
        // remove the current value and the preceeding separator from the working string
        wrk = wrk.substr(loc+1);
        // ready for the next iteration
        loc = wrk.find(",");
        band = i % 5;
        agetype = i / 5;
        // prevadd (i.e. max rate from the previos band is 0 if the first in the band)
        if (band == 0) {
            coct_stg_prevadd[agetype][band] = 0;
        }
        // If not the first in the band then calculate the max rate from the previous band
        // and then add it to the rate for the previous band (i.e. prevadd)
        else {
            coct_stg_prevadd[agetype][band] = round((coct_band_upper[band-1]-(coct_band_lower[band-1]-1)) * coct_rates[agetype][band-1]);
            coct_stg_prevadd[agetype][band] = coct_stg_prevadd[agetype][band] + coct_stg_prevadd[agetype][band-1];
        }
        // Work out the band amounts (only need to do this for bands in the first child group [agetype])
        // but for all bands
        if (agetype == 0) {
            // if first band, then lower/base will be 0
            if (band ==0) {
                coct_band_lower[band] = 0;
            }
            // otehrwise lower will be the upper value of the previous band + 1
            else {
                coct_band_lower[band] = coct_band_upper[band - 1] + 1;
            }
            //calculate the upper band as amtawe times the respective rate for the band
            coct_band_upper[band] = round(amtawe * amtawe_splits[band]);
        }

        // store the rate (percentage multiplier) for the child type/band combination
        coct_rates[agetype][band] = StringToNumber(curr,0.0);
        // increment iteration counter
        i++;
    }
    // Do final values for the last value of curr
    curr = wrk;
    band = i % 5;
    agetype = i / 5;
    coct_rates[agetype][band] = StringToNumber(curr,0.0);
    coct_band_lower[band] = coct_band_upper[band-1] + 1;
    coct_band_upper[band] = round(amtawe * amtawe_splits[band]);
    if (band == 0) coct_stg_prevadd[agetype][band] = 0;
    else {
            coct_stg_prevadd[agetype][band] = round((coct_band_upper[band-1]-(coct_band_lower[band-1]-1)) * coct_rates[agetype][band-1]);
            coct_stg_prevadd[agetype][band] = coct_stg_prevadd[agetype][band] + coct_stg_prevadd[agetype][band-1];
    }
}

// cs_amounts class destructor
cs_amounts::~cs_amounts() {
}

// cs_amounts getyear method
int cs_amounts::getyear() {
    return year;
}

// cs_amounts getamtawe method
int cs_amounts::getamtawe() {
    return amtawe;
}

// cs_amounts getpps method (payment parenting single)
int cs_amounts::getpps() {
    return pps;
}

//cs_amounts getfa (fixed assessment rate)
int cs_amounts::getfa() {
    return fa;
}

// cs_amounts getminrate method (minimum rate of CS)
int cs_amounts::getminrate() {
    return minrate;
}

//cs_amounts getssa (self support amount)
int cs_amounts::getssa() {
    return ssa;
}

// cs_amounts calccostofchild
int cs_amounts::calccostofchild(int minors, int teens, int income) {

    int coc = 0;
    int cgroup = 100;

    // If no children then no cost of children so return 0
    if (minors+teens < 1) {
        return 0;
    }

    //======================================================================
    // Determine child group
    // 1, 2, 3 or more minors = group 0-2 respectively
    // 1, 2, 3 or more teens  = group 3-5 respectively
    // 2 or 3 or mixed

    //If no teens then must be under 13's only (cgroup 0-2)
    if (teens < 1) {
        cgroup = minors -1;
        if (cgroup > 2) {
            cgroup = 2;
        }
    }
    // If no minors or 3 or more teens then must be teens only (cgroup 3-5)
    if (minors < 1 || teens > 2) {
        cgroup = teens + 2;
        if (cgroup > 5) {
            cgroup = 5;
        }
    }
    // If minors and teens and less than 3 teens must be mixed (cgroup 6-7)
    if (minors > 0 && teens > 0 && teens < 3) {
        cgroup = teens + minors + 4;
        if (cgroup > 7) cgroup = 7;
    }
    // Calculate the cost of children for where the income is as much or greater
    // than the highest band
    if (income >= coct_band_upper[4]) {
        coc = (int)coct_stg_prevadd[cgroup][4]+(int)round((coct_band_upper[4]-coct_band_upper[3])*coct_rates[cgroup][4]);
    }
    // Calculcate the cost of chldren when the income is within the bands
    else {
        for (int i = 4; i >= 0; i--) {
            if (income <= coct_band_upper[i] && income >= coct_band_lower[i]) {
                // calculate for lowest band
                if (i == 0) {
                    coc = (int)round((income * coct_rates[cgroup][i]));
                }
                // calculate for other bands
                else {
                    coc = (int)round((int)(income - coct_band_upper[i-1]) * coct_rates[cgroup][i]) + (int)coct_stg_prevadd[cgroup][i];
                }
            }
        }
    }
    return coc;
}
std::string cs_amounts::getyearnotes() {
    std::string r = "Yr=";
    r.append(NumberToString(year));
    r.append(" AMTAWE=$");
    r.append(NumberToString(amtawe));
    r.append(" SSA=$");
    r.append(NumberToString(ssa));
    r.append(" MinCS=$");
    r.append(NumberToString(minrate));
    r.append(" FxdAss=$");
    r.append(NumberToString(fa));
    r.append(" PPS=");
    r.append(NumberToString(pps));
    return r;
}
//===============================================================================================
// Class for CS care rates (i.e. conversion of level of care)
// determines care percentage and cost percentage from the given
// level of care in nights.
//===============================================================================================
class cs_carerates {
    public:
        cs_carerates();
        cs_carerates(int carenights);
        ~cs_carerates();
        int getcarerate();
        int getcostrate();
        int GetLevelOfCare();
        std::string Display();
    protected:
    int levelofcare;
    int carerate;
    int costrate;
};
// Constructor which takes the level of care in nights
// and then
cs_carerates::cs_carerates() {
}
cs_carerates::cs_carerates(int carenights) {
    levelofcare = carenights;
    // Set the carerate (i.e. actual percentage of care)
    float cn = ((float)carenights / 365) * 100;
    if (cn < 50) {
        carerate = (int)floor(cn);
    }
    else {
        carerate = (int)ceil(cn);
    }

    // set the cost rate
    if (carerate <= 13) costrate=0;
	else if (carerate >= 14 && carerate <= 34) costrate = 24;
    else if (carerate >= 35 && carerate <= 47) costrate = 25 + ((carerate - 35) * 2);
    else if (carerate >= 48 && carerate <= 52) costrate = 50;
    else if (carerate >= 53 && carerate <= 65) costrate = 51 + ((carerate - 53) * 2);
	else if (carerate >= 66 && carerate <= 86) costrate = 76;
	else costrate = 100;
}
//destructor (does nothing)
cs_carerates::~cs_carerates() {
}
int cs_carerates::getcarerate() {
    return carerate;
}
int cs_carerates::getcostrate() {
    return costrate;
}
int cs_carerates::GetLevelOfCare() {
    return levelofcare;
}
std::string cs_carerates::Display() {
    std::string r = "\n";
    r.append("Level of Care=");
    r.append( NumberToString(levelofcare));
    r.append(" Care Rate=");
    r.append( NumberToString(carerate));
    r.append(" Cost Rate=");
    r.append( NumberToString(costrate));
    r.append("\n");
    return r;
}

//===============================================================================================
// Class to hold basic adult information
//===============================================================================================
class cs_adult {

    public:
    cs_adult(std::string, int, int);
    ~cs_adult();
    std::string getaname();
    int getati();
    void setati(int);
    int getaid();
    bool setrlt(int, int);
    void addrlt(int);
    bool rmvrlt(int);
    int rltsize();
    void setaname(std::string);
    std::string Display();

    private:
    std::string aname;
    int aid;
    int ati;
    std::deque<int> rlt;
};
// Constructor
cs_adult::cs_adult(std::string adults_name, int adults_ati, int adults_aid) {
    aname = adults_name;
    ati = adults_ati;
    aid = adults_aid;
    if (DEBUG) std::cout << "\ncs_adult constructor made :-\n" << Display();
}
// Destructor (does nothing)
cs_adult::~cs_adult() {
}
// Getaname (returns adults name as string)
std::string cs_adult::getaname() {
    return aname;
}
// getati (returns adutls ati (Adjusted Taxable Income))
int cs_adult::getati() {
    return ati;
}
// getaid ( returns adults id ( unique identifier))
int cs_adult::getaid() {
    return aid;
}
void cs_adult::setati(int adults_ati) {
    ati = adults_ati;
}
void cs_adult::addrlt(int typ) {
    rlt.push_back(typ);
}
bool cs_adult::setrlt(int cid, int typ) {
    if ((signed)rlt.size() > (signed)cid) {
        rlt[cid]=typ;
        return true;
    }
    return false;
}
bool cs_adult::rmvrlt(int cid) {
    if ((signed)rlt.size() > (signed)cid) {
        rlt.erase(rlt.begin() + cid);
        return true;
    }
    return false;
}
int cs_adult::rltsize() {
    return rlt.size();
}
void cs_adult::setaname(std::string n) {
    aname = n;
}
std::string cs_adult::Display() {
    std::string r ="\n";
    r.append("Adult=");
    r.append( NumberToString(aid));
    r.append(" Name=");
    r.append(aname);
    r.append(" ATI=");
    r.append( NumberToString(ati));
    for (int i=0; i < (signed) rlt.size(); i++) {
        r.append(" \n\tRelationship to child ");
        r.append( NumberToString(i));
        r.append(" is ");
        r.append( NumberToString(rlt[i]));
    }
    r.append("\n");
    return r;
}
//===============================================================================================
// Class to hold basic child information
//===============================================================================================
class cs_child {
    public:
    cs_child(std::string childs_name, int childs_age, int childs_cid);
    ~cs_child();
    std::string GetChildsName();
    int getcage();
    bool isteen();
    void setteen(bool);
    void addrlt(int);
    void addcare(int);
    bool setrlt(int, int);
    bool setcare(int, int);
    bool rmvrlt(int);
    bool rmvcare(int);
    int getrlt(int);
    int GetCare(int);
    int rltsize();
    void setcname(std::string);
    int parenttypecount(int);
    int caresz();
    bool care365();
    void setrltcounts(int, int, int, int, int);
    std::string Display();
    signed int GetTotalCare();

    private:
    std::string cname;
    int cid;
    int age;
    bool teen;
    RTYPES rltcounts;
    std::deque<int> rlt;
    std::deque<int> care;
};
// constructor
cs_child::cs_child(std::string childs_name, int childs_age, int childs_cid) {
    cname = childs_name;
    age = childs_age;
    if (age >= 13) teen = true;
    else teen = false;
    cid = childs_cid;
    rltcounts.parents = 0;
    rltcounts.others = 0;
    rltcounts.nonparentcarers = 0;
    rltcounts.parentsabroad = 0;
    rltcounts.deceasedparents = 0;
}
// destructor (does nothing)
cs_child::~cs_child() {
}
std::string cs_child::GetChildsName() {
    return cname;
}
int cs_child::getcage() {
    return age;
}
bool cs_child::isteen() {
    return teen;
}
void cs_child::addrlt(int typ) {
    rlt.push_back(typ);
    //rlt[rlt.size()-1]=typ;
}
void cs_child::addcare(int c) {
    care.push_back(c);
}
bool cs_child::setrlt(int aid, int typ) {
    if ((signed)rlt.size() > (signed)aid) {
        rlt[aid]=typ;
        return true;
    }
    return false;
}
bool cs_child::setcare(int aid, int c) {
    if ((signed)care.size() > (signed)aid) {
        care[aid]=c;
        return true;
    }
    return false;
}
bool cs_child::rmvrlt(int aid) {
    if ((signed)rlt.size() > (signed)aid) {
        rlt.erase(rlt.begin() + aid);
        care.erase(care.begin() + aid);
        return true;
    }
    return false;
}
bool cs_child::rmvcare(int aid) {
    if ((signed)care.size() > (signed)aid) {
        care.erase(care.begin() + aid);
        rlt.erase(rlt.begin() +aid);
        return true;
    }
    return false;
}
int cs_child::getrlt(int aid) {
    return rlt[aid];
}
int cs_child::GetCare(int aid) {
    return care[aid];
}
int cs_child::rltsize() {
    return rlt.size();
}
void cs_child::setcname(std::string s) {
    cname = s;
}
void cs_child::setteen(bool ac) {
    teen = ac;
}
int cs_child::parenttypecount(int typ) {
    int r=0;
    for (int i=0; i < (signed int)rlt.size(); i++) {
        if (rlt[i] == typ) r+=1;
    }
    return r;
}
int cs_child::caresz() {
    return care.size();
}
bool cs_child::care365() {
    int tc=0;
    for (int i=0;i < (signed int)care.size(); i++) {
        tc+=care[i];
    }
    if (tc == 365) return true;
    return false;
}
signed int cs_child::GetTotalCare() {
    signed int tc = 0;
    for (int i=0; i < (signed int)care.size(); i++) {
        tc+=care[i];
    }
    return 365-tc;
}
std::string cs_child::Display() {
    std::string r="\n";
    r.append("CID=");
    r.append( NumberToString(cid));
    r.append(" Name=");
    r.append(cname);
    r.append(" Age=");
    r.append( NumberToString(age));
    if (teen) r.append(" Teen=N");
    else r.append(" Teen=Y");
    for (int i=0; i < (signed)rlt.size(); i++) {
        r.append("\n\t Rlt=");
        r.append( NumberToString(rlt[i]));
        r.append(" Care=");
        r.append( NumberToString(care[i]));
    }
    r.append("\n");
    return r;
}
//===============================================================================================
// Class for relationships
// a relationship will exist for every combination of child and adult.
// (i.e. number of relationships = number of children * number of adults.
//===============================================================================================
class cs_relationship {
    public:
        cs_relationship(int, int, int, int);
        cs_relationship(int, int, int , int , std::string);
        ~cs_relationship();
        int getchild();
        int GetAdult();
        DINT GetAdultandChild();
        int GetRelationshipType();
        int GetCare();
        int GetCarePercentage();
        int GetCostPercentage();
        std::string GetChildsName();
        bool IsInCase();
        void SetInCase(int);
        std::string Display();
    private:
        int child;
        int adult;
        int rtype;
        int ncare;
        bool incase;
        int owningcase;
        cs_carerates cr;
        std::string childname;
};
cs_relationship::~cs_relationship() {
}
cs_relationship::cs_relationship(int c, int a, int r, int nc) {
    child=c;
    adult=a;
    rtype=r;
    ncare=nc;
    if (childname.size() < 1) childname="";
    cr = cs_carerates(nc);
    incase = false;
    owningcase=-1;
}
cs_relationship::cs_relationship(int c, int a, int r, int nc, std::string cn) {
    childname = cn;
    child=c;
    adult=a;
    rtype=r;
    ncare=nc;
    cr = cs_carerates(nc);
    incase = false;
    owningcase=-1;
}
int cs_relationship::getchild() {
    return child;
}
int cs_relationship::GetAdult() {
    return adult;
}
DINT cs_relationship::GetAdultandChild() {
    DINT d(child,adult);
    return d;
}
int cs_relationship::GetRelationshipType() {
    return rtype;
}
int cs_relationship::GetCare() {
    return ncare;
}
int cs_relationship::GetCarePercentage() {
    return cr.getcarerate();
}
int cs_relationship::GetCostPercentage() {
    return cr.getcostrate();
}
std::string cs_relationship::GetChildsName() {
    return childname;
}
bool cs_relationship::IsInCase() {
    return incase;
}
void cs_relationship::SetInCase(int c) {
    incase = true;
    owningcase=c;
}
std::string cs_relationship::Display() {
    std::string r = "\nChild=";
    r.append ( NumberToString(child));
    r.append("(");
    r.append(childname);
    r.append(") Adult=");
    r.append( NumberToString(adult));
    r.append(" Type=");
    r.append( NumberToString(rtype));
    r.append(" Care=");
    r.append( NumberToString(ncare));
    r.append(" Care\%=");
    r.append( NumberToString(cr.getcarerate()));
    r.append(" Cost\%=");
    r.append( NumberToString(cr.getcostrate()));
    r.append(" Level of Care=");
    r.append( NumberToString(cr.GetLevelOfCare()));
    r.append( " In a case=");
    r.append( BoolToString(incase));
    r.append( " Owning Case=");
    r.append( NumberToString(owningcase));
    r.append("\n");
    return r;
}
//===============================================================================================
// Class for an adult case.
// An adult case reflects a sublevel case between an adult and a child, thus it mat be that there
// are multiple adult cases for the same adult that will be combined into a full case. There may
// also be additional adult cases as an adult can be involved in multiple full cases.
//===============================================================================================
class cs_adultcase {
    public:
        cs_adultcase(int, int, std::deque<cs_adult>, std::deque<cs_relationship>);
        ~cs_adultcase();
        std::string GetAdultName();
        int GetRlt();           // the specific relationship object
        int GetRlTyp();         // the relationship type
        int GetAdult();
        int GetCare();
        int GetCarepc();
        int GetCostpc();
        int GetChild();
        std::string Display();
    private:
        int adult;
        int child;
        std::string aname;
        int rlt;
        int rltyp;
        int ati;
        int care;
        int carepc;
        int costpc;
        int mc_cost;
        int mc_cap;

};
cs_adultcase::cs_adultcase(int a, int r, std::deque<cs_adult> adlt, std::deque<cs_relationship> rltshp) {

    if (DEBUG) std::cout << "\n\tAdultCase Construction:" << "\n\t\tInput Item a(1st)=" << a
        << "\n\t\tInput Item r(2nd)=" << r << "\n";

    adult = a;
    aname = adlt[a].getaname();
    ati = adlt[a].getati();
    rlt = r;
    child = rltshp[r].getchild();
    rltyp = rltshp[r].GetRelationshipType();
    care = rltshp[r].GetCare();
    carepc = rltshp[r].GetCarePercentage();
    costpc = rltshp[r].GetCostPercentage();
    mc_cost = 0;
    mc_cap = 0;

    if (DEBUG) std::cout << "\n\tAdullt Case Built\n";
}
cs_adultcase::~cs_adultcase() {
}
int cs_adultcase::GetRlTyp() {
    return rltyp;
}
int cs_adultcase::GetAdult() {
    return adult;
}
int cs_adultcase::GetCare() {
    return care;
}
int cs_adultcase::GetCarepc() {
    return carepc;
}
int cs_adultcase::GetCostpc() {
    return costpc;
}
int cs_adultcase::GetChild() {
    return child;
}
int cs_adultcase::GetRlt() {
    return rlt;
}
std::string cs_adultcase::GetAdultName() {
    return aname;
}
std::string cs_adultcase::Display() {
    std::string r="\nAdult=";
    r.append( NumberToString(adult));
    r.append( " Name=");
    r.append(aname);
    r.append(" Child=");
    r.append( NumberToString(child));
    r.append("\n\tRLT=");
    r.append( NumberToString(rlt));
    r.append("\tRLT Type=");
    r.append( NumberToString(rltyp));
    r.append("\n\tATI=");
    r.append( NumberToString(ati));
    r.append("\n\tCare=");
    r.append( NumberToString(care));
    r.append("\tCare\%=");
    r.append( NumberToString(carepc));
    r.append("\tCost\%=");
    r.append( NumberToString(costpc));
    r.append("\n\tMultiCase Cost=");
    r.append( NumberToString(mc_cost));
    r.append("\tMultiCase Cap=");
    r.append( NumberToString(mc_cap));
    r.append("\n");
    return r;

}
//===============================================================================================
// Class for each child case.
// A child case is a sublevel case which is used as the basis for a fullcase. A fullcase may have
// multiple children.
// A child case will only exist if the child has at least 1 parent and either another parent or
// a non-parent carer.
//===============================================================================================
class cs_childcase {
    public:
        cs_childcase(int, std::deque<cs_child>);
        ~cs_childcase();
        int getchild();
        void AddAdultCase(int, std::deque<cs_adultcase>);
        void SetMainCarer(int, int);
        bool IsMerged();
        void SetMerged();
        std::string GetChildName();
        int GetMainCarerAdultCase();
        int GetMainCarerAdult();
        int GetMainCarerCare();
        int GetMainCarerType();
        int GetAdultCaseCount();
        int GetAdultCase(int);
        std::string Display();
        int GetACSize();
        int GetAC(int);
        bool IsThisChildName(std::string);
        void AddTotalRecipientCarePercent(int);
        int GetTotalRecipientCarePercent();
    private:
        std::string cname;                   // Name of the child
        int child;                      // index of the corresponding child deque
        bool merged;                    // Flag to indicate whether or not this has been merged into a full case
        signed int main_carer_ac;       // index of the adult case deque entry that has the most care of this child
        signed int main_carer_adult;    // index of the adult deque entry for the adult with the most care
        signed int main_carer_typ;      // the relationship type of the main carer
        signed int main_carer_care;     // the level of care of the main carer
        signed int total_recipient_carepc;  // The total percentage of care for the CS recipients.
        std::deque<int> ac;
};

cs_childcase::cs_childcase(int c, std::deque<cs_child> chld) {
    child = c;
    cname = chld[c].GetChildsName();
    merged=false;
    main_carer_ac = -1;
    main_carer_care = 0;
    main_carer_adult = -1;
    main_carer_typ = -1;
    total_recipient_carepc = 0;
}
cs_childcase::~cs_childcase() {
}
int cs_childcase::getchild() {
    return child;
}
void cs_childcase::AddAdultCase(int adultcase, std::deque<cs_adultcase> adultcases) {
    ac.push_back(adultcase);
    if ( adultcases[adultcase].GetCare() > main_carer_care) {
        main_carer_ac = adultcase;
        main_carer_care = adultcases[adultcase].GetCare();
        main_carer_adult = adultcases[adultcase].GetAdult();
        main_carer_typ = adultcases[adultcase].GetRlTyp();
    }
}

bool cs_childcase::IsMerged() {
    return merged;
}
std::string cs_childcase::GetChildName() {
    return cname;
}
int cs_childcase::GetMainCarerAdultCase() {
    return main_carer_ac;
}
int cs_childcase::GetMainCarerAdult() {
    return main_carer_adult;
}
int cs_childcase::GetMainCarerType() {
    return main_carer_typ;
}
int cs_childcase::GetMainCarerCare() {
    return main_carer_care;
}
int cs_childcase::GetAdultCaseCount() {
    return ac.size();
}
int cs_childcase::GetAdultCase(int acase) {
    return ac[acase];
}
int cs_childcase::GetACSize() {
    return ac.size();
}
int cs_childcase::GetAC(int ix) {
    return ac[ix];
}
void cs_childcase::SetMerged() {
    merged=true;
}
void cs_childcase::AddTotalRecipientCarePercent(int carepc) {
    total_recipient_carepc+=carepc;
}
int cs_childcase::GetTotalRecipientCarePercent() {
    return total_recipient_carepc;
}
bool cs_childcase::IsThisChildName(std::string childname) {
    if (cname == childname) return true;
    return false;
}
std::string cs_childcase::Display() {
    std::string r="\n\tChild=";
    r.append( NumberToString(child));
    r.append( " Name=");
    r.append(cname);
    r.append(" Merged=");
    if(merged) r.append("Y");
    else r.append("N");
    r.append("\n\t\tMain Carer Adult Case=");
    r.append( NumberToString(main_carer_ac));
    r.append("\n\t\tMain Carer Adult=");
    r.append( NumberToString(main_carer_adult));
    r.append("\n\t\tMain Carer Care=");
    r.append( NumberToString(main_carer_care));
    r.append("\n\t\tMain Carer Type=");
    r.append( NumberToString(main_carer_typ));
    r.append("\n\t\tAdult Case Count=");
    r.append( NumberToString((signed)ac.size()));
    for (int i=0; i < (signed)ac.size(); i++) {
        r.append("\n\t\t\tAdultCase=");
        r.append( NumberToString((signed)ac[i]));
    }
    r.append("\n\t\tTotal Recipient Level of Care=");
    r.append(NumberToString(total_recipient_carepc));
    r.append("\n");
    return r;
}
//===============================================================================================
// Class for a full relationship
// A fullrelationship will exist for each fulladult/fullchildcase combination and is used
// to hold information at this sub level
//===============================================================================================
class cs_full_adultcase;
class cs_fullcase;

class cs_full_relationship {
    public:
        cs_full_relationship(int, std::string, int, int, int, int, int, bool);
        std::string GetChildsName();
        int GetCare();
        int GetChild();
        int GetAdult();
        int GetRType();
        int GetCarePercentage();
        int GetCostPercentage();
        int GetCostOfTheChild();
        int GetMultiCaseCap();
        int GetMultiCaseCost();
        bool GetAge();
        void SetMCValues(int, int);
        void SetMCCap(int);
        int GetMCCost();
        void SetPCSPC(float);
        float GetPCSPC();
        float GetEffPCSPC();
        int GetCOTC();
        void SetCOTC(int);
        int GetPreAdjCOTC();
        void SetPreAdjCOTC(int);
        void AddRcvTotCare(int);
        int GetRcvTotCare();
        std::string Display();
        friend class cs_full_adultcase;
        friend class cs_fullcase;
    private:
        std::string childname;           // the child's name
        int child;                  // Reference to the child
        bool age;                   // Age Indicator (true = 13 or over)
        int adult;                  // reference to the adult
        int rtype;                  // Relationship type
        int care;                   // the level of care (# of nights care the owning adult has)
        int carepc;                 // care percentage
        int costpc;                 // cost percentage (as per CS legilsation e.g. 14%-under 35% = 24%)
        float pcspc;                // Parents child support percentage for this child
        float effpcspc;             // Effective Parents child support percentage (i.e if negatibe then 0)
        int preadj_cotc;            // Cost of the child before adjustments
        int cotc;                   // cost of this child
        int mccost;                 // MultiCase Cost of this child
        int mccap;                  // MultiCase Cap for this child;
        int rcv_tot_care;
};
cs_full_relationship::cs_full_relationship(int chld, std::string chldname, int adlt, int typ, int carelevel,
    int carepercentage, int costpercentage, bool teen) {

        child = chld;
        adult = adlt;
        rtype = typ;
        childname = chldname;
        care = carelevel;
        carepc = carepercentage;
        costpc = costpercentage;
        pcspc = 0;
        effpcspc = 0;
        preadj_cotc=0;
        cotc = 0;
        age = teen;
        mccost = 0;
        mccap = 0;
        rcv_tot_care=0;
}
std::string cs_full_relationship::GetChildsName() {
        return childname;
}
int cs_full_relationship::GetCare() {
        return care;
}
int cs_full_relationship::GetChild() {
    return child;
}
int cs_full_relationship::GetAdult() {
    return adult;
}
int cs_full_relationship::GetRType() {
    return rtype;
}
int cs_full_relationship::GetCarePercentage() {
    return carepc;
}
int cs_full_relationship::GetCostPercentage() {
    return costpc;
}
int cs_full_relationship::GetCostOfTheChild() {
    return cotc;
}
int cs_full_relationship::GetMultiCaseCap() {
    return mccap;
}
int cs_full_relationship::GetMultiCaseCost() {
    return mccost;
}
bool cs_full_relationship::GetAge() {
    return age;
}
void cs_full_relationship::SetMCValues(int cost, int cap) {
    mccost = cost;
    mccap = cap;
}
void cs_full_relationship::SetMCCap(int cap) {
    mccap=cap;
}
int cs_full_relationship::GetMCCost() {
    return mccost;
}
void cs_full_relationship::SetPCSPC(float p) {
    pcspc = p;
    if (pcspc < 0) {
        effpcspc = 0;
    } else {
        effpcspc = pcspc;
    }
}
float cs_full_relationship::GetPCSPC() {
    return pcspc;
}
float cs_full_relationship::GetEffPCSPC() {
    return effpcspc;
}
int cs_full_relationship::GetPreAdjCOTC() {
    return preadj_cotc;
}
void cs_full_relationship::SetPreAdjCOTC(int p) {
    preadj_cotc=p;
}
int cs_full_relationship::GetCOTC() {
    return cotc;
}
void cs_full_relationship::SetCOTC(int c) {
    cotc = c;
}
void cs_full_relationship::AddRcvTotCare(int care) {
    rcv_tot_care+=care;
}
int cs_full_relationship::GetRcvTotCare() {
    return rcv_tot_care;
}

std::string cs_full_relationship::Display() {
    std::string r="\n\tFull RLT Child=";
    r.append( NumberToString(child));
    r.append("\tAdult=");
    r.append( NumberToString(adult));
    r.append("\tChilds Name=");
    r.append(childname);
    r.append("\n\tCare=");
    r.append( NumberToString(care));
    r.append("\tCare%=");
    r.append( NumberToString(carepc));
    r.append("\tCost%");
    r.append("\n\tPre Adjusted Cost of this Child=");
    r.append( NumberToString(preadj_cotc));
    r.append( NumberToString(costpc));
    r.append("\n\t\tCost of this Child=");
    r.append( NumberToString(cotc));
    r.append("\n\t\tMuliCase Cost=");
    r.append( NumberToString(mccost));
    r.append("\n\t\tMultiCase Cap=");
    r.append ( NumberToString(mccap));
    r.append("\n");
    return r;
}

//===============================================================================================
// Class for a full adultcase
// A fulladult case represents the overall adult for a case
//===============================================================================================
class cs_full_adultcase {
    public:
        cs_full_adultcase(int, std::string, int, int);
        ~cs_full_adultcase();
        int GetAdult();
        std::string GetAdultName();
        int GetATI();
        int GetATILessSSA();
        int GetSSA();
        int GetRDCCost();
        int GetATILessRDCCost();
        int GetRDCMinors();
        int GetRDCTeens();
        int GetMCMinors();
        int GetMCTeens();
        void AddFullRelationship(int, std::string, int, int, int, int, int, bool);
        void AddMCTeenRlt(int, std::string, int, int, int, int, int);
        void AddMCMinorRlt(int, std::string, int, int, int, int, int);
        int GetRltSize();
        int GetMCMinorsSize();
        int GetMCTeensSize();
        std::string GetRltChildsName(int);
        std::string GetMCTeensChildsName(int);
        std::string GetMCMinorsChildsName(int);
        int GetRltAdult(int);
        int GetRltChild(int);
        int GetRltCare(int);
        int GetRltRType(int);
        int GetRltCarePercentage(int);
        int GetRltCostPercentage(int);
        int GetRltCostOfTheChild(int);
        int GetRltMultiCaseCap(int);
        int GetRltMultiCaseCost(int);
        bool GetRltAge(int);
        void AddRelevantDependantMinor();
        void AddRelevantDependantTeen();
        void AddMultiCaseMinor();
        void AddMultiCaseTeen();
        void SetRDCCost(int);
        int GetMCMinorsCostPercentage(int);
        int GetMCTeensCostPercentage(int);
        void SetMCMinorsValues(int, int, int);
        void SetMCTeensValues(int, int, int);
        void SetMCCost(int);
        void SetMCCap(int);
        void SetRltMCCap(int, int);
        int GetMCMinorsMCCost(int);
        int GetMCTeensMCCost(int);
        std::string MCTeensDisplay(int);
        std::string MCMinorsDisplay(int);
        void SetPIP(float);
        int GetMCA();
        int GetMCCap();
        int GetATILessMCA();
        int GetCSI();
        float GetPIP();
        void SetRltPCSPC(int, float);
        float GetRltPCSPC(int);
        float GetRltEffPCSPC(int);
        int GetRltCOTC(int);
        void SetRltCOTC(int, int);
        int GetRltPreAdjCOTC(int);
        void SetRltPreAdjCOTC(int, int);
        void AddCSCost(float);
        void AddCSGets(float);
        float GetCSGets();
        float GetPays();
        void AddRltRcvTotCare(int, int);
        int GetRltRcvTotCare(int);
        friend class cs_fullcase;
    private:
        int fullcase;           // Identifier (unused at present)
        int adult;              // The adult
        std::string aname;           // The adult's name
        int rdc_minors;         // Thew number of relevant depenant minors this adult has
        int rdc_teens;          // The number of relevant dependant teens this adult has
        int mc_minors;          // The number of multi-case minors not of this case
        int mc_teens;           // The number of multi-case teens not of this case
        int ati;                // The Adjusted Taxable Income for this adult
        int ssa;                // The Self-Support Amount
        int ati_less_ssa;       // The ATI less the SSA
        int rdc_cost;           // The cost of the relevant dependant children
        int ati_less_rdc_cost;  // The ATI less the rdc cost (also less the SSA)
        int mca;                // The multi-case allowance
        int mccap;              // The Muli-case cap
        int ati_less_mca;       // The ati less the mca (also les the rdc cost and the SSA)
        int csi;                // The child support income of this adult
        float pip;              // The parents income support percentage
        float pays;             // The amount this adult pays in CS
        float gets;             // The amount this adult receives in CS
        bool splitcase;         // Whether or not this is a split case
        std::string Display();
        std::deque<cs_full_relationship> rlt;    // The relationships to the CS children in this case.
        std::deque<cs_full_relationship> teenmc;    // List of Cost percentages for the multi-case teens.
        std::deque<cs_full_relationship> minormc;   // List of cost percentages for the multi-case teens.
};
cs_full_adultcase::cs_full_adultcase(int a, std::string anm, int ti, int sa) {
    adult = a;
    aname = anm;
    ati = ti;
    ssa = sa;
    ati_less_ssa = ati-ssa;
    if (ati_less_ssa < 0) ati_less_ssa = 0;
    rdc_cost=0;
    ati_less_rdc_cost = 0;
    rdc_minors = 0;
    mca = 0;
    mccap = 0;
    ati_less_mca = 0;
    csi = 0;
    pip = 0.0f;
    pays = 0.0f;
    gets = 0.0f;
    rdc_teens = 0;
    mc_minors = 0;
    mc_teens=0;
}
cs_full_adultcase::~cs_full_adultcase() {
}
int cs_full_adultcase::GetAdult() {
    return adult;
}
std::string cs_full_adultcase::GetAdultName() {
    return aname;
}
int cs_full_adultcase::GetATI() {
    return ati;
}
int cs_full_adultcase::GetATILessSSA() {
    return ati_less_ssa;
}
int cs_full_adultcase::GetRDCCost() {
    return rdc_cost;
}
int cs_full_adultcase::GetATILessRDCCost() {
    return ati_less_rdc_cost;
}
int cs_full_adultcase::GetSSA() {
    return ssa;
}
float cs_full_adultcase::GetPIP() {
    return pip;
}
int cs_full_adultcase::GetRDCMinors() {
    return rdc_minors;
}
int cs_full_adultcase::GetRDCTeens() {
    return rdc_teens;
}
int cs_full_adultcase::GetMCMinors() {
    return mc_minors;
}
int cs_full_adultcase::GetMCTeens() {
    return mc_teens;
}
void cs_full_adultcase::AddFullRelationship(int chld, std::string chldname, int adult, int rtype,
        int levelofcare, int carepc, int costpc, bool teen) {
    rlt.push_back(cs_full_relationship(chld, chldname, adult, rtype, levelofcare, carepc, costpc, teen));
}
void cs_full_adultcase::AddMCTeenRlt(int chld, std::string childname, int adult, int rtype,
        int levelofcare, int carepc, int costpc) {
    teenmc.push_back(cs_full_relationship(chld, childname, adult, rtype, levelofcare, carepc, costpc, true));
}
void cs_full_adultcase::AddMCMinorRlt(int chld, std::string childname, int adult, int rtype,
        int levelofcare, int carepc, int costpc) {
    minormc.push_back(cs_full_relationship(chld, childname, adult, rtype, levelofcare, carepc, costpc, false));
}
int cs_full_adultcase::GetRltSize() {
    return rlt.size();
}
int cs_full_adultcase::GetMCMinorsSize() {
    return minormc.size();
}
int cs_full_adultcase::GetMCTeensSize() {
    return teenmc.size();
}
std::string cs_full_adultcase::GetRltChildsName(int chld) {
    return rlt[chld].GetChildsName();
}
std::string cs_full_adultcase::GetMCTeensChildsName(int chld) {
    return teenmc[chld].GetChildsName();
}
std::string cs_full_adultcase::GetMCMinorsChildsName(int chld) {
    return minormc[chld].GetChildsName();
}
int cs_full_adultcase::GetRltCare(int chld) {
    return rlt[chld].GetCare();
}
int cs_full_adultcase::GetRltAdult(int chld) {
    return rlt[chld].GetAdult();
}
int cs_full_adultcase::GetRltChild(int chld) {
    return rlt[chld].GetChild();
}
int cs_full_adultcase::GetRltRType(int chld) {
    return rlt[chld].GetRType();
}
int cs_full_adultcase::GetRltCarePercentage(int chld) {
    return rlt[chld].GetCarePercentage();
}
int cs_full_adultcase::GetRltCostPercentage(int chld) {
    return rlt[chld].GetCostPercentage();
}
int cs_full_adultcase::GetRltCostOfTheChild(int chld) {
    return rlt[chld].GetCostOfTheChild();
}
int cs_full_adultcase::GetRltMultiCaseCap(int chld) {
    return rlt[chld].GetMultiCaseCap();
}
int cs_full_adultcase::GetRltMultiCaseCost(int chld) {
    return rlt[chld].GetMultiCaseCost();
}
bool cs_full_adultcase::GetRltAge(int chld) {
    return rlt[chld].GetAge();
}
int cs_full_adultcase::GetMCMinorsCostPercentage(int chld) {
    return minormc[chld].GetCostPercentage();
}
int cs_full_adultcase::GetMCTeensCostPercentage(int chld) {
    return teenmc[chld].GetCostPercentage();
}
void cs_full_adultcase::AddRelevantDependantMinor() {
    rdc_minors+=1;
}
void cs_full_adultcase::AddRelevantDependantTeen() {
    rdc_teens+=1;
}
void cs_full_adultcase::AddMultiCaseMinor() {
    mc_minors+=1;
}
void cs_full_adultcase::AddMultiCaseTeen() {
    mc_teens+=1;
}
void cs_full_adultcase::SetRDCCost(int rdccost) {
    rdc_cost = rdccost;
    ati_less_rdc_cost = ati_less_ssa - rdccost;
    if (ati_less_rdc_cost < 0) ati_less_rdc_cost = 0;
}
void cs_full_adultcase::SetMCMinorsValues(int child, int cost, int cap) {
    minormc[child].SetMCValues(cost, cap);
}
void cs_full_adultcase::SetMCTeensValues(int child, int cost, int cap) {
    teenmc[child].SetMCValues(cost, cap);
}
void cs_full_adultcase::SetMCCost(int mccost) {
    mca = mccost;
    ati_less_mca = ati_less_rdc_cost - mccost;
    if (ati_less_mca < 0) ati_less_mca = 0;
    csi = ati_less_mca;
}
void cs_full_adultcase::SetMCCap(int cap) {
    mccap = cap;
}
void cs_full_adultcase::SetPIP(float p) {
    pip=p;
}
int cs_full_adultcase::GetMCA() {
    return mca;
}
int cs_full_adultcase::GetMCCap() {
    return mccap;
}
int cs_full_adultcase::GetATILessMCA() {
    return ati_less_mca;
}
int cs_full_adultcase::GetCSI() {
    return csi;
}
void cs_full_adultcase::SetRltPCSPC(int chld, float pcspc) {
    rlt[chld].SetPCSPC(pcspc);
}
float cs_full_adultcase::GetRltPCSPC(int chld) {
    return rlt[chld].GetPCSPC();
}
float cs_full_adultcase::GetRltEffPCSPC(int chld) {
    return rlt[chld].GetEffPCSPC();
}
void cs_full_adultcase::SetRltCOTC(int chld, int cotc) {
    rlt[chld].SetCOTC(cotc);
}
int cs_full_adultcase::GetRltCOTC(int chld) {
    return rlt[chld].GetCOTC();
}
void cs_full_adultcase::SetRltPreAdjCOTC(int chld, int pcotc) {
    rlt[chld].SetPreAdjCOTC(pcotc);
}
int cs_full_adultcase::GetRltPreAdjCOTC(int chld) {
    return rlt[chld].GetPreAdjCOTC();
}
void cs_full_adultcase::SetRltMCCap(int chld, int cap) {
    rlt[chld].SetMCCap(cap);
    mccap=mccap+cap;
}
int cs_full_adultcase::GetMCMinorsMCCost(int chld) {
    return minormc[chld].GetMCCost();
}
int cs_full_adultcase::GetMCTeensMCCost(int chld) {
    return teenmc[chld].GetMCCost();
}

std::string cs_full_adultcase::MCTeensDisplay(int chld) {
    return teenmc[chld].Display();
}
std::string cs_full_adultcase::MCMinorsDisplay(int chld) {
    return minormc[chld].Display();
}
void cs_full_adultcase::AddCSCost(float cost) {
    pays+=cost;
}
float cs_full_adultcase::GetPays() {
    return pays;
}
void cs_full_adultcase::AddCSGets(float g) {
    gets+=g;
}
float cs_full_adultcase::GetCSGets() {
    return gets;
}
void cs_full_adultcase::AddRltRcvTotCare(int chld, int care) {
    rlt[chld].AddRcvTotCare(care);
}
int cs_full_adultcase::GetRltRcvTotCare(int chld) {
    return rlt[chld].GetRcvTotCare();
}
std::string cs_full_adultcase::Display() {
    std::string r = "\n\tFULL ADULT CASE";
    r.append("\t\tAdults Name      =\t");
    r.append(aname);
    r.append("\n\t\tAdult Index                      =\t");
    r.append(NumberToString(adult));
    r.append("\n\t\tNumber of Relevant Dependants    =\t");
    r.append( NumberToString(rdc_teens + rdc_minors));
    r.append("\n\t\t                   Teens         =\t");
    r.append( NumberToString(rdc_teens));
    r.append("\n\t\t                   Minors        =\t");
    r.append( NumberToString(rdc_minors));
    r.append("\n\t\tNumber of Multi-Case Children    =\t");
    r.append( NumberToString(mc_teens + mc_minors));
    r.append("\n\t\t                   Teens         =\t");
    r.append( NumberToString(mc_teens));
    r.append("\n\t\t                   Minors        =\t");
    r.append( NumberToString(mc_minors));
    r.append("\n\t\tAdjusted Taxable Income          =\t$");
    r.append(NumberToString(ati));
    r.append("\n\t\tSelf Support Amount              =\t$");
    r.append( NumberToString(ssa));
    r.append("\n\t\tATI Less SSA                     =\t$");
    r.append(NumberToString(ati_less_ssa));
    r.append("\n\t\tRelevant Dependant Children Cost =\t$");
    r.append(NumberToString(rdc_cost));
    r.append("\n\t\tATI less RDC Amount              =\t$");
    r.append( NumberToString(ati_less_rdc_cost));
    r.append("\n\t\tMulti Case Allowance             =\t$");
    r.append( NumberToString(mca));
    r.append("\n\t\tATI Less Multi Case Allownace    =\t$");
    r.append( NumberToString(ati_less_mca));
    r.append("\n\t\tChild Support Income             =\t$");
    r.append( NumberToString(csi));
    r.append("\n\t\tParents Income Percentage        =\t");
    r.append( NumberToString(pip));
    r.append("%");
    r.append("\n\t\tOverall Child Support to pay     =\t$");
    float opays = pays-gets;
    if (opays < 0) opays = 0;
    r.append( NumberToString(Round2dp(opays,2)));
    r.append("\n\t\tOverall Child Support to receive =\t$");
    float ogets = gets-pays;
    if (ogets < 0) ogets = 0;
    r.append( NumberToString(Round2dp(ogets,2)));
    r.append("\n\t\tTotal CS Payments (not offset)   =\t$");
    r.append( NumberToString(Round2dp(pays,2)));
    r.append("\n\t\tTotal CS Receipts (not offset)   =\t$");
    r.append( NumberToString(Round2dp(gets,2)));
    for (int i=0; i < (signed)rlt.size(); i++) {
        r.append(rlt[i].Display());
    }
    r.append("\n");
    return r;
}

//===============================================================================================
// Class for a full case
// A full case an the overall case, an instance is built for all child cases that have the same
// parents.
//===============================================================================================
class cs_fullcase {
    public:
        cs_fullcase(int);
        ~cs_fullcase();
        void SetCostPerChild(float);
        float GetCostPerChild();
        void AddChildCase(int);
        int GetfccaseSize();
        int Getfccase(int);
        std::string GetfccaseChildName(int);
        void SetChildCounts(int, int);
        int GetfacaseSize();
        int GetfacaseAdult(int);
        void AddFullAdultCase(int, std::string, int, int);
        std::string GetfacaseAdultName(int);
        int GetfacaseATI(int);
        int GetfacaseSSA(int);
        int GetfacaseATILessSSA(int);
        int GetfacaseRDCCost(int);
        int GetfacaseATILessRDCCost(int);
        int GetfacaseRDCMinors(int);
        int GetfacaseRDCTeens(int);
        int GetfacaseMCMinors(int);
        int GetfacaseMCTeens(int);
        float GetfacasePIP(int);
        void SetfacasePIP(int, float);
        void AddFullRelationshipToAdult(int, int,std::string, int, int, int, int, int, bool);
        void AddMCTeenRltToAdult(int, int, std::string, int, int, int, int, int);
        void AddMCMinorRltToAdult(int, int, std::string, int, int, int, int, int);
        int GetAdultsRltSize(int);
        int GetAdultsMCMinorsSize(int);
        int GetAdultsMCTeensSize(int);
        int GetAdultsMCMinorsCostPercentage(int, int);
        int GetAdultsMCTeensCostPercentage(int, int);
        std::string GetAdultsRltChildsName(int, int);
        std::string GetAdultsMCTeensChildsName(int, int);
        std::string GetAdultsMCMinorsChildsName(int, int);
        int GetAdultsRltCare(int, int);
        int GetAdultsRltAdult(int, int);
        int GetAdultsRltChild(int, int);
        int GetAdultsRltRType(int, int);
        int GetAdultsRltCarePercentage(int, int);
        int GetAdultsRltCostPercentage(int, int);
        int GetAdultsRltPreAdjCostOfTheChild(int, int);
        int GetAdultsRltCostOfTheChild(int, int);
        int GetAdultsRltMultiCaseCap(int, int);
        int GetAdultsRltMultiCaseCost(int, int);
        bool GetAdultsRltAge(int, int);
        void AddRelevantDependantMinor(int);
        void AddRelevantDependantTeen(int);
        void AddMultiCaseMinor(int);
        void AddMultiCaseTeen(int);
        void SetfacaseRDCCost(int, int);
        void SetAdultsMCMinorsValues(int, int, int, int);
        void SetAdultsMCTeensValues(int, int, int, int);
        void SetfacaseMCCost(int, int);
        int GetfacaseMCA(int);
        int GetfacaseATILessMCA(int);
        int GetfacaseCSI(int);
        int GetfacaseMCCap(int);
        void IncreaseCombinedCSIncome(int amnt, cs_amounts);
        int GetCCSI();
        int GetCOC();
        void SetParentAbroad();
        void SetNonParentCarer();
        void SetAdultsRltMCCap(int, int, int);
        void SetAdultsRltPCSPC(int, int, float);
        float GetAdultsRltPCSPC(int, int);
        void SetAdultsRltPreAdjCOTC(int, int, int);
        void SetAdultsRltCOTC(int, int, int);
        int GetAdultsMCMinorsMCCost(int, int);
        int GetAdultsMCTeensMCCost(int, int);
        std::string AdultsMCTeensDisplay(int, int);
        std::string AdultsMCMinorsDisplay(int, int);
        bool IsParentAbroadApplicable();
        void AddfacaseCSCost(int, float);
        float GetfacasePays(int);
        void AddfacaseCSGets(int, float);
        float GetfacaseCSGets(int);
        void AddAdultsRltRcvTotCare(int, int, int);
        int GetAdultsRltRcvTotCare(int, int);
        void SetfacaseMCCap(int, int);
        std::string Display();
        friend class cs_full_adultcase;
        friend class cs_full_relationship;
        std::deque<int> fccase;
        std::deque<cs_full_adultcase> facase;
    private:
        bool splitcare;
        int ccsi;                           //combined child support income
        bool parent_abroad;
        bool non_parent_carer;
        int coc;                            // overall cost of children
        float costperchild;
        int cs_teens;                       // number of child support teenagers
        int cs_minors;                      // number of child support minors
};
cs_fullcase::cs_fullcase(int chldcase) {
    fccase.push_back(chldcase);
    splitcare=false;
    parent_abroad=false;
    non_parent_carer=false;
    ccsi=0;
    coc=0;
    cs_teens=0;
    cs_minors=0;
    costperchild=0;
}
cs_fullcase::~cs_fullcase() {
}
void cs_fullcase::SetCostPerChild(float cpc) {
    costperchild = cpc;
}
float cs_fullcase::GetCostPerChild() {
    return costperchild;
}
void cs_fullcase::AddChildCase(int cc) {
    fccase.push_back(cc);
}
int cs_fullcase::GetfccaseSize() {
    return fccase.size();
}
int cs_fullcase::Getfccase(int fc) {
    return fccase[fc];
}
void cs_fullcase::SetChildCounts(int minors, int teens) {
    cs_minors=minors;
    cs_teens=teens;
}
int cs_fullcase::GetfacaseSize() {
    return facase.size();
}
int cs_fullcase::GetfacaseAdult(int a) {
    return facase[a].GetAdult();
}
std::string cs_fullcase::GetfacaseAdultName(int a) {
    return facase[a].GetAdultName();
}
void cs_fullcase::AddFullAdultCase(int adult, std::string adultname, int ati, int ssa) {
    facase.push_back(cs_full_adultcase(adult,adultname,ati,ssa));
}
int cs_fullcase::GetfacaseATI(int adult) {
    return facase[adult].GetATI();
}
int cs_fullcase::GetfacaseSSA(int adult) {
    return facase[adult].GetSSA();
}
int cs_fullcase::GetfacaseATILessSSA(int adult) {
    return facase[adult].GetATILessSSA();
}
int cs_fullcase::GetfacaseRDCCost(int adult) {
    return facase[adult].GetRDCCost();
}
int cs_fullcase::GetfacaseATILessRDCCost(int adult) {
    return facase[adult].GetATILessRDCCost();
}
int cs_fullcase::GetfacaseRDCMinors(int adult) {
    return facase[adult].GetRDCMinors();
}
int cs_fullcase::GetfacaseRDCTeens(int adult) {
    return facase[adult].GetRDCTeens();
}
int cs_fullcase::GetfacaseMCMinors(int adult) {
    return facase[adult].GetMCMinors();
}
int cs_fullcase::GetfacaseMCTeens(int adult) {
    return facase[adult].GetMCTeens();
}
int cs_fullcase::GetfacaseMCA(int adult) {
    return facase[adult].GetMCA();
}
int cs_fullcase::GetfacaseATILessMCA(int adult) {
    return facase[adult].GetATILessMCA();
}
int cs_fullcase::GetfacaseCSI(int adult) {
    return facase[adult].GetCSI();
}
int cs_fullcase::GetfacaseMCCap(int adult) {
    return facase[adult].GetMCCap();
}
float cs_fullcase::GetfacasePIP(int adult) {
    return facase[adult].GetPIP();
}
void cs_fullcase::SetfacasePIP(int adult, float pip) {
    facase[adult].SetPIP(pip);
}
void cs_fullcase::AddFullRelationshipToAdult(int adult, int child, std::string childsname, int adlt,
        int rtype, int levelofcare, int carepc, int costpc, bool teen) {
    facase[adult].AddFullRelationship(child, childsname, adlt, rtype, levelofcare, carepc, costpc, teen);
}
void cs_fullcase::AddMCTeenRltToAdult(int adult, int child, std::string childsname, int adlt,
        int rtype, int levelofcare, int carepc, int costpc) {
    facase[adult].AddMCTeenRlt(child, childsname, adlt, rtype, levelofcare, carepc, costpc);
}
void cs_fullcase::AddMCMinorRltToAdult(int adult, int child, std::string childsname, int adlt,
        int rtype, int levelofcare, int carepc, int costpc) {
    facase[adult].AddMCMinorRlt(child, childsname, adlt, rtype, levelofcare, carepc, costpc);
}
int cs_fullcase::GetAdultsRltSize(int adult) {
    return facase[adult].GetRltSize();
}
int cs_fullcase::GetAdultsMCMinorsSize(int adult){
    return facase[adult].GetMCMinorsSize();
}
int cs_fullcase::GetAdultsMCTeensSize(int adult) {
    return facase[adult].GetMCTeensSize();
}
std::string cs_fullcase::GetAdultsRltChildsName(int adult, int child) {
    return facase[adult].GetRltChildsName(child);
}
std::string cs_fullcase::GetAdultsMCTeensChildsName(int adult, int child) {
    return facase[adult].GetMCTeensChildsName(child);
}
std::string cs_fullcase::GetAdultsMCMinorsChildsName(int adult, int child) {
    return facase[adult].GetMCMinorsChildsName(child);
}
int cs_fullcase::GetAdultsRltChild(int adult, int child) {
    return facase[adult].GetRltChild(child);
}
int cs_fullcase::GetAdultsRltAdult(int adult, int child) {
    return facase[adult].GetRltAdult(child);
}
int cs_fullcase::GetAdultsRltRType(int adult, int child) {
    return facase[adult].GetRltRType(child);
}
int cs_fullcase::GetAdultsRltCare(int adult, int child) {
    return facase[adult].GetRltCare(child);
}
int cs_fullcase::GetAdultsRltCarePercentage(int adult, int child) {
    return facase[adult].GetRltCarePercentage(child);
}
int cs_fullcase::GetAdultsRltCostPercentage(int adult, int child) {
    return facase[adult].GetRltCostPercentage(child);
}
int cs_fullcase::GetAdultsRltMultiCaseCap(int adult, int child) {
    return facase[adult].GetRltMultiCaseCap(child);
}
int cs_fullcase::GetAdultsRltMultiCaseCost(int adult, int child) {
    return facase[adult].GetRltMultiCaseCost(child);
}
int cs_fullcase::GetAdultsMCMinorsCostPercentage(int adult, int child) {
    return facase[adult].GetMCMinorsCostPercentage(child);
}
int cs_fullcase::GetAdultsMCTeensCostPercentage(int adult, int child) {
    return facase[adult].GetMCTeensCostPercentage(child);
}
int cs_fullcase::GetAdultsRltPreAdjCostOfTheChild(int adult, int child) {
    return facase[adult].GetRltPreAdjCOTC(child);
}
int cs_fullcase::GetAdultsRltCostOfTheChild(int adult, int child) {
    return facase[adult].GetRltCostOfTheChild(child);
}
bool cs_fullcase::GetAdultsRltAge(int adult, int child) {
    return facase[adult].GetRltAge(child);
}
void cs_fullcase::SetAdultsRltPCSPC(int adult, int child, float pcspc) {
    facase[adult].SetRltPCSPC(child, pcspc);
}
void cs_fullcase::SetAdultsRltPreAdjCOTC(int adult, int child, int pcotc) {
    facase[adult].SetRltPreAdjCOTC(child, pcotc);
}
void cs_fullcase::SetAdultsRltCOTC(int adult, int child, int cotc) {
    facase[adult].SetRltCOTC(child, cotc);
}
float cs_fullcase::GetAdultsRltPCSPC(int adult, int child) {
    return facase[adult].GetRltPCSPC(child);
}
void cs_fullcase::AddRelevantDependantMinor(int adult) {
    facase[adult].AddRelevantDependantMinor();
}
void cs_fullcase::AddRelevantDependantTeen(int adult) {
    facase[adult].AddRelevantDependantTeen();
}
void cs_fullcase::AddMultiCaseMinor(int adult) {
    facase[adult].AddMultiCaseMinor();
}
void cs_fullcase::AddMultiCaseTeen(int adult) {
    facase[adult].AddMultiCaseTeen();
}
void cs_fullcase::SetfacaseRDCCost(int adult, int rdccost) {
    facase[adult].SetRDCCost(rdccost);
}
void cs_fullcase::SetAdultsMCMinorsValues(int adult, int child, int cost, int cap) {
    facase[adult].SetMCMinorsValues(child, cost, cap);
}
void cs_fullcase::SetAdultsMCTeensValues(int adult, int child, int cost, int cap) {
    facase[adult].SetMCTeensValues(child, cost, cap);
}
void cs_fullcase::SetfacaseMCCost(int adult, int mccost) {
    facase[adult].SetMCCost(mccost);
}
void cs_fullcase::IncreaseCombinedCSIncome(int amnt, cs_amounts yr) {
    ccsi=ccsi+amnt;
    if (parent_abroad && non_parent_carer) ccsi = ccsi * 2;
    coc = Round2dp(yr.calccostofchild(cs_minors,cs_teens,ccsi),0);
}
int cs_fullcase::GetCCSI() {
    return ccsi;
}
int cs_fullcase::GetCOC() {
    return coc;
}
void cs_fullcase::SetParentAbroad() {
    parent_abroad=true;
}
void cs_fullcase::SetNonParentCarer() {
    non_parent_carer=true;
}
bool cs_fullcase::IsParentAbroadApplicable() {
    if (parent_abroad && non_parent_carer) return true;
    else return false;
}
void cs_fullcase::SetAdultsRltMCCap(int adult, int child, int cap) {
    facase[adult].SetRltMCCap(child, cap);
}
int cs_fullcase::GetAdultsMCMinorsMCCost(int adult, int child) {
    return facase[adult].GetMCMinorsMCCost(child);
}
int cs_fullcase::GetAdultsMCTeensMCCost(int adult, int child) {
    return facase[adult].GetMCTeensMCCost(child);
}
std::string cs_fullcase::AdultsMCTeensDisplay(int adult, int child) {
    return facase[adult].MCTeensDisplay(child);
}
std::string cs_fullcase::AdultsMCMinorsDisplay(int adult, int child) {
    return facase[adult].MCMinorsDisplay(child);
}
void cs_fullcase::AddfacaseCSCost(int adult, float cost) {
    facase[adult].AddCSCost(cost);
}
float cs_fullcase::GetfacasePays(int adult) {
    return facase[adult].GetPays();
}
void cs_fullcase::AddfacaseCSGets(int adult, float gets) {
    facase[adult].AddCSGets(gets);
}
float cs_fullcase::GetfacaseCSGets(int adult) {
    return facase[adult].GetCSGets();
}
void cs_fullcase::AddAdultsRltRcvTotCare(int adult, int child, int care) {
    facase[adult].AddRltRcvTotCare(child, care);
}
int cs_fullcase::GetAdultsRltRcvTotCare(int adult, int child) {
    return facase[adult].GetRltRcvTotCare(child);
}
void cs_fullcase::SetfacaseMCCap(int adult, int mccap) {
    facase[adult].SetMCCap(mccap);
}
std::string cs_fullcase::Display() {
    std::string r = "\nFULLCASE\n\tNumber of FullChild Case Entries =\t";
    r.append(NumberToString(fccase.size()));
    r.append("\n\tNumber of FullAdult Case Enrties =\t");
    r.append(NumberToString(facase.size()));
    r.append("\n\tNumber of Child Support Children =\t");
    r.append(NumberToString(cs_teens+cs_minors));
    r.append("\n\tNumber of Child Support Teens    =\t");
    r.append(NumberToString(cs_teens));
    r.append("\n\tNumber of Child Support Minors   =\t");
    r.append(NumberToString(cs_minors));
    r.append("\n\tCombined Child Support Income    =\t$");
    r.append(NumberToString(ccsi));
    r.append("\n\tCost of Children                 =\t$");
    r.append(NumberToString(coc));
    r.append("\n\tCost Per Child                   =\t$");
    r.append( NumberToString(costperchild));
    r.append("\n\tSplit Care Flag                  =\t");
    r.append(BoolToString(splitcare));
    r.append("\n\tParent Abroad Flag               =\t");
    r.append(BoolToString(parent_abroad));
    r.append("\n\tNon Parent Carer Flag            =\t");
    r.append(BoolToString(non_parent_carer));
    r.append("\n");
    for (int f=0; f < GetfacaseSize(); f ++) {
        r.append(facase[f].Display());
    }
    return r;
}
#endif
