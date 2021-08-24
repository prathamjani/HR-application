#include <iostream>
#include <occi.h>
#include <cstring>
#include <limits>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;


struct Employee {
    int  employeeNumber;
    char lastName[50];
    char firstName[50];
    char extension[10];
    char email[100];
    char officecode[10];
    int  reportsTo;
    char jobTitle[50];
};


// FUNCTION PROTOTYPES
int menu();
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp);
void displayEmployee(Connection* conn, struct Employee emp);
void displayAllEmployees(Connection* conn);
void getEmployee(struct Employee* emp);
void insertEmployee(Connection* conn, struct Employee emp);
void updateEmployee(Connection* conn, int employeeNumber);
void deleteEmployee(Connection* conn, int employeeNumber);

int main(void)
{
    /* OCCI Variables */
    Environment* env = nullptr;
    Connection* conn = nullptr;

    /* Used Variables */
    string user = "dbs211_212c01";
    string pass = "31445279";
    string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";

    try {
        env = Environment::createEnvironment();
        conn = env->createConnection(user, pass, constr);
        while (true) {
            int option = menu();
            if (option == 0) {
                cout << "Program Terminated.\n";
                break;
            }
            if (option == 1) {
                cout << "Enter Employee Number: ";
                int employeeNumber;
                cin >> employeeNumber;

                Employee* emp = new Employee;
                int output = findEmployee(conn, employeeNumber, emp);

                if (output == 0) {
                    cout << "Employee " << employeeNumber << " does not exist.\n";
                }
                else {
                    displayEmployee(conn, *emp);
                }
                delete emp;
            }
            else if (option == 2) {
                displayAllEmployees(conn);
            }
            else if (option == 3) {
                Employee* emp = new Employee;
                getEmployee(emp);
                insertEmployee(conn, *emp);
                delete emp;
            }
            else if (option == 4) {
                cout << "Employee Number: ";
                int employeeNumber;
                cin >> employeeNumber;
                updateEmployee(conn, employeeNumber);
            }
            else if (option == 5) {
                cout << "Employee Number: ";
                int employeeNumber;
                cin >> employeeNumber;
                deleteEmployee(conn, employeeNumber);
            }
            else {
                cout << "Option not Available Yet.\n";
            }
        }

        /* Terminating and Closing the Connection and Environment */
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << "\n";

    }
}

// This function displays menu options and returns user selection option (int)
int menu() {
    while (true) {
        cout << "\n******************** HR Menu ********************" << endl;
        cout << "1) Find Employee\n"
            "2) Employees Report\n"
            "3) Add Employee\n"
            "4) Update Employee\n"
            "5) Remove Employee\n"
            "0) Exit\n"
            "Enter an option (0-5): ";
        int option;
        cin >> option;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter numbers (0-5) only.\n";
            continue;
        }
        else if (option >= 0 && option <= 5) {
            return option;
        }
        else {
            cout << "Please enter valid number.\n";
        }
    }
}

// This function returns 0 if the employee does not exist. It returns 1 if the employee exists.
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp) {

    try {
        Statement* stmt = conn->createStatement();
        ResultSet* rs = stmt->executeQuery("SELECT employeenumber, lastname, firstname, extension, email, officecode, reportsto, jobtitle FROM DBS211_EMPLOYEES WHERE employeenumber=" + to_string(employeeNumber));

        if (!rs->next())
        {
            return 0;
        }
        else
        {
            emp->employeeNumber = rs->getInt(1);
            strcpy_s(emp->lastName, rs->getString(2).c_str());
            strcpy_s(emp->firstName, rs->getString(3).c_str());
            strcpy_s(emp->extension, rs->getString(4).c_str());
            strcpy_s(emp->email, rs->getString(5).c_str());
            strcpy_s(emp->officecode, rs->getString(6).c_str());
            emp->reportsTo = rs->getInt(7);
            strcpy_s(emp->jobTitle, rs->getString(8).c_str());

            return 1;
        }
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
        return 0;
    }
}

// This function displays employee information
void displayEmployee(Connection* conn, struct Employee emp) {

    /* Displaying all the members of emp paramenter */
    cout << "\n-------------- Employee Information -------------\n";
    cout << "Employee Number: " << emp.employeeNumber << "\n";
    cout << "Last Name:  " << emp.lastName << "\n";
    cout << "First Name:  " << emp.firstName << "\n";
    cout << "Extension:  " << emp.extension << "\n";
    cout << "Email:  " << emp.email << "\n";
    cout << "Office Code:  " << emp.officecode << "\n";
    cout << "Manager ID:  " << emp.reportsTo << "\n";
    cout << "Job Title:  " << emp.jobTitle << "\n";
}

