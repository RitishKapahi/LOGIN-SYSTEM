#include <iostream>
#include <string>
#include <conio.h>
#include <cctype>
#include <ctime>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <windows.h>

using namespace std;

class Password;
class User;
class FileHandler;
class Modify;
class UserPanel;
class LoginSystem;

class Password
{
    int EncryptedPassword[100];
    int PasswordLength;

    bool Uppercase_check(char ch)
    {
        return ch >= 'A' && ch <= 'Z';
    }

    bool Lowercase_check(char ch)
    {
        return ch >= 'a' && ch <= 'z';
    }

    bool Digit_check(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    bool Special_check(char ch)
    {
        return !Uppercase_check(ch) && !Lowercase_check(ch) && !Digit_check(ch);
    }

public:
    Password()
    {
        PasswordLength = 0;

        for (int i = 0; i < 100; i++)
        {
            EncryptedPassword[i] = 0;
        }
    }

    string PasswordInput()
    {
        string password;
        char ch;
        while (true)
        {
            ch = getch();

            if (ch == 13) // Enter key
            {
                break;
            }
            else if (ch == 8) // Backspace key
            {
                if (!password.empty())
                {
                    password.pop_back();
                    cout << "\b \b";
                }
            }
            else
            {
                if (password.length() < 100)
                {
                    password += ch;
                    cout << "*";
                }
            }
        }

        cout << endl;
        return password;
    }

    bool ValidPasswordVerification(const string &Password)
    {
        if (Password.length() < 8 || Password.length() > 100)
            return false;

        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;

        for (char ch : Password)
        {
            if (Uppercase_check(ch))
            {
                hasUpper = true;
            }
            else if (Lowercase_check(ch))
            {
                hasLower = true;
            }
            else if (Digit_check(ch))
            {
                hasDigit = true;
            }
            else if (Special_check(ch))
            {
                hasSpecial = true;
            }
        }

        return hasUpper && hasLower && hasDigit && hasSpecial;
    }

    bool PasswordEncrypter(const string &Password)
    {
        // This keeps the old users.dat layout. It is encoding, not real encryption.
        if (!ValidPasswordVerification(Password))
        {
            return false;
        }

        PasswordLength = static_cast<int>(Password.length());
        int i;

        for (i = 0; i < PasswordLength; i++)
        {
            EncryptedPassword[i] = ((Password[i] + 1015) / 2) + 4;
        }

        return true;
    }
    bool VerifyPassword(const string &Password)
    {
        if (PasswordLength < 8 || PasswordLength > 100)
        {
            return false;
        }

        int i;
        int encrypted;

        if (Password.length() != PasswordLength)
        {
            return false;
        }

        for (i = 0; i < PasswordLength; i++)
        {
            encrypted = ((Password[i] + 1015) / 2) + 4;

            if (encrypted != EncryptedPassword[i])
            {
                return false;
            }
        }

        return true;
    }

    string getEncryptedString() const
    {
        string PasswordString = "";

        for (int i = 0; i < PasswordLength; i++)
        {
            PasswordString += to_string(EncryptedPassword[i]) + " ";
        }
        return PasswordString;
    }

    void LoadPasswordString(const string &EncryptedString)
    {
        PasswordLength = 0;
        string temp = "";

        for (char ch : EncryptedString)
        {
            if (ch == ' ')
            {
                if (!temp.empty())
                {
                    if (PasswordLength >= 100)
                    {
                        PasswordLength = 0;
                        return;
                    }

                    try
                    {
                        EncryptedPassword[PasswordLength] = stoi(temp);
                        PasswordLength++;
                    }
                    catch (...)
                    {
                        PasswordLength = 0;
                        return;
                    }
                    temp = "";
                }
            }
            else
            {
                temp += ch;
            }
        }
        if (!temp.empty() && PasswordLength < 100)
        {
            try
            {
                EncryptedPassword[PasswordLength] = stoi(temp);
                PasswordLength++;
            }
            catch (...)
            {
                PasswordLength = 0;
            }
        }
    }

    int PasswordLengthGet() const
    {
        return PasswordLength;
    }
};

class User
{
    string Username;
    string SecretCode;
    string CreationDate;
    Password UserPassword;
    string EncryptedPasswordData;

public:
    User() // Default Constructor
    {
        Username = "NO DATA";
        SecretCode = "NO DATA";
        CreationDate = "NO DATA";
        EncryptedPasswordData = "NO DATA";
    }

    User(const string &uname, const string &pass, const string &code)
    {
        SetUsername(uname);
        SetPassword(pass);
        SetSecretCode(code);
        SetCreationDate();
    }

    void SetUsername(const string &uname)
    {
        Username = uname;
    }

    void SetPassword(const string &pass)
    {
        if (UserPassword.PasswordEncrypter(pass))
        {
            EncryptedPasswordData = UserPassword.getEncryptedString();
        }
    }

    void SetSecretCode(const string &code)
    {
        SecretCode = code;
    }

    void SetCreationDate()
    {
        time_t now = time(0);
        CreationDate = ctime(&now);
        CreationDate.pop_back();
    }

    string GetUsername() const
    {
        return Username;
    }

    string GetEncryptedPasswordData() const
    {
        return EncryptedPasswordData;
    }
    string GetSecretCode() const
    {
        return SecretCode;
    }

    string GetCreationDate() const
    {
        return CreationDate;
    }

    bool verifyPassword(const string &inputPassword)
    {
        if (UserPassword.PasswordLengthGet() == 0)
        {
            UserPassword.LoadPasswordString(EncryptedPasswordData);
        }
        return UserPassword.VerifyPassword(inputPassword);
    }

    bool verifySecretCode(const string &inputCode) const
    {
        return !SecretCode.empty() && SecretCode == inputCode;
    }

    string toString() const
    {
        return Username + "|" + EncryptedPasswordData + "|" + SecretCode + "|" + CreationDate;
    }

    void fromString(const string &data)
    {
        size_t pos1 = data.find("|");

        if (pos1 == string::npos)
        {
            return;
        }

        size_t pos2 = data.find("|", pos1 + 1);

        if (pos2 == string::npos)
        {
            return;
        }

        size_t pos3 = data.find("|", pos2 + 1);

        if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos)
        {
            Username = data.substr(0, pos1);
            EncryptedPasswordData = data.substr(pos1 + 1, pos2 - pos1 - 1);
            SecretCode = data.substr(pos2 + 1, pos3 - pos2 - 1);
            CreationDate = data.substr(pos3 + 1);
        }
    }
};
class FileHandler
{
private:
    static const string FILENAME;

    static string normalizeUsername(const string &username)
    {
        string result = username;

        for (size_t i = 0; i < result.length(); i++)
        {
            result[i] = static_cast<char>(tolower(
                static_cast<unsigned char>(result[i])));
        }

        return result;
    }

    static bool writeString(ofstream &file, const string &str)
    {
        if (str.length() > 4096)
        {
            return false;
        }

        int len = static_cast<int>(str.length());
        file.write(reinterpret_cast<const char *>(&len), sizeof(len));
        file.write(str.c_str(), len);
        return static_cast<bool>(file);
    }

    static bool readString(ifstream &file, string &str)
    {
        int len = 0;
        file.read(reinterpret_cast<char *>(&len), sizeof(len));

        if (!file || len < 0 || len > 4096)
        {
            return false;
        }

        str.clear();

        if (len > 0)
        {
            str.resize(static_cast<size_t>(len));
            file.read(&str[0], len);
        }

        return static_cast<bool>(file);
    }
    static bool readRecord(ifstream &file, string &username,
                           string &password, string &code, string &date)
    {
        return readString(file, username) &&
               readString(file, password) &&
               readString(file, code) &&
               readString(file, date);
    }

public:
    static bool saveUser(const User &user)
    {
        ofstream file(FILENAME, ios::binary | ios::app);
        if (!file)
        {
            cout << "\nError: Unable to open file for writing!\n";
            return false;
        }
        bool saved = writeString(file, user.GetUsername()) &&
                     writeString(file, user.GetEncryptedPasswordData()) &&
                     writeString(file, user.GetSecretCode()) &&
                     writeString(file, user.GetCreationDate());
        file.close();
        return saved && static_cast<bool>(file);
    }

