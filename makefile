build:
	g++ shamir.cpp helpers.cpp SecretPair.cpp -o shamir -lsodium
