#include <strings.h>
#define NOB_IMPLEMENTATION
#include "includes/nob.h"



void add_sources(Nob_Cmd* cmd) {
  nob_cc_inputs(cmd, "src/buffer.c");  
}

void build_exec(char* mainFile, char* output) {
  Nob_Cmd cmd = {0};
  nob_cc(&cmd);
  nob_cc_flags(&cmd);
  nob_cc_inputs(&cmd, mainFile);
  add_sources(&cmd);
  nob_cmd_append(&cmd, "-g", "-Iincludes");
  nob_cmd_append(&cmd, "-Wno-missing-braces");
  nob_cc_output(&cmd, output);
  nob_cmd_run(&cmd);

  printf("[DONE] %s compiled ! \n", output);
  
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  nob_mkdir_if_not_exists("dist");
  build_exec("src/main.c", "dist/main");
  build_exec("tests/main.c", "dist/tests");

  return 0;
}