    static bool findUser(const string &username, User &foundUser)
    {
        ifstream file(FILENAME, ios::binary);
        if (!file)
            return false;

        while (file.peek() != EOF)
        {
            string uname, epass, code, date;

            if (!readRecord(file, uname, epass, code, date))
            {
                return false;
            }

            if (normalizeUsername(uname) == normalizeUsername(username))
            {
                string full = uname + "|" + epass + "|" + code + "|" + date;
                foundUser.fromString(full);
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    static bool updateUser(const string &username, const User &updatedUser)
    {
        ifstream file(FILENAME, ios::binary);
        if (!file)
            return false;

        vector<User> users;
        bool found = false;

        while (file.peek() != EOF)
        {
            string uname, epass, code, date;

            if (!readRecord(file, uname, epass, code, date))
            {
                return false;
            }
            string full = uname + "|" + epass + "|" + code + "|" + date;
            User temp;
            temp.fromString(full);

            if (normalizeUsername(uname) == normalizeUsername(username))
            {
                users.push_back(updatedUser);
                found = true;
            }
            else
            {
                users.push_back(temp);
            }
        }

        file.close();

        if (found)
        {
            ofstream outFile(FILENAME, ios::binary | ios::trunc);
            for (const User &user : users)
            {
                if (!writeString(outFile, user.GetUsername()) ||
                    !writeString(outFile, user.GetEncryptedPasswordData()) ||
                    !writeString(outFile, user.GetSecretCode()) ||
                    !writeString(outFile, user.GetCreationDate()))
                {
                    return false;
                }
            }
            outFile.close();
            return static_cast<bool>(outFile);
        }

        return found;
    }

    static bool usernameExists(const string &username)
    {
        User temp;
        return findUser(username, temp);
    }
};

const string FileHandler::FILENAME = "users.dat";

class ProjectTools
{
public:
    static string CleanLine(const string &value)
    {
        string result = value;

        for (size_t i = 0; i < result.length(); i++)
        {
            if (result[i] == '|' || result[i] == '\n' || result[i] == '\r')
            {
                result[i] = ' ';
            }
        }

        return result;
    }

    static vector<string> SplitLine(const string &line, char separator)
    {
        vector<string> parts;
        string part;
        stringstream input(line);
        while (getline(input, part, separator))
        {
            parts.push_back(part);
        }

        if (!line.empty() && line[line.length() - 1] == separator)
        {
            parts.push_back("");
        }

        return parts;
    }

    static string CurrentDate()
    {
        time_t now = time(0);
        string date = ctime(&now);

        if (!date.empty() && date[date.length() - 1] == '\n')
        {
            date.erase(date.length() - 1);
        }

        return date;
    }

    static string Lowercase(const string &value)
    {
        string result = value;

        for (size_t i = 0; i < result.length(); i++)
        {
            result[i] = static_cast<char>(tolower(
                static_cast<unsigned char>(result[i])));
        }

        return result;
    }

    static bool ValidSimpleText(const string &value, int minimum, int maximum)
    {
        if (static_cast<int>(value.length()) < minimum ||
            static_cast<int>(value.length()) > maximum)
        {
            return false;
        }

        if (value.find('|') != string::npos ||
            value.find('\n') != string::npos ||
            value.find('\r') != string::npos)
        {
            return false;
        }
        return true;
    }

    static int AskNumber(const string &question, int minimum, int maximum)
    {
        string input;

        while (true)
        {
            cout << question;

            if (!getline(cin, input))
            {
                cin.clear();
                return maximum;
            }

            stringstream numberInput(input);
            int number = 0;
            char extra = 0;

            if ((numberInput >> number) && !(numberInput >> extra) &&
                number >= minimum && number <= maximum)
            {
                return number;
            }

            cout << "Please enter a number from " << minimum
                 << " to " << maximum << ".\n";
        }
    }

    static void Pause()
    {
        string unused;
        cout << "\nPress Enter to continue...";
        getline(cin, unused);
    }

    static void ShowLine(char symbol = '-', int length = 60)
    {
        for (int i = 0; i < length; i++)
        {
            cout << symbol;
        }

        cout << endl;
    }
};

class ActivityLog
{
    static const string FILENAME;

public:
    static void Add(const string &username, const string &action)
    {
        ofstream file(FILENAME, ios::app);

        if (!file)
        {
            return;
        }

        file << ProjectTools::CleanLine(ProjectTools::CurrentDate()) << '|'
             << ProjectTools::CleanLine(username) << '|'
             << ProjectTools::CleanLine(action) << endl;
    }

    static vector<string> ReadRecent(const string &username, int amount)
    {
        vector<string> events;
        ifstream file(FILENAME);
        string line;

        if (!file)
        {
            return events;
        }

        while (getline(file, line))
        {
            vector<string> parts = ProjectTools::SplitLine(line, '|');

            if (parts.size() == 3 && parts[1] == username)
            {
                events.push_back(parts[0] + " - " + parts[2]);
            }
        }

        while (static_cast<int>(events.size()) > amount)
        {
            events.erase(events.begin());
        }

        return events;
    }

    static vector<string> ReadAllRecent(int amount)
    {
        vector<string> events;
        ifstream file(FILENAME);
        string line;

        if (!file)
        {
            return events;
        }
        while (getline(file, line))
        {
            if (!line.empty())
            {
                events.push_back(line);
            }
        }

        while (static_cast<int>(events.size()) > amount)
        {
            events.erase(events.begin());
        }

        return events;
    }

    static const string &GetFileName()
    {
        return FILENAME;
    }
};

const string ActivityLog::FILENAME = "activity.log";

class WorkspaceData
{
    static bool RenameOwnerInFile(const string &fileName,
                                  const string &oldName,
                                  const string &newName)
    {
        ifstream input(fileName);

        if (!input)
        {
            return true;
        }

        vector<string> lines;
        string line;

        while (getline(input, line))
        {
            vector<string> parts = ProjectTools::SplitLine(line, '|');

            if (parts.size() >= 3 && parts[1] == oldName)
            {
                parts[1] = newName;
                line.clear();

                for (size_t i = 0; i < parts.size(); i++)
                {
                    if (i > 0)
                    {
                        line += '|';
                    }
                    line += parts[i];
                }
            }

            lines.push_back(line);
        }

        input.close();
        ofstream output(fileName, ios::trunc);

        if (!output)
        {
            return false;
        }

        for (size_t i = 0; i < lines.size(); i++)
        {
            output << lines[i] << endl;
        }

        output.close();
        return static_cast<bool>(output);
    }

public:
    static bool RenameUser(const string &oldName, const string &newName)
    {
        const string files[] = {
            "tasks.txt", "notes.txt", "expenses.txt",
            "contacts.txt", "activity.log"};

        for (int i = 0; i < 5; i++)
        {
            if (!RenameOwnerInFile(files[i], oldName, newName))
            {
                return false;
            }
        }

        return true;
    }
};

class TaskItem
{
public:
    int ID;
    string Username;
    string Title;
    string Description;
    string DueDate;
    string Priority;
    string Status;
    string CreationDate;

    TaskItem()
    {
        ID = 0;
        Username = "";
        Title = "";
        Description = "";
        DueDate = "No due date";
        Priority = "Normal";
        Status = "Open";
        CreationDate = "";
    }
};
class TaskHandler
{
    static const string FILENAME;

    static bool LoadAll(vector<TaskItem> &tasks)
    {
        tasks.clear();
        ifstream file(FILENAME);
        string line;

        if (!file)
        {
            return true;
        }

        while (getline(file, line))
        {
            vector<string> parts = ProjectTools::SplitLine(line, '|');

            if (parts.size() != 8)
            {
                continue;
            }

            TaskItem task;
            task.ID = atoi(parts[0].c_str());
            task.Username = parts[1];
            task.Title = parts[2];
            task.Description = parts[3];
            task.DueDate = parts[4];
            task.Priority = parts[5];
            task.Status = parts[6];
            task.CreationDate = parts[7];

            if (task.ID > 0 && !task.Username.empty())
            {
                tasks.push_back(task);
            }
        }

        return true;
    }

    static bool SaveAll(const vector<TaskItem> &tasks)
    {
        ofstream file(FILENAME, ios::trunc);

        if (!file)
        {
            return false;
        }

        for (size_t i = 0; i < tasks.size(); i++)
        {
            file << tasks[i].ID << '|'
                 << ProjectTools::CleanLine(tasks[i].Username) << '|'
                 << ProjectTools::CleanLine(tasks[i].Title) << '|'
                 << ProjectTools::CleanLine(tasks[i].Description) << '|'
                 << ProjectTools::CleanLine(tasks[i].DueDate) << '|'
                 << ProjectTools::CleanLine(tasks[i].Priority) << '|'
                 << ProjectTools::CleanLine(tasks[i].Status) << '|'
                 << ProjectTools::CleanLine(tasks[i].CreationDate) << endl;
        }
        file.close();
        return static_cast<bool>(file);
    }

    static int NextID(const vector<TaskItem> &tasks)
    {
        int largestID = 0;

        for (size_t i = 0; i < tasks.size(); i++)
        {
            if (tasks[i].ID > largestID)
            {
                largestID = tasks[i].ID;
            }
        }

        return largestID + 1;
    }

    static int FindTask(const vector<TaskItem> &tasks,
                        const string &username, int taskID)
    {
        for (size_t i = 0; i < tasks.size(); i++)
        {
            if (tasks[i].ID == taskID && tasks[i].Username == username)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    static void ShowTasks(const vector<TaskItem> &tasks,
                          const string &username, const string &search = "")
    {
        cout << "\n"
             << left << setw(6) << "ID"
             << setw(24) << "TASK"
             << setw(14) << "PRIORITY"
             << setw(16) << "STATUS"
             << setw(16) << "DUE DATE" << endl;
        ProjectTools::ShowLine('-', 76);

        int shown = 0;
        string searchText = ProjectTools::Lowercase(search);

        for (size_t i = 0; i < tasks.size(); i++)
        {
            if (tasks[i].Username != username)
            {
                continue;
            }

            string title = ProjectTools::Lowercase(tasks[i].Title);
            string description = ProjectTools::Lowercase(tasks[i].Description);

            if (!searchText.empty() &&
                title.find(searchText) == string::npos &&
                description.find(searchText) == string::npos)
            {
                continue;
            }
            cout << left << setw(6) << tasks[i].ID
                 << setw(24) << tasks[i].Title.substr(0, 21)
                 << setw(14) << tasks[i].Priority
                 << setw(16) << tasks[i].Status
                 << setw(16) << tasks[i].DueDate.substr(0, 14) << endl;
            cout << "      " << tasks[i].Description << endl;
            shown++;
        }

        if (shown == 0)
        {
            cout << "No tasks found.\n";
        }

        ProjectTools::ShowLine('-', 76);
    }

public:
    static void ShowMenu(const string &username)
    {
        bool open = true;

        while (open)
        {
            system("cls");
            cout << "\n========== TASK MANAGER ==========\n";
            cout << "Tasks belong to " << username << ".\n";
            cout << "1. Show all tasks\n";
            cout << "2. Add a task\n";
            cout << "3. Edit a task\n";
            cout << "4. Change task status\n";
            cout << "5. Search tasks\n";
            cout << "6. Delete a task\n";
            cout << "7. Task summary\n";
            cout << "8. Return to account menu\n";

            int choice = ProjectTools::AskNumber("Choose an option (1-8): ", 1, 8);

            if (choice == 1)
            {
                View(username);
            }
            else if (choice == 2)
            {
                Add(username);
            }
            else if (choice == 3)
            {
                Edit(username);
            }
            else if (choice == 4)
            {
                ChangeStatus(username);
            }
            else if (choice == 5)
            {
                Search(username);
            }
            else if (choice == 6)
            {
                Delete(username);
            }
            else if (choice == 7)
            {
                Summary(username);
            }
            else
            {
                open = false;
            }
        }
    }

    static void View(const string &username)
    {
        vector<TaskItem> tasks;
        LoadAll(tasks);
        system("cls");
        cout << "\n========== MY TASKS ==========\n";
        ShowTasks(tasks, username);
        ProjectTools::Pause();
    }

    static void Add(const string &username)
    {
        system("cls");
        TaskItem task;
        task.Username = username;
        cout << "\n========== ADD TASK ==========\n";
        cout << "Task title: ";
        getline(cin, task.Title);
        cout << "Short description: ";
        getline(cin, task.Description);
        cout << "Due date (example: 2026-12-31, or leave blank): ";
        getline(cin, task.DueDate);

        if (!ProjectTools::ValidSimpleText(task.Title, 1, 60))
        {
            cout << "Title must contain 1 to 60 characters and cannot contain |.\n";
            ProjectTools::Pause();
            return;
        }

        if (task.Description.length() > 180 ||
            task.Description.find('|') != string::npos)
        {
            cout << "Description must be 180 characters or fewer.\n";
            ProjectTools::Pause();
            return;
        }
        if (task.DueDate.empty())
        {
            task.DueDate = "No due date";
        }

        cout << "Priority: 1. Low  2. Normal  3. High\n";
        int priorityChoice = ProjectTools::AskNumber("Choose priority: ", 1, 3);

        if (priorityChoice == 1)
        {
            task.Priority = "Low";
        }
        else if (priorityChoice == 3)
        {
            task.Priority = "High";
        }

        vector<TaskItem> tasks;
        LoadAll(tasks);
        task.ID = NextID(tasks);
        task.Status = "Open";
        task.CreationDate = ProjectTools::CurrentDate();
        tasks.push_back(task);

        if (SaveAll(tasks))
        {
            ActivityLog::Add(username, "Added task " + task.Title);
            cout << "Task added successfully.\n";
        }
        else
        {
            cout << "Unable to save the task list.\n";
        }

        ProjectTools::Pause();
    }

    static void Edit(const string &username)
    {
        vector<TaskItem> tasks;
        LoadAll(tasks);
        system("cls");
        ShowTasks(tasks, username);
        int taskID = ProjectTools::AskNumber("Task ID to edit: ", 1, 1000000);
        int index = FindTask(tasks, username, taskID);

        if (index < 0)
        {
            cout << "That task does not belong to this account.\n";
            ProjectTools::Pause();
            return;
        }

        string value;
        cout << "New title (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            if (!ProjectTools::ValidSimpleText(value, 1, 60))
            {
                cout << "Invalid title.\n";
                ProjectTools::Pause();
                return;
            }

            tasks[index].Title = value;
        }
        cout << "New description (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            if (value.length() > 180 || value.find('|') != string::npos)
            {
                cout << "Invalid description.\n";
                ProjectTools::Pause();
                return;
            }

            tasks[index].Description = value;
        }

        cout << "New due date (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            tasks[index].DueDate = ProjectTools::CleanLine(value);
        }

        if (SaveAll(tasks))
        {
            ActivityLog::Add(username, "Edited task " + to_string(taskID));
            cout << "Task updated.\n";
        }
        else
        {
            cout << "Unable to save the task list.\n";
        }

        ProjectTools::Pause();
    }

    static void ChangeStatus(const string &username)
    {
        vector<TaskItem> tasks;
        LoadAll(tasks);
        system("cls");
        ShowTasks(tasks, username);
        int taskID = ProjectTools::AskNumber("Task ID: ", 1, 1000000);
        int index = FindTask(tasks, username, taskID);

        if (index < 0)
        {
            cout << "Task not found.\n";
            ProjectTools::Pause();
            return;
        }

        cout << "1. Open\n2. In Progress\n3. Done\n";
        int choice = ProjectTools::AskNumber("Choose a status: ", 1, 3);

        if (choice == 1)
        {
            tasks[index].Status = "Open";
        }
        else if (choice == 2)
        {
            tasks[index].Status = "In Progress";
        }
        else
        {
            tasks[index].Status = "Done";
        }
        if (SaveAll(tasks))
        {
            ActivityLog::Add(username, "Changed task status");
            cout << "Task status updated.\n";
        }
        else
        {
            cout << "Unable to save the task list.\n";
        }

        ProjectTools::Pause();
    }

    static void Search(const string &username)
    {
        vector<TaskItem> tasks;
        LoadAll(tasks);
        string search;
        system("cls");
        cout << "Search words in a title or description: ";
        getline(cin, search);
        ShowTasks(tasks, username, search);
        ProjectTools::Pause();
    }

    static void Delete(const string &username)
    {
        vector<TaskItem> tasks;
        LoadAll(tasks);
        system("cls");
        ShowTasks(tasks, username);
        int taskID = ProjectTools::AskNumber("Task ID to delete: ", 1, 1000000);
        int index = FindTask(tasks, username, taskID);

        if (index < 0)
        {
            cout << "Task not found.\n";
            ProjectTools::Pause();
            return;
        }

        cout << "Delete \"" << tasks[index].Title << "\"? (y/n): ";
        string answer;
        getline(cin, answer);

        if (answer != "y" && answer != "Y" &&
            answer != "yes" && answer != "YES")
        {
            cout << "No changes were made.\n";
            ProjectTools::Pause();
            return;
        }

        string deletedTitle = tasks[index].Title;
        tasks.erase(tasks.begin() + index);

        if (SaveAll(tasks))
        {
            ActivityLog::Add(username, "Deleted task " + deletedTitle);
            cout << "Task deleted.\n";
        }
        else
        {
            cout << "Unable to save the task list.\n";
        }

        ProjectTools::Pause();
    }

    static void Summary(const string &username)
    {
        vector<TaskItem> tasks;
        LoadAll(tasks);
        int total = 0;
        int open = 0;
        int progress = 0;
        int done = 0;
        for (size_t i = 0; i < tasks.size(); i++)
        {
            if (tasks[i].Username != username)
            {
                continue;
            }

            total++;

            if (tasks[i].Status == "Open")
            {
                open++;
            }
            else if (tasks[i].Status == "In Progress")
            {
                progress++;
            }
            else if (tasks[i].Status == "Done")
            {
                done++;
            }
        }

        system("cls");
        cout << "\n========== TASK SUMMARY ==========\n";
        cout << "Total tasks       : " << total << endl;
        cout << "Open              : " << open << endl;
        cout << "In Progress       : " << progress << endl;
        cout << "Done              : " << done << endl;
        ProjectTools::Pause();
    }
};

const string TaskHandler::FILENAME = "tasks.txt";

class NoteItem
{
public:
    int ID;
    string Username;
    string Title;
    string Content;
    string CreationDate;

    NoteItem()
    {
        ID = 0;
        Username = "";
        Title = "";
        Content = "";
        CreationDate = "";
    }
};

class NoteHandler
{
    static const string FILENAME;

    static bool LoadAll(vector<NoteItem> &notes)
    {
        notes.clear();
        ifstream file(FILENAME);
        string line;

        if (!file)
        {
            return true;
        }

        while (getline(file, line))
        {
            vector<string> parts = ProjectTools::SplitLine(line, '|');
            if (parts.size() != 5)
            {
                continue;
            }

            NoteItem note;
            note.ID = atoi(parts[0].c_str());
            note.Username = parts[1];
            note.Title = parts[2];
            note.Content = parts[3];
            note.CreationDate = parts[4];

            if (note.ID > 0 && !note.Username.empty())
            {
                notes.push_back(note);
            }
        }

        return true;
    }

    static bool SaveAll(const vector<NoteItem> &notes)
    {
        ofstream file(FILENAME, ios::trunc);

        if (!file)
        {
            return false;
        }

        for (size_t i = 0; i < notes.size(); i++)
        {
            file << notes[i].ID << '|'
                 << ProjectTools::CleanLine(notes[i].Username) << '|'
                 << ProjectTools::CleanLine(notes[i].Title) << '|'
                 << ProjectTools::CleanLine(notes[i].Content) << '|'
                 << ProjectTools::CleanLine(notes[i].CreationDate) << endl;
        }

        file.close();
        return static_cast<bool>(file);
    }

    static int NextID(const vector<NoteItem> &notes)
    {
        int largestID = 0;

        for (size_t i = 0; i < notes.size(); i++)
        {
            if (notes[i].ID > largestID)
            {
                largestID = notes[i].ID;
            }
        }

        return largestID + 1;
    }
    static int FindNote(const vector<NoteItem> &notes,
                        const string &username, int noteID)
    {
        for (size_t i = 0; i < notes.size(); i++)
        {
            if (notes[i].ID == noteID && notes[i].Username == username)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    static void ShowNotes(const vector<NoteItem> &notes,
                          const string &username, const string &search = "")
    {
        string searchText = ProjectTools::Lowercase(search);
        int shown = 0;

        for (size_t i = 0; i < notes.size(); i++)
        {
            if (notes[i].Username != username)
            {
                continue;
            }

            string title = ProjectTools::Lowercase(notes[i].Title);
            string content = ProjectTools::Lowercase(notes[i].Content);

            if (!searchText.empty() &&
                title.find(searchText) == string::npos &&
                content.find(searchText) == string::npos)
            {
                continue;
            }

            ProjectTools::ShowLine('-', 60);
            cout << "Note " << notes[i].ID << " - " << notes[i].Title << endl;
            cout << notes[i].Content << endl;
            cout << "Created: " << notes[i].CreationDate << endl;
            shown++;
        }

        ProjectTools::ShowLine('-', 60);

        if (shown == 0)
        {
            cout << "No notes found.\n";
        }
        else
        {
            cout << "Notes shown: " << shown << endl;
        }
    }

public:
    static void ShowMenu(const string &username)
    {
        bool open = true;

        while (open)
        {
            system("cls");
            cout << "\n========== PERSONAL NOTES ==========\n";
            cout << "1. Show my notes\n";
            cout << "2. Add a note\n";
            cout << "3. Edit a note\n";
            cout << "4. Search notes\n";
            cout << "5. Delete a note\n";
            cout << "6. Return to account menu\n";
            int choice = ProjectTools::AskNumber("Choose an option (1-6): ", 1, 6);

            if (choice == 1)
            {
                View(username);
            }
            else if (choice == 2)
            {
                Add(username);
            }
            else if (choice == 3)
            {
                Edit(username);
            }
            else if (choice == 4)
            {
                Search(username);
            }
            else if (choice == 5)
            {
                Delete(username);
            }
            else
            {
                open = false;
            }
        }
    }

    static void View(const string &username)
    {
        vector<NoteItem> notes;
        LoadAll(notes);
        system("cls");
        cout << "\n========== MY NOTES ==========\n";
        ShowNotes(notes, username);
        ProjectTools::Pause();
    }

    static void Add(const string &username)
    {
        NoteItem note;
        note.Username = username;
        system("cls");
        cout << "\n========== ADD A NOTE ==========\n";
        cout << "Title: ";
        getline(cin, note.Title);
        cout << "Note text (one line): ";
        getline(cin, note.Content);

        if (!ProjectTools::ValidSimpleText(note.Title, 1, 60))
        {
            cout << "Title must contain 1 to 60 characters and cannot contain |.\n";
            ProjectTools::Pause();
            return;
        }

        if (note.Content.empty() || note.Content.length() > 250 ||
            note.Content.find('|') != string::npos)
        {
            cout << "Note text must contain 1 to 250 characters.\n";
            ProjectTools::Pause();
            return;
        }

        vector<NoteItem> notes;
        LoadAll(notes);
        note.ID = NextID(notes);
        note.CreationDate = ProjectTools::CurrentDate();
        notes.push_back(note);

        if (SaveAll(notes))
        {
            ActivityLog::Add(username, "Added note " + note.Title);
            cout << "Note saved.\n";
        }
        else
        {
            cout << "Unable to save the notes file.\n";
        }

        ProjectTools::Pause();
    }

    static void Edit(const string &username)
    {
        vector<NoteItem> notes;
        LoadAll(notes);
        system("cls");
        ShowNotes(notes, username);
        int noteID = ProjectTools::AskNumber("Note ID to edit: ", 1, 1000000);
        int index = FindNote(notes, username, noteID);

        if (index < 0)
        {
            cout << "Note not found.\n";
            ProjectTools::Pause();
            return;
        }
        string title;
        string content;
        cout << "New title (Enter to keep current): ";
        getline(cin, title);
        cout << "New note text (Enter to keep current): ";
        getline(cin, content);

        if (!title.empty())
        {
            if (!ProjectTools::ValidSimpleText(title, 1, 60))
            {
                cout << "Invalid title.\n";
                ProjectTools::Pause();
                return;
            }

            notes[index].Title = title;
        }

        if (!content.empty())
        {
            if (content.length() > 250 || content.find('|') != string::npos)
            {
                cout << "Invalid note text.\n";
                ProjectTools::Pause();
                return;
            }

            notes[index].Content = content;
        }

        if (SaveAll(notes))
        {
            ActivityLog::Add(username, "Edited note " + to_string(noteID));
            cout << "Note updated.\n";
        }
        else
        {
            cout << "Unable to save the notes file.\n";
        }

        ProjectTools::Pause();
    }

    static void Search(const string &username)
    {
        vector<NoteItem> notes;
        LoadAll(notes);
        string search;
        system("cls");
        cout << "Search words in a title or note: ";
        getline(cin, search);
        ShowNotes(notes, username, search);
        ProjectTools::Pause();
    }

    static void Delete(const string &username)
    {
        vector<NoteItem> notes;
        LoadAll(notes);
        system("cls");
        ShowNotes(notes, username);
        int noteID = ProjectTools::AskNumber("Note ID to delete: ", 1, 1000000);
        int index = FindNote(notes, username, noteID);

        if (index < 0)
        {
            cout << "Note not found.\n";
            ProjectTools::Pause();
            return;
        }

        cout << "Delete this note? (y/n): ";
        string answer;
        getline(cin, answer);
        if (answer != "y" && answer != "Y" &&
            answer != "yes" && answer != "YES")
        {
            cout << "No changes were made.\n";
            ProjectTools::Pause();
            return;
        }

        notes.erase(notes.begin() + index);

        if (SaveAll(notes))
        {
            ActivityLog::Add(username, "Deleted note " + to_string(noteID));
            cout << "Note deleted.\n";
        }
        else
        {
            cout << "Unable to save the notes file.\n";
        }

        ProjectTools::Pause();
    }
};

const string NoteHandler::FILENAME = "notes.txt";

class ExpenseItem
{
public:
    int ID;
    string Username;
    string Category;
    double Amount;
    string Description;
    string Date;

    ExpenseItem()
    {
        ID = 0;
        Username = "";
        Category = "Other";
        Amount = 0.0;
        Description = "";
        Date = "";
    }
};

class ExpenseHandler
{
    static const string FILENAME;

    static bool LoadAll(vector<ExpenseItem> &expenses)
    {
        expenses.clear();
        ifstream file(FILENAME);
        string line;

        if (!file)
        {
            return true;
        }

        while (getline(file, line))
        {
            vector<string> parts = ProjectTools::SplitLine(line, '|');
            if (parts.size() != 6)
            {
                continue;
            }

            ExpenseItem expense;
            expense.ID = atoi(parts[0].c_str());
            expense.Username = parts[1];
            expense.Category = parts[2];
            expense.Description = parts[4];
            expense.Date = parts[5];
            stringstream amountInput(parts[3]);
            amountInput >> expense.Amount;

            if (expense.ID > 0 && !expense.Username.empty() && amountInput)
            {
                expenses.push_back(expense);
            }
        }

        return true;
    }

    static bool SaveAll(const vector<ExpenseItem> &expenses)
    {
        ofstream file(FILENAME, ios::trunc);

        if (!file)
        {
            return false;
        }

        for (size_t i = 0; i < expenses.size(); i++)
        {
            file << expenses[i].ID << '|'
                 << ProjectTools::CleanLine(expenses[i].Username) << '|'
                 << ProjectTools::CleanLine(expenses[i].Category) << '|'
                 << fixed << setprecision(2) << expenses[i].Amount << '|'
                 << ProjectTools::CleanLine(expenses[i].Description) << '|'
                 << ProjectTools::CleanLine(expenses[i].Date) << endl;
        }

        file.close();
        return static_cast<bool>(file);
    }

    static int NextID(const vector<ExpenseItem> &expenses)
    {
        int largestID = 0;

        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (expenses[i].ID > largestID)
            {
                largestID = expenses[i].ID;
            }
        }

        return largestID + 1;
    }
    static int FindExpense(const vector<ExpenseItem> &expenses,
                           const string &username, int expenseID)
    {
        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (expenses[i].ID == expenseID && expenses[i].Username == username)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    static void ShowExpenses(const vector<ExpenseItem> &expenses,
                             const string &username, const string &search = "")
    {
        cout << "\n"
             << left << setw(6) << "ID"
             << setw(16) << "CATEGORY"
             << setw(14) << "AMOUNT"
             << setw(28) << "DESCRIPTION"
             << "DATE" << endl;
        ProjectTools::ShowLine('-', 80);
        string searchText = ProjectTools::Lowercase(search);
        int shown = 0;

        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (expenses[i].Username != username)
            {
                continue;
            }

            string category = ProjectTools::Lowercase(expenses[i].Category);
            string description = ProjectTools::Lowercase(expenses[i].Description);

            if (!searchText.empty() &&
                category.find(searchText) == string::npos &&
                description.find(searchText) == string::npos)
            {
                continue;
            }

            cout << left << setw(6) << expenses[i].ID
                 << setw(16) << expenses[i].Category
                 << "$" << setw(13) << fixed << setprecision(2) << expenses[i].Amount
                 << setw(28) << expenses[i].Description.substr(0, 25)
                 << expenses[i].Date << endl;
            shown++;
        }

        if (shown == 0)
        {
            cout << "No expenses found.\n";
        }

        ProjectTools::ShowLine('-', 80);
        cout.unsetf(ios::floatfield);
    }

public:
    static void ShowMenu(const string &username)
    {
        bool open = true;

        while (open)
        {
            system("cls");
            cout << "\n========== EXPENSE TRACKER ==========\n";
            cout << "1. Show my expenses\n";
            cout << "2. Add an expense\n";
            cout << "3. Search expenses\n";
            cout << "4. Delete an expense\n";
            cout << "5. Spending summary\n";
            cout << "6. Return to account menu\n";
            int choice = ProjectTools::AskNumber("Choose an option (1-6): ", 1, 6);

            if (choice == 1)
            {
                View(username);
            }
            else if (choice == 2)
            {
                Add(username);
            }
            else if (choice == 3)
            {
                Search(username);
            }
            else if (choice == 4)
            {
                Delete(username);
            }
            else if (choice == 5)
            {
                Summary(username);
            }
            else
            {
                open = false;
            }
        }
    }

    static void View(const string &username)
    {
        vector<ExpenseItem> expenses;
        LoadAll(expenses);
        system("cls");
        cout << "\n========== MY EXPENSES ==========\n";
        ShowExpenses(expenses, username);
        ProjectTools::Pause();
    }

    static void Add(const string &username)
    {
        ExpenseItem expense;
        expense.Username = username;
        system("cls");
        cout << "\n========== ADD EXPENSE ==========\n";
        cout << "1. Food\n2. Travel\n3. Bills\n4. Shopping\n5. Education\n6. Other\n";
        int categoryChoice = ProjectTools::AskNumber("Choose a category (1-6): ", 1, 6);

        if (categoryChoice == 1)
        {
            expense.Category = "Food";
        }
        else if (categoryChoice == 2)
        {
            expense.Category = "Travel";
        }
        else if (categoryChoice == 3)
        {
            expense.Category = "Bills";
        }
        else if (categoryChoice == 4)
        {
            expense.Category = "Shopping";
        }
        else if (categoryChoice == 5)
        {
            expense.Category = "Education";
        }
        else
        {
            expense.Category = "Other";
        }

        string amountText;
        cout << "Amount: ";
        getline(cin, amountText);
        stringstream amountInput(amountText);
        char extra = 0;

        if (!(amountInput >> expense.Amount) || (amountInput >> extra) ||
            expense.Amount <= 0.0 || expense.Amount > 1000000000.0)
        {
            cout << "Enter a positive amount using numbers only.\n";
            ProjectTools::Pause();
            return;
        }

        cout << "Short description: ";
        getline(cin, expense.Description);

        if (expense.Description.empty() || expense.Description.length() > 100 ||
            expense.Description.find('|') != string::npos)
        {
            cout << "Description must contain 1 to 100 characters.\n";
            ProjectTools::Pause();
            return;
        }

        vector<ExpenseItem> expenses;
        LoadAll(expenses);
        expense.ID = NextID(expenses);
        expense.Date = ProjectTools::CurrentDate();
        expenses.push_back(expense);

        if (SaveAll(expenses))
        {
            ActivityLog::Add(username, "Added expense in " + expense.Category);
            cout << "Expense saved.\n";
        }
        else
        {
            cout << "Unable to save the expense list.\n";
        }
        ProjectTools::Pause();
    }

    static void Search(const string &username)
    {
        vector<ExpenseItem> expenses;
        LoadAll(expenses);
        string search;
        system("cls");
        cout << "Search by category or description: ";
        getline(cin, search);
        ShowExpenses(expenses, username, search);
        ProjectTools::Pause();
    }

    static void Delete(const string &username)
    {
        vector<ExpenseItem> expenses;
        LoadAll(expenses);
        system("cls");
        ShowExpenses(expenses, username);
        int expenseID = ProjectTools::AskNumber("Expense ID to delete: ", 1, 1000000);
        int index = FindExpense(expenses, username, expenseID);

        if (index < 0)
        {
            cout << "Expense not found.\n";
            ProjectTools::Pause();
            return;
        }

        cout << "Delete this expense? (y/n): ";
        string answer;
        getline(cin, answer);

        if (answer != "y" && answer != "Y" &&
            answer != "yes" && answer != "YES")
        {
            cout << "No changes were made.\n";
            ProjectTools::Pause();
            return;
        }

        expenses.erase(expenses.begin() + index);

        if (SaveAll(expenses))
        {
            ActivityLog::Add(username, "Deleted expense " + to_string(expenseID));
            cout << "Expense deleted.\n";
        }
        else
        {
            cout << "Unable to save the expense list.\n";
        }

        ProjectTools::Pause();
    }

    static void Summary(const string &username)
    {
        vector<ExpenseItem> expenses;
        LoadAll(expenses);
        vector<string> categories;
        vector<double> totals;
        categories.push_back("Food");
        categories.push_back("Travel");
        categories.push_back("Bills");
        categories.push_back("Shopping");
        categories.push_back("Education");
        categories.push_back("Other");
        totals.resize(categories.size(), 0.0);
        double overallTotal = 0.0;
        int count = 0;

        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (expenses[i].Username != username)
            {
                continue;
            }
            overallTotal += expenses[i].Amount;
            count++;

            for (size_t category = 0; category < categories.size(); category++)
            {
                if (expenses[i].Category == categories[category])
                {
                    totals[category] += expenses[i].Amount;
                }
            }
        }

        system("cls");
        cout << "\n========== SPENDING SUMMARY ==========\n";
        cout << fixed << setprecision(2);
        cout << "Number of entries: " << count << endl;
        cout << "Total spending   : $" << overallTotal << endl;
        ProjectTools::ShowLine('-', 42);

        for (size_t i = 0; i < categories.size(); i++)
        {
            cout << left << setw(16) << categories[i] << "$" << totals[i] << endl;
        }

        cout.unsetf(ios::floatfield);
        ProjectTools::Pause();
    }
};

const string ExpenseHandler::FILENAME = "expenses.txt";

class ContactItem
{
public:
    int ID;
    string Username;
    string Name;
    string Phone;
    string Email;
    string Group;
    string CreationDate;

    ContactItem()
    {
        ID = 0;
        Username = "";
        Name = "";
        Phone = "";
        Email = "";
        Group = "Personal";
        CreationDate = "";
    }
};

class ContactHandler
{
    static const string FILENAME;

    static bool LoadAll(vector<ContactItem> &contacts)
    {
        contacts.clear();
        ifstream file(FILENAME);
        string line;

        if (!file)
        {
            return true;
        }
        while (getline(file, line))
        {
            vector<string> parts = ProjectTools::SplitLine(line, '|');

            if (parts.size() != 7)
            {
                continue;
            }

            ContactItem contact;
            contact.ID = atoi(parts[0].c_str());
            contact.Username = parts[1];
            contact.Name = parts[2];
            contact.Phone = parts[3];
            contact.Email = parts[4];
            contact.Group = parts[5];
            contact.CreationDate = parts[6];

            if (contact.ID > 0 && !contact.Username.empty())
            {
                contacts.push_back(contact);
            }
        }

        return true;
    }

    static bool SaveAll(const vector<ContactItem> &contacts)
    {
        ofstream file(FILENAME, ios::trunc);

        if (!file)
        {
            return false;
        }

        for (size_t i = 0; i < contacts.size(); i++)
        {
            file << contacts[i].ID << '|'
                 << ProjectTools::CleanLine(contacts[i].Username) << '|'
                 << ProjectTools::CleanLine(contacts[i].Name) << '|'
                 << ProjectTools::CleanLine(contacts[i].Phone) << '|'
                 << ProjectTools::CleanLine(contacts[i].Email) << '|'
                 << ProjectTools::CleanLine(contacts[i].Group) << '|'
                 << ProjectTools::CleanLine(contacts[i].CreationDate) << endl;
        }

        file.close();
        return static_cast<bool>(file);
    }

    static int NextID(const vector<ContactItem> &contacts)
    {
        int largestID = 0;

        for (size_t i = 0; i < contacts.size(); i++)
        {
            if (contacts[i].ID > largestID)
            {
                largestID = contacts[i].ID;
            }
        }
        return largestID + 1;
    }

    static int FindContact(const vector<ContactItem> &contacts,
                           const string &username, int contactID)
    {
        for (size_t i = 0; i < contacts.size(); i++)
        {
            if (contacts[i].ID == contactID && contacts[i].Username == username)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    static void ShowContacts(const vector<ContactItem> &contacts,
                             const string &username, const string &search = "")
    {
        cout << "\n"
             << left << setw(6) << "ID"
             << setw(22) << "NAME"
             << setw(18) << "PHONE"
             << setw(30) << "EMAIL"
             << "GROUP" << endl;
        ProjectTools::ShowLine('-', 92);
        string searchText = ProjectTools::Lowercase(search);
        int shown = 0;

        for (size_t i = 0; i < contacts.size(); i++)
        {
            if (contacts[i].Username != username)
            {
                continue;
            }

            string name = ProjectTools::Lowercase(contacts[i].Name);
            string email = ProjectTools::Lowercase(contacts[i].Email);
            string phone = contacts[i].Phone;

            if (!searchText.empty() &&
                name.find(searchText) == string::npos &&
                email.find(searchText) == string::npos &&
                phone.find(searchText) == string::npos)
            {
                continue;
            }

            cout << left << setw(6) << contacts[i].ID
                 << setw(22) << contacts[i].Name.substr(0, 20)
                 << setw(18) << contacts[i].Phone.substr(0, 16)
                 << setw(30) << contacts[i].Email.substr(0, 28)
                 << contacts[i].Group << endl;
            shown++;
        }

        ProjectTools::ShowLine('-', 92);

        if (shown == 0)
        {
            cout << "No contacts found.\n";
        }
        else
        {
            cout << "Contacts shown: " << shown << endl;
        }
    }

public:
    static void ShowMenu(const string &username)
    {
        bool open = true;
        while (open)
        {
            system("cls");
            cout << "\n========== CONTACT BOOK ==========\n";
            cout << "1. Show my contacts\n";
            cout << "2. Add a contact\n";
            cout << "3. Edit a contact\n";
            cout << "4. Search contacts\n";
            cout << "5. Delete a contact\n";
            cout << "6. Return to account menu\n";

            int choice = ProjectTools::AskNumber("Choose an option (1-6): ", 1, 6);

            if (choice == 1)
            {
                View(username);
            }
            else if (choice == 2)
            {
                Add(username);
            }
            else if (choice == 3)
            {
                Edit(username);
            }
            else if (choice == 4)
            {
                Search(username);
            }
            else if (choice == 5)
            {
                Delete(username);
            }
            else
            {
                open = false;
            }
        }
    }

    static void View(const string &username)
    {
        vector<ContactItem> contacts;
        LoadAll(contacts);
        system("cls");
        cout << "\n========== MY CONTACTS ==========\n";
        ShowContacts(contacts, username);
        ProjectTools::Pause();
    }

    static void Add(const string &username)
    {
        ContactItem contact;
        contact.Username = username;
        system("cls");
        cout << "\n========== ADD CONTACT ==========\n";
        cout << "Contact name: ";
        getline(cin, contact.Name);
        cout << "Phone number: ";
        getline(cin, contact.Phone);
        cout << "Email address (optional): ";
        getline(cin, contact.Email);
        cout << "Group (Personal, Work, School, or Other): ";
        getline(cin, contact.Group);

        if (!ProjectTools::ValidSimpleText(contact.Name, 1, 60))
        {
            cout << "Name must contain 1 to 60 characters.\n";
            ProjectTools::Pause();
            return;
        }

        if (!contact.Phone.empty() &&
            !ProjectTools::ValidSimpleText(contact.Phone, 3, 24))
        {
            cout << "Phone number must contain 3 to 24 characters.\n";
            ProjectTools::Pause();
            return;
        }

        if (contact.Email.find('|') != string::npos || contact.Email.length() > 100)
        {
            cout << "Email address is too long or contains an invalid character.\n";
            ProjectTools::Pause();
            return;
        }

        if (contact.Group.empty())
        {
            contact.Group = "Personal";
        }

        if (contact.Group.length() > 24 || contact.Group.find('|') != string::npos)
        {
            cout << "Group must contain 24 characters or fewer.\n";
            ProjectTools::Pause();
            return;
        }

        vector<ContactItem> contacts;
        LoadAll(contacts);
        for (size_t i = 0; i < contacts.size(); i++)
        {
            if (contacts[i].Username == username &&
                ProjectTools::Lowercase(contacts[i].Name) ==
                    ProjectTools::Lowercase(contact.Name) &&
                contacts[i].Phone == contact.Phone)
            {
                cout << "This contact is already in your contact book.\n";
                ProjectTools::Pause();
                return;
            }
        }

        contact.ID = NextID(contacts);
        contact.CreationDate = ProjectTools::CurrentDate();
        contacts.push_back(contact);

        if (SaveAll(contacts))
        {
            ActivityLog::Add(username, "Added contact " + contact.Name);
            cout << "Contact saved.\n";
        }
        else
        {
            cout << "Unable to save the contact book.\n";
        }

        ProjectTools::Pause();
    }

    static void Edit(const string &username)
    {
        vector<ContactItem> contacts;
        LoadAll(contacts);
        system("cls");
        ShowContacts(contacts, username);
        int contactID = ProjectTools::AskNumber("Contact ID to edit: ", 1, 1000000);
        int index = FindContact(contacts, username, contactID);

        if (index < 0)
        {
            cout << "Contact not found.\n";
            ProjectTools::Pause();
            return;
        }

        string value;
        cout << "New name (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            if (!ProjectTools::ValidSimpleText(value, 1, 60))
            {
                cout << "Invalid name.\n";
                ProjectTools::Pause();
                return;
            }

            contacts[index].Name = value;
        }

        cout << "New phone (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            if (!ProjectTools::ValidSimpleText(value, 3, 24))
            {
                cout << "Invalid phone number.\n";
                ProjectTools::Pause();
                return;
            }
            contacts[index].Phone = value;
        }

        cout << "New email (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            if (value.length() > 100 || value.find('|') != string::npos)
            {
                cout << "Invalid email address.\n";
                ProjectTools::Pause();
                return;
            }

            contacts[index].Email = value;
        }

        cout << "New group (Enter to keep current): ";
        getline(cin, value);

        if (!value.empty())
        {
            if (value.length() > 24 || value.find('|') != string::npos)
            {
                cout << "Invalid group.\n";
                ProjectTools::Pause();
                return;
            }

            contacts[index].Group = value;
        }

        if (SaveAll(contacts))
        {
            ActivityLog::Add(username, "Edited contact " + to_string(contactID));
            cout << "Contact updated.\n";
        }
        else
        {
            cout << "Unable to save the contact book.\n";
        }

        ProjectTools::Pause();
    }

    static void Search(const string &username)
    {
        vector<ContactItem> contacts;
        LoadAll(contacts);
        string search;
        system("cls");
        cout << "Search by name, phone, or email: ";
        getline(cin, search);
        ShowContacts(contacts, username, search);
        ProjectTools::Pause();
    }

    static void Delete(const string &username)
    {
        vector<ContactItem> contacts;
        LoadAll(contacts);
        system("cls");
        ShowContacts(contacts, username);
        int contactID = ProjectTools::AskNumber("Contact ID to delete: ", 1, 1000000);
        int index = FindContact(contacts, username, contactID);
        if (index < 0)
        {
            cout << "Contact not found.\n";
            ProjectTools::Pause();
            return;
        }

        cout << "Delete " << contacts[index].Name << "? (y/n): ";
        string answer;
        getline(cin, answer);

        if (answer != "y" && answer != "Y" &&
            answer != "yes" && answer != "YES")
        {
            cout << "No changes were made.\n";
            ProjectTools::Pause();
            return;
        }

        contacts.erase(contacts.begin() + index);

        if (SaveAll(contacts))
        {
            ActivityLog::Add(username, "Deleted contact " + to_string(contactID));
            cout << "Contact deleted.\n";
        }
        else
        {
            cout << "Unable to save the contact book.\n";
        }

        ProjectTools::Pause();
    }
};

const string ContactHandler::FILENAME = "contacts.txt";

class Modify
{
public:
    static void modifyMenu(User &user)
    {
        while (true)
        {
            cout << "\n===== MODIFY MENU =====\n";
            cout << "1.  Change Password\n";
            cout << "2.  Change Username\n";
            cout << "3.  Change Secret Code\n";
            cout << "4.  Back to User Panel\n";
            cout << "Enter your choice: ";

            string choice;
            getline(cin, choice);

            system("cls");

            if (choice == "1")
            {
                changePassword(user);
            }
            else if (choice == "2")
            {
                changeUsername(user);
            }
            else if (choice == "3")
            {
                changeSecretCode(user);
            }
            else if (choice == "4")
            {
                break;
            }
            else
            {
                cout << " Invalid choice. Try again.\n";
            }
        }
    }
    static void changePassword(User &user)
    {
        Password NewPass;
        string password;

        cout << "Enter New Password: \n";
        cout << "Instruction For Setting Up The Password\n";
        cout << "Minimum 8 Character \n";
        cout << "1 Special Character \n";
        cout << "1 Digit Character \n";
        cout << "1 UpperCase Character \n";
        cout << "1 LowerCase Character \n";
        password = NewPass.PasswordInput();

        if (!NewPass.ValidPasswordVerification(password))
        {
            cout << "Password does not meet requirements!\n";
            return;
        }

        cout << "Confirm New Password: \n";
        string confirmation = NewPass.PasswordInput();

        if (password != confirmation)
        {
            cout << "Passwords do not match. Nothing was changed.\n";
            return;
        }

        user.SetPassword(password);

        if (FileHandler::updateUser(user.GetUsername(), user))
        {
            cout << "Password successfully updated.\n";
            ActivityLog::Add(user.GetUsername(), "Changed account password");
        }
        else
        {
            cout << "Failed to update password.\n";
        }
    }

