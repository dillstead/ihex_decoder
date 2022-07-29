CFLAGS += -Werror -Wno-format -pedantic -std=c99 -Wall -Wextra -D_POSIX_C_SOURCE -fsanitize=address,undefined -g3 $(OPTIONS)
LDFLAGS += -fsanitize=address,undefined

ihex_decoder: main.o decode.o read.o
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o ihex_decoder
clean:
	rm *.o ihex_decoder
