#include <stdio.h>
#define CLAY_IMPLEMENTATION
#include <clay.h>

Clay_RenderCommandArray layout()
{
    Clay_BeginLayout();
    CLAY({ .id = CLAY_ID("background"),
        .backgroundColor = (Clay_Color) { 33, 40, 48, 255 },
        .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } , .layoutDirection=CLAY_TOP_TO_BOTTOM} }

            )
    {
        CLAY_TEXT(CLAY_STRING("Zi Editor"), CLAY_TEXT_CONFIG());
        CLAY({ .custom = { .customData = "content" },
            .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } } });
        CLAY_TEXT(CLAY_STRING("Status bar"), CLAY_TEXT_CONFIG());
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
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
    return (Clay_Dimensions) { .width = text.length * 1, // <- this will only work for monospace fonts, see the
                                                         // renderers/ directory for more advanced text measurement
        .height = 1 };
}

void layout_init(int screenWidth, int screenHeight)
{
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    Clay_Initialize(
        arena, (Clay_Dimensions) { screenWidth, screenHeight }, (Clay_ErrorHandler) { HandleClayErrors, NULL });
    Clay_SetMeasureTextFunction(MeasureText, NULL);
}

void layout_set_dimension(int width, int height)
{
    Clay_SetLayoutDimensions((Clay_Dimensions){width, height});
}