    static void changeUsername(User &user)
    {
        string NewUsername;
        string OldUsername;

        cout << "Enter New Username: ";
        getline(cin, NewUsername);

        if (NewUsername.length() < 3 || NewUsername.length() > 32)
        {
            cout << "Username must contain between 3 and 32 characters.\n";
            return;
        }

        for (size_t i = 0; i < NewUsername.length(); i++)
        {
            unsigned char character = static_cast<unsigned char>(NewUsername[i]);
            bool allowed = isalnum(character) || character == '_' ||
                           character == '-' || character == '.';
            if (!allowed)
            {
                cout << "Use only letters, numbers, dot, dash, or underscore.\n";
                return;
            }
        }

        if (FileHandler::usernameExists(NewUsername))
        {
            cout << "Username Already Taken, Please Try Another";
            return;
        }

        OldUsername = user.GetUsername();
        user.SetUsername(NewUsername);

        if (FileHandler::updateUser(OldUsername, user))
        {
            cout << " Username successfully updated.\n";

            if (!WorkspaceData::RenameUser(OldUsername, NewUsername))
            {
                cout << "Warning: one or more workspace files could not be renamed.\n";
            }

            ActivityLog::Add(NewUsername, "Changed account username");
        }
        else
        {
            user.SetUsername(OldUsername);
            cout << " Failed to update username in file.\n";
        }
    }

