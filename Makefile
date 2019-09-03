CC = gcc
LINK = $(CC)

SRC = ./src/c
BUILD = ./bin
MISC  = ./misc

CFLAGS = $$(python3-config --includes --ldflags)
 
BINS = $(BUILD)/class \
	$(BUILD)/simple \
	$(BUILD)/func

BINS2 = class \
	simple \
	func

all: prebuild \
	$(BINS)

$(BUILD)/class: \
	$(SRC)/class.c
	$(CC) $(SRC)/class.c $(CFLAGS) -o $(BUILD)/class 

$(BUILD)/simple: \
	$(SRC)/simple.c
	$(CC) $(SRC)/simple.c $(CFLAGS) -o $(BUILD)/simple
	
$(BUILD)/func: \
	$(SRC)/func.c
	$(CC) $(SRC)/func.c $(CFLAGS) -o $(BUILD)/func

clean:
	rm -rf $(BUILD)
	
prebuild:
	test -d $(BUILD) || mkdir -p $(BUILD)
	