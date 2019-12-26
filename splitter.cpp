//splits txt file into blocks of 7 char each
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <streambuf>
#include "shamir.h"

using namespace std;

void splitter(string efile){
    ifstream t(efile);
    ofstream out("test.out");
    string file;

    t.seekg(0, ios::end);   
    file.reserve(t.tellg());
    t.seekg(0, ios::beg);

    file.assign((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    int ct = 0;
    out << file;
    while (1){
        string tmp;
        bool f = 0;
        if (file.size() <= ct + 6) {
            tmp = file.substr(ct, file.size() - ct);
            f = 1;
        }
        else tmp = file.substr(ct, 7);
        ct += 7;
        cout << "block " << ct / 7 << " is: " << tmp << endl;
        split(tmp, 3, 2);
        cout << endl;
        if (f) break;
    }
}