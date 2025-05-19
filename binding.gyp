{
  "targets": [{
    "target_name": "lead-keys",
    "sources": [
      "src/native/addon.c",
      "src/native/seq_lead_keys.c",
      "src/native/command_tree.c"
    ],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
      "src/native"
    ],
    "dependencies": [
      "<!(node -p \"require('node-addon-api').gyp\")"
    ],
    "cflags!": [ "-fno-exceptions" ],
    "cflags_cc!": [ "-fno-exceptions" ],
    "xcode_settings": {
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
      "CLANG_ENABLE_OBJC_ARC": "YES",
      "OTHER_CFLAGS": [
        "-framework ApplicationServices"
      ]
    },
    "conditions": [
      ['OS=="mac"', {
        "xcode_settings": {
          "OTHER_CFLAGS": [
            "-framework ApplicationServices"
          ]
        }
      }]
    ]
  }]
} 