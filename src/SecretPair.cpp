#include "SecretPair.h"
typedef long long ll;

SecretPair::SecretPair()
{
    x = 0;
    y = 0;
}

SecretPair::SecretPair(ll number, ll eval)
{
    x = number;
    y = eval;
}

ll SecretPair::getX()
{
    return x;
}

ll SecretPair::getY()
{
    return y;
}

