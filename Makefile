CFLAGS = -Wall -pedantic -O3 -march=native

wc-avx2: wc-avx2.c
	$(CC) $(CFLAGS) $< -o $@

test: wc-avx2
	echo 'these are words: abc 	def ghi' | ./wc-avx2

clean:
	rm wc-avx2
