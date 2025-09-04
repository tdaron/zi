#include <strings.h>
#define NOB_IMPLEMENTATION
#include "includes/nob.h"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  nob_mkdir_if_not_exists("dist");
  char *output = "dist/main";

  Nob_Cmd cmd = {0};
  nob_cc(&cmd);
  nob_cc_flags(&cmd);
  nob_cc_inputs(&cmd, "src/main.c", "src/buffer.c", "src/renderer.c");
  nob_cmd_append(&cmd, "-g","-lraylib");
  nob_cmd_append(&cmd, "-Wno-missing-braces");
  nob_cc_output(&cmd, output);

  nob_cmd_run(&cmd);

  printf("Compilation suceeded ! \n");
  return 0;
}
