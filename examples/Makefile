CXX      = clang++                                   # replace by g++ if desired
CXXFLAGS = -std=c++1z -I../nargs -Wall -pedantic -g

example-gl-function     : example-gl-function.cpp
	$(CXX) $(CXXFLAGS) -o example-gl-function example-gl-function.cpp


clean:
	rm example-gl-function

#%.o : %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
