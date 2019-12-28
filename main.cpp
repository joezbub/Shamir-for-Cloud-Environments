#include <iostream>
#include <stdlib.h>
#include <random>
#include <sodium.h>
#include "shamir.h"
using namespace std;

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
    unsigned char  tag;

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

int main(int argc, char **argv){

    if (sodium_init() != 0) {
        return 1;
    }
    
    if (argc < 2) return -1;

    else if (argc == 3){
        string option = argv[1];
        if (option == "-crypt") {
            clock_t begin = clock();
            unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

            crypto_secretstream_xchacha20poly1305_keygen(key);
            if (encrypt("encrypted.txt", argv[2], key) != 0) {
                return 1;
            }

            cout << "----------SPLITTING-----------" << endl;
            cout << "Blocks: " << endl;
            read_file("encrypted.txt", N, K);

            cout << endl << "----------COMBINING-----------" << endl;
            cout << "Blocks: " << endl;
            handle_text(N, K); 

            if (decrypt("decrypted.txt", "combined_shares.txt", key) != 0) { 
                return 1;
            }

            clock_t end1 = clock();
            double esecs1 = double(end1 - begin) / CLOCKS_PER_SEC;
            cout << esecs1 << " seconds" << endl;
        }  
        else if (option == "-split") { //splits specified file into n shares e.x(split-1.txt)
            read_file(argv[2], N, K);
        } 
    }

    else if (argc == 2){
        string option = argv[1];
        if (option == "-restore"){ //restores file from split files e.x(split-1.txt)
            handle_text(N, K); //outputs to combined_shares.txt
        }
    }

    return 0;
}