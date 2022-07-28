Decodes IHEX files to standard output.

Usage:

```
./ihex_decoder <input_file>
```


Building:

```
make
```

Testing:

```
cd ./tests
./test.py <path to ihex_decoder
```

Disabling the checksum validation (useful for fuzzing):

```
make OPTIONS="-DDISABLE_CHECKSUM"
```

Example Usage:

```
[tests (main=)]$ ../ihex_decoder ./test7.hex 
00000100     214601360121470136007EFE09D21901
00000110     2146017E17C20001FF5F160021480119
00000120     194E79234623965778239EDA3F01B2CA
00000130     3F0156702B5E712B722B732146013421
Starting address 00000000
```
