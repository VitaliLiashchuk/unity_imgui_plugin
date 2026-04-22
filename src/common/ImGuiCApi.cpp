#include <imgui.h>
#include <cstring>

extern "C" {

bool igBegin(const char* name, bool* p_open, int flags)     { return ImGui::Begin(name, p_open, flags); }
void igEnd()                                                  { ImGui::End(); }
bool igBeginChild(const char* id, float w, float h, int border_flags, int window_flags) { return ImGui::BeginChild(id, {w, h}, border_flags, window_flags); }
void igEndChild()                                             { ImGui::EndChild(); }
bool igIsWindowAppearing()                                    { return ImGui::IsWindowAppearing(); }
bool igIsWindowCollapsed()                                    { return ImGui::IsWindowCollapsed(); }
bool igIsWindowFocused(int flags)                             { return ImGui::IsWindowFocused(flags); }
bool igIsWindowHovered(int flags)                             { return ImGui::IsWindowHovered(flags); }
void igGetWindowPos(ImVec2* out)                              { *out = ImGui::GetWindowPos(); }
void igGetWindowSize(ImVec2* out)                             { *out = ImGui::GetWindowSize(); }
float igGetWindowWidth()                                      { return ImGui::GetWindowWidth(); }
float igGetWindowHeight()                                     { return ImGui::GetWindowHeight(); }

void igSetNextWindowPos(float x, float y, int cond, float pivot_x, float pivot_y) { ImGui::SetNextWindowPos({x, y}, cond, {pivot_x, pivot_y}); }
void igSetNextWindowSize(float w, float h, int cond)          { ImGui::SetNextWindowSize({w, h}, cond); }
void igSetNextWindowSizeConstraints(float min_w, float min_h, float max_w, float max_h) { ImGui::SetNextWindowSizeConstraints({min_w, min_h}, {max_w, max_h}); }
void igSetNextWindowContentSize(float w, float h)             { ImGui::SetNextWindowContentSize({w, h}); }
void igSetNextWindowCollapsed(bool collapsed, int cond)       { ImGui::SetNextWindowCollapsed(collapsed, cond); }
void igSetNextWindowFocus()                                   { ImGui::SetNextWindowFocus(); }
void igSetNextWindowBgAlpha(float alpha)                      { ImGui::SetNextWindowBgAlpha(alpha); }
void igSetWindowPos(float x, float y, int cond)               { ImGui::SetWindowPos({x, y}, cond); }
void igSetWindowSize(float w, float h, int cond)              { ImGui::SetWindowSize({w, h}, cond); }
void igSetWindowCollapsed(bool collapsed, int cond)           { ImGui::SetWindowCollapsed(collapsed, cond); }
void igSetWindowFocus()                                       { ImGui::SetWindowFocus(); }

void igGetContentRegionAvail(ImVec2* out)                     { *out = ImGui::GetContentRegionAvail(); }
void igGetCursorPos(ImVec2* out)                              { *out = ImGui::GetCursorPos(); }
float igGetCursorPosX()                                       { return ImGui::GetCursorPosX(); }
float igGetCursorPosY()                                       { return ImGui::GetCursorPosY(); }
void igSetCursorPos(float x, float y)                         { ImGui::SetCursorPos({x, y}); }
void igSetCursorPosX(float x)                                 { ImGui::SetCursorPosX(x); }
void igSetCursorPosY(float y)                                 { ImGui::SetCursorPosY(y); }
void igGetCursorScreenPos(ImVec2* out)                        { *out = ImGui::GetCursorScreenPos(); }

void igSeparator()                                            { ImGui::Separator(); }
void igSeparatorText(const char* label)                       { ImGui::SeparatorText(label); }
void igSameLine(float offset, float spacing)                  { ImGui::SameLine(offset, spacing); }
void igNewLine()                                              { ImGui::NewLine(); }
void igSpacing()                                              { ImGui::Spacing(); }
void igDummy(float w, float h)                                { ImGui::Dummy({w, h}); }
void igIndent(float w)                                        { ImGui::Indent(w); }
void igUnindent(float w)                                      { ImGui::Unindent(w); }
void igBeginGroup()                                           { ImGui::BeginGroup(); }
void igEndGroup()                                             { ImGui::EndGroup(); }

void igText(const char* text)                                 { ImGui::TextUnformatted(text); }
void igTextColored(float r, float g, float b, float a, const char* text) { ImGui::TextColored({r, g, b, a}, "%s", text); }
void igTextDisabled(const char* text)                         { ImGui::TextDisabled("%s", text); }
void igTextWrapped(const char* text)                          { ImGui::TextWrapped("%s", text); }
void igLabelText(const char* label, const char* text)         { ImGui::LabelText(label, "%s", text); }
void igBulletText(const char* text)                           { ImGui::BulletText("%s", text); }

bool igButton(const char* label, float w, float h)            { return ImGui::Button(label, {w, h}); }
bool igSmallButton(const char* label)                         { return ImGui::SmallButton(label); }
bool igInvisibleButton(const char* id, float w, float h, int flags) { return ImGui::InvisibleButton(id, {w, h}, flags); }
bool igCheckbox(const char* label, bool* v)                   { return ImGui::Checkbox(label, v); }
bool igRadioButton(const char* label, bool active)            { return ImGui::RadioButton(label, active); }
void igProgressBar(float fraction, float w, float h, const char* overlay) { ImGui::ProgressBar(fraction, {w, h}, overlay); }
void igBullet()                                               { ImGui::Bullet(); }

bool igDragFloat(const char* label, float* v, float speed, float min, float max, const char* fmt, int flags)         { return ImGui::DragFloat(label, v, speed, min, max, fmt, flags); }
bool igDragFloat2(const char* label, float* v, float speed, float min, float max, const char* fmt, int flags)        { return ImGui::DragFloat2(label, v, speed, min, max, fmt, flags); }
bool igDragFloat3(const char* label, float* v, float speed, float min, float max, const char* fmt, int flags)        { return ImGui::DragFloat3(label, v, speed, min, max, fmt, flags); }
bool igDragFloat4(const char* label, float* v, float speed, float min, float max, const char* fmt, int flags)        { return ImGui::DragFloat4(label, v, speed, min, max, fmt, flags); }
bool igDragInt(const char* label, int* v, float speed, int min, int max, const char* fmt, int flags)                 { return ImGui::DragInt(label, v, speed, min, max, fmt, flags); }
bool igDragInt2(const char* label, int* v, float speed, int min, int max, const char* fmt, int flags)                { return ImGui::DragInt2(label, v, speed, min, max, fmt, flags); }
bool igDragInt3(const char* label, int* v, float speed, int min, int max, const char* fmt, int flags)                { return ImGui::DragInt3(label, v, speed, min, max, fmt, flags); }
bool igDragInt4(const char* label, int* v, float speed, int min, int max, const char* fmt, int flags)                { return ImGui::DragInt4(label, v, speed, min, max, fmt, flags); }

bool igSliderFloat(const char* label, float* v, float min, float max, const char* fmt, int flags)                    { return ImGui::SliderFloat(label, v, min, max, fmt, flags); }
bool igSliderFloat2(const char* label, float* v, float min, float max, const char* fmt, int flags)                   { return ImGui::SliderFloat2(label, v, min, max, fmt, flags); }
bool igSliderFloat3(const char* label, float* v, float min, float max, const char* fmt, int flags)                   { return ImGui::SliderFloat3(label, v, min, max, fmt, flags); }
bool igSliderFloat4(const char* label, float* v, float min, float max, const char* fmt, int flags)                   { return ImGui::SliderFloat4(label, v, min, max, fmt, flags); }
bool igSliderInt(const char* label, int* v, int min, int max, const char* fmt, int flags)                            { return ImGui::SliderInt(label, v, min, max, fmt, flags); }
bool igSliderInt2(const char* label, int* v, int min, int max, const char* fmt, int flags)                           { return ImGui::SliderInt2(label, v, min, max, fmt, flags); }
bool igSliderInt3(const char* label, int* v, int min, int max, const char* fmt, int flags)                           { return ImGui::SliderInt3(label, v, min, max, fmt, flags); }
bool igSliderInt4(const char* label, int* v, int min, int max, const char* fmt, int flags)                           { return ImGui::SliderInt4(label, v, min, max, fmt, flags); }
bool igSliderAngle(const char* label, float* v_rad, float v_deg_min, float v_deg_max, const char* fmt, int flags)    { return ImGui::SliderAngle(label, v_rad, v_deg_min, v_deg_max, fmt, flags); }

bool igInputText(const char* label, char* buf, int buf_size, int flags)     { return ImGui::InputText(label, buf, (size_t)buf_size, flags); }
bool igInputFloat(const char* label, float* v, float step, float step_fast, const char* fmt, int flags)              { return ImGui::InputFloat(label, v, step, step_fast, fmt, flags); }
bool igInputFloat2(const char* label, float* v, const char* fmt, int flags)                                          { return ImGui::InputFloat2(label, v, fmt, flags); }
bool igInputFloat3(const char* label, float* v, const char* fmt, int flags)                                          { return ImGui::InputFloat3(label, v, fmt, flags); }
bool igInputFloat4(const char* label, float* v, const char* fmt, int flags)                                          { return ImGui::InputFloat4(label, v, fmt, flags); }
bool igInputInt(const char* label, int* v, int step, int step_fast, int flags)                                        { return ImGui::InputInt(label, v, step, step_fast, flags); }
bool igInputInt2(const char* label, int* v, int flags)                                                               { return ImGui::InputInt2(label, v, flags); }
bool igInputInt3(const char* label, int* v, int flags)                                                               { return ImGui::InputInt3(label, v, flags); }
bool igInputInt4(const char* label, int* v, int flags)                                                               { return ImGui::InputInt4(label, v, flags); }

bool igColorEdit3(const char* label, float* col, int flags)                 { return ImGui::ColorEdit3(label, col, flags); }
bool igColorEdit4(const char* label, float* col, int flags)                 { return ImGui::ColorEdit4(label, col, flags); }
bool igColorPicker3(const char* label, float* col, int flags)               { return ImGui::ColorPicker3(label, col, flags); }
bool igColorPicker4(const char* label, float* col, int flags)               { return ImGui::ColorPicker4(label, col, flags); }
bool igColorButton(const char* id, float r, float g, float b, float a, int flags, float w, float h) { return ImGui::ColorButton(id, {r, g, b, a}, flags, {w, h}); }

bool igTreeNode(const char* label)                                          { return ImGui::TreeNode(label); }
bool igTreeNodeEx(const char* label, int flags)                             { return ImGui::TreeNodeEx(label, flags); }
void igTreePush(const char* str_id)                                         { ImGui::TreePush(str_id); }
void igTreePop()                                                            { ImGui::TreePop(); }
float igGetTreeNodeToLabelSpacing()                                         { return ImGui::GetTreeNodeToLabelSpacing(); }
bool igCollapsingHeader(const char* label, int flags)                       { return ImGui::CollapsingHeader(label, flags); }
bool igCollapsingHeaderOpen(const char* label, bool* p_visible, int flags)  { return ImGui::CollapsingHeader(label, p_visible, flags); }
void igSetNextItemOpen(bool is_open, int cond)                              { ImGui::SetNextItemOpen(is_open, cond); }

bool igSelectable(const char* label, bool selected, int flags, float w, float h) { return ImGui::Selectable(label, selected, flags, {w, h}); }
bool igSelectableToggle(const char* label, bool* p_selected, int flags, float w, float h) { return ImGui::Selectable(label, p_selected, flags, {w, h}); }

bool igBeginCombo(const char* label, const char* preview, int flags)        { return ImGui::BeginCombo(label, preview, flags); }
void igEndCombo()                                                           { ImGui::EndCombo(); }

void igSetItemTooltip(const char* text)                                     { ImGui::SetItemTooltip("%s", text); }
bool igBeginTooltip()                                                       { return ImGui::BeginTooltip(); }
void igEndTooltip()                                                         { ImGui::EndTooltip(); }

bool igBeginMenuBar()                                                       { return ImGui::BeginMenuBar(); }
void igEndMenuBar()                                                         { ImGui::EndMenuBar(); }
bool igBeginMainMenuBar()                                                   { return ImGui::BeginMainMenuBar(); }
void igEndMainMenuBar()                                                     { ImGui::EndMainMenuBar(); }
bool igBeginMenu(const char* label, bool enabled)                           { return ImGui::BeginMenu(label, enabled); }
void igEndMenu()                                                            { ImGui::EndMenu(); }
bool igMenuItem(const char* label, const char* shortcut, bool selected, bool enabled) { return ImGui::MenuItem(label, shortcut, selected, enabled); }
bool igMenuItemToggle(const char* label, const char* shortcut, bool* p_selected, bool enabled) { return ImGui::MenuItem(label, shortcut, p_selected, enabled); }

void igOpenPopup(const char* str_id, int flags)                             { ImGui::OpenPopup(str_id, flags); }
bool igBeginPopup(const char* str_id, int flags)                            { return ImGui::BeginPopup(str_id, flags); }
bool igBeginPopupModal(const char* name, bool* p_open, int flags)           { return ImGui::BeginPopupModal(name, p_open, flags); }
void igEndPopup()                                                           { ImGui::EndPopup(); }
void igCloseCurrentPopup()                                                  { ImGui::CloseCurrentPopup(); }
bool igIsPopupOpen(const char* str_id, int flags)                           { return ImGui::IsPopupOpen(str_id, flags); }

bool igBeginTable(const char* str_id, int columns, int flags, float outer_w, float outer_h, float inner_w) { return ImGui::BeginTable(str_id, columns, flags, {outer_w, outer_h}, inner_w); }
void igEndTable()                                                           { ImGui::EndTable(); }
void igTableNextRow(int row_flags, float min_height)                        { ImGui::TableNextRow(row_flags, min_height); }
bool igTableNextColumn()                                                    { return ImGui::TableNextColumn(); }
bool igTableSetColumnIndex(int column)                                      { return ImGui::TableSetColumnIndex(column); }
void igTableSetupColumn(const char* label, int flags, float init_width, unsigned int user_id) { ImGui::TableSetupColumn(label, flags, init_width, user_id); }
void igTableSetupScrollFreeze(int cols, int rows)                           { ImGui::TableSetupScrollFreeze(cols, rows); }
void igTableHeadersRow()                                                    { ImGui::TableHeadersRow(); }
void igTableHeader(const char* label)                                       { ImGui::TableHeader(label); }
int igTableGetColumnCount()                                                 { return ImGui::TableGetColumnCount(); }
int igTableGetColumnIndex()                                                 { return ImGui::TableGetColumnIndex(); }
int igTableGetRowIndex()                                                    { return ImGui::TableGetRowIndex(); }
const char* igTableGetColumnName(int column)                                { return ImGui::TableGetColumnName(column); }

void igPushID_Str(const char* str_id)                                       { ImGui::PushID(str_id); }
void igPushID_Int(int int_id)                                               { ImGui::PushID(int_id); }
void igPopID()                                                              { ImGui::PopID(); }

void igPushStyleColor_U32(int idx, unsigned int col)                        { ImGui::PushStyleColor(idx, col); }
void igPushStyleColor_Vec4(int idx, float r, float g, float b, float a)    { ImGui::PushStyleColor(idx, {r, g, b, a}); }
void igPopStyleColor(int count)                                             { ImGui::PopStyleColor(count); }
void igPushStyleVar_Float(int idx, float val)                               { ImGui::PushStyleVar(idx, val); }
void igPushStyleVar_Vec2(int idx, float x, float y)                        { ImGui::PushStyleVar(idx, {x, y}); }
void igPopStyleVar(int count)                                               { ImGui::PopStyleVar(count); }
void igPushItemWidth(float item_width)                                      { ImGui::PushItemWidth(item_width); }
void igPopItemWidth()                                                       { ImGui::PopItemWidth(); }
void igSetNextItemWidth(float item_width)                                   { ImGui::SetNextItemWidth(item_width); }
float igCalcItemWidth()                                                     { return ImGui::CalcItemWidth(); }
void igPushTextWrapPos(float wrap_local_pos_x)                              { ImGui::PushTextWrapPos(wrap_local_pos_x); }
void igPopTextWrapPos()                                                     { ImGui::PopTextWrapPos(); }

bool igIsItemHovered(int flags)                                             { return ImGui::IsItemHovered(flags); }
bool igIsItemActive()                                                       { return ImGui::IsItemActive(); }
bool igIsItemFocused()                                                      { return ImGui::IsItemFocused(); }
bool igIsItemClicked(int mouse_button)                                      { return ImGui::IsItemClicked(mouse_button); }
bool igIsItemVisible()                                                      { return ImGui::IsItemVisible(); }
bool igIsItemEdited()                                                       { return ImGui::IsItemEdited(); }
bool igIsItemActivated()                                                    { return ImGui::IsItemActivated(); }
bool igIsItemDeactivated()                                                  { return ImGui::IsItemDeactivated(); }
bool igIsItemDeactivatedAfterEdit()                                         { return ImGui::IsItemDeactivatedAfterEdit(); }
bool igIsAnyItemHovered()                                                   { return ImGui::IsAnyItemHovered(); }
bool igIsAnyItemActive()                                                    { return ImGui::IsAnyItemActive(); }
void igGetItemRectMin(ImVec2* out)                                          { *out = ImGui::GetItemRectMin(); }
void igGetItemRectMax(ImVec2* out)                                          { *out = ImGui::GetItemRectMax(); }
void igGetItemRectSize(ImVec2* out)                                         { *out = ImGui::GetItemRectSize(); }
void igSetNextItemAllowOverlap()                                            { ImGui::SetNextItemAllowOverlap(); }

void igCalcTextSize(const char* text, float* out_w, float* out_h, bool hide_after_double_hash, float wrap_width)
{
    ImVec2 r = ImGui::CalcTextSize(text, nullptr, hide_after_double_hash, wrap_width);
    *out_w = r.x;
    *out_h = r.y;
}
float igGetTextLineHeight()                                                 { return ImGui::GetTextLineHeight(); }
float igGetTextLineHeightWithSpacing()                                      { return ImGui::GetTextLineHeightWithSpacing(); }
float igGetFrameHeight()                                                    { return ImGui::GetFrameHeight(); }
float igGetFrameHeightWithSpacing()                                         { return ImGui::GetFrameHeightWithSpacing(); }
int   igGetFrameCount()                                                     { return ImGui::GetFrameCount(); }
double igGetTime()                                                          { return ImGui::GetTime(); }
float igGetFramerate()                                                      { return ImGui::GetIO().Framerate; }
float igGetDeltaTime()                                                      { return ImGui::GetIO().DeltaTime; }
void  igGetDisplaySize(float* w, float* h)                                  { *w = ImGui::GetIO().DisplaySize.x; *h = ImGui::GetIO().DisplaySize.y; }

float igGetScrollX()                                                        { return ImGui::GetScrollX(); }
float igGetScrollY()                                                        { return ImGui::GetScrollY(); }
void  igSetScrollX(float x)                                                 { ImGui::SetScrollX(x); }
void  igSetScrollY(float y)                                                 { ImGui::SetScrollY(y); }
float igGetScrollMaxX()                                                     { return ImGui::GetScrollMaxX(); }
float igGetScrollMaxY()                                                     { return ImGui::GetScrollMaxY(); }
void  igSetScrollHereX(float center)                                        { ImGui::SetScrollHereX(center); }
void  igSetScrollHereY(float center)                                        { ImGui::SetScrollHereY(center); }

} // extern "C"
