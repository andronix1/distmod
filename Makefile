CC=gcc
CFLAGS=-O3 -I include
EXAMPLES_CFLAGS=-lm

LD=ar
LD_FLAGS=rcsv

BUILD_DIR=build
BUILD_EXAMPLES_DIR=$(BUILD_DIR)/examples
BUILD_OBJ_DIR=$(BUILD_DIR)/obj
LIBRARY_OUTPUT=$(BUILD_DIR)/libdistrand.a

run: build
	$(BUILD_OUTPUT)

examples: build-examples
	@echo "running tests..."
	./$(BUILD_EXAMPLES_DIR)/distribution
	@echo "finished!"

build: build-obj
	$(LD) $(LD_FLAGS) $(LIBRARY_OUTPUT) $(shell find $(BUILD_OBJ_DIR) -name '*.o')

build-obj: setup src/distributed/edsrm/mnt src/distributed/edsrm/2rng src/distributed/ziggurat/mnt src/prob_eq/dbd src/uniform/multiplicative src/uniform/mt19937_64 

build-examples: setup examples/distribution

examples/%: examples/%.c
	$(CC) $(CFLAGS) $(EXAMPLES_CFLAGS) $^ $(LIBRARY_OUTPUT) -o $(BUILD_EXAMPLES_DIR)/$* -lm

src/%: src/%.c
	mkdir -p $(BUILD_OBJ_DIR)/$@
	$(CC) $(CFLAGS) -c $^ -o $(BUILD_OBJ_DIR)/$@.o

setup:
	mkdir -p $(BUILD_EXAMPLES_DIR)
	mkdir -p $(BUILD_OBJ_DIR)

