const SeqLeadKeys = require("../src/js/index.js");

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
