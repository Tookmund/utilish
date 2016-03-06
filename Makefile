FLAGS = -Wall -Werror

ish: ish.o token.o

$(OBJS): $(HFILES)

clean:
	rm *.o ish
