all:
	gcc toralize.c -o toralize.so -fPIC -shared -ldl -D_SNU_SOURCE