const SeqLeadKeys = require("../src/js/index.js");
const robot = require("robotjs");

describe("SeqLeadKeys", () => {
  let keyListener;

  beforeEach(() => {
    keyListener = new SeqLeadKeys();
  });

  afterEach(() => {
    keyListener.stop();
  });

  test("should add command successfully", () => {
    expect(keyListener.addCommand("abc")).toBe(true);
    keyListener.start();
    keyListener.on("command", (sequence) => {
      expect(sequence).toBe("abc");
    });
    // Simulate double-cmd followed by "abc"
    robot.keyToggle("command", "down");
    robot.keyToggle("command", "up");
    robot.setKeyboardDelay(100);
    robot.keyToggle("command", "down");
    robot.keyToggle("command", "up");
    robot.keyTap("a");
    robot.keyTap("b");
    robot.keyTap("c");
  });

  test("should reject non-string commands", () => {
    expect(() => keyListener.addCommand(123)).toThrow(
      "Sequence must be a string"
    );
  });

  test("should start and stop listening", () => {
    expect(keyListener.start()).toBe(true);
    keyListener.stop();
  });
});
