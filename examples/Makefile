CXX      = clang++                                   # replace by g++ if desired
CXXFLAGS = -std=c++1z -I../nargs -Wall -pedantic -g

all : example-gl-function opengl-with-adl

example-gl-function     : example-gl-function.cpp
	$(CXX) $(CXXFLAGS) -o example-gl-function example-gl-function.cpp

opengl-with-adl: opengl-with-adl.cpp
	$(CXX) $(CXXFLAGS) -o opengl-with-adl opengl-with-adl.cpp -lglut -lGL


clean:
	rm example-gl-function opengl-with-adl

#%.o : %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
