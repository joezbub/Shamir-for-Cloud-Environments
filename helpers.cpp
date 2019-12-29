#include "helpers.h"
#include <iostream>

typedef long long ll;

ll generateRandomNumber(ll min, ll max)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<ll> dist(min, max);
    return dist(generator);
}

vector<ll> generateCoefficients(int k, ll secret){
    /* coefficients of the k-1 degree polynom */
    vector<ll> coefficients(k);

    /* f(0) must be equal with the secret */
    coefficients[0] = secret;

    for (int i = 1; i < k; i++)
        /* coefficients should be smaller than PRIME */
        coefficients[i] = generateRandomNumber(MIN_SECRET, MAX_SECRET);

    return coefficients;
}

ll modmult(ll a, ll b){ 
    ll res = 0; 
    a %= PRIME; 
    while (b) { 
        if (b & 1) 
            res = (res + a) % PRIME; 
        a = (2 * a) % PRIME; 
        b >>= 1; // b = b / 2 
    } 
    return res; 
} 

ll mypow(ll a, ll b){
	ll ans=1;
	while(b){
		if(b & 1)
			ans = modmult(ans, a % PRIME) % PRIME;
		b /= 2;
        a = modmult(a, a % PRIME) % PRIME;
	}
	return ans;
}

vector<SecretPair> calculateSecretPairs(int n, vector<ll> coefficients) {
    vector<SecretPair> shareSecretPoints(n);
    int k = coefficients.size();
    /* Calculate P(x) for the n numbers */
    for (int number = 1; number <= n; number++) {
        ll accumulator = coefficients[0];
        for (int exp = 1; exp < k; exp++) {
            ll current = 1;
            //current = mypow(number, exp);
            for (int i = 1; i <= exp; i++) 
                current = (current * number) % PRIME; //overflow
            current = modmult(current, coefficients[exp] % PRIME) % PRIME;
            accumulator = ((accumulator % PRIME) + (current % PRIME)) % PRIME;
        }
        /* Store the (x,y) pair */
        shareSecretPoints[number-1] = SecretPair(number, accumulator);
    }
    return shareSecretPoints;
}

/* ax + by = gcd(a, b) */
void gcdExtended(ll a, ll b, ll *x, ll *y)
{
    if (b == 0){
        *x = 1;
        *y = 0;
    } 
    else {
        ll n = a / b;
        ll c = a % b;

        ll x1, y1;
        gcdExtended(b, c, &x1, &y1);
        *x = y1;
        *y = x1 - y1 * n;
    }
}

ll reconstructSecret(vector<SecretPair> secretPairs) {
    ll secret = 0;

    /* Formula 
     * Sum[0-k](Prod((x-xj)/(xi-xj)) * yi) */
     
    int k = secretPairs.size();

    for (int i = 0; i < k; i++) {
        
        /* Prod of (x-xj) */
        ll upper = 1;

        /* Prod of (xi-xj) */
        ll lower = 1;

        /* Calculate the current Prod((x-xj)/(xi-xj)) */
        for (int j = 0; j < k; j++) {
            if (i == j)
                continue;

            ll xi = secretPairs[i].getX();
            ll xj = secretPairs[j].getX();

            upper = (upper * xj * -1) % PRIME;
            lower = (lower * (xi -xj)) % PRIME;
        }

        ll yi = secretPairs[i].getY();
        
        /* Calculate the inverse from gcd for lower */
        ll x, y;
        ll prime = PRIME;

        int ok = 0;
        if (lower < 0) {
            ok = 1;
            lower *= -1;
        }

        gcdExtended(prime, lower, &x, &y);

        if (ok == 1)
            y *= -1;

        lower = (y + PRIME) % PRIME;

        /* Proceed with upper/lower * yi */
        ll current = modmult(upper, lower % PRIME) % PRIME;
        current = modmult(current, yi % PRIME) % PRIME;
        secret = (PRIME + secret + current) % PRIME;
    }

    return secret;
}
