CC = gcc

.PHONY: clean

userschedtest: userschedtest.c
    $(CC) -o '$@' '$<'

clean:
    rm userschedtest