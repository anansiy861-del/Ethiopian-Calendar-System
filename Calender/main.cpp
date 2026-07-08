#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
using namespace std;

// ==================== Utility Functions ====================

// Check if Gregorian year is leap
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Check if Ethiopian year is leap
bool isEthiopianLeapYear(int year) {
    return (year % 4 == 3);
}

// Days in Gregorian month
int getDaysInMonth(int month, int year) {
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
        case 4: case 6: case 9: case 11: return 30;
        case 2: return isLeapYear(year) ? 29 : 28;
        default: return -1;
    }
}

// Julian Day Number calculation
int JULIAN(int year, int month, int day) {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153*m + 2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;
}

// Convert Julian Day ? Ethiopian date
void julianToEthiopian(int julian, int &ethiopianYear, int &ethiopianMonth, int &ethiopianDay) {
    int r = (julian - 1723856) % 1461;
    int n = r % 365 + 365 * (r / 1460);
    ethiopianYear = 4 * ((julian - 1723856)/1461) + (r / 365) - (r / 1460);
    if(n < 360) { // Months 1-12
        ethiopianMonth = n / 30 + 1;
        ethiopianDay = n % 30 + 1;
    } else { // Pagume
        ethiopianMonth = 13;
        ethiopianDay = n - 360 + 1;
    }
}

// Convert Ethiopian date ? Julian Day
int ethiopianToJulian(int year, int month, int day) {
    int jd = 1723856 + 365 * year + year/4;
    if(month <= 12) jd += 30*(month - 1) + (day - 1);
    else jd += 360 + (day - 1); // Pagume
    return jd;
}

// Day of week from Julian (0=Sun,6=Sat)
int dayOfWeek(int julian) {
    return (julian + 1) % 7;
}

// Ethiopian month names
string getEthiopianMonthName(int month) {
    switch(month) {
        case 1: return "Meskerem";
        case 2: return "Tikimt";
        case 3: return "Hidar";
        case 4: return "Tahsas";
        case 5: return "Tir";
        case 6: return "Yekatit";
        case 7: return "Megabit";
        case 8: return "Miazia";
        case 9: return "Ginbot";
        case 10: return "Sene";
        case 11: return "Hamle";
        case 12: return "Nehase";
        case 13: return "Pagume";
        default: return "";
    }
}

// Get the holiday name for a given Julian Day Number (JDN) and Gregorian year
string getHolidayName(int jdn, int gYear) {
    // 1. Calculate Orthodox Easter (Fasika) JDN for this Gregorian year
    int a = gYear % 4;
    int b = gYear % 7;
    int c = gYear % 19;
    int d = (19 * c + 15) % 30;
    int e = (2 * a + 4 * b - d + 34) % 7;
    int easterMonth = (d + e + 114) / 31;
    int easterDay = ((d + e + 114) % 31) + 1;
    
    int julianYear = gYear;
    int m = easterMonth;
    if (m <= 2) {
        julianYear--;
        m += 12;
    }
    int fasikaJDN = 1721117 + easterDay + (153 * (m - 3) + 2) / 5 + 365 * julianYear + julianYear / 4;
    
    // Check Orthodox movable holidays
    if (jdn == fasikaJDN) return "Fasika (Ethiopian Easter)";
    if (jdn == fasikaJDN - 2) return "Siklet (Good Friday)";
    if (jdn == fasikaJDN - 7) return "Hosanna (Palm Sunday)";
    if (jdn == fasikaJDN - 55) return "Start of Great Lent (Hudade)";
    if (jdn == fasikaJDN - 69) return "Fast of Nineveh (Tsome Nenewe)";
    if (jdn == fasikaJDN + 40) return "Erget (Ascension)";
    if (jdn == fasikaJDN + 50) return "Pentecost (Paracletus)";
    if (jdn == fasikaJDN + 51) return "Start of Fast of the Apostles (Tsome Hawariat)";
    
    // 2. Check Ethiopian fixed holidays
    int ethYear, ethMonth, ethDay;
    julianToEthiopian(jdn, ethYear, ethMonth, ethDay);
    
    if (ethMonth == 1 && ethDay == 1) return "Enkutatash (Ethiopian New Year)";
    if (ethMonth == 1 && ethDay == 17) return "Meskel (Finding of the True Cross)";
    // Genna: Tahsas 29 in common years, Tahsas 28 in the year after an Ethiopian leap year (ethYear % 4 == 0)
    if ((ethYear % 4 == 0 && ethMonth == 4 && ethDay == 28) ||
        (ethYear % 4 != 0 && ethMonth == 4 && ethDay == 29)) {
        return "Genna (Ethiopian Christmas)";
    }
    if (ethMonth == 5 && ethDay == 11) return "Timket (Epiphany)";
    if (ethMonth == 6 && ethDay == 23) return "Adwa Victory Day";
    if (ethMonth == 8 && ethDay == 23) return "International Labour Day";
    if (ethMonth == 8 && ethDay == 27) return "Patriots' Victory Day";
    if (ethMonth == 9 && ethDay == 20) return "Downfall of the Derg (National Day)";
    
    // 3. Check Islamic movable holidays (using Tabular Hijri approximation)
    long z = jdn - 1948440;
    long cycle = z / 10631;
    z %= 10631;
    if (z < 0) {
        z += 10631;
        cycle--;
    }
    long year_in_cycle = (30 * z + 29) / 10631;
    z -= (10631 * year_in_cycle + 3) / 30;
    long day_of_year = z + 1;
    int hMonth = (day_of_year + 28.5001) / 29.5;
    if (hMonth > 12) hMonth = 12;
    int hDay = day_of_year - (int)(29.5001 * hMonth - 29);
    
    if (hMonth == 10 && hDay == 1) return "Eid al-Fitr (approx.)";
    if (hMonth == 12 && hDay == 10) return "Eid al-Adha (approx.)";
    if (hMonth == 3 && hDay == 12) return "Mawlid (Prophet's Birthday) (approx.)";
    
    return "";
}

