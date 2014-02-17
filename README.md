This code creates a simple db with random graph generation
key | value = key1;key2;key3 ...

Then it retrieves 2 randomly generated keys and returns how far the 2 keys are, and how long it took to find the path.

This was used for a school project to do various benchmarks with rocksdb. 

Compiled with:
g++ -g -std=c++11 graph_search.cpp -o graphSearch -Iinclude/ -L. -lz -lbz2 -lrocksdb -pthread -lsnappy 

Have fun !
