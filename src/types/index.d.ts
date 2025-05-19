declare module "../build/Release/lead-keys.node" {
  export function setLeadCallback(cb: (sequence: string) => void): void;
  export function setPartialCallback(cb: (sequence: string) => void): void;
  export function setCompleteCallback(cb: (sequence: string) => void): void;
  export function addCommand(sequence: string): void;
  export function start(): void;
  export function stop(): void;
}
