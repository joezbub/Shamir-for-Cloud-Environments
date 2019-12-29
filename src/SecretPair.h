/* A pair that will be sent to one of the users (x, f(x)) */
typedef long long ll;

class SecretPair
{
    private:
        /* x and f(x) */
        ll x, y;

    public:
        /* constructors */
        SecretPair();
        SecretPair(ll number, ll eval);

        /* getters */
        ll getX();
        ll getY();
};
