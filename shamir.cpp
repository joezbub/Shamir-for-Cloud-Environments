#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "helpers.h"

using namespace std;

typedef long long ll;

vector<SecretPair> split(ll secret, int n, int k){
    if (n < k) {
        return {};
    }

    //cout << "P(x) = (";
    
    /* Generate the polynom that will be used */
    vector<ll> coefficients = generateCoefficients(k, secret);
    /*
    for (int i = 0; i < k; i++)
        if (i == k - 1)
            cout << coefficients[i] << " * x^" << k - 1 << ") % ";
        else
            cout << coefficients[i] << " * x^" << i << " + ";
    cout << PRIME << "\n";
    
    cout << "\nThe generated pairs x f(x) are:\n";
    */
    /* Generate the points that will be sent to the n users */
    vector<SecretPair> shareSecretPoints = calculateSecretPairs(n, coefficients);
    return shareSecretPoints;

    /*
    for (int i = 0; i < n; i++)
        cout << shareSecretPoints[i].getX() << " " << shareSecretPoints[i].getY() << "\n";
    cout << "\n";
    */
}

ll restore(int k, vector<SecretPair> secrets){
       
    return (ll) reconstructSecret(secrets);
    /*
    // convert long long to string
    string res = "";
    string binary = bitset<56>(ans).to_string();
    cout << binary << endl;
    for (int i = 6; i >= 0; i--){
        string tmp = binary.substr(i * 8, 8);
        int val = stoi(tmp, 0, 2);
        res += val;
    }
    reverse(res.begin(), res.end());
    cout << res << endl;
    */
}
