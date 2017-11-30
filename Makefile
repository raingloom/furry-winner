CC= clang -std=c++11 -Isrc/
check: src/Monopoly.cpp
	$(CC) -DDOCTEST_CONFIG_DISABLE -c -o /dev/null src/Monopoly.cpp
out/Monopoly.test: src/Monopoly.cpp src/test.cpp
	$(CC) -g -O0 -lstdc++ -o out/Monopoly.test src/test.cpp
out/doc.pdf: src/doc.md prep.lua
	lua prep.lua src/doc.md > out/tmp.md
	pandoc --from=markdown --to=latex out/tmp.md -o out/doc.pdf
gdb: out/Monopoly.test
	gdb -q out/Monopoly.test
test: out/Monopoly.test
	out/Monopoly.test
clean:
	rm out/*
