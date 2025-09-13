#include <strings.h>
#define NOB_IMPLEMENTATION
#include "includes/nob.h"

#define DIST_FOLDER "dist/"
#define OBJ_FOLDER DIST_FOLDER "obj/"

const char* compile_object_async(Nob_Procs* procs, const char* source_path)
{
    const char* source_filename = nob_path_name(source_path);
    const char* last_dot = strrchr(source_filename, '.');
    size_t base_name_len = (last_dot == NULL) ? strlen(source_filename) : (size_t)(last_dot - source_filename);

    Nob_String_View base_name = nob_sv_from_parts(source_filename, base_name_len);
    const char* obj_path = nob_temp_sprintf(OBJ_FOLDER SV_Fmt ".o", SV_Arg(base_name));

    if (!nob_needs_rebuild1(obj_path, source_path)) {
        nob_log(NOB_INFO, "%s is up to date.", (obj_path));
        return obj_path;
    }

    Nob_Cmd cmd = { 0 };
    nob_cmd_append(&cmd, "gcc");
    nob_cmd_append(&cmd, "-c");
    nob_cc_flags(&cmd);
    nob_cc_inputs(&cmd, source_path);
    nob_cmd_append(&cmd, "-g", "-Iincludes", "-Iconfig");
    nob_cmd_append(&cmd, "-Wno-missing-braces");
    nob_cc_output(&cmd, obj_path);

    nob_cmd_run(&cmd, .async = procs);
    return obj_path;
}

void link_executable(const char* output_path, const char* main_obj, Nob_File_Paths common_objs)
{
    Nob_Cmd cmd = { 0 };
    nob_cc(&cmd);

    nob_cmd_append(&cmd, main_obj);
    nob_cmd_append(&cmd, OBJ_FOLDER"clay.o");
    nob_da_append_many(&cmd, common_objs.items, common_objs.count);

    nob_cc_output(&cmd, output_path);

    if (!nob_cmd_run(&cmd)) {
        nob_log(NOB_ERROR, "Failed to link executable: %s", output_path);
        exit(1);
    }
    printf("[DONE] %s linked! \n", output_path);
}

int main(int argc, char** argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    nob_mkdir_if_not_exists(DIST_FOLDER);
    nob_mkdir_if_not_exists(OBJ_FOLDER);
    nob_log(NOB_INFO, "Compiling source files...");

    Nob_Procs procs = { 0 };
    if (nob_needs_rebuild1(OBJ_FOLDER "clay.o", "includes/clay.h")) {
        Nob_Cmd cmd = { 0 };
        nob_cc(&cmd);
        nob_cmd_append(&cmd, "-c");
        nob_cmd_append(&cmd, "-x", "c");
        nob_cmd_append(&cmd, "-O3");
        nob_cmd_append(&cmd, "-DCLAY_IMPLEMENTATION");
        nob_cc_inputs(&cmd, "includes/clay.h");
        nob_cc_output(&cmd, OBJ_FOLDER "clay.o");
        nob_cmd_run(&cmd, .async = &procs);
    } else {
        nob_log(NOB_INFO, OBJ_FOLDER"clay.o is up to date.");
    }

    Nob_File_Paths common_objs = { 0 };

    const char* common_sources[] = { 
        "src/termgfx.c", 
        "src/buffer.c", 
        "src/renderer.c", 
        "src/layout.c", 
        "src/editor.c",
        "src/utf8.c",
        "src/io.c", 
    };


    for (size_t i = 0; i < NOB_ARRAY_LEN(common_sources); ++i) {
        const char* obj_path = compile_object_async(&procs, common_sources[i]);
        nob_da_append(&common_objs, obj_path);
    }

    const char* main_obj_path = compile_object_async(&procs, "src/main.c");
    const char* tests_obj_path = compile_object_async(&procs, "tests/main_tests.c");

    if (!nob_procs_wait(procs)) {
        nob_log(NOB_ERROR, "A compilation command failed.");
        return 1;
    }
    nob_log(NOB_INFO, "All source files compiled successfully.");

    nob_log(NOB_INFO, "Linking executables...");
    link_executable("dist/main", main_obj_path, common_objs);
    link_executable("dist/tests", tests_obj_path, common_objs);

    nob_da_free(procs);
    nob_da_free(common_objs);

    return 0;
}