// ==================== Calendar Printing ====================
void printGregorianToEthiopian(int year) {
    cout << "\n==================== Gregorian Year: " << year << " ====================\n\n";

    for(int month = 1; month <= 12; month++) {
        int daysInMonth = getDaysInMonth(month, year);
        int julianFirstDay = JULIAN(year, month, 1);
        int firstDayOfWeek = dayOfWeek(julianFirstDay);

        int ethYear, ethMonth, ethDay;
        julianToEthiopian(julianFirstDay, ethYear, ethMonth, ethDay);

        // Header
        cout << "-------------------- Month " << month << " --------------------\n";
        cout << "Gregorian Month " << month << " | Ethiopian Month: " 
             << getEthiopianMonthName(ethMonth) << " " << ethYear << "\n";
        cout << "Sun    Mon    Tue    Wed    Thu    Fri    Sat\n";

        // Leading spaces
        for(int i = 0; i < firstDayOfWeek; i++) cout << "       ";

        string holidays = "";
        // Print days
        for(int day = 1; day <= daysInMonth; day++) {
            int julian = JULIAN(year, month, day);
            julianToEthiopian(julian, ethYear, ethMonth, ethDay);

            cout << setw(2) << setfill('0') << day << "/" 
                 << setw(2) << setfill('0') << ethDay << "  ";

            string hol = getHolidayName(julian, year);
            if (!hol.empty()) {
                holidays += "  * Day " + to_string(day) + ": " + hol + "\n";
            }

            if((firstDayOfWeek + day) % 7 == 0) cout << "\n";
        }
        
        if((firstDayOfWeek + daysInMonth) % 7 != 0) cout << "\n";
        
        if (!holidays.empty()) {
            cout << "\nHolidays / Observances:\n" << holidays;
        }
        cout << "------------------------------------------------\n\n";
    }
}

