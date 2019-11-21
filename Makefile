GLOG = /usr/local

all:
	g++ -std=c++11 -O3 -g -Wall src/*.cc -shared -fPIC -o lib/libpipeline.so \
		-I include \
		-I ${GLOG}/include -L ${GLOG}/lib -lglog \
		-lpthread \

#	g++ -std=c++11 -O3 -g -Wall test/*.cpp -o bin/test_pipeline \
		-I include -L lib -lpipeline \
		-I /usr/local/include -L /usr/local/lib -lglog \
		-lpthread \
		`pkg-config opencv --libs --cflags` \
		-I /usr/local/neuware/include -L /usr/local/neuware/lib64 -lcnrt -lcnml
