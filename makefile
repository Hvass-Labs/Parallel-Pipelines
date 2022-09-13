CXX=g++
CXXFLAGS=-Wall -lpthread

all: main1 main2 main3 main4

main1:
	$(CXX) $(CXXFLAGS) main1.cpp -o main1

main2:
	$(CXX) $(CXXFLAGS) main2.cpp -o main2

main3:
	$(CXX) $(CXXFLAGS) main3.cpp -o main3

main4:
	$(CXX) $(CXXFLAGS) main4.cpp -o main4

clean:
	$(RM) main1 main2 main3 main4
