using System;
using System.Runtime.InteropServices;
using System.Text;

public static class ImGui
{
#if UNITY_IOS && !UNITY_EDITOR
    const string Lib = "__Internal";
#else
    const string Lib = "UnityImGuiPlugin";
#endif

    // -----------------------------------------------------------------------
    // Structs
    // -----------------------------------------------------------------------
    [StructLayout(LayoutKind.Sequential)]
    public struct ImVec2
    {
        public float x, y;
        public ImVec2(float x, float y) { this.x = x; this.y = y; }
        public static implicit operator ImVec2(UnityEngine.Vector2 v) => new ImVec2(v.x, v.y);
        public static implicit operator UnityEngine.Vector2(ImVec2 v) => new UnityEngine.Vector2(v.x, v.y);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ImVec4
    {
        public float x, y, z, w;
        public ImVec4(float x, float y, float z, float w) { this.x = x; this.y = y; this.z = z; this.w = w; }
        public static implicit operator ImVec4(UnityEngine.Color c) => new ImVec4(c.r, c.g, c.b, c.a);
        public static implicit operator UnityEngine.Color(ImVec4 v) => new UnityEngine.Color(v.x, v.y, v.z, v.w);
    }

    // -----------------------------------------------------------------------
    // Enums
    // -----------------------------------------------------------------------
    [Flags] public enum ImGuiWindowFlags : int
    {
        None                   = 0,
        NoTitleBar             = 1 << 0,
        NoResize               = 1 << 1,
        NoMove                 = 1 << 2,
        NoScrollbar            = 1 << 3,
        NoScrollWithMouse      = 1 << 4,
        NoCollapse             = 1 << 5,
        AlwaysAutoResize       = 1 << 6,
        NoBackground           = 1 << 7,
        NoSavedSettings        = 1 << 8,
        NoMouseInputs          = 1 << 9,
        MenuBar                = 1 << 10,
        HorizontalScrollbar    = 1 << 11,
        NoFocusOnAppearing     = 1 << 12,
        NoBringToDisplayOnFocus= 1 << 13,
        AlwaysVerticalScrollbar= 1 << 14,
        AlwaysHorizontalScrollbar=1<< 15,
        NoNavInputs            = 1 << 16,
        NoNavFocus             = 1 << 17,
        UnsavedDocument        = 1 << 18,
        NoNav                  = NoNavInputs | NoNavFocus,
        NoDecoration           = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
        NoInputs               = NoMouseInputs | NoNavInputs | NoNavFocus,
    }

    [Flags] public enum ImGuiChildFlags : int
    {
        None         = 0,
        Borders      = 1 << 0,
        AlwaysUseWindowPadding = 1 << 1,
        ResizeX      = 1 << 2,
        ResizeY      = 1 << 3,
        AutoResizeX  = 1 << 4,
        AutoResizeY  = 1 << 5,
        AlwaysAutoResize = 1 << 6,
        FrameStyle   = 1 << 7,
    }

    public enum ImGuiCond : int
    {
        None         = 0,
        Always       = 1 << 0,
        Once         = 1 << 1,
        FirstUseEver = 1 << 2,
        Appearing    = 1 << 3,
    }

    public enum ImGuiDir : int { None = -1, Left, Right, Up, Down }

    [Flags] public enum ImGuiInputTextFlags : int
    {
        None                = 0,
        CharsDecimal        = 1 << 0,
        CharsHexadecimal    = 1 << 1,
        CharsScientific     = 1 << 2,
        CharsUppercase      = 1 << 3,
        CharsNoBlank        = 1 << 4,
        AllowTabInput       = 1 << 5,
        EnterReturnsTrue    = 1 << 6,
        EscapeClearsAll     = 1 << 7,
        CtrlEnterForNewLine = 1 << 8,
        ReadOnly            = 1 << 14,
        Password            = 1 << 15,
        AlwaysOverwrite     = 1 << 16,
        AutoSelectAll       = 1 << 17,
        ParseEmptyRefVal    = 1 << 18,
        DisplayEmptyRefVal  = 1 << 19,
        NoHorizontalScroll  = 1 << 20,
        NoUndoRedo          = 1 << 21,
        ElideLeft           = 1 << 22,
    }

    [Flags] public enum ImGuiTreeNodeFlags : int
    {
        None             = 0,
        Selected         = 1 << 0,
        Framed           = 1 << 1,
        AllowOverlap     = 1 << 2,
        NoTreePushOnOpen = 1 << 3,
        NoAutoOpenOnLog  = 1 << 4,
        DefaultOpen      = 1 << 5,
        OpenOnDoubleClick= 1 << 6,
        OpenOnArrow      = 1 << 7,
        Leaf             = 1 << 8,
        Bullet           = 1 << 9,
        FramePadding     = 1 << 10,
        SpanAvailWidth   = 1 << 11,
        SpanFullWidth    = 1 << 12,
        SpanLabelWidth   = 1 << 13,
        SpanAllColumns   = 1 << 14,
        CollapsingHeader = Framed | NoTreePushOnOpen | NoAutoOpenOnLog,
    }

    [Flags] public enum ImGuiSelectableFlags : int
    {
        None             = 0,
        NoAutoClosePopups= 1 << 0,
        SpanAllColumns   = 1 << 1,
        AllowDoubleClick = 1 << 2,
        Disabled         = 1 << 3,
        AllowOverlap     = 1 << 4,
        Highlight        = 1 << 5,
    }

    [Flags] public enum ImGuiComboFlags : int
    {
        None           = 0,
        PopupAlignLeft = 1 << 0,
        HeightSmall    = 1 << 1,
        HeightRegular  = 1 << 2,
        HeightLarge    = 1 << 3,
        HeightLargest  = 1 << 4,
        NoArrowButton  = 1 << 5,
        NoPreview      = 1 << 6,
        WidthFitPreview= 1 << 7,
    }

