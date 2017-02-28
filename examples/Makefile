CXX      = clang++                                   # replace by g++ if desired
CXXFLAGS = -std=c++1z -I../nargs -Wall -pedantic -g

example-1     : example-1.cpp
	$(CXX) $(CXXFLAGS) -o example-1 example-1.cpp


clean:
	rm example-1

#%.o : %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
