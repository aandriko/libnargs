CXX      = clang++
CXXFLAGS = -std=c++14 -I../ -Wall -pedantic
LDFLAGS  = -L/usr/local/lib

experiments: experiments.cc 
	$(CXX) $(CXXFLAGS) -o experiments experiments.cc
#$(CXX) $(LDFLAGS) $< -o $@

#test_type_map.o: type_map.hpp

clean:
	rm *.o experiments

#%.o : %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
