#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
using namespace std;

// ==================== Utility Functions ====================

// Check if Gregorian year is leap
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
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

        // Print days
        for(int day = 1; day <= daysInMonth; day++) {
            int julian = JULIAN(year, month, day);
            julianToEthiopian(julian, ethYear, ethMonth, ethDay);

            cout << setw(2) << setfill('0') << day << "/" 
                 << setw(2) << setfill('0') << ethDay << "  ";

            if((firstDayOfWeek + day) % 7 == 0) cout << "\n";
        }
        cout << "\n------------------------------------------------\n\n";
    }
}

void printEthiopianToGregorian(int year) {
    cout << "\n==================== Ethiopian Year: " << year << " ====================\n\n";

    for(int month = 1; month <= 13; month++) {
        int daysInMonth = (month == 13) ? 5 : 30; // Pagume default 5, leap handled in Julian
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

        for(int day = 1; day <= daysInMonth; day++) {
            int jdDay = ethiopianToJulian(year, month, day);
            int gD, gM, gY;
            // Reverse Julian
            int l = jdDay + 68569;
            int n = (4 * l)/146097;
            l = l - (146097 * n + 3)/4;
            int i = (4000 * (l + 1))/1461001;
            l = l - (1461 * i)/4 + 31;
            int j = (80 * l)/2447;
            gD = l - (2447 * j)/80;
            l = j/11;
            gM = j + 2 - (12 * l);
            gY = 100 * (n - 49) + i + l;

            cout << setw(2) << setfill('0') << gD << "/" 
                 << setw(2) << setfill('0') << gM << "  ";

            if((firstDayOfWeek + day) % 7 == 0) cout << "\n";
        }

        cout << "\n------------------------------------------------\n\n";
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

    int birthJD = ethiopianToJulian(birthY, birthM, birthD);
    int diffDays = currJD - birthJD;

    int years = diffDays / 365;
    int months = (diffDays % 365) / 30;
    int days = (diffDays % 365) % 30;

    cout << "Your age is approximately: " << years << " years, " << months << " months, " << days << " days\n";
}

// ==================== Main Menu ====================
int main() {
    char mainChoice, subChoice;
    do {
        cout << "\n====== Calendar & Age Menu ======\n";
        cout << "1) Gregorian ? Ethiopian calendar\n";
        cout << "2) Ethiopian ? Gregorian calendar\n";
        cout << "3) Calculate current age\n";
        cout << "4) Exit\n";
        cout << "Enter choice: ";
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
                cout << "a) Calculate age from Gregorian birthdate\n";
                cout << "b) Calculate age from Ethiopian birthdate\n";
                cout << "Enter choice: ";
                cin >> subChoice;
                if(subChoice == 'a' || subChoice == 'A') calculateAgeFromGregorian();
                else if(subChoice == 'b' || subChoice == 'B') calculateAgeFromEthiopian();
                else cout << "Invalid choice!\n";
                break;
            }
            case '4': cout << "Exiting program...\n"; break;
            default: cout << "Invalid choice!\n"; break;
        }

    } while(mainChoice != '4');

    return 0;
}
