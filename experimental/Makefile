CXX      = clang++
CXXFLAGS = -std=c++1z -I../nargs -Wall -pedantic -g
LDFLAGS  = -L/usr/local/lib

default_arg_collector: default_arg_collector.cc
	$(CXX) $(CXXFLAGS) -o default_arg_collector default_arg_collector.cc


binder: binder.cc
	$(CXX) $(CXXFLAGS) -o binder binder.cc

experiments: experiments.cc 
	$(CXX) $(CXXFLAGS) -o experiments experiments.cc

v1: v1.cc 
	$(CXX) $(CXXFLAGS) -o v1 v1.cc

arg_keeper: arg_keeper.cc
	$(CXX) $(CXXFLAGS) -o arg_keeper arg_keeper.cc

#$(CXX) $(LDFLAGS) $< -o $@

#test_type_map.o: type_map.hpp

clean:
	rm *.o experiments v1 arg_keeper binder default_arg_collector

#%.o : %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
