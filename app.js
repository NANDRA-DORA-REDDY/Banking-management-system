// LocalStorage Initialization
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

function login() {
  const userIn = document.getElementById('username').value;
  const passIn = document.getElementById('password').value;
  const found = users.find(u => u.username === userIn && u.password === passIn);

  if (found) {
    currentUser = found;
    document.getElementById('login-view').classList.add('hidden');
    document.getElementById('dashboard-view').classList.remove('hidden');
    document.getElementById('user-display').innerText = currentUser.username;
    document.getElementById('role-display').innerText = currentUser.role;

    if (currentUser.role === 'Admin') {
      document.getElementById('admin-panel').classList.remove('hidden');
    }
    renderSummary();
  } else {
    document.getElementById('login-error').innerText = 'Invalid credentials';
  }
}

function logout() {
  currentUser = null;
  document.getElementById('dashboard-view').classList.add('hidden');
  document.getElementById('login-view').classList.remove('hidden');
}

function showSection(id) {
  document.getElementById('create-customer').classList.add('hidden');
  document.getElementById('create-account').classList.add('hidden');
  document.getElementById('deposit-withdraw').classList.add('hidden');
  document.getElementById(id).classList.remove('hidden');
}

function addCustomer() {
  const id = parseInt(document.getElementById('cust-id').value);
  const name = document.getElementById('cust-name').value;
  const phone = document.getElementById('cust-phone').value;
  const email = document.getElementById('cust-email').value;

  if (customers.some(c => c.id === id)) return alert('Customer ID exists');

  customers.push({ id, name, phone, email });
  saveData();
  alert('Customer added!');
  renderSummary();
}

function createAccount() {
  const accountNo = parseInt(document.getElementById('acc-num').value);
  const customerId = parseInt(document.getElementById('acc-cust-id').value);
  const type = document.getElementById('acc-type').value;
  const balance = parseFloat(document.getElementById('acc-deposit').value);

  if (!customers.some(c => c.id === customerId)) return alert('Customer does not exist');
  if (accounts.some(a => a.accountNo === accountNo)) return alert('Account number exists');

  accounts.push({ accountNo, customerId, type, balance });
  saveData();
  alert('Account created!');
  renderSummary();
}

function handleTransaction(type) {
  const accountNo = parseInt(document.getElementById('trans-acc-num').value);
  const amount = parseFloat(document.getElementById('trans-amount').value);
  const account = accounts.find(a => a.accountNo === accountNo);

  if (!account) return alert('Account not found');
  if (amount <= 0) return alert('Enter valid amount');

  if (type === 'Withdrawal' && account.balance < amount) {
    return alert('Insufficient balance');
  }

  account.balance += (type === 'Deposit' ? amount : -amount);
  transactions.push({
    id: Date.now(),
    accountNo,
    type,
    amount,
    balanceAfter: account.balance,
    date: new Date().toLocaleString()
  });

  saveData();
  alert(`${type} successful!`);
  renderSummary();
}

function renderReports() {
  let output = "=== SYSTEM REPORT ===\n";
  output += `Total Customers: ${customers.length}\n`;
  output += `Total Accounts: ${accounts.length}\n`;
  output += `Total Transactions: ${transactions.length}\n\n`;
  output += "--- ACCOUNTS ---\n";
  accounts.forEach(a => {
    output += `Acc: ${a.accountNo} | Cust ID: ${a.customerId} | Type: ${a.type} | Balance: $${a.balance.toFixed(2)}\n`;
  });
  document.getElementById('output-area').innerText = output;
}

function renderSummary() {
  document.getElementById('output-area').innerText = "Select an action above to perform operations.";
}
