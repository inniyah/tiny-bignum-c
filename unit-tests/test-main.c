int test_bignum_main(int argc, char **argv);

int main(int argc, char **argv) {
	int ret = 0;
	ret |= test_bignum_main(argc, argv);
	return ret;
}
