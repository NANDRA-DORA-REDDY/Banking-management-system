# Banking Management System

A full-featured Banking Management System built for the web using HTML5, CSS3, and JavaScript. This project is hosted live on **GitHub Pages** and includes persistent browser storage (`localStorage`) along with role-based authentication (Admin and Customer).

The repository also includes the original compiled C++ source code (`BankingManagementSystem.cpp`) from which the web system's logic and validation rules were modeled.

---

## 🚀 Live Demo

Check out the live application on GitHub Pages:
`(https://nandra-dora-reddy.github.io/Banking-management-system/)` 

---

---

## ✨ Features

### 👑 Admin Features
* **Customer Management:** Add new customers, view customer details, update existing info, and delete customers (with linked account protection).
* **Account Management:** Create accounts, update account types, search specific accounts, and view all system accounts.
* **User Management:** Register new Admin or Customer accounts linked to specific customer IDs.
* **System Reports:** Generate total bank balances, active customer counts, transaction logs, and summary reports.

### 👤 Customer Features
* **Account Overview:** View linked account numbers, current balances, and account types.
* **Transactions:** Deposit funds, withdraw money (with real-time balance checks), and check balance.
* **Transaction History:** View itemized logs of all previous account activities.
* **Security:** Change account passwords securely.

---

## 📂 Repository Structure

```text
banking-system/
├── index.html                  # Main Web Application Interface
├── style.css                   # Responsive Styling & Layouts
├── app.js                      # Application Logic & LocalStorage Engine
├── BankingManagementSystem.cpp # Original C++ Console Application Source Code
├── README.md                   # Project Documentation
└── LICENSE                     # Project License
