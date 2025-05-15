## Seq-Lead-Keys

**Seq-Lead-Keys** is a lightweight macOS utility that lets you bind and trigger custom command sequences via a double-Command key press.

### 🔧 Requirements

- macOS
- Xcode Command Line Tools
- Accessibility permissions (for global keyboard monitoring)

### 🚀 Installation & Build

```bash
make clean && make
```

### ⚙️ Usage

```bash
./seq-lead-keys -c <sequence> [-c <sequence> …]
```

- **-c**, **--command** `<sequence>`  
  Define one or more key sequences to listen for.

### 🔍 Operation

1. Quickly press ⌘ twice to enter “listen” mode.
2. Type your predefined sequence.
3. If it matches, the corresponding action fires.

### 🛠 Troubleshooting

> **“Failed to create event tap. Check Accessibility permissions”**
>
> 1. System Preferences → Security & Privacy → Privacy → Accessibility
> 2. Unlock and add Terminal (or the `seq-lead-keys` binary).

### 📂 Project Layout

- **seq-lead-keys.c** — Core event loop & logic
- **command_tree.c / .h** — Sequence-matching engine

---

**License:** MIT
