#include <random>
#include <vector>
#include <string>
#include <cmath>
#include "SecretPair.h"

const long long MIN_SECRET = 1;
const long long MAX_SECRET = 992429121339693966;

const long long PRIME = 992429121339693967;
using namespace std;
typedef long long ll;

/* Generate a random number in the interval (min, max) */
ll generateRandomNumber(ll min, ll max);

/* Generate random coefficients for the polynom used */
vector<ll> generateCoefficients(int k, ll secret);

/* Calculate the n (x,y) pairs for a given polynom */
vector<SecretPair> calculateSecretPairs(int n, vector<ll> coefficients);

/* find the find the inverse inv for a number num such that inv * num % p=1 */
void gcdExtended(long long a, long long b, long long *x, long long *y);

/* Reconstruct the secret from k (x,y) pairs */
long long reconstructSecret(vector<SecretPair> secretPairs);

