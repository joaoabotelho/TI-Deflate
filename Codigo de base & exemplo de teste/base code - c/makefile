run: exec
	gzip $(file)
	rm -rf $(file)
	./exec $(file).gz
exec: gzip.cpp huffman.cpp
	g++ -g -Wall gzip.cpp huffman.cpp -o exec