    static void changeSecretCode(User &user)
    {
        string NewCode;

        cout << "Enter New Secret Code : ";
        getline(cin, NewCode);

        if (NewCode.empty())
        {
            cout << "Secret Code Can't Be Empty\n";
            return;
        }

        user.SetSecretCode(NewCode);

        if (FileHandler::updateUser(user.GetUsername(), user))
        {
            cout << " Secret code successfully updated.\n";
            ActivityLog::Add(user.GetUsername(), "Changed account recovery code");
        }
        else
        {
            cout << " Failed to update secret code.\n";
        }
    }
};
class UserPanel
{
public:
    static void show(User &user)
    {
        bool open = true;

        while (open)
        {
            cout << "\n================ USER PANEL ================\n";
            cout << "Welcome, " << user.GetUsername() << "!\n";
            cout << "1. Account Details\n";
            cout << "2. Modification (Password Required)\n";
            cout << "3. Personal Task Manager\n";
            cout << "4. Personal Notes\n";
            cout << "5. Expense Tracker\n";
            cout << "6. Contact Book\n";
            cout << "7. Recent Activity\n";
            cout << "8. Sign Out\n";
            cout << "============================================\n";
            int choice = ProjectTools::AskNumber("Enter your choice (1-8): ", 1, 8);
            system("cls");

            if (choice == 1)
            {
                cout << "\n------ Account Details ------\n";
                cout << "Username      : " << user.GetUsername() << endl;
                cout << "Password      : " << "****************" << endl;
                cout << "Recovery Code : Saved and hidden" << endl;
                cout << "Creation Date : " << user.GetCreationDate() << endl;
                cout << "\nThis account also has private tasks, notes, expenses, and contacts.\n";
                ProjectTools::Pause();
            }
            else if (choice == 2)
            {
                Password tempPass;
                string input;

                cout << "Enter your password to continue: ";
                input = tempPass.PasswordInput();

                if (user.verifyPassword(input))
                {
                    cout << "\n Password verified.\n";
                    ActivityLog::Add(user.GetUsername(), "Opened account modification menu");
                    Modify::modifyMenu(user);
                }
                else
                {
                    cout << "\nIncorrect password. Access denied to modification panel.\n";
                    ActivityLog::Add(user.GetUsername(), "Failed account modification check");
                    ProjectTools::Pause();
                }
            }
            else if (choice == 3)
            {
                TaskHandler::ShowMenu(user.GetUsername());
            }
            else if (choice == 4)
            {
                NoteHandler::ShowMenu(user.GetUsername());
            }
            else if (choice == 5)
            {
                ExpenseHandler::ShowMenu(user.GetUsername());
            }
            else if (choice == 6)
            {
                ContactHandler::ShowMenu(user.GetUsername());
            }
            else if (choice == 7)
            {
                vector<string> events = ActivityLog::ReadRecent(user.GetUsername(), 20);
                cout << "\n========== RECENT ACTIVITY ==========\n";

                for (int i = static_cast<int>(events.size()) - 1; i >= 0; i--)
                {
                    cout << events[i] << endl;
                }

                if (events.empty())
                {
                    cout << "No account activity has been recorded yet.\n";
                }

                ProjectTools::Pause();
            }
            else
            {
                cout << "\nSigned out successfully.\n";
                ActivityLog::Add(user.GetUsername(), "Signed out");
                open = false;
            }
        }
    }
};

class LoginSystem
{
public:
    void MainMenu()
    {
        while (true)
        {
            cout << "\n=========== LOGIN SYSTEM MENU ===========\n";
            cout << "1. Sign Up\n";
            cout << "2. Login\n";
            cout << "3. Exit\n";
            cout << "=========================================\n";
            int choice = ProjectTools::AskNumber("Enter your choice (1-3): ", 1, 3);
            system("cls");

            switch (choice)
            {
            case 1:
                signUp();
                break;
            case 2:
                login();
                break;
            case 3:
                cout << "\n Exiting the program. Goodbye!\n";
                return;
            default:
                cout << "\n Invalid choice. Try again.\n";
                system("cls");
                break;
            }
        }
    }

private:
    bool validUsername(const string &username)
    {
        if (username.length() < 3 || username.length() > 32)
        {
            return false;
        }
        for (size_t i = 0; i < username.length(); i++)
        {
            unsigned char character = static_cast<unsigned char>(username[i]);

            if (!isalnum(character) && character != '_' &&
                character != '-' && character != '.')
            {
                return false;
            }
        }

        return true;
    }

