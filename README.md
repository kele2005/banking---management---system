# Multi-Branch Banking Management System

A console-based banking management system built in C++, simulating multi-branch operations for tellers and customers — account creation, transactions, and reporting.

## Overview

This project models how a retail bank might handle day-to-day branch operations from the command line: tellers register and manage customers, customers log in to their own accounts to transact, and branch managers can pull reports across the system.

## Features

- **Teller operations**: register new customers, look up and search accounts, process transactions, apply interest, and branch-restricted access (tellers only manage accounts within their own branch)
- **Customer accounts**: four account types (Savings, Cheque, Fixed Deposit, Student) with type-specific minimum deposits and interest rates
- **Customer self-service**: balance check, deposit, withdraw, transfer between accounts, view statement, change PIN
- **Security**: PIN-protected login with account lockout after repeated failed attempts, encrypted credential storage, auto-generated account numbers and PINs
- **Reporting**: daily transaction reports, customer account summaries, branch performance reports, and CSV export of transactions/customers
- **Persistence**: account, transaction, and branch data stored to disk between sessions, with automatic backups

## Tech Stack

- **Language**: C++17
- **Structure**: Object-oriented — separate classes for accounts, tellers, branches, transactions, and customer management, coordinated through a menu-driven controller
- **Storage**: Flat binary/text data files (no external database or libraries required)

## Getting Started

```bash
# Linux / macOS
g++ -std=c++17 -Wall -o banking src/main.cpp src/utils.cpp src/branch.cpp \
    src/teller.cpp src/account.cpp src/transaction.cpp \
    src/customer_manager.cpp src/menu.cpp -I include
./banking
```

```powershell
# Windows (MinGW)
g++ -std=c++17 -Wall -o banking.exe src/main.cpp src/utils.cpp src/branch.cpp ^
    src/teller.cpp src/account.cpp src/transaction.cpp ^
    src/customer_manager.cpp src/menu.cpp -I include
banking.exe
```

Data directories (`data/`, `data/backup/`, `exports/`) are created automatically on first run.

### Demo teller login

| Teller ID | Password | Branch |
|---|---|---|
| T001 | teller123 | Johannesburg |
| T002 | teller456 | Cape Town |
| T003 | teller789 | Durban |

## What I learned

Building this reinforced object-oriented design in C++ (separating concerns across account, teller, transaction, and branch classes), file-based data persistence, input validation, and thinking through basic security practices like credential encryption and account lockouts.

## Possible extensions

- Replace flat-file storage with a proper database (SQLite/MySQL)
- Add unit tests for transaction and validation logic
- Build a simple web or GUI front end on top of the existing core logic
