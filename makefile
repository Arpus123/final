CC=g++
CFLAGS= -Wall -pthread -std=c++11 
# el ggdb3 es para el valgrind  -ggdb3
OBJ=main.o message.o client.o broker.o simclient.o
# La regla siguiente es que cada archivo .c se compile en un .o con el mismo nombre
# Si se modifica un header cualquiera se recompila todo, explicitar las dependencias bien mas adelante
%.o: %.cpp *.h
	$(CC) -c -o $@ $*.cpp $(CFLAGS)
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o
