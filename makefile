CC=g++
CFLAGS= -Wall -pthread -std=c++11
OBJ=main.o message.o client.o broker.o
# La regla siguiente es que cada archivo .c se compile en un .o con el mismo nombre
%.o: %.cpp *.h
	$(CC) -c -o $@ $*.cpp $(CFLAGS)
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o
