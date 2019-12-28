#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "helpers.h"

vector<SecretPair> split(long long secret, int n, int k);

long long restore(int k, vector<SecretPair> secrets);