void printEthiopianToGregorian(int year) {
    cout << "\n==================== Ethiopian Year: " << year << " ====================\n\n";

    for(int month = 1; month <= 13; month++) {
        int daysInMonth = (month == 13) ? (isEthiopianLeapYear(year) ? 6 : 5) : 30;
        int ethDay, ethMonth, ethYear;
        ethYear = year;
        ethMonth = month;
        ethDay = 1;

        int julianFirstDay = ethiopianToJulian(ethYear, ethMonth, ethDay);
        int firstDayOfWeek = dayOfWeek(julianFirstDay);

        // Header
        cout << "-------------------- Month " << month << " --------------------\n";
        cout << "Ethiopian Month " << getEthiopianMonthName(month) << " | Gregorian Year: ";
        
        int gYear, gMonth, gDay;
        gYear = gMonth = gDay = 0;
        // Gregorian date of first day
        int jd = ethiopianToJulian(ethYear, ethMonth, 1);
        // Reverse Julian to Gregorian
        int l = jd + 68569;
        int n = (4 * l)/146097;
        l = l - (146097 * n + 3)/4;
        int i = (4000 * (l + 1))/1461001;
        l = l - (1461 * i)/4 + 31;
        int j = (80 * l)/2447;
        gDay = l - (2447 * j)/80;
        l = j/11;
        gMonth = j + 2 - (12 * l);
        gYear = 100 * (n - 49) + i + l;
        cout << gYear << "\n";

        cout << "Sun    Mon    Tue    Wed    Thu    Fri    Sat\n";

        for(int i = 0; i < firstDayOfWeek; i++) cout << "       ";

        string holidays = "";
        for(int day = 1; day <= daysInMonth; day++) {
            int jdDay = ethiopianToJulian(year, month, day);
            int gD, gM, gY;
            // Reverse Julian
            int l2 = jdDay + 68569;
            int n2 = (4 * l2)/146097;
            l2 = l2 - (146097 * n2 + 3)/4;
            int i2 = (4000 * (l2 + 1))/1461001;
            l2 = l2 - (1461 * i2)/4 + 31;
            int j2 = (80 * l2)/2447;
            gD = l2 - (2447 * j2)/80;
            l2 = j2/11;
            gM = j2 + 2 - (12 * l2);
            gY = 100 * (n2 - 49) + i2 + l2;

            cout << setw(2) << setfill('0') << gD << "/" 
                 << setw(2) << setfill('0') << gM << "  ";

            string hol = getHolidayName(jdDay, gY);
            if (!hol.empty()) {
                holidays += "  * Day " + to_string(day) + ": " + hol + "\n";
            }

            if((firstDayOfWeek + day) % 7 == 0) cout << "\n";
        }

        if((firstDayOfWeek + daysInMonth) % 7 != 0) cout << "\n";

        if (!holidays.empty()) {
            cout << "\nHolidays / Observances:\n" << holidays;
        }
        cout << "------------------------------------------------\n\n";
    }
}

// ==================== Age Calculation ====================
void calculateAgeFromGregorian() {
    int birthY, birthM, birthD;
    cout << "Enter Gregorian birthdate (YYYY MM DD): ";
    cin >> birthY >> birthM >> birthD;

    time_t t = time(0);
    tm* now = localtime(&t);
    int currY = now->tm_year + 1900;
    int currM = now->tm_mon + 1;
    int currD = now->tm_mday;

    int ageY = currY - birthY;
    int ageM = currM - birthM;
    int ageD = currD - birthD;

    if(ageD < 0) { ageD += getDaysInMonth(currM-1 <= 0 ? 12 : currM-1, currY); ageM--; }
    if(ageM < 0) { ageM += 12; ageY--; }

    cout << "Your age is: " << ageY << " years, " << ageM << " months, " << ageD << " days\n";
}

void calculateAgeFromEthiopian() {
    int birthY, birthM, birthD;
    cout << "Enter Ethiopian birthdate (YYYY MM DD): ";
    cin >> birthY >> birthM >> birthD;

    time_t t = time(0);
    tm* now = localtime(&t);
    int currY = now->tm_year + 1900;
    int currM = now->tm_mon + 1;
    int currD = now->tm_mday;
    int currJD = JULIAN(currY, currM, currD);

    int currEthY, currEthM, currEthD;
    julianToEthiopian(currJD, currEthY, currEthM, currEthD);

    int ageY = currEthY - birthY;
    int ageM = currEthM - birthM;
    int ageD = currEthD - birthD;

    if (ageD < 0) {
        int prevM = currEthM - 1;
        int prevY = currEthY;
        if (prevM <= 0) {
            prevM = 13;
            prevY = currEthY - 1;
        }
        int daysInPrevMonth = (prevM == 13) ? (isEthiopianLeapYear(prevY) ? 6 : 5) : 30;
        ageD += daysInPrevMonth;
        ageM--;
    }
    if (ageM < 0) {
        ageM += 13;
        ageY--;
    }

    cout << "Your precise age is: " << ageY << " years, " << ageM << " months, " << ageD << " days\n";
}

// ==================== Ethiopian Time Converter ====================

