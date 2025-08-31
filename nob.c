#include <strings.h>
#define NOB_IMPLEMENTATION
#include "includes/nob.h"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  nob_mkdir_if_not_exists("dist");
  Nob_Cmd cmd = {0};

  nob_cc(&cmd);

  char *output = "dist/main";

  if (argc > 1 && strcasecmp(argv[1], "test") == 0) {
    nob_cc_inputs(&cmd, "src/tests.c");
    output = "dist/tests";
  } else {
    nob_cc_flags(&cmd);
    // TODO: Async build
    nob_cc_inputs(&cmd, "src/main.c", "src/layout.c", "src/text_handling.c",
                  "src/renderer/sdl.c", "src/renderer/sdl_clay.c");
    nob_cmd_append(&cmd, "-lSDL3");
    nob_cmd_append(&cmd, "-lSDL3_ttf");
    nob_cmd_append(&cmd, "-g");
  }
  nob_cc_output(&cmd, output);

  nob_cmd_run(&cmd);

  nob_cmd_append(&cmd, output);
  nob_cmd_run(&cmd);

  return 0;
}