    [Flags] public enum ImGuiTableFlags : int
    {
        None                  = 0,
        Resizable             = 1 << 0,
        Reorderable           = 1 << 1,
        Hideable              = 1 << 2,
        Sortable              = 1 << 3,
        NoSavedSettings       = 1 << 4,
        ContextMenuInBody     = 1 << 5,
        RowBg                 = 1 << 6,
        BordersInnerH         = 1 << 7,
        BordersOuterH         = 1 << 8,
        BordersInnerV         = 1 << 9,
        BordersOuterV         = 1 << 10,
        BordersH              = BordersInnerH | BordersOuterH,
        BordersV              = BordersInnerV | BordersOuterV,
        BordersInner          = BordersInnerH | BordersInnerV,
        BordersOuter          = BordersOuterH | BordersOuterV,
        Borders               = BordersInner | BordersOuter,
        NoBordersInBody       = 1 << 11,
        NoBordersInBodyUntilResize = 1 << 12,
        ScrollX               = 1 << 13,
        ScrollY               = 1 << 14,
        SizingFixedFit        = 1 << 15,
        SizingFixedSame       = 2 << 15,
        SizingStretchProp     = 3 << 15,
        SizingStretchSame     = 4 << 15,
    }

    [Flags] public enum ImGuiTableColumnFlags : int
    {
        None                 = 0,
        Disabled             = 1 << 0,
        DefaultHide          = 1 << 1,
        DefaultSort          = 1 << 2,
        WidthStretch         = 1 << 3,
        WidthFixed           = 1 << 4,
        NoResize             = 1 << 5,
        NoReorder            = 1 << 6,
        NoHide               = 1 << 7,
        NoClip               = 1 << 8,
        NoSort               = 1 << 9,
        NoSortAscending      = 1 << 10,
        NoSortDescending     = 1 << 11,
        NoHeaderLabel        = 1 << 12,
        NoHeaderWidth        = 1 << 13,
        PreferSortAscending  = 1 << 14,
        PreferSortDescending = 1 << 15,
        IndentEnable         = 1 << 16,
        IndentDisable        = 1 << 17,
        AngledHeader         = 1 << 18,
    }

    [Flags] public enum ImGuiColorEditFlags : int
    {
        None            = 0,
        NoAlpha         = 1 << 1,
        NoPicker        = 1 << 2,
        NoOptions       = 1 << 3,
        NoSmallPreview  = 1 << 4,
        NoInputs        = 1 << 5,
        NoTooltip       = 1 << 6,
        NoLabel         = 1 << 7,
        NoSidePreview   = 1 << 8,
        NoDragDrop      = 1 << 9,
        NoBorder        = 1 << 10,
        AlphaOpaque     = 1 << 11,
        AlphaNoBg       = 1 << 12,
        AlphaPreview    = 1 << 13,
        AlphaPreviewHalf= 1 << 14,
        HDR             = 1 << 19,
        DisplayRGB      = 1 << 20,
        DisplayHSV      = 1 << 21,
        DisplayHex      = 1 << 22,
        Uint8           = 1 << 23,
        Float           = 1 << 24,
        PickerHueBar    = 1 << 25,
        PickerHueWheel  = 1 << 26,
        InputRGB        = 1 << 27,
        InputHSV        = 1 << 28,
    }

    [Flags] public enum ImGuiSliderFlags : int
    {
        None            = 0,
        Logarithmic     = 1 << 5,
        NoRoundToFormat = 1 << 6,
        NoInput         = 1 << 7,
        WrapAround      = 1 << 8,
        ClampOnInput    = 1 << 9,
        ClampZeroRange  = 1 << 10,
        AlwaysClamp     = ClampOnInput | ClampZeroRange,
    }

    [Flags] public enum ImGuiHoveredFlags : int
    {
        None                          = 0,
        ChildWindows                  = 1 << 0,
        RootWindow                    = 1 << 1,
        AnyWindow                     = 1 << 2,
        NoPopupHierarchy              = 1 << 3,
        AllowWhenBlockedByPopup       = 1 << 5,
        AllowWhenBlockedByActiveItem  = 1 << 7,
        AllowWhenOverlappedByItem     = 1 << 8,
        AllowWhenOverlappedByWindow   = 1 << 9,
        AllowWhenDisabled             = 1 << 10,
        NoNavOverride                 = 1 << 11,
        AllowWhenOverlapped           = AllowWhenOverlappedByItem | AllowWhenOverlappedByWindow,
        RectOnly                      = AllowWhenBlockedByPopup | AllowWhenBlockedByActiveItem | AllowWhenOverlapped,
        RootAndChildWindows           = RootWindow | ChildWindows,
        ForTooltip                    = 1 << 12,
        Stationary                    = 1 << 13,
        DelayNone                     = 1 << 14,
        DelayShort                    = 1 << 15,
        DelayNormal                   = 1 << 16,
        NoSharedDelay                 = 1 << 17,
    }

    [Flags] public enum ImGuiFocusedFlags : int
    {
        None            = 0,
        ChildWindows    = 1 << 0,
        RootWindow      = 1 << 1,
        AnyWindow       = 1 << 2,
        NoPopupHierarchy= 1 << 3,
        RootAndChildWindows = RootWindow | ChildWindows,
    }

    [Flags] public enum ImGuiPopupFlags : int
    {
        None                    = 0,
        MouseButtonLeft         = 0,
        MouseButtonRight        = 1,
        MouseButtonMiddle       = 2,
        NoReopen                = 1 << 5,
        NoOpenOverExistingPopup = 1 << 7,
        NoOpenOverItems         = 1 << 8,
        AnyPopupId              = 1 << 10,
        AnyPopupLevel           = 1 << 11,
        AnyPopup                = AnyPopupId | AnyPopupLevel,
    }

    public enum ImGuiMouseButton : int { Left = 0, Right = 1, Middle = 2 }

