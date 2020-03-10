# Shamir Secret Sharing Architecture for Documents in Multi-Provider Clouds

This is a C++ implementation of a secret sharing architecture which ensures authenticity, availability, and perfect privacy. It allows users to encrypt and split as well as combine and decrypt their files stored in the cloud. The only way to compromise this sytem is if multiple cloud providers collude, which is still unlikely given that the shares are additionally encrypted.

## How to Use It

Run the executable main file in the bin directory.

To change the (k, n) scheme from the default (2, 3):
```bash
./main -config *N* *K*
```

To encrypt and split files:
```bash
./main -encrypt *file-to-encrypt*
```

To combine and decrypt files:
```bash
./main -decrypt *file-to-decrypt*
```

## How it works

My implementation encrypts files using the ChaCha20-Poly1305 construction. My software timings show that this construction has a significant efficiency advatage over AES256-GCM, which surprised me since AES is also a commonly authentication encryption construction for data in the Cloud. My architecture then splits the original file into fragments using the Shamir's Secret Sharing algorithm through a default (2, 3) threshold scheme (able to be changed as shown above). The mathematics behhind this scheme ensure perfect privacy and space efficiency.
 
My secret sharing algorithm is a way to split an arbitrary secret `S` into `N` parts of which at least `K` are required to reconstruct `S`. For example, a root password can be split among five people, and if three or more of them combine their parts, they can recover the root password.

### Splitting a secret

Splitting a secret works by encoding the secret as the constant in a random polynomial of `K` degree. For example, if we're splitting the secret number `42` among five people with a threshold of three (`N=5,K=3`), we might end up with the polynomial:

```
f(x) = 71x^3 - 87x^2 + 18x + 42
```

To generate parts, we evaluate this polynomial for values of `x` greater than zero:

```
f(1) =   44
f(2) =  298
f(3) = 1230
f(4) = 3266
f(5) = 6822
```

These `(x,y)` pairs are then handed out to the five people. 

### Joining parts 

When three or more of them decide to recover the original secret, they pool their parts together:

```
f(1) =   44
f(3) = 1230
f(4) = 3266
```

Using these points, they construct a [Lagrange polynomial](https://en.wikipedia.org/wiki/Lagrange_polynomial), `g`, and calculate `g(0)`. If the number of parts is equal to or greater than the degree of the original polynomial (i.e. `K`), then `f` and `g` will be exactly the same, and `f(0) = g(0) = 42`, the encoded secret. If the number of parts is less than the threshold `K`, the polynomial will be different and `g(0)` will not be `42`.

### Implementation details

I split the file into blocks of 7 bytes each for splitting according to the secret sharing algorithm. I can only hold 7 bytes for each block because the implementation uses C++ `long long` (8 bytes) to store the blocks. The split key and blocks can be found in the resulting `.dat` files. The resulting payload fragments indicate a series of 64-bit integers.