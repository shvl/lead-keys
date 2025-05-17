import { EventEmitter } from "events";
// @ts-ignore
import addon from "../build/Release/seq-lead-keys.node";

interface NativeAddon {
  setLeadCallback: (callback: (sequence: string) => void) => void;
  setPartialCallback: (callback: (sequence: string) => void) => void;
  setCompleteCallback: (callback: (sequence: string) => void) => void;
  addCommand: (sequence: string) => void;
  start: () => void;
  stop: () => void;
}

// declare const addon: NativeAddon;

class SeqLeadKeys extends EventEmitter {
  private _addon: NativeAddon;
  private _staredPromise: Promise<void> | null = null;
  private _isStarted: boolean = false;

  constructor() {
    super();
    this._addon = addon;

    // Set up event handlers for the native addon
    this._addon.setLeadCallback((sequence: string) => {
      this.emit("lead", sequence);
    });

    this._addon.setPartialCallback((sequence: string) => {
      this.emit("command-partial", sequence);
    });

    this._addon.setCompleteCallback((sequence: string) => {
      this.emit("command", sequence);
    });
  }

  addCommand(sequence: string): void {
    if (typeof sequence !== "string") {
      throw new Error("Sequence must be a string");
    }
    this._addon.addCommand(sequence);
  }

  start(): SeqLeadKeys {
    if (this._isStarted) {
      return this;
    }
    this._staredPromise = new Promise<void>((resolve) => {
      this._addon.start();
      this._isStarted = true;
      resolve();
    });
    return this;
  }

  stop(): void {
    if (!this._isStarted) {
      return;
    }
    this._isStarted = false;
    this._addon.stop();
    this._staredPromise = null;
  }
}

export = SeqLeadKeys;