// This function displays all employees' information if exists
void displayAllEmployees(Connection* conn) {
    try {
        Statement* stmt = conn->createStatement();
        ResultSet* rs = stmt->executeQuery("SELECT a.employeenumber, a.firstname ||' '|| a.lastname, a.email, phone, a.extension, b.firstname ||' '|| b.lastname FROM DBS211_EMPLOYEES a FULL OUTER JOIN DBS211_EMPLOYEES b ON a.reportsto = b.employeenumber JOIN DBS211_OFFICES ON a.officecode = DBS211_OFFICES.officecode ORDER BY a.employeenumber");

        if (!rs->next())
        {
            cout << "There is no employees' information to be displayed." << endl;
        }
        else
        {
            cout << "\n------   ---------------   ---------------------------------  ----------------  ---------  -----------------" << endl;
            cout.setf(ios::left);
            cout.width(9);
            cout << "ID";
            cout.width(18);
            cout << "Employee Name";
            cout.width(35);
            cout << "Email";
            cout.width(18);
            cout << "Phone";
            cout.width(11);
            cout << "Extension";
            cout << "Manager Name" << endl;

            cout << "------   ---------------   ---------------------------------  ----------------  ---------  -----------------" << endl;
            do
            {
                cout.setf(ios::left);
                cout.width(9);
                cout << rs->getInt(1);
                cout.width(18);
                cout << rs->getString(2);
                cout.width(35);
                cout << rs->getString(3);
                cout.width(18);
                cout << rs->getString(4);
                cout.width(11);
                cout << rs->getString(5);
                cout << rs->getString(6) << endl;

            } while (rs->next());
            cout << endl;
        }
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
}

// This function gets the new employee’s information from the user 
void getEmployee(struct Employee* emp) {
    int E_id;
    string E_last;
    string E_first;
    string E_extension;
    string E_email;
    string E_officecode = "1";
    int E_reportto = 1002;
    string E_jobtitle;

    cout << "-------------- New Employee Information -------------\n";
    cout << "Employee Number: ";
    cin >> E_id;

    cout << "Last Name: ";
    cin.ignore(50, '\n');
    getline(cin, E_last);

    cout << "First Name: ";
    //cin.ignore(50, '\n');
    getline(cin, E_first);

    cout << "Extension: ";
    cin >> E_extension;

    cout << "Email: ";
    cin >> E_email;

    cout << "Office Code: " << E_officecode << "\n";
    cout << "Manager ID: " << E_reportto << "\n";
    cout << "Job Title: ";
    cin.ignore(80, '\n');
    getline(cin, E_jobtitle);


    /* Storing employee data into employee variable (emp) */
    emp->employeeNumber = E_id;
    strcpy_s(emp->lastName, E_last.c_str());
    strcpy_s(emp->firstName, E_first.c_str());
    strcpy_s(emp->extension, E_extension.c_str());
    strcpy_s(emp->email, E_email.c_str());
    strcpy_s(emp->officecode, E_officecode.c_str());
    emp->reportsTo = E_reportto;
    strcpy_s(emp->jobTitle, E_jobtitle.c_str());
}

// This function inserts the given employee information stored in the parameter emp to the employees table
void insertEmployee(Connection* conn, struct Employee emp) {

    if (findEmployee(conn, emp.employeeNumber, &emp) == 1) {
        cout << "An employee with the same employee number exists.\n";
    }
    else {
        try {
            int E_id = emp.employeeNumber;
            string E_last = emp.lastName;
            string E_first = emp.firstName;
            string E_extension = emp.extension;
            string E_email = emp.email;
            string E_officecode = emp.officecode;
            int E_reportto = emp.reportsTo;
            string E_jobtitle = emp.jobTitle;

            Statement* stmt = conn->createStatement();
            string sql = "INSERT INTO DBS211_EMPLOYEES (EMPLOYEENUMBER, LASTNAME, FIRSTNAME, EXTENSION, EMAIL, OFFICECODE, REPORTSTO, JOBTITLE) VALUES ('" + to_string(E_id) + "', '" + E_last + "', '" + E_first + "', '" + E_extension + "', '" + E_email + "', '" + E_officecode + "', '" + to_string(E_reportto) + "', '" + E_jobtitle + "')";
            stmt->executeQuery(sql);

            /* Closing and Terminating the Statement */
            conn->commit();
            conn->terminateStatement(stmt);
            cout << "The new employee is added successfully.\n";
        }
        catch (SQLException& sqlExcp) {
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();

        }
    }
}

// This function updates the phone extension for the given employee
void updateEmployee(Connection* conn, int employeeNumber) {
    Employee* emp = new Employee;
    if (findEmployee(conn, employeeNumber, emp)) {
        try {
            string E_last = emp->lastName;
            string E_first = emp->firstName;
            string E_extension;

            cout << "Last Name: " << E_last << "\n";
            cout << "First Name: " << E_first << "\n";
            cout << "Extension: ";
            cin >> E_extension;

            Statement* stmt = conn->createStatement();
            string sql = "UPDATE DBS211_EMPLOYEES SET EXTENSION = '" + E_extension + "' WHERE  EMPLOYEENUMBER='" + to_string(employeeNumber) + "'";
            stmt->executeQuery(sql);

            /* Closing and Terminating the Statement */
            conn->commit();
            conn->terminateStatement(stmt);
            cout << "The employee's extension is updated successfully.\n";
        }
        catch (SQLException& sqlExcp) {
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();

        }
    }
    else {
        cout << "The employee with ID " << employeeNumber << " does not exist\n";
    }
    delete emp;
}

// This function deletes a row with the given employee number from table employees
void deleteEmployee(Connection* conn, int employeeNumber) {
    Employee* emp = new Employee;
    if (findEmployee(conn, employeeNumber, emp)) {
        try {
            Statement* stmt = conn->createStatement();
            string sql = "DELETE FROM DBS211_EMPLOYEES WHERE  EMPLOYEENUMBER='" + to_string(employeeNumber) + "'";
            stmt->executeQuery(sql);

            /* Closing and Terminating the Statement */
            conn->commit();
            conn->terminateStatement(stmt);
            cout << "The employee with ID " << employeeNumber << " is deleted successfully.\n";
        }
        catch (SQLException& sqlExcp) {
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();

        }
    }
    else {
        cout << "The employee with ID " << employeeNumber << " does not exist\n";
    }
    delete emp;
}
