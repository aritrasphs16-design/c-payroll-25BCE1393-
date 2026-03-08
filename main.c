/*
 * Project Title: Employee Payroll System
 * Student Name:  [Your Name]
 * Register No:   [Your Reg No]
 * Final WASM-compatible version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

typedef struct {
    int   empId;
    char  name[50];
    float basicPay;
    float otHours;
    float otRate;
    float grossPay;
    float taxDeduction;
    float netPay;
} Employee;

#define MAX_EMP   100
#define DATA_FILE "payroll.dat"

Employee employees[MAX_EMP];
int empCount = 0;

/* ── Get input from browser via JS prompt ───────── */
void getInput(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    /* Call browser's JS to get input */
    char *result = (char *)EM_ASM_PTR({
        var s = window.prompt(UTF8ToString($0), "");
        if (s === null) s = "";
        var len = lengthBytesUTF8(s) + 1;
        var buf = _malloc(len);
        stringToUTF8(s, buf, len);
        return buf;
    }, prompt);
    strncpy(buf, result, size - 1);
    buf[size - 1] = '\0';
    free(result);
    /* Echo input to terminal */
    printf("%s\n", buf);
    fflush(stdout);
}

int getInt(const char *prompt) {
    char buf[32];
    getInput(prompt, buf, sizeof(buf));
    return atoi(buf);
}

float getFloat(const char *prompt) {
    char buf[32];
    getInput(prompt, buf, sizeof(buf));
    return (float)atof(buf);
}

/* ── Calculations ───────────────────────────────── */
float computeGross(float b, float h, float r) { return b + h * r; }

float computeTax(float g) {
    if      (g <= 10000) return 0;
    else if (g <= 20000) return g * 0.05f;
    else if (g <= 40000) return g * 0.10f;
    else                 return g * 0.20f;
}

int findById(int id) {
    for (int i = 0; i < empCount; i++)
        if (employees[i].empId == id) return i;
    return -1;
}

void line() {
    printf("=========================================\n");
    fflush(stdout);
}

/* ── FUNCTION 1: Add ────────────────────────────── */
void addEmployee() {
    if (empCount >= MAX_EMP) {
        printf("[ERROR] Database full!\n"); fflush(stdout); return;
    }
    Employee e;
    printf("\n--- Add New Employee ---\n"); fflush(stdout);

    e.empId = getInt("Enter Employee ID: ");
    if (e.empId <= 0) {
        printf("[ERROR] ID must be positive!\n"); fflush(stdout); return;
    }
    if (findById(e.empId) != -1) {
        printf("[ERROR] ID %d already exists!\n", e.empId); fflush(stdout); return;
    }

    getInput("Enter Name: ", e.name, sizeof(e.name));
    if (strlen(e.name) == 0) {
        printf("[ERROR] Name cannot be empty!\n"); fflush(stdout); return;
    }

    e.basicPay = getFloat("Enter Basic Pay (Rs.): ");
    if (e.basicPay < 0) {
        printf("[ERROR] Pay cannot be negative!\n"); fflush(stdout); return;
    }

    e.otHours = getFloat("Enter OT Hours (0 if none): ");
    if (e.otHours < 0) e.otHours = 0;

    e.otRate = getFloat("Enter OT Rate/hour (e.g. 150): ");
    if (e.otRate <= 0) e.otRate = 150.0f;

    e.grossPay     = computeGross(e.basicPay, e.otHours, e.otRate);
    e.taxDeduction = computeTax(e.grossPay);
    e.netPay       = e.grossPay - e.taxDeduction;

    employees[empCount++] = e;
    printf("[SUCCESS] Employee %d (%s) added!\n", e.empId, e.name);
    printf("  Gross: Rs.%.2f | Tax: Rs.%.2f | Net: Rs.%.2f\n",
           e.grossPay, e.taxDeduction, e.netPay);
    fflush(stdout);
}

