#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <limits>
#include <algorithm>
#include <cctype>
#include <cstring>

using namespace std;

struct Customer {
    int id;
    string name;
    string phone;
    string email;
};

struct Account {
    int accountNo;
    int customerId;
    string type;
    double balance;
};

struct Transaction {
    long long id;
    int accountNo;
    string type;
    double amount;
    double balanceAfter;
    string date;
    string description;
};

struct User {
    string username;
    string password;
    string role;
    int customerId;
};

class Bank {
private:
    vector<Customer> customers;
    vector<Account> accounts;
    vector<Transaction> transactions;
    vector<User> users;

    const string CUSTOMER_FILE;
    const string ACCOUNT_FILE;
    const string TRANSACTION_FILE;
    const string USER_FILE;

    long long nextTransactionId;

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int readInt(const string &prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Invalid input. Please enter a number.\n";
            clearInput();
        }
    }

    double readAmount(const string &prompt) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (value >= 0)
                    return value;
            }
            cout << "Invalid amount. Please enter a non-negative number.\n";
            clearInput();
        }
    }

    string readLine(const string &prompt) {
        string value;
        cout << prompt;
        getline(cin, value);
        return value;
    }

    string currentDateTime() {
        time_t now = time(NULL);
        tm *local = localtime(&now);
        char buffer[30];
        if (local != NULL)
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
        else
            strcpy(buffer, "Unknown");
        return string(buffer);
    }

    Customer* findCustomer(int id) {
        for (size_t i = 0; i < customers.size(); ++i)
            if (customers[i].id == id)
                return &customers[i];
        return NULL;
    }

    Account* findAccount(int accountNo) {
        for (size_t i = 0; i < accounts.size(); ++i)
            if (accounts[i].accountNo == accountNo)
                return &accounts[i];
        return NULL;
    }

    User* findUser(const string &username) {
        for (size_t i = 0; i < users.size(); ++i)
            if (users[i].username == username)
                return &users[i];
        return NULL;
    }

    bool customerIdExists(int id) { return findCustomer(id) != NULL; }
    bool accountNoExists(int no) { return findAccount(no) != NULL; }
    bool usernameExists(const string &name) { return findUser(name) != NULL; }

    void saveCustomers() {
        ofstream file(CUSTOMER_FILE.c_str());
        if (!file) { cout << "Error: Cannot open customer file.\n"; return; }
        for (size_t i = 0; i < customers.size(); ++i) {
            const Customer &c = customers[i];
            file << c.id << '|' << c.name << '|' << c.phone << '|' << c.email << '\n';
        }
    }

    void loadCustomers() {
        ifstream file(CUSTOMER_FILE.c_str());
        if (!file) return;
        customers.clear();
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            Customer c;
            string id;
            getline(ss, id, '|');
            getline(ss, c.name, '|');
            getline(ss, c.phone, '|');
            getline(ss, c.email);
            if (!id.empty()) {
                try { c.id = atoi(id.c_str()); customers.push_back(c); }
                catch (...) {}
            }
        }
    }

    void saveAccounts() {
        ofstream file(ACCOUNT_FILE.c_str());
        if (!file) { cout << "Error: Cannot open account file.\n"; return; }
        file << fixed << setprecision(2);
        for (size_t i = 0; i < accounts.size(); ++i) {
            const Account &a = accounts[i];
            file << a.accountNo << '|' << a.customerId << '|' << a.type << '|' << a.balance << '\n';
        }
    }

    void loadAccounts() {
        ifstream file(ACCOUNT_FILE.c_str());
        if (!file) return;
        accounts.clear();
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            Account a;
            string accountNo, customerId, balance;
            getline(ss, accountNo, '|');
            getline(ss, customerId, '|');
            getline(ss, a.type, '|');
            getline(ss, balance);
            if (!accountNo.empty()) {
                a.accountNo = atoi(accountNo.c_str());
                a.customerId = atoi(customerId.c_str());
                a.balance = atof(balance.c_str());
                accounts.push_back(a);
            }
        }
    }

    void saveTransactions() {
        ofstream file(TRANSACTION_FILE.c_str());
        if (!file) { cout << "Error: Cannot open transaction file.\n"; return; }
        file << fixed << setprecision(2);
        for (size_t i = 0; i < transactions.size(); ++i) {
            const Transaction &t = transactions[i];
            file << t.id << '|' << t.accountNo << '|' << t.type << '|'
                 << t.amount << '|' << t.balanceAfter << '|' << t.date << '|'
                 << t.description << '\n';
        }
    }

    void loadTransactions() {
        ifstream file(TRANSACTION_FILE.c_str());
        if (!file) return;
        transactions.clear();
        long long maxId = 0;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            Transaction t;
            string id, accountNo, amount, balanceAfter;
            getline(ss, id, '|');
            getline(ss, accountNo, '|');
            getline(ss, t.type, '|');
            getline(ss, amount, '|');
            getline(ss, balanceAfter, '|');
            getline(ss, t.date, '|');
            getline(ss, t.description);
            if (!id.empty()) {
                t.id = atoll(id.c_str());
                t.accountNo = atoi(accountNo.c_str());
                t.amount = atof(amount.c_str());
                t.balanceAfter = atof(balanceAfter.c_str());
                transactions.push_back(t);
                if (t.id > maxId) maxId = t.id;
            }
        }
        nextTransactionId = maxId + 1;
    }

    void saveUsers() {
        ofstream file(USER_FILE.c_str());
        if (!file) { cout << "Error: Cannot open user file.\n"; return; }
        for (size_t i = 0; i < users.size(); ++i) {
            const User &u = users[i];
            file << u.username << '|' << u.password << '|' << u.role << '|' << u.customerId << '\n';
        }
    }

    void createDefaultAdmin() {
        User admin;
        admin.username = "admin";
        admin.password = "admin123";
        admin.role = "Admin";
        admin.customerId = 0;
        users.push_back(admin);
        saveUsers();
    }

    void loadUsers() {
        ifstream file(USER_FILE.c_str());
        if (!file) {
            createDefaultAdmin();
            return;
        }
        users.clear();
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            User u;
            string customerId;
            getline(ss, u.username, '|');
            getline(ss, u.password, '|');
            getline(ss, u.role, '|');
            getline(ss, customerId);
            if (!u.username.empty()) {
                u.customerId = customerId.empty() ? 0 : atoi(customerId.c_str());
                users.push_back(u);
            }
        }
        if (users.empty()) createDefaultAdmin();
    }

    void saveAll() {
        saveCustomers();
        saveAccounts();
        saveTransactions();
        saveUsers();
    }

    void loadAll() {
        loadCustomers();
        loadAccounts();
        loadTransactions();
        loadUsers();
    }

    void addCustomer() {
        cout << "\n========== ADD CUSTOMER ==========\n";
        Customer c;
        c.id = readInt("Enter Customer ID: ");
        if (customerIdExists(c.id)) { cout << "Customer ID already exists.\n"; return; }
        c.name = readLine("Enter Name: ");
        c.phone = readLine("Enter Phone: ");
        c.email = readLine("Enter Email: ");
        customers.push_back(c);
        saveCustomers();
        cout << "Customer added successfully.\n";
    }

    void viewCustomers() {
        cout << "\n========== CUSTOMER LIST ==========\n";
        if (customers.empty()) { cout << "No customers found.\n"; return; }
        cout << left << setw(10) << "ID" << setw(25) << "NAME" << setw(18) << "PHONE" << setw(30) << "EMAIL" << '\n';
        cout << string(83, '-') << '\n';
        for (size_t i = 0; i < customers.size(); ++i) {
            const Customer &c = customers[i];
            cout << left << setw(10) << c.id << setw(25) << c.name << setw(18) << c.phone << setw(30) << c.email << '\n';
        }
    }

    void updateCustomer() {
        cout << "\n========== UPDATE CUSTOMER ==========\n";
        int id = readInt("Enter Customer ID: ");
        Customer *c = findCustomer(id);
        if (!c) { cout << "Customer not found.\n"; return; }
        c->name = readLine("New Name: ");
        c->phone = readLine("New Phone: ");
        c->email = readLine("New Email: ");
        saveCustomers();
        cout << "Customer updated successfully.\n";
    }

    void deleteCustomer() {
        cout << "\n========== DELETE CUSTOMER ==========\n";
        int id = readInt("Enter Customer ID: ");
        if (!findCustomer(id)) { cout << "Customer not found.\n"; return; }
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].customerId == id) {
                cout << "Cannot delete customer because an account is linked to it.\n";
                return;
            }
        }
        for (vector<Customer>::iterator it = customers.begin(); it != customers.end(); ++it) {
            if (it->id == id) { customers.erase(it); break; }
        }
        saveCustomers();
        cout << "Customer deleted successfully.\n";
    }

    void createAccount() {
        cout << "\n========== CREATE ACCOUNT ==========\n";
        int customerId = readInt("Enter Customer ID: ");
        Customer *customer = findCustomer(customerId);
        if (!customer) { cout << "Customer not found. Create the customer first.\n"; return; }
        Account a;
        a.accountNo = readInt("Enter Account Number: ");
        if (accountNoExists(a.accountNo)) { cout << "Account number already exists.\n"; return; }
        a.customerId = customerId;
        a.type = readLine("Enter Account Type (Savings/Current): ");
        a.balance = readAmount("Enter Initial Deposit: ");
        accounts.push_back(a);
        saveAccounts();
        cout << "Account created successfully for " << customer->name << ".\n";
    }

    void viewAccounts() {
        cout << "\n========== ACCOUNT LIST ==========\n";
        if (accounts.empty()) { cout << "No accounts found.\n"; return; }
        cout << left << setw(12) << "ACCOUNT" << setw(12) << "CUSTOMER" << setw(25) << "NAME" << setw(15) << "TYPE" << setw(15) << "BALANCE" << '\n';
        cout << string(79, '-') << '\n';
        cout << fixed << setprecision(2);
        for (size_t i = 0; i < accounts.size(); ++i) {
            const Account &a = accounts[i];
            Customer *c = findCustomer(a.customerId);
            cout << left << setw(12) << a.accountNo << setw(12) << a.customerId << setw(25) << (c ? c->name : "Unknown") << setw(15) << a.type << setw(15) << a.balance << '\n';
        }
    }

    void searchAccount() {
        cout << "\n========== SEARCH ACCOUNT ==========\n";
        int accountNo = readInt("Enter Account Number: ");
        Account *a = findAccount(accountNo);
        if (!a) { cout << "Account not found.\n"; return; }
        Customer *c = findCustomer(a->customerId);
        cout << "\nAccount Number : " << a->accountNo << '\n';
        cout << "Customer ID    : " << a->customerId << '\n';
        cout << "Customer Name  : " << (c ? c->name : "Unknown") << '\n';
        cout << "Account Type   : " << a->type << '\n';
        cout << "Balance        : " << fixed << setprecision(2) << a->balance << '\n';
    }

    void updateAccount() {
        cout << "\n========== UPDATE ACCOUNT ==========\n";
        int accountNo = readInt("Enter Account Number: ");
        Account *a = findAccount(accountNo);
        if (!a) { cout << "Account not found.\n"; return; }
        a->type = readLine("Enter New Account Type: ");
        saveAccounts();
        cout << "Account updated successfully.\n";
    }

    void addTransaction(int accountNo, const string &type, double amount, double balanceAfter, const string &description) {
        Transaction t;
        t.id = nextTransactionId++;
        t.accountNo = accountNo;
        t.type = type;
        t.amount = amount;
        t.balanceAfter = balanceAfter;
        t.date = currentDateTime();
        t.description = description;
        transactions.push_back(t);
        saveTransactions();
    }

    void deposit() {
        cout << "\n========== DEPOSIT ==========\n";
        int accountNo = readInt("Enter Account Number: ");
        Account *a = findAccount(accountNo);
        if (!a) { cout << "Account not found.\n"; return; }
        double amount = readAmount("Enter Deposit Amount: ");
        if (amount <= 0) { cout << "Deposit must be greater than zero.\n"; return; }
        a->balance += amount;
        addTransaction(a->accountNo, "Deposit", amount, a->balance, "Cash deposit");
        saveAccounts();
        cout << "Deposit successful.\nNew Balance: " << fixed << setprecision(2) << a->balance << '\n';
    }

    void withdraw() {
        cout << "\n========== WITHDRAWAL ==========\n";
        int accountNo = readInt("Enter Account Number: ");
        Account *a = findAccount(accountNo);
        if (!a) { cout << "Account not found.\n"; return; }
        double amount = readAmount("Enter Withdrawal Amount: ");
        if (amount <= 0) { cout << "Withdrawal must be greater than zero.\n"; return; }
        if (amount > a->balance) { cout << "Insufficient balance.\n"; return; }
        a->balance -= amount;
        addTransaction(a->accountNo, "Withdrawal", amount, a->balance, "Cash withdrawal");
        saveAccounts();
        cout << "Withdrawal successful.\nNew Balance: " << fixed << setprecision(2) << a->balance << '\n';
    }

    void checkBalance() {
        cout << "\n========== CHECK BALANCE ==========\n";
        int accountNo = readInt("Enter Account Number: ");
        Account *a = findAccount(accountNo);
        if (!a) { cout << "Account not found.\n"; return; }
        cout << "Account Number: " << a->accountNo << '\n';
        cout << "Balance: " << fixed << setprecision(2) << a->balance << '\n';
    }

    void viewTransactions() {
        cout << "\n========== TRANSACTION HISTORY ==========\n";
        int accountNo = readInt("Enter Account Number: ");
        bool found = false;
        cout << left << setw(8) << "ID" << setw(15) << "TYPE" << setw(15) << "AMOUNT" << setw(18) << "BALANCE" << setw(22) << "DATE" << "DESCRIPTION\n";
        cout << string(100, '-') << '\n';
        for (size_t i = 0; i < transactions.size(); ++i) {
            const Transaction &t = transactions[i];
            if (t.accountNo == accountNo) {
                found = true;
                cout << left << setw(8) << t.id << setw(15) << t.type << fixed << setprecision(2) << setw(15) << t.amount << setw(18) << t.balanceAfter << setw(22) << t.date << t.description << '\n';
            }
        }
        if (!found) cout << "No transactions found for this account.\n";
    }

    User* login() {
        cout << "\n========== LOGIN ==========\n";
        string username = readLine("Username: ");
        string password = readLine("Password: ");
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].username == username && users[i].password == password) {
                cout << "Login successful.\nRole: " << users[i].role << '\n';
                return &users[i];
            }
        }
        cout << "Invalid username or password.\n";
        return NULL;
    }

    void registerUser() {
        cout << "\n========== REGISTER USER ==========\n";
        string username = readLine("Enter Username: ");
        if (usernameExists(username)) { cout << "Username already exists.\n"; return; }
        string password = readLine("Enter Password: ");
        string role = readLine("Enter Role (Admin/Customer): ");
        for (size_t i = 0; i < role.size(); ++i) role[i] = (char)tolower((unsigned char)role[i]);
        if (role != "admin" && role != "customer") { cout << "Invalid role. Use Admin or Customer.\n"; return; }
        User u;
        u.username = username;
        u.password = password;
        u.role = (role == "admin") ? "Admin" : "Customer";
        u.customerId = 0;
        if (u.role == "Customer") {
            u.customerId = readInt("Enter Customer ID: ");
            if (!customerIdExists(u.customerId)) { cout << "Customer does not exist.\n"; return; }
        }
        users.push_back(u);
        saveUsers();
        cout << "User registered successfully.\n";
    }

    void changePassword(User *currentUser) {
        cout << "\n========== CHANGE PASSWORD ==========\n";
        string oldPassword = readLine("Enter Current Password: ");
        if (currentUser->password != oldPassword) { cout << "Incorrect current password.\n"; return; }
        string newPassword = readLine("Enter New Password: ");
        string confirm = readLine("Confirm New Password: ");
        if (newPassword != confirm) { cout << "Passwords do not match.\n"; return; }
        currentUser->password = newPassword;
        saveUsers();
        cout << "Password changed successfully.\n";
    }

    void customerReport() { viewCustomers(); }

    void accountReport() {
        cout << "\n========== ACCOUNT REPORT ==========\n";
        double totalBalance = 0;
        cout << left << setw(12) << "ACCOUNT" << setw(12) << "CUSTOMER" << setw(25) << "NAME" << setw(15) << "TYPE" << setw(15) << "BALANCE" << '\n';
        cout << string(79, '-') << '\n';
        for (size_t i = 0; i < accounts.size(); ++i) {
            const Account &a = accounts[i];
            Customer *c = findCustomer(a.customerId);
            totalBalance += a.balance;
            cout << left << setw(12) << a.accountNo << setw(12) << a.customerId << setw(25) << (c ? c->name : "Unknown") << setw(15) << a.type << fixed << setprecision(2) << setw(15) << a.balance << '\n';
        }
        cout << "\nTotal Accounts : " << accounts.size() << '\n';
        cout << "Total Balance  : " << fixed << setprecision(2) << totalBalance << '\n';
    }

    void transactionReport() {
        cout << "\n========== TRANSACTION REPORT ==========\n";
        double deposits = 0, withdrawals = 0;
        for (size_t i = 0; i < transactions.size(); ++i) {
            if (transactions[i].type == "Deposit") deposits += transactions[i].amount;
            else if (transactions[i].type == "Withdrawal") withdrawals += transactions[i].amount;
        }
        cout << "Total Transactions : " << transactions.size() << '\n';
        cout << "Total Deposits     : " << fixed << setprecision(2) << deposits << '\n';
        cout << "Total Withdrawals  : " << withdrawals << '\n';
        cout << "Net Transaction    : " << deposits - withdrawals << '\n';
    }

    void fullReport() {
        cout << "\n============================================\n";
        cout << "       BANK MANAGEMENT SYSTEM REPORT\n";
        cout << "============================================\n";
        double totalBalance = 0, deposits = 0, withdrawals = 0;
        for (size_t i = 0; i < accounts.size(); ++i) totalBalance += accounts[i].balance;
        for (size_t i = 0; i < transactions.size(); ++i) {
            if (transactions[i].type == "Deposit") deposits += transactions[i].amount;
            else if (transactions[i].type == "Withdrawal") withdrawals += transactions[i].amount;
        }
        cout << "Total Customers    : " << customers.size() << '\n';
        cout << "Total Accounts     : " << accounts.size() << '\n';
        cout << "Total Transactions : " << transactions.size() << '\n';
        cout << "Total Bank Balance : " << fixed << setprecision(2) << totalBalance << '\n';
        cout << "Total Deposits     : " << deposits << '\n';
        cout << "Total Withdrawals  : " << withdrawals << '\n';
        cout << "============================================\n";
    }

    void customerMenu() {
        while (true) {
            cout << "\n========== CUSTOMER MANAGEMENT ==========\n1. Add Customer\n2. View Customers\n3. Update Customer\n4. Delete Customer\n5. Back\n";
            int choice = readInt("Enter choice: ");
            switch (choice) {
                case 1: addCustomer(); break;
                case 2: viewCustomers(); break;
                case 3: updateCustomer(); break;
                case 4: deleteCustomer(); break;
                case 5: return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    void accountMenu() {
        while (true) {
            cout << "\n========== ACCOUNT MANAGEMENT ==========\n1. Create Account\n2. View Accounts\n3. Search Account\n4. Update Account\n5. Back\n";
            int choice = readInt("Enter choice: ");
            switch (choice) {
                case 1: createAccount(); break;
                case 2: viewAccounts(); break;
                case 3: searchAccount(); break;
                case 4: updateAccount(); break;
                case 5: return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    void transactionMenu() {
        while (true) {
            cout << "\n========== TRANSACTION MANAGEMENT ==========\n1. Deposit\n2. Withdraw\n3. Check Balance\n4. Transaction History\n5. Back\n";
            int choice = readInt("Enter choice: ");
            switch (choice) {
                case 1: deposit(); break;
                case 2: withdraw(); break;
                case 3: checkBalance(); break;
                case 4: viewTransactions(); break;
                case 5: return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    void reportMenu() {
        while (true) {
            cout << "\n========== REPORTS & SUMMARY ==========\n1. Customer Report\n2. Account Report\n3. Transaction Report\n4. Complete Summary\n5. Back\n";
            int choice = readInt("Enter choice: ");
            switch (choice) {
                case 1: customerReport(); break;
                case 2: accountReport(); break;
                case 3: transactionReport(); break;
                case 4: fullReport(); break;
                case 5: return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    void securityMenu(User *currentUser) {
        while (true) {
            cout << "\n========== SECURITY & LOGIN ==========\n1. Change Password\n";
            if (currentUser->role == "Admin") cout << "2. Register User\n";
            cout << "3. Back\n";
            int choice = readInt("Enter choice: ");
            if (choice == 1) changePassword(currentUser);
            else if (choice == 2 && currentUser->role == "Admin") registerUser();
            else if (choice == 3) return;
            else cout << "Invalid choice.\n";
        }
    }

    void adminMenu(User *user) {
        while (true) {
            cout << "\n============================================\n";
            cout << "       BANKING MANAGEMENT SYSTEM\n";
            cout << "              ADMIN MENU\n";
            cout << "============================================\n";
            cout << "1. Customer Management\n2. Account Management\n3. Transaction Management\n4. Security & Login\n5. Reports & Summary\n6. Save All Data\n7. Logout\n";
            cout << "============================================\n";
            int choice = readInt("Enter choice: ");
            switch (choice) {
                case 1: customerMenu(); break;
                case 2: accountMenu(); break;
                case 3: transactionMenu(); break;
                case 4: securityMenu(user); break;
                case 5: reportMenu(); break;
                case 6: saveAll(); cout << "All data saved successfully.\n"; break;
                case 7: cout << "Logged out successfully.\n"; return;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    bool isMyAccount(int accountNo, int customerId) {
        Account *a = findAccount(accountNo);
        return a != NULL && a->customerId == customerId;
    }

    void customerUserMenu(User *user) {
        while (true) {
            cout << "\n============================================\n";
            cout << "       BANKING MANAGEMENT SYSTEM\n";
            cout << "           CUSTOMER MENU\n";
            cout << "============================================\n";
            cout << "1. View My Account\n2. Check Balance\n3. Deposit\n4. Withdraw\n5. Transaction History\n6. Change Password\n7. Logout\n";
            cout << "============================================\n";
            int choice = readInt("Enter choice: ");

            if (choice == 1) {
                cout << "\n========== MY ACCOUNTS ==========\n";
                Customer *c = findCustomer(user->customerId);
                if (!c) { cout << "Customer record not found.\n"; continue; }
                cout << "Customer ID : " << c->id << '\n';
                cout << "Name        : " << c->name << '\n';
                cout << "Phone       : " << c->phone << '\n';
                cout << "Email       : " << c->email << '\n';
                bool found = false;
                for (size_t i = 0; i < accounts.size(); ++i) {
                    if (accounts[i].customerId == user->customerId) {
                        found = true;
                        cout << "\nAccount No : " << accounts[i].accountNo << '\n';
                        cout << "Type       : " << accounts[i].type << '\n';
                        cout << "Balance    : " << fixed << setprecision(2) << accounts[i].balance << '\n';
                    }
                }
                if (!found) cout << "No account linked to this customer.\n";
            }
            else if (choice >= 2 && choice <= 5) {
                int accountNo = readInt("Enter Your Account Number: ");
                if (!isMyAccount(accountNo, user->customerId)) {
                    cout << "Access denied. This account does not belong to you.\n";
                    continue;
                }
                Account *a = findAccount(accountNo);
                if (choice == 2) {
                    cout << "Current Balance: " << fixed << setprecision(2) << a->balance << '\n';
                }
                else if (choice == 3) {
                    double amount = readAmount("Enter Deposit Amount: ");
                    if (amount <= 0) cout << "Amount must be greater than zero.\n";
                    else {
                        a->balance += amount;
                        addTransaction(a->accountNo, "Deposit", amount, a->balance, "Customer deposit");
                        saveAccounts();
                        cout << "Deposit successful.\nNew Balance: " << fixed << setprecision(2) << a->balance << '\n';
                    }
                }
                else if (choice == 4) {
                    double amount = readAmount("Enter Withdrawal Amount: ");
                    if (amount <= 0) cout << "Amount must be greater than zero.\n";
                    else if (amount > a->balance) cout << "Insufficient balance.\n";
                    else {
                        a->balance -= amount;
                        addTransaction(a->accountNo, "Withdrawal", amount, a->balance, "Customer withdrawal");
                        saveAccounts();
                        cout << "Withdrawal successful.\nNew Balance: " << fixed << setprecision(2) << a->balance << '\n';
                    }
                }
                else {
                    bool found = false;
                    cout << "\n========== MY TRANSACTIONS ==========\n";
                    for (size_t i = 0; i < transactions.size(); ++i) {
                        const Transaction &t = transactions[i];
                        if (t.accountNo == accountNo) {
                            found = true;
                            cout << "ID: " << t.id << '\n';
                            cout << "Type: " << t.type << '\n';
                            cout << "Amount: " << fixed << setprecision(2) << t.amount << '\n';
                            cout << "Balance After: " << t.balanceAfter << '\n';
                            cout << "Date: " << t.date << '\n';
                            cout << "Description: " << t.description << '\n';
                            cout << "--------------------------------\n";
                        }
                    }
                    if (!found) cout << "No transactions found.\n";
                }
            }
            else if (choice == 6) changePassword(user);
            else if (choice == 7) { cout << "Logged out successfully.\n"; return; }
            else cout << "Invalid choice.\n";
        }
    }

public:
    Bank()
        : CUSTOMER_FILE("customers.txt"),
          ACCOUNT_FILE("accounts.txt"),
          TRANSACTION_FILE("transactions.txt"),
          USER_FILE("users.txt"),
          nextTransactionId(1) {
        loadAll();
    }

    void run() {
        cout << "\n============================================\n";
        cout << "     WELCOME TO BANKING MANAGEMENT SYSTEM\n";
        cout << "============================================\n";
        cout << "Default admin login on first run:\n";
        cout << "Username: admin\nPassword: admin123\n";

        while (true) {
            cout << "\n========== MAIN MENU ==========\n1. Login\n2. Exit\n";
            int choice = readInt("Enter choice: ");
            if (choice == 1) {
                User *user = login();
                if (user != NULL) {
                    if (user->role == "Admin") adminMenu(user);
                    else customerUserMenu(user);
                }
            }
            else if (choice == 2) {
                saveAll();
                cout << "All data saved.\n";
                cout << "Thank you for using the Banking Management System.\n";
                break;
            }
            else cout << "Invalid choice.\n";
        }
    }
};

int main() {
    Bank bank;
    bank.run();
    return 0;
}