    public enum ImGuiCol : int
    {
        Text, TextDisabled, WindowBg, ChildBg, PopupBg,
        Border, BorderShadow, FrameBg, FrameBgHovered, FrameBgActive,
        TitleBg, TitleBgActive, TitleBgCollapsed, MenuBarBg,
        ScrollbarBg, ScrollbarGrab, ScrollbarGrabHovered, ScrollbarGrabActive,
        CheckMark, SliderGrab, SliderGrabActive,
        Button, ButtonHovered, ButtonActive,
        Header, HeaderHovered, HeaderActive,
        Separator, SeparatorHovered, SeparatorActive,
        ResizeGrip, ResizeGripHovered, ResizeGripActive,
        TabHovered, Tab, TabSelected, TabSelectedOverline, TabDimmed, TabDimmedSelected, TabDimmedSelectedOverline,
        PlotLines, PlotLinesHovered, PlotHistogram, PlotHistogramHovered,
        TableHeaderBg, TableBorderStrong, TableBorderLight, TableRowBg, TableRowBgAlt,
        TextLink, TextSelectedBg, DragDropTarget,
        NavCursor, NavWindowingHighlight, NavWindowingDimBg, ModalWindowDimBg,
        COUNT,
    }

    public enum ImGuiStyleVar : int
    {
        Alpha, DisabledAlpha, WindowPadding, WindowRounding, WindowBorderSize,
        WindowMinSize, WindowTitleAlign, ChildRounding, ChildBorderSize,
        PopupRounding, PopupBorderSize, FramePadding, FrameRounding, FrameBorderSize,
        ItemSpacing, ItemInnerSpacing, IndentSpacing, CellPadding,
        ScrollbarSize, ScrollbarRounding, GrabMinSize, GrabRounding,
        TabRounding, TabBorderSize, TabBarBorderSize, TabBarOverlineSize,
        ButtonTextAlign, SelectableTextAlign, SeparatorTextBorderSize,
        SeparatorTextAlign, SeparatorTextPadding,
        COUNT,
    }

    // -----------------------------------------------------------------------
    // Native P/Invoke
    // -----------------------------------------------------------------------
    [DllImport(Lib)] static extern bool   igBegin(string name, IntPtr p_open, int flags);
    [DllImport(Lib, EntryPoint = "igBegin")] static extern bool igBegin_Open(string name, ref bool p_open, int flags);
    [DllImport(Lib)] static extern void   igEnd();
    [DllImport(Lib)] static extern bool   igBeginChild(string id, float w, float h, int border_flags, int window_flags);
    [DllImport(Lib)] static extern void   igEndChild();
    [DllImport(Lib)] static extern bool   igIsWindowAppearing();
    [DllImport(Lib)] static extern bool   igIsWindowCollapsed();
    [DllImport(Lib)] static extern bool   igIsWindowFocused(int flags);
    [DllImport(Lib)] static extern bool   igIsWindowHovered(int flags);
    [DllImport(Lib)] static extern void   igGetWindowPos(out ImVec2 r);
    [DllImport(Lib)] static extern void   igGetWindowSize(out ImVec2 r);
    [DllImport(Lib)] static extern float  igGetWindowWidth();
    [DllImport(Lib)] static extern float  igGetWindowHeight();

    [DllImport(Lib)] static extern void   igSetNextWindowPos(float x, float y, int cond, float px, float py);
    [DllImport(Lib)] static extern void   igSetNextWindowSize(float w, float h, int cond);
    [DllImport(Lib)] static extern void   igSetNextWindowSizeConstraints(float min_w, float min_h, float max_w, float max_h);
    [DllImport(Lib)] static extern void   igSetNextWindowContentSize(float w, float h);
    [DllImport(Lib)] static extern void   igSetNextWindowCollapsed(bool collapsed, int cond);
    [DllImport(Lib)] static extern void   igSetNextWindowFocus();
    [DllImport(Lib)] static extern void   igSetNextWindowBgAlpha(float alpha);
    [DllImport(Lib)] static extern void   igSetWindowPos(float x, float y, int cond);
    [DllImport(Lib)] static extern void   igSetWindowSize(float w, float h, int cond);
    [DllImport(Lib)] static extern void   igSetWindowCollapsed(bool collapsed, int cond);
    [DllImport(Lib)] static extern void   igSetWindowFocus();

    [DllImport(Lib)] static extern void   igGetContentRegionAvail(out ImVec2 r);
    [DllImport(Lib)] static extern void   igGetCursorPos(out ImVec2 r);
    [DllImport(Lib)] static extern float  igGetCursorPosX();
    [DllImport(Lib)] static extern float  igGetCursorPosY();
    [DllImport(Lib)] static extern void   igSetCursorPos(float x, float y);
    [DllImport(Lib)] static extern void   igSetCursorPosX(float x);
    [DllImport(Lib)] static extern void   igSetCursorPosY(float y);
    [DllImport(Lib)] static extern void   igGetCursorScreenPos(out ImVec2 r);

    [DllImport(Lib)] static extern void   igSeparator();
    [DllImport(Lib)] static extern void   igSeparatorText(string label);
    [DllImport(Lib)] static extern void   igSameLine(float offset, float spacing);
    [DllImport(Lib)] static extern void   igNewLine();
    [DllImport(Lib)] static extern void   igSpacing();
    [DllImport(Lib)] static extern void   igDummy(float w, float h);
    [DllImport(Lib)] static extern void   igIndent(float w);
    [DllImport(Lib)] static extern void   igUnindent(float w);
    [DllImport(Lib)] static extern void   igBeginGroup();
    [DllImport(Lib)] static extern void   igEndGroup();

    [DllImport(Lib)] static extern void   igText(string text);
    [DllImport(Lib)] static extern void   igTextColored(float r, float g, float b, float a, string text);
    [DllImport(Lib)] static extern void   igTextDisabled(string text);
    [DllImport(Lib)] static extern void   igTextWrapped(string text);
    [DllImport(Lib)] static extern void   igLabelText(string label, string text);
    [DllImport(Lib)] static extern void   igBulletText(string text);

    [DllImport(Lib)] static extern bool   igButton(string label, float w, float h);
    [DllImport(Lib)] static extern bool   igSmallButton(string label);
    [DllImport(Lib)] static extern bool   igInvisibleButton(string id, float w, float h, int flags);
    [DllImport(Lib)] static extern bool   igCheckbox(string label, ref bool v);
    [DllImport(Lib)] static extern bool   igRadioButton(string label, bool active);
    [DllImport(Lib)] static extern void   igProgressBar(float fraction, float w, float h, string overlay);
    [DllImport(Lib)] static extern void   igBullet();

