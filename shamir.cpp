#include <iostream>
#include <stdlib.h>
#include "helpers.h"

using namespace std;

typedef long long ll;

string convert(int n)
{
    string ans = "";
    if (n / 2 != 0) {
        ans += convert(n / 2);
    }
    if (n % 2 == 0) ans += '0';
    else ans += '1';
    return ans;
}

void help();
void split();
void restore();

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Usage: ./shamir --help\n";
        return -1;
    }

    string option = argv[1];

    if (option.compare("--split") == 0) {
        split();
    }
    else if (option.compare("--restore") == 0) {
        restore();
    }
    else {
        help();
    }

    return 0;
}

void help()
{
    cout << "1. ./shamir --split.\n";
    cout << "2. ./shamir --restore.\n";
}

void split(){
    cout << "Please select a string of 7 characters\n";
    cout << "Secret = ";
    
    /* Read the secret and convert to long long*/
    string tmp;
    cin >> tmp;
    if (tmp.size() > 7) return;
    char convc[tmp.size() * 8];
    char *endp;
    for (int i = 0; i < tmp.size(); i++){
        int cval = (int) tmp[i];
        int ct = 0;
        for (int j = 7; j >= 0; --j){
            convc[i * 8 + ct] = (cval & (1 << j)) ? '1' : '0';
            ++ct;
        }
    }
    ll secret = strtoull (convc, &endp, 2);
    cout << "long long is: " << secret << endl;

    int n, k;
    /* Read n and k */
    cout << "n = ";
    cin >> n;

    cout << "k = ";
    cin >> k;

    /* Check for n >= k */
    if (n < k) {
        return;
    }

    cout << "P(x) = (";
    
    /* Generate the polynom that will be used */
    vector<ll> coefficients = generateCoefficients(k, secret);
    for (int i = 0; i < k; i++)
        if (i == k - 1)
            cout << coefficients[i] << " * x^" << k - 1 << ") % ";
        else
            cout << coefficients[i] << " * x^" << i << " + ";
    cout << PRIME << "\n";
    
    cout << "\nThe generated pairs x f(x) are:\n";

    /* Generate the points that will be sent to the n users */
    vector<SecretPair> shareSecretPoints = calculateSecretPairs(n, coefficients);
    for (int i = 0; i < n; i++)
        cout << shareSecretPoints[i].getX() << " " << 
            shareSecretPoints[i].getY() << "\n";
}

void restore()
{
    cout << "k = ";

    /* Get the minimum required to restore */
    int k;
    cin >> k;

    cout << "Please insert " << k << " pairs\n";

    /* Get the (x, f(x)) pairs */
    vector<SecretPair> secrets;
    for (int i = 0; i < k; i++) {
        ll x, y;
        cin >> x >> y;
        secrets.push_back(SecretPair(x, y));
    }
    ll ans = reconstructSecret(secrets);
    cout << "Your long long was = " << ans << "\n";

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
}