/* ── FUNCTION 2: Display All ────────────────────── */
void displayAll() {
    if (empCount == 0) {
        printf("[INFO] No records.\n"); fflush(stdout); return;
    }
    line();
    printf("%-6s %-18s %9s %7s %10s %8s %10s\n",
           "ID","Name","Basic","OT Hrs","Gross","Tax","Net");
    line();
    for (int i = 0; i < empCount; i++) {
        printf("%-6d %-18s %9.2f %7.2f %10.2f %8.2f %10.2f\n",
               employees[i].empId, employees[i].name,
               employees[i].basicPay, employees[i].otHours,
               employees[i].grossPay, employees[i].taxDeduction,
               employees[i].netPay);
    }
    line();
    printf("Total: %d employees\n", empCount);
    fflush(stdout);
}

/* ── FUNCTION 3: Search ─────────────────────────── */
void searchEmployee() {
    int id = getInt("Enter Employee ID to search: ");
    int i = findById(id);
    if (i == -1) {
        printf("[ERROR] ID %d not found!\n", id); fflush(stdout); return;
    }
    Employee *e = &employees[i];
    line();
    printf("  ID       : %d\n",       e->empId);
    printf("  Name     : %s\n",       e->name);
    printf("  Basic    : Rs.%.2f\n",  e->basicPay);
    printf("  OT Hours : %.2f\n",     e->otHours);
    printf("  OT Rate  : Rs.%.2f\n",  e->otRate);
    printf("  Gross    : Rs.%.2f\n",  e->grossPay);
    printf("  Tax      : Rs.%.2f\n",  e->taxDeduction);
    printf("  Net Pay  : Rs.%.2f\n",  e->netPay);
    line();
    fflush(stdout);
}

/* ── FUNCTION 4: Update ─────────────────────────── */
void updateEmployee() {
    int id = getInt("Enter Employee ID to update: ");
    int i = findById(id);
    if (i == -1) {
        printf("[ERROR] ID %d not found!\n", id); fflush(stdout); return;
    }
    Employee *e = &employees[i];
    printf("Current Basic: Rs.%.2f\n", e->basicPay); fflush(stdout);
    float nb = getFloat("New Basic Pay (-1 to keep): ");
    if (nb >= 0) e->basicPay = nb;

    printf("Current OT Hours: %.2f\n", e->otHours); fflush(stdout);
    float nh = getFloat("New OT Hours (-1 to keep): ");
    if (nh >= 0) e->otHours = nh;

    e->grossPay     = computeGross(e->basicPay, e->otHours, e->otRate);
    e->taxDeduction = computeTax(e->grossPay);
    e->netPay       = e->grossPay - e->taxDeduction;

    printf("[SUCCESS] Updated! New Net Pay: Rs.%.2f\n", e->netPay);
    fflush(stdout);
}

/* ── FUNCTION 5: Delete ─────────────────────────── */
void deleteEmployee() {
    int id = getInt("Enter Employee ID to delete: ");
    int i = findById(id);
    if (i == -1) {
        printf("[ERROR] ID %d not found!\n", id); fflush(stdout); return;
    }
    char buf[8];
    printf("Delete %s? (y/n): ", employees[i].name); fflush(stdout);
    getInput("Confirm delete (y/n): ", buf, sizeof(buf));
    if (buf[0] != 'y' && buf[0] != 'Y') {
        printf("[INFO] Cancelled.\n"); fflush(stdout); return;
    }
    for (int j = i; j < empCount - 1; j++)
        employees[j] = employees[j+1];
    empCount--;
    printf("[SUCCESS] Employee %d deleted!\n", id);
    fflush(stdout);
}

/* ── FUNCTION 6: Payslip ────────────────────────── */
void generatePayslip() {
    int id = getInt("Enter Employee ID for payslip: ");
    int i = findById(id);
    if (i == -1) {
        printf("[ERROR] ID %d not found!\n", id); fflush(stdout); return;
    }
    Employee *e = &employees[i];
    const char *slab;
    if      (e->grossPay <= 10000) slab = "0%  (<=Rs.10000)";
    else if (e->grossPay <= 20000) slab = "5%  (<=Rs.20000)";
    else if (e->grossPay <= 40000) slab = "10% (<=Rs.40000)";
    else                           slab = "20% (>Rs.40000)";

    line();
    printf("           PAYSLIP\n");
    line();
    printf("  Emp ID   : %d\n",              e->empId);
    printf("  Name     : %s\n",              e->name);
    printf("-----------------------------------------\n");
    printf("  Basic Pay        : Rs.%9.2f\n", e->basicPay);
    printf("  OT (%.0fh x %.0f): Rs.%9.2f\n",
           e->otHours, e->otRate, e->otHours * e->otRate);
    printf("  GROSS PAY        : Rs.%9.2f\n", e->grossPay);
    printf("-----------------------------------------\n");
    printf("  Tax Slab : %s\n",               slab);
    printf("  Tax      :         Rs.%9.2f\n", e->taxDeduction);
    printf("-----------------------------------------\n");
    printf("  NET PAY  :         Rs.%9.2f\n", e->netPay);
    line();
    fflush(stdout);
}

