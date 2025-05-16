# seq-lead-keys

A library for listening to command key sequences on macOS.

## Installation

```bash
npm install seq-lead-keys
```

## Usage

### As a Node.js module

```javascript
const SeqLeadKeys = require("seq-lead-keys");

const keyListener = new SeqLeadKeys();
keyListener.addCommand("abc");
keyListener.addCommand("xyz");
keyListener.start();

// Later, when you want to stop:
keyListener.stop();
```

### As a command-line tool

```bash
# Build the command-line tool
npm run build:cli

# Run the tool
./build/seq-lead-keys -c abc -c xyz
```

## Development

### Project Structure

```
├── binding.gyp              # node-gyp build description
├── package.json             # npm metadata & scripts
├── README.md
├── .gitignore
│
├── src/
│   ├── native/              # all C/C++ code lives here
│   │   ├── addon.c          # entry point: N-API module registration
│   │   ├── seq_lead_keys.c  # core library implementation
│   │   ├── seq_lead_keys.h  # library header
│   │   ├── command_tree.c   # command tree implementation
│   │   ├── command_tree.h   # command tree header
│   │   ├── parse_args.c     # command-line argument parsing
│   │   ├── parse_args.h     # argument parsing header
│   │   └── cmd.c            # command-line interface
│   │
│   └── js/                  # JS "wrapper" code lives here
│       └── index.js         # exports & high-level API
│
└── test/
    └── test.js              # unit tests
```

### Building

```bash
# Install dependencies
npm install

# Build the Node.js addon
npm run install

# Build the command-line tool
npm run build:cli
```

### Testing

```bash
npm test
```

## License

MIT
