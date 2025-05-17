# seq-lead-keys

A library for listening to command key sequences on macOS.

## How It Works

This library allows you to map sequences of keys (like "⌘ ⌘ → a → b") to actions, similar to Vim's command sequences. Instead of requiring complex keyboard shortcuts that can be hard to remember and may conflict with existing system shortcuts, it uses simple sequential key presses.

Key advantages of this approach:

- Avoids conflicts with system/application shortcuts
- More combinations available than traditional hotkeys
- Easier to remember logical sequences (e.g. "q" for quick actions)
- Reduces strain from holding multiple modifier keys

### Lead Sequence (“Activation”)

The user presses a short “lead” sequence of keys (e.g. ⌘ ⌘) to enter a special listening mode.

During this mode, the system captures the next key (or keys) as the actual command.

### Command Key(s)

After the lead sequence, every subsequent key (or key-combo) is treated as an instruction until you either time out or press an explicit “exit” sequence.

You can group commands:

```
⌘ ⌘ → Q → 1
⌘ ⌘ → Q → 2
```

Here ⌘ ⌘ Q invokes “quick action,” while 1 or 2 selects specific sub-actions (e.g. “open inbox,” “start timer”).

## Installation

```bash
npm install seq-lead-keys
```

## Usage

### As a Node.js module

While primarily a command-line tool, seq-lead-keys can also be used as a Node.js module in Electron apps.

```bash
npm install seq-lead-keys
```

```javascript
const SeqLeadKeys = require("seq-lead-keys");
const robot = require("robotjs");

const keyListener = new SeqLeadKeys();
keyListener.addCommand("c");
keyListener.addCommand("v");
keyListener.addCommand("q1");
keyListener.addCommand("q2");
keyListener.addCommand("abc");

keyListener.addListener("command", (sequence) => {
  console.log(`command ${sequence} activated`);

  // map hotkeys with robotjs
  if (sequence === "c") {
    // map to hotkey cmd+v
    robot.keyTap("c", "command");
  }
  if (sequence === "v") {
    // map to hotkey cmd+v
    robot.keyTap("v", "command");
  }
  if (sequence === "q1") {
    // map to hotkey cmd+v
    robot.keyTap("q", ["command", "shift"]);
  }
});

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

```bash
# Install dependencies
npm install

# Build the Node.js addon
npm run install

# Build the command-line tool
npm run build:cli
```

### Command Line Tool

```bash
# Build the command-line tool
npm run build:cli

# Run the tool
./build/seq-lead-keys -c abc -c xyz
```

### Testing

TBD

```bash
npm test
```

## License

MIT