    void signUp()
    {
        string Username;
        string SecretCode;
        string UserPassword;

        cout << "SIGN UP \n";
        cout << "Enter a Username: ";
        getline(cin, Username);

        if (!validUsername(Username))
        {
            cout << "Use 3 to 32 letters, numbers, dots, dashes, or underscores.\n";
            ProjectTools::Pause();
            return;
        }

        if (FileHandler::usernameExists(Username))
        {
            cout << "\n Username already exists. Please try another.\n";
            return;
        }

        Password passObj;

        cout << "Create Password \n ";
        cout << "Instruction For Setting Up The Password\n";
        cout << "Minimum 8 characters, maximum 100 characters\n";
        cout << "1 Special Character \n";
        cout << "1 Digit Character \n";
        cout << "1 UpperCase Character \n";
        cout << "1 LowerCase Character \n";

        UserPassword = passObj.PasswordInput();

        if (!passObj.ValidPasswordVerification(UserPassword))
        {
            cout << "\n Password doesn't meet requirements.\n";
            system("cls");
        }
        else
        {
            cout << "Enter a Secret Code (used for recovery): ";
            getline(cin, SecretCode);
            if (SecretCode.empty() || SecretCode.length() > 100 ||
                SecretCode.find('|') != string::npos)
            {
                cout << "Recovery code must contain 1 to 100 characters.\n";
                ProjectTools::Pause();
                return;
            }

            User newUser(Username, UserPassword, SecretCode);
            if (FileHandler::saveUser(newUser))
            {
                cout << "\n Sign Up successfully!\n";
                ActivityLog::Add(Username, "Created a new account");
            }
            else
            {
                cout << "\n Failed to Sign Up. Please Try Again\n";
            }

            ProjectTools::Pause();
        }
    }

