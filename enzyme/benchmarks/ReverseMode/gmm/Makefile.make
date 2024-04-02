# RUN: if [ %llvmver -ge 12 ] || [ %llvmver -le 9 ]; then cd %S && LD_LIBRARY_PATH="%bldpath:$LD_LIBRARY_PATH" BENCH="%bench" BENCHLINK="%blink" LOAD="%newLoadClangEnzyme" make -B gmm.o results.json -f %s; fi

.PHONY: clean

dir := $(abspath $(lastword $(MAKEFILE_LIST))/../../../..)

clean:
	rm -f *.ll *.o results.txt results.json

$(dir)/benchmarks/ReverseMode/gmm/target/release/libgmmrs.a: src/lib.rs Cargo.toml
	ENZYME_PRINT_AA=1 ENZYME_PRINT=1 ENZYME_LOOSE_TYPES=1 cargo +enzyme rustc --release --lib --crate-type=staticlib --features=libm &> compile.log

gmm.o: gmm.cpp $(dir)/benchmarks/ReverseMode/gmm/target/release/libgmmrs.a
	clang++ $(LOAD) $(BENCH) gmm.cpp -I /usr/include/c++/11 -I/usr/include/x86_64-linux-gnu/c++/11 -O2 -o gmm.o -lpthread $(BENCHLINK) -lm $(dir)/benchmarks/ReverseMode/gmm/target/release/libgmmrs.a -L /usr/lib/gcc/x86_64-linux-gnu/11

results.json: gmm.o
	./$^
