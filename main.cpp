#include <iostream>
#include <stdlib.h>
#include <random>
#include <sodium.h>
#include <string>
#include "shamir.h"

#define CHUNK_SIZE 4096
#define N 3
#define K 2

void handle_text(int n, int k);
void read_file(const char * efile, int n, int k);

typedef long long ll;

static int encrypt(const char *target_file, const char *source_file, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    unsigned char  buf_in[CHUNK_SIZE];
    unsigned char  buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE *fp_t, *fp_s;
    unsigned long long out_len;
    size_t rlen;
    int eof;
    unsigned char tag;
    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    crypto_secretstream_xchacha20poly1305_init_push(&st, header, key);
    fwrite(header, 1, sizeof header, fp_t);
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len, buf_in, rlen, NULL, 0, tag);
        fwrite(buf_out, 1, (size_t) out_len, fp_t);
    } while (! eof);
    fclose(fp_t);
    fclose(fp_s);
    return 0;
}

static int decrypt(const char *target_file, const char *source_file, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    unsigned char  buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES], 
    buf_out[CHUNK_SIZE], 
    header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE *fp_t, *fp_s;
    unsigned long long out_len;
    size_t rlen;
    int eof, ret = -1;
    unsigned char tag;
    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    fread(header, 1, sizeof header, fp_s);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0) {
        goto ret; /* incomplete header */
    }
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, rlen, NULL, 0) != 0) {
            goto ret; /* corrupted chunk */
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && ! eof) {
            goto ret; /* premature end (end of file reached before the end of the stream) */
        }
        fwrite(buf_out, 1, (size_t) out_len, fp_t);
    } while (! eof);

    ret = 0;
ret:
    fclose(fp_t);
    fclose(fp_s);
    return ret;
}

void split_keys(unsigned char array[]){
    vector<SecretPair> vec[N];
    int size = crypto_secretstream_xchacha20poly1305_KEYBYTES, i, j;
    size = ((size / 7) + 1) * 7;
    uint8_t buffer[size];
    for (i = 0; i < crypto_secretstream_xchacha20poly1305_KEYBYTES; ++i){
        if (i < crypto_secretstream_xchacha20poly1305_KEYBYTES) buffer[i] = array[i];
        else buffer[i] = 0;
    }
    ll tmp;
    uint8_t *ptr = &buffer[0];
    while (size){
        tmp = 0;
        memcpy(&tmp, ptr, 7);
        vector<SecretPair> tmpvec = split(tmp, N, K);
        for (i = 0; i < N; ++i) vec[i].push_back(tmpvec[i]);
        ptr += 7;
        size -= 7;
    }
    for (i = 1; i <= N; ++i){
        string rm = "key-" + to_string(i) + ".dat";
        remove(rm.c_str());
    }
    for (i = 1; i <= N; ++i){
        ofstream out("key-" + to_string(i) + ".dat");
        for (j = 0; j < vec[i - 1].size(); ++j){
            out << vec[i - 1][j].getY() << "\n";
        }
        out.close();
    }
}

int main(int argc, char **argv){

    if (sodium_init() != 0) {
        return 1;
    }
    string option = argv[1];
    if (argc != 3) {
        printf("Format: ./main -encrypt input_file\n");
        printf("or ./main -decrypt output_file\n");
        return 0;
    }
    else {
        printf("\n");
        if (option == "-decrypt") {
            string outp = argv[2];
            clock_t begin = clock(), end;
            unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
            ll val;
            int size = crypto_secretstream_xchacha20poly1305_KEYBYTES, i, j;
            int blocks = (((size / 7) + 1) * 7) / 7;

            printf("Combining key and file according to (%d, %d) scheme...\n", K, N);
            
            vector<vector<SecretPair> > ans(blocks);      
            int ct = 0;      
            for (i = 1; i <= N; ++i){
                string f = "key-" + to_string(i) + ".dat";
                ifstream in(f);
                if (!in.good()) continue;
                for (j = 0; j < blocks; ++j){
                    in >> val;
                    ans[j].push_back(SecretPair((ll)i, val));
                }
                ++ct;
                in.close();
                remove(f.c_str());
            }

            if (ct < K) {
                printf("Only %d key files found. %d required!\n", ct, K);
                return 0;
            }

            for (i = 0; i < blocks; ++i){
                if (size < 7){
                    val = restore(K, ans[i]);
                    memcpy(&key[i * 7], &val, size);
                    size = 0;
                }
                else {
                    val = restore(K, ans[i]);
                    memcpy(&key[i * 7], &val, 7);
                    size -= 7;
                }
            }

            printf("Keys recovered...\n");
            
            handle_text(N, K);
            printf("File recovered...\n");

            if (decrypt(outp.c_str(), "combined_shares.dat", key) != 0) {
                return 1;
            }
            printf("File decrypted...\n");

            remove("combined_shares.dat");
            remove("encrypted.dat");
            printf("Cleaned folder...\n");

            end = clock();
            double esecs = double(end - begin) / CLOCKS_PER_SEC;
            printf("\nDecrypted and Combined in %f seconds.\n\n", esecs);
        }  

        else if (option == "-encrypt"){
            clock_t begin = clock(), end;
            unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
            string inp = argv[2];
            crypto_secretstream_xchacha20poly1305_keygen(key);
            
            printf("Splitting key and file according to (%d, %d) scheme...\n", K, N);

            if (encrypt("encrypted.dat", inp.c_str(), key) != 0) {
                return 1;
            }
            printf("File encrypted...\n");

            split_keys(key);
            printf("Keys split...\n");

            read_file("encrypted.dat", N, K);
            printf("File split...\n");

            end = clock();
            double esecs = double(end - begin) / CLOCKS_PER_SEC;
            printf("\nEncrypted and Split in %f seconds.\n\n", esecs);
        }
    }
    return 0;
}