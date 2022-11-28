#include <iostream>
#include "func_help.h"

using namespace std;

void FuncHelp::execute()
{
    cout << "1. CREATE PROJECT:" << endl;
    cout << "work --create/-c" << endl;
    cout << "then input the project information." << endl;
    cout << "2. ADJUST PROJECT:" << endl;
    cout << "work --adjust/-a *id* [-n *new_name*] [-i *new_info*] [-d *new_deadline*]" << endl;
    cout << "3. FINISH PROJECT:" << endl;
    cout << "work --finish/-f *id*" << endl;
    cout << "4. CANCEL PROJECT:" << endl;
    cout << "work --erase/-e *id*" << endl;
    cout << "5. LIST UNDONE PROJECT:" << endl;
    cout << "work --date/-d" << endl;
    cout << "6. LIST DONE PROJECT:" << endl;
    cout << "work --list/-l" << endl;
}
