const addon = require("./build/Release/seq-lead-keys.node");
const EventEmitter = require("events");

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
    this._addon.start();
    return this;
  }

  stop() {
    this._addon.stop();
  }
}

module.exports = SeqLeadKeys;