/* ── FUNCTION 7: Report ─────────────────────────── */
void payrollReport() {
    if (empCount == 0) {
        printf("[INFO] No records.\n"); fflush(stdout); return;
    }
    float tg=0, tt=0, tn=0;
    line();
    printf("       FULL PAYROLL REPORT\n");
    line();
    printf("%-6s %-18s %10s %8s %10s\n",
           "ID","Name","Gross","Tax","Net");
    printf("-----------------------------------------\n");
    for (int i = 0; i < empCount; i++) {
        printf("%-6d %-18s %10.2f %8.2f %10.2f\n",
               employees[i].empId, employees[i].name,
               employees[i].grossPay,
               employees[i].taxDeduction,
               employees[i].netPay);
        tg += employees[i].grossPay;
        tt += employees[i].taxDeduction;
        tn += employees[i].netPay;
    }
    printf("-----------------------------------------\n");
    printf("%-25s %10.2f %8.2f %10.2f\n","TOTALS",tg,tt,tn);
    printf("Avg Net Pay: Rs.%.2f\n", tn/empCount);
    line();
    fflush(stdout);
}

/* ── FUNCTION 8: Save ───────────────────────────── */
void saveToFile() {
    FILE *fp = fopen(DATA_FILE,"wb");
    if (!fp) {
        printf("[ERROR] Cannot save!\n"); fflush(stdout); return;
    }
    fwrite(&empCount, sizeof(int), 1, fp);
    fwrite(employees, sizeof(Employee), empCount, fp);
    fclose(fp);
    printf("[SUCCESS] %d record(s) saved.\n", empCount);
    fflush(stdout);
}

/* ── FUNCTION 9: Load ───────────────────────────── */
void loadFromFile() {
    FILE *fp = fopen(DATA_FILE,"rb");
    if (!fp) {
        printf("[INFO] No saved data. Starting fresh.\n");
        fflush(stdout); return;
    }
    fread(&empCount, sizeof(int), 1, fp);
    if (empCount < 0 || empCount > MAX_EMP) {
        empCount = 0; fclose(fp);
        printf("[ERROR] Corrupted file.\n"); fflush(stdout); return;
    }
    fread(employees, sizeof(Employee), empCount, fp);
    fclose(fp);
    printf("[SUCCESS] Loaded %d record(s).\n", empCount);
    fflush(stdout);
}

/* ── MAIN ───────────────────────────────────────── */
int main() {
    loadFromFile();
    printf("\n=========================================\n");
    printf("  EMPLOYEE PAYROLL MANAGEMENT SYSTEM\n");
    printf("=========================================\n");
    fflush(stdout);

    int choice;
    do {
        printf("\n--------- MAIN MENU ---------\n");
        printf(" 1. Add Employee\n 2. View All\n 3. Search\n");
        printf(" 4. Update\n 5. Delete\n 6. Payslip\n");
        printf(" 7. Report\n 8. Save\n 9. Load\n 0. Exit\n");
        printf("-----------------------------\n");
        fflush(stdout);

        choice = getInt("Enter choice (0-9): ");

        switch(choice) {
            case 1: addEmployee();    break;
            case 2: displayAll();     break;
            case 3: searchEmployee(); break;
            case 4: updateEmployee(); break;
            case 5: deleteEmployee(); break;
            case 6: generatePayslip(); break;
            case 7: payrollReport();  break;
            case 8: saveToFile();     break;
            case 9: loadFromFile();   break;
            case 0: saveToFile(); printf("Goodbye!\n"); fflush(stdout); break;
            default: printf("[ERROR] Enter 0-9.\n"); fflush(stdout);
        }
    } while(choice != 0);

    return 0;
}