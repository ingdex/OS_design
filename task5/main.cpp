#include <iostream>
#include "filesys.h"

//void

using namespace std;



int main() {
    int op = 1;
    string filename = "./volume";
    FileSys fileSys(filename);
    cout << "1: init    2:mkdir    3:ls" << endl;
    //string filename;
    char type;

    while (op)
    {
        cin >> op;
        switch (op)
        {
            case 1:
                fileSys.init();
                cout << "init complete" << endl;
                break;
            case 2:
                cout << "filename: ";
                cin >> filename;
                cout << "type(0 or 1): ";
                cin >> type;
                fileSys.createFile(filename, type, 1, 0);
                cout << "createFile complete" << endl;
                break;
            case 3:
                cout << "displayFile:" << endl;
                fileSys.displayFile(0);
                cout << "displayFile complete" << endl;
                break;
            default:
                cout << "no such operation" << endl;
                break;
        }
    }


    return 0;
}