// Convert Gregorian 24h time to Ethiopian time
// Ethiopian time starts at 06:00 Gregorian (12:00 Ethiopian morning)
void convertGregorianToEthiopianTime() {
    int gHour, gMin, gSec;
    cout << "\nEnter Gregorian time (HH MM SS): ";
    cin >> gHour >> gMin >> gSec;

    int ethHour = (gHour - 6 + 24) % 24;
    int half    = ethHour / 12; // 0 = day, 1 = night
    int dispHour = ethHour % 12;
    if (dispHour == 0) dispHour = 12;

    string period;
    if      (gHour >= 6  && gHour < 12) period = "Tewat (Morning)";
    else if (gHour >= 12 && gHour < 18) period = "Ken (Afternoon)";
    else if (gHour >= 18 && gHour < 24) period = "Mishit (Evening)";
    else                                period = "Leilit (Night)";

    cout << "\nGregorian time : "
         << setw(2) << setfill('0') << gHour << ":"
         << setw(2) << setfill('0') << gMin  << ":"
         << setw(2) << setfill('0') << gSec  << "\n";
    cout << "Ethiopian time : "
         << setw(2) << setfill('0') << dispHour << ":"
         << setw(2) << setfill('0') << gMin << ":"
         << setw(2) << setfill('0') << gSec
         << " - " << period << "\n";
}

