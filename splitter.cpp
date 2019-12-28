//splits txt file into blocks of 7 char each
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <streambuf>
#include "shamir.h"
using namespace std;

#define CHUNK_SIZE 42000
#define CHECK_SUM 6969
typedef long long ll;

typedef struct header_ {
    uint32_t tot_sz;
    uint16_t csum;
    uint8_t emptyct;
} header_t;

void read_file(const char * efile, int n, int k){
    vector<SecretPair> complete[n];
    int i;
    ifstream file(efile, ios::binary | ios::ate);
    int tot_sz = file.tellg(), size;
    file.close();
    uint16_t csum = CHECK_SUM;
    uint8_t ct_empty;
    int curr_sz = tot_sz + 7;
    if (curr_sz % 7 != 0){
        int calc = ((curr_sz / 7) + 1) * 7 - curr_sz;
        ct_empty = calc;
    }
    //uint16_t block_ct = (tot_sz + ct_empty) / 7;
    
    header_t header;
    header.tot_sz = tot_sz;
    header.csum = csum;
    header.emptyct = ct_empty;
    //cout << (tot_sz + (int)ct_empty) / 7 << " " << tot_sz << " " << csum << endl;

    ll tmp = 0;
    memcpy(&tmp, (uint8_t * )&header, 7);
    vector<SecretPair> ret = split(tmp, n, k);
    for (i = 0; i < n; ++i) complete[i].push_back(ret[i]);

    ifstream infile;
    infile.open(efile, ios::binary | ios::in);
    uint8_t buffer[CHUNK_SIZE];
    while(infile.read((char *)&buffer, sizeof(uint8_t)*CHUNK_SIZE)) {
        size = infile.gcount();
        uint8_t *ptr = &buffer[0];
        while (size){
            tmp = 0;
            memcpy(&tmp, ptr, 7);
            vector<SecretPair> ret = split(tmp, n, k);
            for (i = 0; i < n; ++i) complete[i].push_back(ret[i]);
            ptr += 7;
            size -= 7;
        }
    }

    if (infile.gcount() > 0) { //remaining parts of buffer
        size = infile.gcount() + ct_empty;
        for (i = 0; i < ct_empty; ++i) buffer[infile.gcount() + i] = 0;
        uint8_t *ptr = &buffer[0];
        while (size){
            tmp = 0;
            memcpy(&tmp, ptr, 7);
            vector<SecretPair> ret = split(tmp, n, k);
            for (i = 0; i < n; ++i) complete[i].push_back(ret[i]);
            ptr += 7;
            size -= 7;
        }
    }
    infile.close();
    for (i = 1; i <= n; ++i){
        string rm = "split-" + to_string(i) + ".txt";
        remove(rm.c_str());
    }
    for (i = 1; i <= n; ++i){
        ofstream out("split-" + to_string(i) + ".txt");
        for (int j = 0; j < complete[i - 1].size(); ++j){
            out << complete[i - 1][j].getY() << "\n";
        }
    }
}

void handle_text(int n, int k){
    int i, j;
    ll val, ret;
    vector<SecretPair> findhead;
    for (i = 1; i <= n; ++i){
        ifstream in("split-" + to_string(i) + ".txt");
        in >> val;
        findhead.push_back(SecretPair(i, val));
    }
    ret = restore(k, findhead);
    header_t header;
    memcpy((uint8_t *)&header, &ret, 56);
    int blocks = ((int)header.emptyct + header.tot_sz) / 7, tot = header.tot_sz, checksum = header.csum;
    //cout << blocks << " " << tot << " " << checksum << endl;
    if ((int)checksum != CHECK_SUM) cout << "WARNING: FILE ALTERED!\n";

    vector<vector<SecretPair> > ans(blocks, vector<SecretPair> (n));
    for (i = 1; i <= n; ++i){
        ifstream in("split-" + to_string(i) + ".txt");
        in >> val;
        for (j = 0; j < blocks; ++j){
            in >> val;
            ans[j][i - 1] = SecretPair(i, val);
        }
    }
    remove("combined_shares.txt");
    FILE *p = fopen("combined_shares.txt", "ab");
    for (i = 0; i < blocks; ++i){
        if (tot < 7){
            ret = restore(k, ans[i]);
            fwrite((void *)&ret, tot, sizeof(uint8_t), p);
            tot = 0;
        }
        else {
            ret = restore(k, ans[i]);
            fwrite((void *)&ret, 7, sizeof(uint8_t), p);
            tot -= 7;
        }
    }
}