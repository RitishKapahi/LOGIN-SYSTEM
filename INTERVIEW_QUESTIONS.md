# Interview Questions and Answers

These answers describe the project as it is currently implemented. The security limitations are stated directly so the project can be explained accurately in an interview.

## Project overview

### 1. What does this project do?

It is a Windows console application with account sign-up, sign-in, password recovery, and several personal organizer tools: tasks, notes, expenses, contacts, and recent activity.

### 2. What happens when the program starts?

`main()` creates a `LoginSystem` object and calls its main menu. The user can sign up, log in, or exit. A successful login opens the user panel.

### 3. What features are available after login?

The user can view account details, change account information, manage tasks, notes, expenses, and contacts, review recent activity, or sign out.

## C++ and design

### 4. Why did you use classes?

Classes group data and related operations. For example, `User` stores account details, while `FileHandler` handles account file operations. Separate handler classes keep each organizer feature understandable.

### 5. What is the role of `LoginSystem`?

It controls the top-level menu, sign-up, sign-in, and recovery flow. The feature-specific classes are called when the user selects an option.

### 6. What is the role of `UserPanel`?

It shows the menu available after authentication and routes each selection to the matching account or organizer feature.

### 7. Why are vectors used?

The handlers load records into `vector` objects so they can search, edit, delete, summarize, and then save the records back to a text file.

### 8. How is menu input checked?

`ProjectTools::AskNumber` reads a full line, parses one integer, rejects extra characters, and checks that the value is within the menu's allowed range.

### 9. Why does password entry show asterisks?

`Password::PasswordInput` reads characters with `getch()` and prints an asterisk for each character, so the password itself is not echoed to the console.

## Accounts and files

### 10. What are the password rules?

The password must contain 8 to 100 characters, with at least one uppercase letter, one lowercase letter, one digit, and one special character.

### 11. How are usernames checked?

They must contain 3 to 32 letters, digits, dots, dashes, or underscores. The account lookup compares usernames without regard to letter case.

### 12. How are account records stored?

`FileHandler` writes four length-prefixed strings to the binary file `users.dat`: username, encoded password values, recovery code, and creation date.

### 13. Is the password securely hashed?

No. The current code applies a reversible arithmetic transformation and stores numeric values. It is not a cryptographic hash, and there is no salt. This is a major limitation and the program should only use test data.

### 14. How does account recovery work?

After three failed password attempts, the program asks for the recovery code. If it matches, the user can enter the account or change the password.

### 15. What happens when the username changes?

The account record is updated, and `WorkspaceData` attempts to replace the old owner name in the task, note, expense, contact, and activity files.

## Organizer features

### 16. What can a task record contain?

A task has an ID, owner, title, description, due date, priority, status, and creation date. Statuses are Open, In Progress, and Done; priorities are Low, Normal, and High.

### 17. How are notes, expenses, and contacts different?

Notes store a title and one-line content. Expenses store a category, amount, description, and date. Contacts store a name, phone, email, group, and creation date.

### 18. How are records kept separate between users?

Each record stores a username. When a handler displays or changes data, it selects records belonging to the logged-in username.

### 19. What does the activity log contain?

It appends a date, username, and action to `activity.log`. The account panel reads the latest entries for the current user.

### 20. Why are organizer records pipe-separated?

The format is easy to read and write with standard file streams. The program replaces or rejects pipe and newline characters in fields because those characters separate records and fields.

## Limitations and next steps

### 21. Why use text files instead of a database?

Text files keep this learning project simple and make the storage code visible. A database would be a better choice for concurrent users, transactions, larger datasets, and reliable querying.

### 22. What are the main security and reliability limitations?

The password representation is reversible, the recovery code is stored without hashing, and there is no encryption, database transaction, or multi-user locking. File write failures and corrupted records also need more extensive handling for a production system.

### 23. How would you improve password storage?

Use a maintained password-hashing library with a salt and a suitable password-hashing algorithm. Do not write a custom password-hashing algorithm.

### 24. How would you test the project?

Check valid and invalid sign-up, duplicate usernames with different letter case, correct and incorrect passwords, the three-attempt recovery flow, each feature's add/edit/search/delete operations, and invalid menu input. Also test missing and malformed data files. The current project does not include an automated test suite.

## Short project explanation

"I built a single-file C++ console application that combines account sign-up and sign-in with a small personal organizer. I used classes to separate account handling, file operations, shared console input, and each organizer feature. Account data is stored in a binary file, while organizer records are kept in pipe-separated text files. I understand the password transformation is not secure hashing, so the application is for learning and demonstration with test data only."
