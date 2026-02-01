# Ethiopian ↔ Gregorian Calendar & Age Calculator (C++)

A menu-driven C++ application that converts between the Gregorian and
Ethiopian calendars and calculates age using either system.

This project demonstrates: - Calendar conversion algorithms - Julian Day
Number (JDN) usage - Date calculations - Menu-driven program design -
Real-world time and age computation

------------------------------------------------------------------------

## Features

### 1. Gregorian → Ethiopian Calendar

-   Displays a full Gregorian year
-   Shows corresponding Ethiopian date for each day
-   Correct weekday alignment
-   Handles leap years

### 2. Ethiopian → Gregorian Calendar

-   Displays Ethiopian months (1--13)
-   Shows corresponding Gregorian dates
-   Includes Pagume (13th month)

### 3. Age Calculator

Submenu options: - Calculate age from Gregorian birthdate - Calculate
age from Ethiopian birthdate

Outputs: - Years - Months - Days

### Core Concepts Used

-   Julian Day Number conversion
-   Leap year rules (Gregorian & Ethiopian)
-   Date arithmetic
-   Modular programming (functions)
-   Input validation
-   Menu-driven interface

------------------------------------------------------------------------

## Technologies Used

-   Language: C++
-   Libraries:
    -   iostream
    -   iomanip
    -   ctime
    -   string

------------------------------------------------------------------------

## How to Compile and Run

### Step 1 --- Compile

g++ main.cpp -o calendar

### Step 2 --- Run

./calendar

(Windows users) calendar.exe

------------------------------------------------------------------------

## Example Menu

====== Calendar & Age Menu ====== 1) Gregorian → Ethiopian calendar 2)
Ethiopian → Gregorian calendar 3) Calculate current age 4) Exit

------------------------------------------------------------------------

## How It Works (Technical Overview)

This program converts dates using a two-step method:

Calendar A → Julian Day Number → Calendar B

Julian Day is a continuous day count, making cross-calendar conversion
accurate.

------------------------------------------------------------------------

## Future Improvements (Planned)

-   Highlight current date in calendars
-   Save calendar output to file
-   Graphical interface (GUI)
-   Holiday marking (Ethiopian & international)
-   Support for more calendar systems

------------------------------------------------------------------------

## Author

Anan
