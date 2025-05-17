import robot from "robotjs";
import SeqLeadKeys from "../index";

const keyListener = new SeqLeadKeys();

keyListener.addListener("command", (sequence: string) => {
  console.log("command", sequence);
  if (sequence === "c") {
    // map to hotkey cmd+v
    robot.keyTap("c", "command");
  }
  if (sequence === "v") {
    // map to hotkey cmd+v
    robot.keyTap("v", "command");
  }
  if (sequence === "abc") {
    // map to hotkey cmd+shift+d
    robot.keyTap("d", ["command", "shift"]);
  }
});

keyListener.on("command-partial", (sequence: string) => {
  console.log("command-partial", sequence);
});

keyListener.on("lead", () => {
  console.log("lead activated");
});

keyListener.addCommand("c");
keyListener.addCommand("v");
keyListener.addCommand("abc");

try {
  keyListener.start();
} catch (error) {
  console.error(error);
}

// Using async IIFE to handle the infinite loop
(async () => {
  while (true) {
    console.log("waiting...");
    await new Promise((resolve) => setTimeout(resolve, 1000));
  }
})();