    void login()
    {
        string username;
        string password;
        bool authenticated = false;
        int attempt;
        string code;

        system("cls");
        cout << "\nLOGIN\n";
        cout << "Enter Username: ";
        getline(cin, username);

        User user;

        if (!FileHandler::findUser(username, user))
        {
            cout << "\n User Not Found. Please Try Again.\n";
            ActivityLog::Add(username, "Unsuccessful sign-in");
            ProjectTools::Pause();
            return;
        }

        Password passObj;

        for (attempt = 1; attempt <= 3; attempt++)
        {
            cout << "Enter Password (Attempt " << attempt << "): ";
            password = passObj.PasswordInput();

            if (user.verifyPassword(password))
            {
                system("cls");
                cout << "\n Login Successful! Welcome, " << user.GetUsername() << "\n";
                authenticated = true;
                ActivityLog::Add(user.GetUsername(), "Signed in successfully");
                UserPanel::show(user);
                break;
            }
            else
            {
                cout << " Incorrect Password. Please Try Again.\n";
                ActivityLog::Add(user.GetUsername(), "Unsuccessful sign-in");
            }
        }

        if (!authenticated)
        {
            cout << "\n All Password Attempts Exhausted.\n";
            cout << "Enter your Secret Code for Recovery: ";
            getline(cin, code);
            if (user.verifySecretCode(code))
            {
                system("cls");
                cout << " Correct Secret Code.\n";
                ActivityLog::Add(user.GetUsername(), "Used account recovery code");

                while (true)
                {
                    system("cls");
                    cout << "Choose An Option\n";
                    cout << "1.  Login to Account" << endl;
                    cout << "2.  Change Password" << endl;
                    int choice = ProjectTools::AskNumber("Enter choice (1-2): ", 1, 2);
                    system("cls");

                    if (choice == 1)
                    {
                        cout << "\n Login successful!" << endl;
                        UserPanel::show(user);
                        break;
                    }
                    else if (choice == 2)
                    {
                        Modify::changePassword(user);
                        return;
                    }
                    else
                    {
                        cout << "Invalid choice. Please enter 1 or 2." << endl;
                    }
                }
            }
            else
            {
                cout << " Incorrect Secret Code. Returning to Main Menu...\n";
                ActivityLog::Add(user.GetUsername(), "Failed account recovery check");
                ProjectTools::Pause();
            }
        }
    }
};

int main()
{
    LoginSystem System;
    System.LoginSystem::MainMenu();
    return 0;
}
