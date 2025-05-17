const SeqLeadKeys = require("../src/js/index.js");
// const robot = require("robotjs");

describe("SeqLeadKeys", () => {
  let keyListener;

  beforeEach(() => {
    keyListener = new SeqLeadKeys();
  });

  afterEach(() => {
    keyListener.stop();
  });

  test("should add command successfully", async () => {
    expect(keyListener.addCommand("abc")).toBe(true);
  });

  test("should reject non-string commands", () => {
    expect(() => keyListener.addCommand(123)).toThrow(
      "Sequence must be a string"
    );
  });
});