    [DllImport(Lib)] static extern bool   igDragFloat(string label, ref float v, float speed, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragFloat2(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, float speed, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragFloat3(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, float speed, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragFloat4(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, float speed, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragInt(string label, ref int v, float speed, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragInt2(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, float speed, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragInt3(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, float speed, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igDragInt4(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, float speed, int min, int max, string fmt, int flags);

    [DllImport(Lib)] static extern bool   igSliderFloat(string label, ref float v, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderFloat2(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderFloat3(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderFloat4(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, float min, float max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderInt(string label, ref int v, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderInt2(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderInt3(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderInt4(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, int min, int max, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igSliderAngle(string label, ref float v_rad, float min_deg, float max_deg, string fmt, int flags);

    [DllImport(Lib)] static extern bool   igInputText(string label, byte[] buf, int buf_size, int flags);
    [DllImport(Lib)] static extern bool   igInputFloat(string label, ref float v, float step, float step_fast, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igInputFloat2(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igInputFloat3(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igInputFloat4(string label, [MarshalAs(UnmanagedType.LPArray)] float[] v, string fmt, int flags);
    [DllImport(Lib)] static extern bool   igInputInt(string label, ref int v, int step, int step_fast, int flags);
    [DllImport(Lib)] static extern bool   igInputInt2(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, int flags);
    [DllImport(Lib)] static extern bool   igInputInt3(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, int flags);
    [DllImport(Lib)] static extern bool   igInputInt4(string label, [MarshalAs(UnmanagedType.LPArray)] int[] v, int flags);

    [DllImport(Lib)] static extern bool   igColorEdit3(string label, [MarshalAs(UnmanagedType.LPArray)] float[] col, int flags);
    [DllImport(Lib)] static extern bool   igColorEdit4(string label, [MarshalAs(UnmanagedType.LPArray)] float[] col, int flags);
    [DllImport(Lib)] static extern bool   igColorPicker3(string label, [MarshalAs(UnmanagedType.LPArray)] float[] col, int flags);
    [DllImport(Lib)] static extern bool   igColorPicker4(string label, [MarshalAs(UnmanagedType.LPArray)] float[] col, int flags);
    [DllImport(Lib)] static extern bool   igColorButton(string id, float r, float g, float b, float a, int flags, float w, float h);

    [DllImport(Lib)] static extern bool   igTreeNode(string label);
    [DllImport(Lib)] static extern bool   igTreeNodeEx(string label, int flags);
    [DllImport(Lib)] static extern void   igTreePush(string str_id);
    [DllImport(Lib)] static extern void   igTreePop();
    [DllImport(Lib)] static extern float  igGetTreeNodeToLabelSpacing();
    [DllImport(Lib)] static extern bool   igCollapsingHeader(string label, int flags);
    [DllImport(Lib)] static extern bool   igCollapsingHeaderOpen(string label, ref bool p_visible, int flags);
    [DllImport(Lib)] static extern void   igSetNextItemOpen(bool is_open, int cond);

    [DllImport(Lib)] static extern bool   igSelectable(string label, bool selected, int flags, float w, float h);
    [DllImport(Lib)] static extern bool   igSelectableToggle(string label, ref bool p_selected, int flags, float w, float h);

    [DllImport(Lib)] static extern bool   igBeginCombo(string label, string preview, int flags);
    [DllImport(Lib)] static extern void   igEndCombo();

    [DllImport(Lib)] static extern void   igSetItemTooltip(string text);
    [DllImport(Lib)] static extern bool   igBeginTooltip();
    [DllImport(Lib)] static extern void   igEndTooltip();

    [DllImport(Lib)] static extern bool   igBeginMenuBar();
    [DllImport(Lib)] static extern void   igEndMenuBar();
    [DllImport(Lib)] static extern bool   igBeginMainMenuBar();
    [DllImport(Lib)] static extern void   igEndMainMenuBar();
    [DllImport(Lib)] static extern bool   igBeginMenu(string label, bool enabled);
    [DllImport(Lib)] static extern void   igEndMenu();
    [DllImport(Lib)] static extern bool   igMenuItem(string label, string shortcut, bool selected, bool enabled);
    [DllImport(Lib)] static extern bool   igMenuItemToggle(string label, string shortcut, ref bool p_selected, bool enabled);

    [DllImport(Lib)] static extern void   igOpenPopup(string str_id, int flags);
    [DllImport(Lib)] static extern bool   igBeginPopup(string str_id, int flags);
    [DllImport(Lib)] static extern bool   igBeginPopupModal(string name, ref bool p_open, int flags);
    [DllImport(Lib, EntryPoint = "igBeginPopupModal")] static extern bool igBeginPopupModalNoClose(string name, IntPtr p_open, int flags);
    [DllImport(Lib)] static extern void   igEndPopup();
    [DllImport(Lib)] static extern void   igCloseCurrentPopup();
    [DllImport(Lib)] static extern bool   igIsPopupOpen(string str_id, int flags);

    [DllImport(Lib)] static extern bool   igBeginTable(string id, int columns, int flags, float ow, float oh, float iw);
    [DllImport(Lib)] static extern void   igEndTable();
    [DllImport(Lib)] static extern void   igTableNextRow(int row_flags, float min_height);
    [DllImport(Lib)] static extern bool   igTableNextColumn();
    [DllImport(Lib)] static extern bool   igTableSetColumnIndex(int column);
    [DllImport(Lib)] static extern void   igTableSetupColumn(string label, int flags, float init_width, uint user_id);
    [DllImport(Lib)] static extern void   igTableSetupScrollFreeze(int cols, int rows);
    [DllImport(Lib)] static extern void   igTableHeadersRow();
    [DllImport(Lib)] static extern void   igTableHeader(string label);
    [DllImport(Lib)] static extern int    igTableGetColumnCount();
    [DllImport(Lib)] static extern int    igTableGetColumnIndex();
    [DllImport(Lib)] static extern int    igTableGetRowIndex();
    [DllImport(Lib)] static extern IntPtr igTableGetColumnName(int column);

    [DllImport(Lib)] static extern void   igPushID_Str(string str_id);
    [DllImport(Lib)] static extern void   igPushID_Int(int int_id);
    [DllImport(Lib)] static extern void   igPopID();

    [DllImport(Lib)] static extern void   igPushStyleColor_U32(int idx, uint col);
    [DllImport(Lib)] static extern void   igPushStyleColor_Vec4(int idx, float r, float g, float b, float a);
    [DllImport(Lib)] static extern void   igPopStyleColor(int count);
    [DllImport(Lib)] static extern void   igPushStyleVar_Float(int idx, float val);
    [DllImport(Lib)] static extern void   igPushStyleVar_Vec2(int idx, float x, float y);
    [DllImport(Lib)] static extern void   igPopStyleVar(int count);
    [DllImport(Lib)] static extern void   igPushItemWidth(float w);
    [DllImport(Lib)] static extern void   igPopItemWidth();
    [DllImport(Lib)] static extern void   igSetNextItemWidth(float w);
    [DllImport(Lib)] static extern float  igCalcItemWidth();
    [DllImport(Lib)] static extern void   igPushTextWrapPos(float x);
    [DllImport(Lib)] static extern void   igPopTextWrapPos();

    [DllImport(Lib)] static extern bool   igIsItemHovered(int flags);
    [DllImport(Lib)] static extern bool   igIsItemActive();
    [DllImport(Lib)] static extern bool   igIsItemFocused();
    [DllImport(Lib)] static extern bool   igIsItemClicked(int mouse_button);
    [DllImport(Lib)] static extern bool   igIsItemVisible();
    [DllImport(Lib)] static extern bool   igIsItemEdited();
    [DllImport(Lib)] static extern bool   igIsItemActivated();
    [DllImport(Lib)] static extern bool   igIsItemDeactivated();
    [DllImport(Lib)] static extern bool   igIsItemDeactivatedAfterEdit();
    [DllImport(Lib)] static extern bool   igIsAnyItemHovered();
    [DllImport(Lib)] static extern bool   igIsAnyItemActive();
    [DllImport(Lib)] static extern void   igGetItemRectMin(out ImVec2 r);
    [DllImport(Lib)] static extern void   igGetItemRectMax(out ImVec2 r);
    [DllImport(Lib)] static extern void   igGetItemRectSize(out ImVec2 r);
    [DllImport(Lib)] static extern void   igSetNextItemAllowOverlap();

    [DllImport(Lib)] static extern void   igCalcTextSize(string text, out float w, out float h, bool hide_after_double_hash, float wrap_width);
    [DllImport(Lib)] static extern float  igGetTextLineHeight();
    [DllImport(Lib)] static extern float  igGetTextLineHeightWithSpacing();
    [DllImport(Lib)] static extern float  igGetFrameHeight();
    [DllImport(Lib)] static extern float  igGetFrameHeightWithSpacing();
    [DllImport(Lib)] static extern int    igGetFrameCount();
    [DllImport(Lib)] static extern double igGetTime();
    [DllImport(Lib)] static extern float  igGetFramerate();
    [DllImport(Lib)] static extern float  igGetDeltaTime();
    [DllImport(Lib)] static extern void   igGetDisplaySize(out float w, out float h);

    [DllImport(Lib)] static extern float  igGetScrollX();
    [DllImport(Lib)] static extern float  igGetScrollY();
    [DllImport(Lib)] static extern void   igSetScrollX(float x);
    [DllImport(Lib)] static extern void   igSetScrollY(float y);
    [DllImport(Lib)] static extern float  igGetScrollMaxX();
    [DllImport(Lib)] static extern float  igGetScrollMaxY();
    [DllImport(Lib)] static extern void   igSetScrollHereX(float center);
    [DllImport(Lib)] static extern void   igSetScrollHereY(float center);

    // -----------------------------------------------------------------------
    // Public API
    // -----------------------------------------------------------------------

    // Windows
    public static bool Begin(string name, ImGuiWindowFlags flags = 0) => igBegin(name, IntPtr.Zero, (int)flags);
    public static bool Begin(string name, ref bool p_open, ImGuiWindowFlags flags = 0) => igBegin_Open(name, ref p_open, (int)flags);
    public static void End() => igEnd();
    public static bool BeginChild(string id, ImVec2 size = default, ImGuiChildFlags border = 0, ImGuiWindowFlags flags = 0) => igBeginChild(id, size.x, size.y, (int)border, (int)flags);
    public static void EndChild() => igEndChild();
    public static bool IsWindowAppearing() => igIsWindowAppearing();
    public static bool IsWindowCollapsed() => igIsWindowCollapsed();
    public static bool IsWindowFocused(ImGuiFocusedFlags flags = 0) => igIsWindowFocused((int)flags);
    public static bool IsWindowHovered(ImGuiHoveredFlags flags = 0) => igIsWindowHovered((int)flags);
    public static ImVec2 GetWindowPos() { igGetWindowPos(out var r); return r; }
    public static ImVec2 GetWindowSize() { igGetWindowSize(out var r); return r; }
    public static float GetWindowWidth() => igGetWindowWidth();
    public static float GetWindowHeight() => igGetWindowHeight();

    // Set next window
    public static void SetNextWindowPos(ImVec2 pos, ImGuiCond cond = 0, ImVec2 pivot = default) => igSetNextWindowPos(pos.x, pos.y, (int)cond, pivot.x, pivot.y);
    public static void SetNextWindowSize(ImVec2 size, ImGuiCond cond = 0) => igSetNextWindowSize(size.x, size.y, (int)cond);
    public static void SetNextWindowSizeConstraints(ImVec2 min, ImVec2 max) => igSetNextWindowSizeConstraints(min.x, min.y, max.x, max.y);
    public static void SetNextWindowContentSize(ImVec2 size) => igSetNextWindowContentSize(size.x, size.y);
    public static void SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0) => igSetNextWindowCollapsed(collapsed, (int)cond);
    public static void SetNextWindowFocus() => igSetNextWindowFocus();
    public static void SetNextWindowBgAlpha(float alpha) => igSetNextWindowBgAlpha(alpha);
    public static void SetWindowPos(ImVec2 pos, ImGuiCond cond = 0) => igSetWindowPos(pos.x, pos.y, (int)cond);
    public static void SetWindowSize(ImVec2 size, ImGuiCond cond = 0) => igSetWindowSize(size.x, size.y, (int)cond);
    public static void SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0) => igSetWindowCollapsed(collapsed, (int)cond);
    public static void SetWindowFocus() => igSetWindowFocus();

    // Content / cursor
    public static ImVec2 GetContentRegionAvail() { igGetContentRegionAvail(out var r); return r; }
    public static ImVec2 GetCursorPos() { igGetCursorPos(out var r); return r; }
    public static float  GetCursorPosX() => igGetCursorPosX();
    public static float  GetCursorPosY() => igGetCursorPosY();
    public static void   SetCursorPos(ImVec2 p) => igSetCursorPos(p.x, p.y);
    public static void   SetCursorPosX(float x) => igSetCursorPosX(x);
    public static void   SetCursorPosY(float y) => igSetCursorPosY(y);
    public static ImVec2 GetCursorScreenPos() { igGetCursorScreenPos(out var r); return r; }

    // Layout
    public static void Separator() => igSeparator();
    public static void SeparatorText(string label) => igSeparatorText(label);
    public static void SameLine(float offset = 0f, float spacing = -1f) => igSameLine(offset, spacing);
    public static void NewLine() => igNewLine();
    public static void Spacing() => igSpacing();
    public static void Dummy(ImVec2 size) => igDummy(size.x, size.y);
    public static void Indent(float w = 0f) => igIndent(w);
    public static void Unindent(float w = 0f) => igUnindent(w);
    public static void BeginGroup() => igBeginGroup();
    public static void EndGroup() => igEndGroup();

    // Text
    public static void Text(string text) => igText(text);
    public static void Text(string fmt, params object[] args) => igText(string.Format(fmt, args));
    public static void TextColored(ImVec4 col, string text) => igTextColored(col.x, col.y, col.z, col.w, text);
    public static void TextColored(ImVec4 col, string fmt, params object[] args) => igTextColored(col.x, col.y, col.z, col.w, string.Format(fmt, args));
    public static void TextDisabled(string text) => igTextDisabled(text);
    public static void TextWrapped(string text) => igTextWrapped(text);
    public static void LabelText(string label, string text) => igLabelText(label, text);
    public static void LabelText(string label, string fmt, params object[] args) => igLabelText(label, string.Format(fmt, args));
    public static void BulletText(string text) => igBulletText(text);

    // Buttons
    public static bool Button(string label, ImVec2 size = default) => igButton(label, size.x, size.y);
    public static bool SmallButton(string label) => igSmallButton(label);
    public static bool InvisibleButton(string id, ImVec2 size, int flags = 0) => igInvisibleButton(id, size.x, size.y, flags);
    public static bool Checkbox(string label, ref bool v) => igCheckbox(label, ref v);
    public static bool RadioButton(string label, bool active) => igRadioButton(label, active);
    public static void ProgressBar(float fraction, ImVec2 size = default, string overlay = null) => igProgressBar(fraction, size.x, size.y, overlay);
    public static void Bullet() => igBullet();

    // Drag
    public static bool DragFloat(string label, ref float v, float speed = 1f, float min = 0f, float max = 0f, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igDragFloat(label, ref v, speed, min, max, fmt, (int)flags);
    public static bool DragFloat2(string label, float[] v, float speed = 1f, float min = 0f, float max = 0f, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igDragFloat2(label, v, speed, min, max, fmt, (int)flags);
    public static bool DragFloat3(string label, float[] v, float speed = 1f, float min = 0f, float max = 0f, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igDragFloat3(label, v, speed, min, max, fmt, (int)flags);
    public static bool DragFloat4(string label, float[] v, float speed = 1f, float min = 0f, float max = 0f, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igDragFloat4(label, v, speed, min, max, fmt, (int)flags);
    public static bool DragInt(string label, ref int v, float speed = 1f, int min = 0, int max = 0, string fmt = "%d", ImGuiSliderFlags flags = 0) => igDragInt(label, ref v, speed, min, max, fmt, (int)flags);
    public static bool DragInt2(string label, int[] v, float speed = 1f, int min = 0, int max = 0, string fmt = "%d", ImGuiSliderFlags flags = 0) => igDragInt2(label, v, speed, min, max, fmt, (int)flags);
    public static bool DragInt3(string label, int[] v, float speed = 1f, int min = 0, int max = 0, string fmt = "%d", ImGuiSliderFlags flags = 0) => igDragInt3(label, v, speed, min, max, fmt, (int)flags);
    public static bool DragInt4(string label, int[] v, float speed = 1f, int min = 0, int max = 0, string fmt = "%d", ImGuiSliderFlags flags = 0) => igDragInt4(label, v, speed, min, max, fmt, (int)flags);

    // Slider
    public static bool SliderFloat(string label, ref float v, float min, float max, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igSliderFloat(label, ref v, min, max, fmt, (int)flags);
    public static bool SliderFloat2(string label, float[] v, float min, float max, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igSliderFloat2(label, v, min, max, fmt, (int)flags);
    public static bool SliderFloat3(string label, float[] v, float min, float max, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igSliderFloat3(label, v, min, max, fmt, (int)flags);
    public static bool SliderFloat4(string label, float[] v, float min, float max, string fmt = "%.3f", ImGuiSliderFlags flags = 0) => igSliderFloat4(label, v, min, max, fmt, (int)flags);
    public static bool SliderInt(string label, ref int v, int min, int max, string fmt = "%d", ImGuiSliderFlags flags = 0) => igSliderInt(label, ref v, min, max, fmt, (int)flags);
    public static bool SliderInt2(string label, int[] v, int min, int max, string fmt = "%d", ImGuiSliderFlags flags = 0) => igSliderInt2(label, v, min, max, fmt, (int)flags);
    public static bool SliderInt3(string label, int[] v, int min, int max, string fmt = "%d", ImGuiSliderFlags flags = 0) => igSliderInt3(label, v, min, max, fmt, (int)flags);
    public static bool SliderInt4(string label, int[] v, int min, int max, string fmt = "%d", ImGuiSliderFlags flags = 0) => igSliderInt4(label, v, min, max, fmt, (int)flags);
    public static bool SliderAngle(string label, ref float v_rad, float min_deg = -360f, float max_deg = 360f, string fmt = "%.0f deg", ImGuiSliderFlags flags = 0) => igSliderAngle(label, ref v_rad, min_deg, max_deg, fmt, (int)flags);

    // Input text
    public static bool InputText(string label, ref string text, uint maxLen = 256, ImGuiInputTextFlags flags = 0)
    {
        var buf = new byte[maxLen];
        if (text != null)
        {
            var src = Encoding.UTF8.GetBytes(text);
            Array.Copy(src, buf, Math.Min(src.Length, buf.Length - 1));
        }
        bool changed = igInputText(label, buf, (int)maxLen, (int)flags);
        if (changed) text = Encoding.UTF8.GetString(buf).TrimEnd('\0');
        return changed;
    }
    public static bool InputFloat(string label, ref float v, float step = 0f, float step_fast = 0f, string fmt = "%.3f", ImGuiInputTextFlags flags = 0) => igInputFloat(label, ref v, step, step_fast, fmt, (int)flags);
    public static bool InputFloat2(string label, float[] v, string fmt = "%.3f", ImGuiInputTextFlags flags = 0) => igInputFloat2(label, v, fmt, (int)flags);
    public static bool InputFloat3(string label, float[] v, string fmt = "%.3f", ImGuiInputTextFlags flags = 0) => igInputFloat3(label, v, fmt, (int)flags);
    public static bool InputFloat4(string label, float[] v, string fmt = "%.3f", ImGuiInputTextFlags flags = 0) => igInputFloat4(label, v, fmt, (int)flags);
    public static bool InputInt(string label, ref int v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0) => igInputInt(label, ref v, step, step_fast, (int)flags);
    public static bool InputInt2(string label, int[] v, ImGuiInputTextFlags flags = 0) => igInputInt2(label, v, (int)flags);
    public static bool InputInt3(string label, int[] v, ImGuiInputTextFlags flags = 0) => igInputInt3(label, v, (int)flags);
    public static bool InputInt4(string label, int[] v, ImGuiInputTextFlags flags = 0) => igInputInt4(label, v, (int)flags);

    // Color
    public static bool ColorEdit3(string label, float[] col, ImGuiColorEditFlags flags = 0) => igColorEdit3(label, col, (int)flags);
    public static bool ColorEdit4(string label, float[] col, ImGuiColorEditFlags flags = 0) => igColorEdit4(label, col, (int)flags);
    public static bool ColorPicker3(string label, float[] col, ImGuiColorEditFlags flags = 0) => igColorPicker3(label, col, (int)flags);
    public static bool ColorPicker4(string label, float[] col, ImGuiColorEditFlags flags = 0) => igColorPicker4(label, col, (int)flags);
    public static bool ColorButton(string id, ImVec4 col, ImGuiColorEditFlags flags = 0, ImVec2 size = default) => igColorButton(id, col.x, col.y, col.z, col.w, (int)flags, size.x, size.y);

    // Trees
    public static bool TreeNode(string label) => igTreeNode(label);
    public static bool TreeNodeEx(string label, ImGuiTreeNodeFlags flags = 0) => igTreeNodeEx(label, (int)flags);
    public static void TreePush(string str_id) => igTreePush(str_id);
    public static void TreePop() => igTreePop();
    public static float GetTreeNodeToLabelSpacing() => igGetTreeNodeToLabelSpacing();
    public static bool CollapsingHeader(string label, ImGuiTreeNodeFlags flags = 0) => igCollapsingHeader(label, (int)flags);
    public static bool CollapsingHeader(string label, ref bool p_visible, ImGuiTreeNodeFlags flags = 0) => igCollapsingHeaderOpen(label, ref p_visible, (int)flags);
    public static void SetNextItemOpen(bool is_open, ImGuiCond cond = 0) => igSetNextItemOpen(is_open, (int)cond);

    // Selectables
    public static bool Selectable(string label, bool selected = false, ImGuiSelectableFlags flags = 0, ImVec2 size = default) => igSelectable(label, selected, (int)flags, size.x, size.y);
    public static bool Selectable(string label, ref bool p_selected, ImGuiSelectableFlags flags = 0, ImVec2 size = default) => igSelectableToggle(label, ref p_selected, (int)flags, size.x, size.y);

    // Combo
    public static bool BeginCombo(string label, string preview, ImGuiComboFlags flags = 0) => igBeginCombo(label, preview, (int)flags);
    public static void EndCombo() => igEndCombo();

    // Tooltips
    public static void SetItemTooltip(string text) => igSetItemTooltip(text);
    public static bool BeginTooltip() => igBeginTooltip();
    public static void EndTooltip() => igEndTooltip();

    // Menus
    public static bool BeginMenuBar() => igBeginMenuBar();
    public static void EndMenuBar() => igEndMenuBar();
    public static bool BeginMainMenuBar() => igBeginMainMenuBar();
    public static void EndMainMenuBar() => igEndMainMenuBar();
    public static bool BeginMenu(string label, bool enabled = true) => igBeginMenu(label, enabled);
    public static void EndMenu() => igEndMenu();
    public static bool MenuItem(string label, string shortcut = null, bool selected = false, bool enabled = true) => igMenuItem(label, shortcut, selected, enabled);
    public static bool MenuItem(string label, ref bool p_selected, bool enabled = true) => igMenuItemToggle(label, null, ref p_selected, enabled);

    // Popups
    public static void OpenPopup(string str_id, ImGuiPopupFlags flags = 0) => igOpenPopup(str_id, (int)flags);
    public static bool BeginPopup(string str_id, ImGuiWindowFlags flags = 0) => igBeginPopup(str_id, (int)flags);
    public static bool BeginPopupModal(string name, ref bool p_open, ImGuiWindowFlags flags = 0) => igBeginPopupModal(name, ref p_open, (int)flags);
    public static bool BeginPopupModal(string name, ImGuiWindowFlags flags = 0) => igBeginPopupModalNoClose(name, IntPtr.Zero, (int)flags);
    public static void EndPopup() => igEndPopup();
    public static void CloseCurrentPopup() => igCloseCurrentPopup();
    public static bool IsPopupOpen(string str_id, ImGuiPopupFlags flags = 0) => igIsPopupOpen(str_id, (int)flags);

    // Tables
    public static bool BeginTable(string id, int columns, ImGuiTableFlags flags = 0, ImVec2 outerSize = default, float innerWidth = 0f) => igBeginTable(id, columns, (int)flags, outerSize.x, outerSize.y, innerWidth);
    public static void EndTable() => igEndTable();
    public static void TableNextRow(int row_flags = 0, float min_height = 0f) => igTableNextRow(row_flags, min_height);
    public static bool TableNextColumn() => igTableNextColumn();
    public static bool TableSetColumnIndex(int column) => igTableSetColumnIndex(column);
    public static void TableSetupColumn(string label, ImGuiTableColumnFlags flags = 0, float init_width = 0f, uint user_id = 0) => igTableSetupColumn(label, (int)flags, init_width, user_id);
    public static void TableSetupScrollFreeze(int cols, int rows) => igTableSetupScrollFreeze(cols, rows);
    public static void TableHeadersRow() => igTableHeadersRow();
    public static void TableHeader(string label) => igTableHeader(label);
    public static int TableGetColumnCount() => igTableGetColumnCount();
    public static int TableGetColumnIndex() => igTableGetColumnIndex();
    public static int TableGetRowIndex() => igTableGetRowIndex();
    public static string TableGetColumnName(int column = -1) => Marshal.PtrToStringAnsi(igTableGetColumnName(column));

    // ID stack
    public static void PushID(string str_id) => igPushID_Str(str_id);
    public static void PushID(int int_id) => igPushID_Int(int_id);
    public static void PopID() => igPopID();

    // Style
    public static void PushStyleColor(ImGuiCol idx, uint col) => igPushStyleColor_U32((int)idx, col);
    public static void PushStyleColor(ImGuiCol idx, ImVec4 col) => igPushStyleColor_Vec4((int)idx, col.x, col.y, col.z, col.w);
    public static void PopStyleColor(int count = 1) => igPopStyleColor(count);
    public static void PushStyleVar(ImGuiStyleVar idx, float val) => igPushStyleVar_Float((int)idx, val);
    public static void PushStyleVar(ImGuiStyleVar idx, ImVec2 val) => igPushStyleVar_Vec2((int)idx, val.x, val.y);
    public static void PopStyleVar(int count = 1) => igPopStyleVar(count);
    public static void PushItemWidth(float w) => igPushItemWidth(w);
    public static void PopItemWidth() => igPopItemWidth();
    public static void SetNextItemWidth(float w) => igSetNextItemWidth(w);
    public static float CalcItemWidth() => igCalcItemWidth();
    public static void PushTextWrapPos(float x = 0f) => igPushTextWrapPos(x);
    public static void PopTextWrapPos() => igPopTextWrapPos();

    // Item utils
    public static bool IsItemHovered(ImGuiHoveredFlags flags = 0) => igIsItemHovered((int)flags);
    public static bool IsItemActive() => igIsItemActive();
    public static bool IsItemFocused() => igIsItemFocused();
    public static bool IsItemClicked(ImGuiMouseButton btn = ImGuiMouseButton.Left) => igIsItemClicked((int)btn);
    public static bool IsItemVisible() => igIsItemVisible();
    public static bool IsItemEdited() => igIsItemEdited();
    public static bool IsItemActivated() => igIsItemActivated();
    public static bool IsItemDeactivated() => igIsItemDeactivated();
    public static bool IsItemDeactivatedAfterEdit() => igIsItemDeactivatedAfterEdit();
    public static bool IsAnyItemHovered() => igIsAnyItemHovered();
    public static bool IsAnyItemActive() => igIsAnyItemActive();
    public static ImVec2 GetItemRectMin() { igGetItemRectMin(out var r); return r; }
    public static ImVec2 GetItemRectMax() { igGetItemRectMax(out var r); return r; }
    public static ImVec2 GetItemRectSize() { igGetItemRectSize(out var r); return r; }
    public static void SetNextItemAllowOverlap() => igSetNextItemAllowOverlap();

    // Misc
    public static ImVec2 CalcTextSize(string text, bool hide_after_double_hash = false, float wrap_width = -1f) { igCalcTextSize(text, out var w, out var h, hide_after_double_hash, wrap_width); return new ImVec2(w, h); }
    public static float  GetTextLineHeight() => igGetTextLineHeight();
    public static float  GetTextLineHeightWithSpacing() => igGetTextLineHeightWithSpacing();
    public static float  GetFrameHeight() => igGetFrameHeight();
    public static float  GetFrameHeightWithSpacing() => igGetFrameHeightWithSpacing();
    public static int    GetFrameCount() => igGetFrameCount();
    public static double GetTime() => igGetTime();
    public static float  GetFramerate() => igGetFramerate();
    public static float  GetDeltaTime() => igGetDeltaTime();
    public static ImVec2 GetDisplaySize() { igGetDisplaySize(out var w, out var h); return new ImVec2(w, h); }

    // Scrolling
    public static float GetScrollX() => igGetScrollX();
    public static float GetScrollY() => igGetScrollY();
    public static void  SetScrollX(float x) => igSetScrollX(x);
    public static void  SetScrollY(float y) => igSetScrollY(y);
    public static float GetScrollMaxX() => igGetScrollMaxX();
    public static float GetScrollMaxY() => igGetScrollMaxY();
    public static void  SetScrollHereX(float center = 0.5f) => igSetScrollHereX(center);
    public static void  SetScrollHereY(float center = 0.5f) => igSetScrollHereY(center);
}
