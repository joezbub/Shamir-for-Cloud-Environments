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
#define CHECK_SUM 69
typedef long long ll;

typedef struct header_ {
    uint32_t tot_sz;
    uint16_t block_ct;
    uint8_t  csum;
} header_t;

void read_file(const char * efile, int n, int k){
    vector<SecretPair> complete[n];
    int i;
    ifstream file(efile, ios::binary | ios::ate);
    int tot_sz = file.tellg(), ct_empty, size;
    file.close();
    uint8_t csum = CHECK_SUM;
    int curr_sz = tot_sz + 7;
    if (curr_sz % 7 != 0){
        ct_empty = ((curr_sz / 7) + 1) * 7 - curr_sz;
    }
    uint16_t block_ct = (tot_sz + ct_empty) / 7;
    
    header_t header;
    header.tot_sz = tot_sz;
    header.block_ct = block_ct;
    header.csum = csum;

    ll tmp = 0;
    memcpy(&tmp, (uint8_t * )&header, 7);
    printf("%llx \n", tmp);
    vector<SecretPair> ret = split(tmp, n, k);
    for (i = 0; i < n; ++i) complete[i].push_back(ret[i]);

    ifstream infile;
    infile.open(efile, ios::binary | ios::in);
    uint8_t buffer[CHUNK_SIZE];
    uint8_t *ptr = &buffer[0];

    while(infile.read((char *)&buffer, sizeof(uint8_t)*CHUNK_SIZE)) {
        size = infile.gcount();
        while (size){
            tmp = 0;
            memcpy(&tmp, ptr, 7);
            printf("%llx \n", tmp);
            vector<SecretPair> ret = split(tmp, n, k);
            for (i = 0; i < n; ++i) complete[i].push_back(ret[i]);
            ptr += 7;
            size -= 7;
        }
    }

    if (infile.gcount() > 0) { //remaining parts of buffer
        size = infile.gcount() + ct_empty;
        for (i = 0; i < ct_empty; ++i) buffer[infile.gcount() + i] = 0;
        while (size){
            tmp = 0;
            memcpy(&tmp, ptr, 7);
            printf("%llx \n", tmp);
            vector<SecretPair> ret = split(tmp, n, k);
            for (i = 0; i < n; ++i) complete[i].push_back(ret[i]);
            ptr += 7;
            size -= 7;
        }
    }

    infile.close();

    for (i = 1; i <= n; ++i){
        ofstream out("split-" + to_string(i) + ".txt");
        for (int j = 0; j < complete[i - 1].size(); ++j){
            out << complete[i - 1][j].getY() << "\n";
        }
    }
}

void handle_text(int n, int k){
    int i, j;
    ll val;
    vector<SecretPair> findhead;
    for (i = 1; i <= n; ++i){
        ifstream in("split-" + to_string(i) + ".txt");
        in >> val;
        findhead.push_back(SecretPair(i, val));
    }
    ll ret = restore(k, findhead);
    printf("%llx\n", ret);
    header_t header;
    memcpy((uint8_t *)&header, &ret, 56);
    int blocks = header.block_ct, tot = header.tot_sz, checksum = header.csum;
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

    FILE *p = fopen("combined_shares.txt", "ab");
    for (i = 0; i < blocks; ++i){
        if (tot < 7){
            ll ret = restore(k, ans[i]);
            printf("%llx \n", ret);
            fwrite((void *)&ret, tot, sizeof(uint8_t), p);
            tot = 0;
        }
        else {
            ll ret = restore(k, ans[i]);
            printf("%llx \n", ret);
            fwrite((void *)&ret, 7, sizeof(uint8_t), p);
            tot -= 7;
        }
    }
}