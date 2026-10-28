# Login System and Personal Organizer

A Windows console application written in C++. It started as an account login program and now includes a small set of personal tools that are available after sign-in.

## Features

- Sign up with a unique username, a password, and an account recovery code.
- Sign in with a maximum of three password attempts before recovery is offered.
- Change the password, username, or recovery code from the account modification menu.
- Add, edit, search, and delete tasks, notes, expenses, and contacts.
- Set task priority, due date, and status; view a task summary.
- Categorize expenses and view spending summaries.
- Review recent account activity.
- Store the data in local files in the directory from which the program is run.

## Requirements

- Windows
- MinGW g++ with C++17 support

The program uses Windows console headers such as `conio.h` and `windows.h`. It does not require CMake, OpenSSL, or the Windows bcrypt library.

## Build and run

Open PowerShell in this project directory and run:

```powershell
g++ -std=c++17 LOGIN_SYSTEM.cpp -o LOGIN_SYSTEM.exe
.\LOGIN_SYSTEM.exe
```

Run the application from this directory so the account and organizer data files are created here. The source is a single C++ file, so do not use the older multi-file command that refers to `src/main.cpp` or `include`.

## Main menu

```text
=========== LOGIN SYSTEM MENU ===========
1. Sign Up
2. Login
3. Exit
=========================================
```

After login, the user panel provides account details and modification, the task manager, personal notes, the expense tracker, the contact book, recent activity, and sign out.

## Data files

The files below are created as the features are used. They are local application data, not source code.

| File | Purpose | Format |
| --- | --- | --- |
| `users.dat` | Account username, encoded password values, recovery code, and creation date | Binary stream of length-prefixed strings |
| `tasks.txt` | User tasks, status, priority, due date, and creation date | Pipe-separated text records |
| `notes.txt` | User note titles, content, and creation date | Pipe-separated text records |
| `expenses.txt` | User expenses, category, amount, description, and date | Pipe-separated text records |
| `contacts.txt` | User contact name, phone, email, group, and creation date | Pipe-separated text records |
| `activity.log` | Timestamped account and organizer actions | Pipe-separated text records |

Each organizer record includes its owner's username. When a username is changed, the application attempts to update that owner field in the organizer files and activity log.

## Code organization

The project keeps the beginner-friendly single-file structure. The main classes are:

- `Password` reads a masked password, checks the required character groups, and compares the stored representation.
- `User` keeps account details and the password helper.
- `FileHandler` reads and writes account records in `users.dat`.
- `ProjectTools` contains shared input, text, date, and console helpers.
- `ActivityLog` records and reads recent actions.
- `TaskHandler`, `NoteHandler`, `ExpenseHandler`, and `ContactHandler` implement the organizer menus and file operations.
- `Modify`, `UserPanel`, and `LoginSystem` control the account menus and program flow.

## Security note

This is a learning project and is not suitable for real accounts or sensitive data. The password is transformed into numeric values using a reversible arithmetic formula. That is not encryption or password hashing. The recovery code is also stored as ordinary account data. The project has no database, salted password hash, encryption-at-rest, lockout timer, or multi-user concurrency protection. Use test data only.

## Documentation

- [Interview questions and answers](INTERVIEW_QUESTIONS.md)
- [Project walkthrough guide](PROJECT_GUIDE.pdf)

## Runtime files and Git

The `.gitignore` file excludes the executable and local account/activity/organizer data so those files are not accidentally committed. Keep backups of your local data separately if you need them.
