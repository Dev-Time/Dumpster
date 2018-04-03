CXX=g++
CPPFLAGS=-g -std=c++17
LDFLAGS=-g
RM=rm -f

all: dv rm dump

dv: dv.o
	g++ $(LDFLAGS) -o dv dv.o

dv.o: dv.cpp
	g++ $(CPPFLAGS) -c dv.cpp

rm: rm.o
	g++ $(LDFLAGS) -o rm rm.o

rm.o: rm.cpp
	g++ $(CPPFLAGS) -c rm.cpp

dump: dump.o
	g++ $(LDFLAGS) -o dump dump.o

dump.o: dump.cpp
	g++ $(CPPFLAGS) -c dump.cpp

clean:
	$(RM) dv rm dump dv.o rm.o dv.o