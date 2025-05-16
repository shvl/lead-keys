CC = gcc
CFLAGS = -Wall -Wextra -framework ApplicationServices -framework CoreFoundation
NODE_GYP = node-gyp

all: build/seq-lead-keys build/Release/seq-lead-keys.node

build/seq-lead-keys: src/native/cmd.c src/native/seq_lead_keys.c src/native/command_tree.c src/native/parse_args.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^

build/Release/seq-lead-keys.node: src/native/addon.c src/native/seq_lead_keys.c src/native/command_tree.c
	$(NODE_GYP) configure
	$(NODE_GYP) build

clean:
	rm -rf build/
	$(NODE_GYP) clean

.PHONY: all clean 