CXX      = clang++
CXXFLAGS = -std=c++1z -I../ -Wall -pedantic -g
LDFLAGS  = -L/usr/local/lib

experiments: experiments.cc 
	$(CXX) $(CXXFLAGS) -o experiments experiments.cc

v1: v1.cc 
	$(CXX) $(CXXFLAGS) -o v1 v1.cc


#$(CXX) $(LDFLAGS) $< -o $@

#test_type_map.o: type_map.hpp

clean:
	rm *.o experiments v1

#%.o : %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
