#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "helpers.h"

typedef long long ll;

vector<SecretPair> split(ll secret, int n, int k){
    if (n < k) {
        return {};
    }

    vector<ll> coefficients = generateCoefficients(k, secret);

    /*
    cout << "P(x) = (";
    for (int i = 0; i < k; i++)
        if (i == k - 1)
            cout << coefficients[i] << " * x^" << k - 1 << ") % ";
        else
            cout << coefficients[i] << " * x^" << i << " + ";
    cout << PRIME << "\n";
    */

    vector<SecretPair> shareSecretPoints = calculateSecretPairs(n, coefficients);
    
    /*
    for (int i = 0; i < n; i++)
        cout << shareSecretPoints[i].getX() << " " << shareSecretPoints[i].getY() << "\n";
    cout << "\n";
    */
   return shareSecretPoints;
}

ll restore(int k, vector<SecretPair> secrets){  
    return reconstructSecret(secrets);
}
