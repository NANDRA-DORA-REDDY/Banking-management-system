// State Management
let customers = JSON.parse(localStorage.getItem('customers')) || [];
let accounts = JSON.parse(localStorage.getItem('accounts')) || [];
let transactions = JSON.parse(localStorage.getItem('transactions')) || [];
let users = JSON.parse(localStorage.getItem('users')) || [
  { username: 'admin', password: 'admin123', role: 'Admin', customerId: 0 }
];
let currentUser = null;

function saveData() {
  localStorage.setItem('customers', JSON.stringify(customers));
  localStorage.setItem('accounts', JSON.stringify(accounts));
  localStorage.setItem('transactions', JSON.stringify(transactions));
  localStorage.setItem('users', JSON.stringify(users));
}

// Authentication
function handleLogin() {
  const userIn = document.getElementById('login-username').value;
  const passIn = document.getElementById('login-password').value;
  const found = users.find(u => u.username === userIn && u.password === passIn);

  if (found) {
    currentUser = found;
    document.getElementById('login-screen').classList.add('hidden');
    document.getElementById('dashboard-screen').classList.remove('hidden');
    document.getElementById('display-user').innerText = currentUser.username;
    document.getElementById('display-role').innerText = currentUser.role;

    // Control visibility based on user role
    const adminElems = document.querySelectorAll('.admin-only');
    adminElems.forEach(el => {
      if (currentUser.role === 'Admin') el.classList.remove('hidden');
      else el.classList.add('hidden');
    });

    switchTab('customers');
  } else {
    document.getElementById('login-error').innerText = 'Invalid username or password.';
  }
}

function handleLogout() {
  currentUser = null;
  document.getElementById('dashboard-screen').classList.add('hidden');
  document.getElementById('login-screen').classList.remove('hidden');
  document.getElementById('login-username').value = '';
  document.getElementById('login-password').value = '';
}

// UI Navigation
function switchTab(tabName) {
  document.querySelectorAll('.tab-content').forEach(el => el.classList.add('hidden'));
  document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));

  document.getElementById(`section-${tabName}`).classList.remove('hidden');
  document.getElementById(`tab-${tabName.substring(0, 4)}`).classList.add('active');

  if (tabName === 'customers') renderCustomers();
  if (tabName === 'accounts') renderAccounts();
}

function showSubForm(formId) {
  document.querySelectorAll('.sub-form').forEach(el => {
    if (!el.parentElement.classList.contains('admin-only')) {
      el.classList.add('hidden');
    }
  });
  const target = document.getElementById(formId);
  if (target) target.classList.remove('hidden');
}

// Customer Operations
function addCustomer() {
  const id = parseInt(document.getElementById('cust-id').value);
  const name = document.getElementById('cust-name').value;
  const phone = document.getElementById('cust-phone').value;
  const email = document.getElementById('cust-email').value;

  if (!id || !name) return alert('Customer ID and Name are required.');
  if (customers.some(c => c.id === id)) return alert('Customer ID already exists.');

  customers.push({ id, name, phone, email });
  saveData();
  alert('Customer added successfully.');
  renderCustomers();
}

function updateCustomer() {
  const id = parseInt(document.getElementById('up-cust-id').value);
  const cust = customers.find(c => c.id === id);
  if (!cust) return alert('Customer not found.');

  cust.name = document.getElementById('up-cust-name').value || cust.name;
  cust.phone = document.getElementById('up-cust-phone').value || cust.phone;
  cust.email = document.getElementById('up-cust-email').value || cust.email;

  saveData();
  alert('Customer updated successfully.');
  renderCustomers();
}

function deleteCustomer() {
  const id = parseInt(document.getElementById('del-cust-id').value);
  const custIndex = customers.findIndex(c => c.id === id);
  if (custIndex === -1) return alert('Customer not found.');

  // Matches C++ validation check
  if (accounts.some(a => a.customerId === id)) {
    return alert('Cannot delete customer because an account is linked to it.');
  }

  customers.splice(custIndex, 1);
  saveData();
  alert('Customer deleted successfully.');
  renderCustomers();
}

function renderCustomers() {
  let out = "ID\t| NAME\t\t| PHONE\t\t| EMAIL\n";
  out += "-".repeat(60) + "\n";
  customers.forEach(c => {
    out += `${c.id}\t| ${c.name}\t| ${c.phone}\t| ${c.email}\n`;
  });
  document.getElementById('customers-list').innerText = out;
}

// Account Operations
function createAccount() {
  const accountNo = parseInt(document.getElementById('acc-num').value);
  const customerId = parseInt(document.getElementById('acc-cust-id').value);
  const type = document.getElementById('acc-type').value;
  const balance = parseFloat(document.getElementById('acc-deposit').value);

  if (!customers.some(c => c.id === customerId)) return alert('Customer does not exist.');
  if (accounts.some(a => a.accountNo === accountNo)) return alert('Account number already exists.');

  accounts.push({ accountNo, customerId, type, balance: balance || 0 });
  saveData();
  alert('Account created successfully.');
  renderAccounts();
}

