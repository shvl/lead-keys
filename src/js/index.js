const addon = require("./build/Release/seq-lead-keys.node");

class SeqLeadKeys {
  constructor() {
    this._addon = addon;
  }

  addCommand(sequence) {
    if (typeof sequence !== "string") {
      throw new Error("Sequence must be a string");
    }
    return this._addon.addCommand(sequence);
  }

  start() {
    return this._addon.start();
  }

  stop() {
    this._addon.stop();
  }
}

module.exports = SeqLeadKeys;