// Live Ethiopian clock — updates every second, press any key to exit
void liveEthiopianClock() {
    cout << "\n  [Live Ethiopian Clock]  Press any key to return to menu...\n";
    cout << "  (Gregorian time shown for reference)\n";
    while (true) {
#ifdef _WIN32
        if (_kbhit()) { _getch(); break; }
#endif
        time_t t = time(0);
        tm* now  = localtime(&t);
        int gH = now->tm_hour, gM = now->tm_min, gS = now->tm_sec;

        int ethHour = (gH - 6 + 24) % 24;
        int dispH   = ethHour % 12;
        if (dispH == 0) dispH = 12;

        string period;
        if      (gH >= 6  && gH < 12) period = "Tewat (Morning)";
        else if (gH >= 12 && gH < 18) period = "Ken (Afternoon)";
        else if (gH >= 18 && gH < 24) period = "Mishit (Evening)";
        else                          period = "Leilit (Night)";

        // Get today's Ethiopian date
        int jdn = JULIAN(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
        int eY, eM, eD;
        julianToEthiopian(jdn, eY, eM, eD);

        cout << "\r  "
             << "ETH: " << setw(2) << setfill('0') << dispH  << ":"
                        << setw(2) << setfill('0') << gM      << ":"
                        << setw(2) << setfill('0') << gS      << " " << period
             << "  |  Date: " << getEthiopianMonthName(eM) << " " << eD << ", " << eY << " EC"
             << "  |  GRE: "  << setw(2) << setfill('0') << gH << ":"
                               << setw(2) << setfill('0') << gM << ":"
                               << setw(2) << setfill('0') << gS
             << "   " << flush;
#ifdef _WIN32
        Sleep(1000);
#endif
    }
    cout << "\n";
}

void timeMenu() {
    char ch;
    cout << "\n====== Ethiopian Time ======\n";
    cout << "a) Convert a specific Gregorian time\n";
    cout << "b) Live Ethiopian Clock\n";
    cout << "Enter choice: ";
    cin >> ch;
    if      (ch == 'a' || ch == 'A') convertGregorianToEthiopianTime();
    else if (ch == 'b' || ch == 'B') liveEthiopianClock();
    else cout << "Invalid choice!\n";
}

// ==================== Liturgical Fasting Planner ====================

// Helper: convert JDN to Gregorian string
string jdnToGregStr(int jdn) {
    int l = jdn + 68569;
    int n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    int i = (4000 * (l + 1)) / 1461001;
    l = l - (1461 * i) / 4 + 31;
    int j = (80 * l) / 2447;
    int gD = l - (2447 * j) / 80;
    l = j / 11;
    int gM = j + 2 - 12 * l;
    int gY = 100 * (n - 49) + i + l;
    string months[] = {"","Jan","Feb","Mar","Apr","May","Jun",
                           "Jul","Aug","Sep","Oct","Nov","Dec"};
    return to_string(gD) + " " + months[gM] + " " + to_string(gY);
}

// Helper: convert JDN to Ethiopian string
string jdnToEthStr(int jdn) {
    int eY, eM, eD;
    julianToEthiopian(jdn, eY, eM, eD);
    return getEthiopianMonthName(eM) + " " + to_string(eD) + ", " + to_string(eY) + " EC";
}

void printFastingPlanner(int ethYear) {
    // Compute Easter (Fasika) JDN for the corresponding Gregorian year
    int gYear = ethYear + 8;
    int a = gYear % 4;
    int b = gYear % 7;
    int c = gYear % 19;
    int d = (19 * c + 15) % 30;
    int e = (2 * a + 4 * b - d + 34) % 7;
    int easterMonth = (d + e + 114) / 31;
    int easterDay   = ((d + e + 114) % 31) + 1;
    int julianYear  = gYear;
    int m = easterMonth;
    if (m <= 2) { julianYear--; m += 12; }
    int fasikaJDN = 1721117 + easterDay + (153 * (m - 3) + 2) / 5
                  + 365 * julianYear + julianYear / 4;

    // Fixed fasts (Ethiopian calendar dates converted to JDN)
    int prophetsStartJDN = ethiopianToJulian(ethYear,     3, 15); // Hidar 15
    int prophetsEndJDN   = isEthiopianLeapYear(ethYear)
                         ? ethiopianToJulian(ethYear, 4, 28)      // Tahsas 28 (leap)
                         : ethiopianToJulian(ethYear, 4, 29);     // Tahsas 29
    int assumptionStartJDN = ethiopianToJulian(ethYear,  12,  1); // Nehase 1
    int assumptionEndJDN   = ethiopianToJulian(ethYear,  12, 15); // Nehase 15
    int apostlesEndJDN     = ethiopianToJulian(ethYear,  11,  5); // Hamle 5

    cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    cout <<   "║   Ethiopian Orthodox Liturgical Fasting Calendar — " << ethYear << " EC   ║\n";
    cout <<   "╚══════════════════════════════════════════════════════════════╝\n\n";

    // Movable fasts (relative to Fasika)
    struct FastEntry { string name; int startJDN; int endJDN; string rule; string diet; };
    FastEntry fasts[] = {
        {"1. Fast of Nineveh (Tsome Nenewe)",
          fasikaJDN - 69, fasikaJDN - 67,
          "3 days, starts 69 days before Easter",
          "Vegan. Fast broken in the afternoon (3PM Gregorian / 9 Ethiopian)."},
        {"2. Great Lent / Hudade (Tsome Hudade)",
          fasikaJDN - 55, fasikaJDN - 1,
          "55 days, starts 55 days before Easter",
          "Vegan. Fast broken after Liturgy (around 3PM / 9 Ethiopian)."},
        {"3. Holy Week (Hemamat)",
          fasikaJDN - 7,  fasikaJDN - 1,
          "Final 7 days of Great Lent",
          "Strictest fast. No food until dusk. Animal products forbidden."},
        {"4. Fasika (Easter) — End of Great Lent",
          fasikaJDN, fasikaJDN,
          "Feast day — end of the 55-day fast",
          "Meat, dairy, and eggs permitted after midnight liturgy."},
        {"5. Fast of the Apostles (Tsome Hawariat)",
          fasikaJDN + 51, apostlesEndJDN,
          "Starts Monday after Pentecost; ends Hamle 5",
          "Vegan. Broken in the afternoon."},
        {"6. Fast of the Prophets (Tsome Nebiyat)",
          prophetsStartJDN, prophetsEndJDN,
          "Hidar 15 to Tahsas 28/29 (43 days)",
          "Vegan Mon-Fri. Fish allowed Sat-Sun. Broken in the afternoon."},
        {"7. Fast of the Assumption (Tsome Filseta)",
          assumptionStartJDN, assumptionEndJDN,
          "Nehase 1-15 (15 days)",
          "Vegan. Broken in the afternoon."},
    };

    for (auto& f : fasts) {
        int duration = f.endJDN - f.startJDN + 1;
        cout << "  " << f.name << "\n";
        cout << "    Rule     : " << f.rule << "\n";
        cout << "    Start    : " << jdnToEthStr(f.startJDN) << "  (" << jdnToGregStr(f.startJDN) << ")\n";
        cout << "    End      : " << jdnToEthStr(f.endJDN)   << "  (" << jdnToGregStr(f.endJDN)   << ")\n";
        if (duration > 1)
            cout << "    Duration : " << duration << " days\n";
        cout << "    Diet     : " << f.diet << "\n";
        cout << "\n";
    }

    cout << "  Weekly Fasts (all year): Every Wednesday & Friday\n";
    cout << "    Diet: Vegan. Fast broken in the afternoon.\n";
    cout << "\n  Note: Islamic holidays vary by moon sighting and are not included here.\n";
    cout << "──────────────────────────────────────────────────────────────\n";
}

// ==================== Bahre Hasab Solver ====================

void solveBahreHasab(int ethYear) {
    cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    cout <<   "║   Interactive Bahre Hasab (Ancient Computus) Solver          ║\n";
    cout <<   "║   Ethiopian Year: " << ethYear << " EC                                   ║\n";
    cout <<   "╚══════════════════════════════════════════════════════════════╝\n\n";

    // Step 1: Amete Alem
    int ameteAlem = ethYear + 5500;
    cout << "  STEP 1 — Amete Alem (Years Since Creation)\n";
    cout << "    Formula  : Amete Alem = Ethiopian Year + 5500\n";
    cout << "    Result   : " << ethYear << " + 5500 = " << ameteAlem << "\n\n";

    // Step 2: Wengelawi (Evangelist)
    string evangelists[] = {"Markos (Mark)", "Lukas (Luke)", "Yohannes (John)", "Matewos (Matthew)"};
    int wengelawi = ameteAlem % 4;
    cout << "  STEP 2 — Wengelawi (Evangelist / Year Patron)\n";
    cout << "    Formula  : Amete Alem mod 4 = " << ameteAlem << " mod 4 = " << wengelawi << "\n";
    cout << "    Result   : " << evangelists[wengelawi] << "\n\n";

    // Step 3: Wenber
    int rawWenber = ameteAlem % 19;
    int wenber    = (rawWenber == 0) ? 18 : rawWenber - 1;
    cout << "  STEP 3 — Wenber (Lunar Cycle Position)\n";
    cout << "    Formula  : (Amete Alem mod 19) - 1\n";
    cout << "    Calc     : " << ameteAlem << " mod 19 = " << rawWenber
         << "  →  Wenber = " << wenber << "\n";
    cout << "    (Rule: 'Give one to the era' — subtract 1 from the remainder)\n\n";

    // Step 4: Abekte and Metqe
    int abekte = (wenber * 11) % 30;
    int metqe  = 30 - abekte;
    cout << "  STEP 4 — Abekte (Lunar Lag) & Metqe (Moon Position)\n";
    cout << "    Abekte   = (Wenber x 11) mod 30 = (" << wenber << " x 11) mod 30 = " << abekte << "\n";
    cout << "    Metqe    = 30 - Abekte = 30 - " << abekte << " = " << metqe << "\n";
    cout << "    (Abekte + Metqe always equals 30)\n\n";

    // Step 5: Beale Metqe (month and weekday)
    string bealeMonth;
    int bealeDay;
    if (metqe > 14) {
        bealeMonth = "Meskerem";
        bealeDay   = metqe;
    } else {
        bealeMonth = "Tikimt";
        bealeDay   = metqe;
    }
    int bealeJDN = (metqe > 14)
        ? ethiopianToJulian(ethYear, 1, metqe)
        : ethiopianToJulian(ethYear, 2, metqe);
    int bealeWeekday = dayOfWeek(bealeJDN);
    string dayNames[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    cout << "  STEP 5 — Beale Metqe (Date of the Announcement)\n";
    cout << "    Rule     : If Metqe > 14 → falls in Meskerem; if ≤ 14 → falls in Tikimt\n";
    cout << "    Result   : " << bealeMonth << " " << bealeDay << ", " << ethYear << " EC"
         << "  (" << dayNames[bealeWeekday] << ")\n";
    cout << "    Gregorian: " << jdnToGregStr(bealeJDN) << "\n\n";

    // Step 6: Tewsak and Mebaja Hamer
    int tewsakTable[] = {7, 6, 5, 4, 3, 2, 8}; // Sun=0..Sat=6
    int tewsak      = tewsakTable[bealeWeekday];
    int mebajaRaw   = bealeDay + tewsak;
    int mebajaHamer = (mebajaRaw > 30) ? mebajaRaw - 30 : mebajaRaw;
    cout << "  STEP 6 — Mebaja Hamer (Foundation Number)\n";
    cout << "    Tewsak for " << dayNames[bealeWeekday] << " = " << tewsak << "\n";
    cout << "    Formula  : Beale Metqe day + Tewsak = " << bealeDay << " + " << tewsak
         << " = " << mebajaRaw;
    if (mebajaRaw > 30) cout << "  →  subtract 30  →  " << mebajaHamer;
    cout << "\n    Mebaja Hamer = " << mebajaHamer << "\n\n";

    // Step 7: Derive movable feasts from Mebaja Hamer
    // Nineveh starts on Mebaja Hamer of Tir
    int ninevehJDN    = ethiopianToJulian(ethYear, 5, mebajaHamer);
    int lentStartJDN  = ninevehJDN + 14;
    int fasikaJDN2    = ninevehJDN + 69;
    int ascensionJDN  = fasikaJDN2 + 40;
    int pentecostJDN  = fasikaJDN2 + 50;
    int apostlesEndJDN2 = ethiopianToJulian(ethYear, 11, 5); // Hamle 5

    cout << "  STEP 7 — Derived Movable Feasts\n";
    cout << "    ┌────────────────────────────────────────────────────────────┐\n";

    struct Feast { string name; int jdn; string rule; };
    Feast feasts[] = {
        {"Nineveh (Tsome Nenewe) Start",    ninevehJDN,    "Mebaja Hamer of Tir"},
        {"Great Lent (Hudade) Start",        lentStartJDN,  "Nineveh + 14 days"},
        {"Fasika (Easter)",                  fasikaJDN2,    "Nineveh + 69 days"},
        {"Erget (Ascension)",               ascensionJDN,  "Easter + 40 days"},
        {"Pentecost (Paracletus)",           pentecostJDN,  "Easter + 50 days"},
        {"End of Apostles' Fast (Hamle 5)",  apostlesEndJDN2, "Fixed — Hamle 5"},
    };
    for (auto& f : feasts) {
        cout << "    │  " << left << setw(36) << f.name
             << "  " << left << setw(30) << jdnToEthStr(f.jdn) << "│\n";
        cout << "    │    Rule: " << f.rule
             << "  →  Gregorian: " << jdnToGregStr(f.jdn)
             << setw(5) << " " << "│\n";
    }
    cout << "    └────────────────────────────────────────────────────────────┘\n\n";
    cout << "  ━━━ Traditional Bahre Hasab derivation complete for " << ethYear << " EC ━━━\n";
}

// ==================== Main Menu ====================
int main() {
    char mainChoice, subChoice;
    do {
        cout << "\n╔══════════════════════════════╗\n";
        cout <<   "║  Ethiopian Calendar System   ║\n";
        cout <<   "╚══════════════════════════════╝\n";
        cout << "  1) Gregorian → Ethiopian calendar\n";
        cout << "  2) Ethiopian → Gregorian calendar\n";
        cout << "  3) Calculate current age\n";
        cout << "  4) Ethiopian Time Visualizer & Converter\n";
        cout << "  5) Liturgical Fasting & Observance Planner\n";
        cout << "  6) Bahre Hasab (Ancient Computus) Solver\n";
        cout << "  7) Exit\n";
        cout << "  Enter choice: ";
        cin >> mainChoice;

        switch(mainChoice) {
            case '1': {
                int year;
                cout << "Enter Gregorian year: ";
                cin >> year;
                printGregorianToEthiopian(year);
                break;
            }
            case '2': {
                int year;
                cout << "Enter Ethiopian year: ";
                cin >> year;
                printEthiopianToGregorian(year);
                break;
            }
            case '3': {
                cout << "  a) Calculate age from Gregorian birthdate\n";
                cout << "  b) Calculate age from Ethiopian birthdate\n";
                cout << "  Enter choice: ";
                cin >> subChoice;
                if      (subChoice == 'a' || subChoice == 'A') calculateAgeFromGregorian();
                else if (subChoice == 'b' || subChoice == 'B') calculateAgeFromEthiopian();
                else cout << "Invalid choice!\n";
                break;
            }
            case '4': {
                timeMenu();
                break;
            }
            case '5': {
                int ethYear;
                cout << "Enter Ethiopian year: ";
                cin >> ethYear;
                printFastingPlanner(ethYear);
                break;
            }
            case '6': {
                int ethYear;
                cout << "Enter Ethiopian year: ";
                cin >> ethYear;
                solveBahreHasab(ethYear);
                break;
            }
            case '7': cout << "  Exiting... Selam!\n"; break;
            default:  cout << "  Invalid choice!\n"; break;
        }

    } while(mainChoice != '7');

    return 0;
}