function searchAccount() {
  const accNum = parseInt(document.getElementById('search-acc-num').value);
  const a = accounts.find(acc => acc.accountNo === accNum);
  if (!a) return alert('Account not found.');

  const c = customers.find(cust => cust.id === a.customerId);
  let out = `Account Number : ${a.accountNo}\n`;
  out += `Customer ID    : ${a.customerId}\n`;
  out += `Customer Name  : ${c ? c.name : 'Unknown'}\n`;
  out += `Account Type   : ${a.type}\n`;
  out += `Balance        : $${a.balance.toFixed(2)}\n`;

  document.getElementById('accounts-list').innerText = out;
}

function updateAccount() {
  const accNum = parseInt(document.getElementById('up-acc-num').value);
  const a = accounts.find(acc => acc.accountNo === accNum);
  if (!a) return alert('Account not found.');

  a.type = document.getElementById('up-acc-type').value;
  saveData();
  alert('Account type updated successfully.');
  renderAccounts();
}

function renderAccounts() {
  let out = "ACCOUNT\t| CUST ID\t| TYPE\t\t| BALANCE\n";
  out += "-".repeat(55) + "\n";
  accounts.forEach(a => {
    out += `${a.accountNo}\t| ${a.customerId}\t\t| ${a.type}\t| $${a.balance.toFixed(2)}\n`;
  });
  document.getElementById('accounts-list').innerText = out;
}

// Transaction Operations
function processTransaction(type) {
  const accNum = parseInt(document.getElementById('trans-acc-num').value);
  const amount = parseFloat(document.getElementById('trans-amount').value);

  const acc = accounts.find(a => a.accountNo === accNum);
  if (!acc) return alert('Account not found.');

  // Access validation for Customer role
  if (currentUser.role === 'Customer' && acc.customerId !== currentUser.customerId) {
    return alert('Access denied. This account does not belong to you.');
  }

  if (amount <= 0 || isNaN(amount)) return alert('Enter a valid amount greater than zero.');
  if (type === 'Withdrawal' && amount > acc.balance) return alert('Insufficient balance.');

  acc.balance += (type === 'Deposit' ? amount : -amount);
  transactions.push({
    id: Date.now(),
    accountNo: accNum,
    type: type,
    amount: amount,
    balanceAfter: acc.balance,
    date: new Date().toLocaleString(),
    description: `${type} via Web UI`
  });

  saveData();
  document.getElementById('transactions-output').innerText = `${type} successful!\nNew Balance: $${acc.balance.toFixed(2)}`;
}

function checkBalance() {
  const accNum = parseInt(document.getElementById('trans-acc-num').value);
  const acc = accounts.find(a => a.accountNo === accNum);
  if (!acc) return alert('Account not found.');

  if (currentUser.role === 'Customer' && acc.customerId !== currentUser.customerId) {
    return alert('Access denied. This account does not belong to you.');
  }

  document.getElementById('transactions-output').innerText = `Account: ${acc.accountNo}\nCurrent Balance: $${acc.balance.toFixed(2)}`;
}

function viewHistory() {
  const accNum = parseInt(document.getElementById('trans-acc-num').value);
  const history = transactions.filter(t => t.accountNo === accNum);

  if (history.length === 0) {
    document.getElementById('transactions-output').innerText = "No transactions found for this account.";
    return;
  }

  let out = "TYPE\t\t| AMOUNT\t| BALANCE AFTER\t| DATE\n";
  out += "-".repeat(65) + "\n";
  history.forEach(t => {
    out += `${t.type}\t| $${t.amount.toFixed(2)}\t| $${t.balanceAfter.toFixed(2)}\t\t| ${t.date}\n`;
  });
  document.getElementById('transactions-output').innerText = out;
}

// Security Operations
function changePassword() {
  const oldP = document.getElementById('old-pass').value;
  const newP = document.getElementById('new-pass').value;
  const confP = document.getElementById('confirm-pass').value;

  if (currentUser.password !== oldP) return alert('Incorrect current password.');
  if (newP !== confP) return alert('New passwords do not match.');

  currentUser.password = newP;
  saveData();
  alert('Password changed successfully.');
}

function registerUser() {
  const username = document.getElementById('reg-username').value;
  const password = document.getElementById('reg-password').value;
  const role = document.getElementById('reg-role').value;
  const customerId = parseInt(document.getElementById('reg-cust-id').value) || 0;

  if (users.some(u => u.username === username)) return alert('Username already exists.');

  users.push({ username, password, role, customerId });
  saveData();
  alert('User registered successfully.');
}

// Reports
function generateReport(type) {
  let out = "";
  if (type === 'summary') {
    const totalBal = accounts.reduce((sum, a) => sum + a.balance, 0);
    out += "============================================\n";
    out += "       BANK MANAGEMENT SYSTEM REPORT\n";
    out += "============================================\n";
    out += `Total Customers    : ${customers.length}\n`;
    out += `Total Accounts     : ${accounts.length}\n`;
    out += `Total Transactions : ${transactions.length}\n`;
    out += `Total Bank Balance : $${totalBal.toFixed(2)}\n`;
    out += "============================================\n";
  } else if (type === 'transactions') {
    const dep = transactions.filter(t => t.type === 'Deposit').reduce((sum, t) => sum + t.amount, 0);
    out += `Total Deposits   : $${dep.toFixed(2)}\n`;
  }
  document.getElementById('reports-output').innerText = out;
}
