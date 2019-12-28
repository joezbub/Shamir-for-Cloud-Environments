//splits txt file into blocks of 7 char each
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <streambuf>
#include "shamir.h"
using namespace std;

#define CHUNK_SIZE 21000
typedef long long ll;

void splitter(uint8_t * buf_in, int size, int n, int k){
    vector<SecretPair> complete[n];
    int i;
    while (size){
        ll tmp = 0;
        int *ptr = (int *)&tmp;
        if (size < 7){
            memcpy(&tmp, buf_in, size);
            printf("%llx \n", tmp);
            vector<SecretPair> ret = split(tmp, 3, 2);
            for (i = 0; i < n; ++i){
                complete[i].push_back(ret[i]);
            }
            size = 0;
        }
        else{
            memcpy(&tmp, buf_in, 7);
            printf("%llx \n", tmp);
            vector<SecretPair> ret = split(tmp, 3, 2);
            for (i = 0; i < n; ++i){
                complete[i].push_back(ret[i]);
            }
            buf_in += 7;
            size -= 7;
        }
    }
    for (i = 1; i <= n; ++i){
        ofstream out("split-" + to_string(i) + ".txt");
        for (int j = 0; j < complete[i - 1].size(); ++j){
            out << complete[i - 1][j].getY() << "\n";
        }
    }
}

void read_file(const char * efile, int n, int k){
    ifstream infile;
    infile.open(efile,ios::binary | ios::in);
    uint8_t buffer[CHUNK_SIZE];
    while(infile.read((char *)&buffer, sizeof(uint8_t)*CHUNK_SIZE)) {
        splitter(buffer, infile.gcount(), n, k);
    }
    if (infile.gcount() > 0) splitter(buffer, infile.gcount(), n, k);
    infile.close();
}

int getlinect(string filename){
    ifstream aFile (filename);   
    int lines_count = 0;
    string line;
    while (getline(aFile , line))
        ++lines_count;
    return lines_count;
}

void handle_text(int n, int k){
    int blocks = getlinect("split-1.txt"), i, j;
    vector<vector<SecretPair> > ans(blocks, vector<SecretPair> (n));
    ll val;
    for (i = 1; i <= n; ++i){
        ifstream in("split-" + to_string(i) + ".txt");
        for (j = 0; j < blocks; ++j){
            in >> val;
            ans[j][i - 1] = SecretPair(i, val);
        }
    }
    ll ret;
    for (i = 0; i < blocks; ++i){
        ret = restore(k, ans[i]);
        printf("%llx \n", ret);
    }
}