LIBS=~/lib/lib.a -lusb -lpthread
CFLAGS=-Wall -Werror --std=c99 -I/home/crpalmer

OBJS = temp-controller.o

temp-controller: $(OBJS) $(LIB)
	$(CC) $(OBJS) -o $@ $(LIBS)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
	gcc -c $(CFLAGS) $*.c -o $*.o
	gcc -MM $(CFLAGS) $*.c > $*.d

clean:
	-rm $(OBJS) $(OBJS:.o=.d)
