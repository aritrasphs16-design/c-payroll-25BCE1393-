# c-payroll-25BCE1393-
# Employee Payroll System — C Mini Project (DA1)

A menu-driven **Employee Payroll Management System** written in C,
compiled to **WebAssembly** and hosted on **GitHub Pages**.

## 🔗 Live Demo
👉 [Click here to run in browser](https://aritrasphs16-design.github.io/c-payroll-YOURROLLNO/)

## 📋 Features
- Add Employee (EmpID, Name, BasicPay, OT Hours, OT Rate)
- View All Employees in tabular format
- Search Employee by ID
- Update Employee records
- Delete Employee
- Generate Payslip (formatted payslip-style output)
- Full Payroll Report with totals and averages
- Save/Load records to browser storage
- Input validation throughout

## 🧮 Payroll Formula
- **Gross Pay** = Basic Pay + (OT Hours × OT Rate)
- **Tax Slabs**:
  - ≤ Rs.10,000 → 0%
  - ≤ Rs.20,000 → 5%
  - ≤ Rs.40,000 → 10%
  - > Rs.40,000 → 20%
- **Net Pay** = Gross Pay − Tax

## 📁 Files
| File | Purpose |
|------|---------|
| `main.c` | Full C source code |
| `index.html` | Web interface (runs in browser) |
| `index.js` | Emscripten JS glue code |
| `index.wasm` | Compiled WebAssembly binary |

## 🚀 How to Run Locally
```bash
# 1. Activate Emscripten
C:\Users\Aritra\emsdk\emsdk_env.bat

# 2. Compile
emcc main.c -o index.js -s FORCE_FILESYSTEM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -O1

# 3. Run local server
emrun --no_browser --port 8080 .

# 4. Open browser
# Go to: http://localhost:8080/index.html
```

## 👤 Student Details
- **Name:** [ARITRA NASKAR]
- **Register No:** [25BCE1393]
- **Department:** [SCOPE]
- **Course:** Problem Solving using C
- **Faculty:** Mr. Dinakaran M
- **Institution:** VIT CHENNAI
