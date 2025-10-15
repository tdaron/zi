#include "termgfx.h"
#include <clay.h>
#include <editor.h>
#include <palette.h>
#include <stdio.h>
#include <string.h>

char* clayMemory;

void buffersBar()
{

    CLAY({.layout = { .sizing = {
                        CLAY_SIZING_GROW(0),
                        CLAY_SIZING_FIT(0),
                    }, .childGap=2, }, .backgroundColor=STATUS_COLOR })
    {
        for (int i = 0; i < editor.viewsCount; i++) {
            char* n = editor.views[i].buffer->name;
            Clay_String name = { .length = strlen(n), .chars = n, .isStaticallyAllocated = true };
            Clay_Color c = INACTIVE_BUFFER;
            if (i == editor.currentView) {
                c = (Clay_Color)ACTIVE_BUFFER;
            }
            CLAY_TEXT(name, CLAY_TEXT_CONFIG({ .textColor = c }));
        }
    }
}
char toto[50];

void statusBar()
{

    CLAY({ .backgroundColor = STATUS_COLOR,
        .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) }, .childGap = 2 } })
    {
        char* mode_name = editor.modes.data[editor.current_mode].short_name;
        Clay_String s = {.length = strlen(mode_name), .chars = mode_name, .isStaticallyAllocated=false};
        CLAY_TEXT(s, CLAY_TEXT_CONFIG({ .textColor = ACTIVE_BUFFER }));
        char* n = editor.views[editor.currentView].buffer->name;
        Clay_String name = { .length = strlen(n), .chars = n, .isStaticallyAllocated = true };
        CLAY({ .layout = { .childGap = 0 } })
        {
            CLAY_TEXT(name, CLAY_TEXT_CONFIG({ .textColor = ACTIVE_BUFFER }));
            CLAY_TEXT(CLAY_STRING("[+]"), CLAY_TEXT_CONFIG({ .textColor = ACTIVE_BUFFER }));
        }
    }
}

void bottomline()
{
    CLAY({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(1) } } })
    {
        // if (editor.mode == INPUT_MODE) {
        //     Clay_String input
        //         = { .isStaticallyAllocated = true, .chars = editor.userInput.input, .length = editor.userInput.length };

        //     CLAY_TEXT(SV_TO_CLAY(editor.userInput.prefix), CLAY_TEXT_CONFIG({ .textColor = ACTIVE_BUFFER }));
        //     CLAY_TEXT(input, CLAY_TEXT_CONFIG({ .textColor = ACTIVE_BUFFER }));

        if (editor.message != 0) {
            char* n = editor.message;
            Clay_String message = { .length = strlen(n), .chars = n, .isStaticallyAllocated = true };

            CLAY_TEXT(message, CLAY_TEXT_CONFIG({ .textColor = ACTIVE_BUFFER }));
        }
    }
}
Clay_RenderCommandArray layout()
{
    Clay_BeginLayout();
    CLAY({ .id = CLAY_ID("background"),
        .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .layoutDirection = CLAY_TOP_TO_BOTTOM },
        .backgroundColor = BACKGROUND_COLOR }

    )
    {
        buffersBar();
        CLAY({ .custom = { .customData = "content" },
            .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } } });
        statusBar();
        bottomline();
    }
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return renderCommands;
}

void HandleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    printf("[CLAY] %s\n", errorData.errorText.chars);
}

// Example measure text function
Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData)
{
    (void)(config);
    (void)(userData);
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
    return (Clay_Dimensions) { .width = text.length * 1, // <- this will only work for monospace fonts, see the
                                                         // renderers/ directory for more advanced text measurement
        .height = 1 };
}

void layout_init(int screenWidth, int screenHeight)
{
    Clay_SetMaxMeasureTextCacheWordCount(0); // Dont need much
    Clay_SetMaxElementCount(256); // Dont need much.
    uint64_t totalMemorySize = Clay_MinMemorySize();
    clayMemory = malloc(totalMemorySize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, clayMemory);
    Clay_Initialize(
        arena, (Clay_Dimensions) { screenWidth, screenHeight }, (Clay_ErrorHandler) { HandleClayErrors, NULL });
    Clay_SetMeasureTextFunction(MeasureText, NULL);
}

void layout_free() {
    free(clayMemory);
}

void layout_set_dimension(int width, int height) { Clay_SetLayoutDimensions((Clay_Dimensions) { width, height }); }
