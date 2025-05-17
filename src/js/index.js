const addon = require("../../build/Release/seq-lead-keys.node");
const EventEmitter = require("events");
// abc
class SeqLeadKeys extends EventEmitter {
  constructor() {
    super();
    this._addon = addon;

    // Set up event handlers for the native addon
    this._addon.setLeadCallback((sequence) => {
      this.emit("lead", sequence);
    });

    this._addon.setPartialCallback((sequence) => {
      this.emit("command-partial", sequence);
    });

    this._addon.setCompleteCallback((sequence) => {
      this.emit("command", sequence);
    });
  }

  addCommand(sequence) {
    if (typeof sequence !== "string") {
      throw new Error("Sequence must be a string");
    }
    return this._addon.addCommand(sequence);
  }

  start() {
    new Promise((resolve) => {
      this._addon.start();
      resolve();
    });
    return this;
  }

  runInThread() {
    if (!this._addon.runInThread()) {
      throw new Error("Failed to start event loop in separate thread");
    }
    return this;
  }

  stop() {
    this._addon.stop();
  }
}

module.exports = SeqLeadKeys;
