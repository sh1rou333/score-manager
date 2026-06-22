#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 100
#define MAX_STR_LEN    256
#define PAGE_SIZE      5
#define MAX_NODES      50

// ============ 原有的结构定义 ============
typedef struct {
    char data[MAX_STACK_SIZE][MAX_STR_LEN];
    int top;
} StrStack;

typedef struct {
    int year;
    int month;
    int day;
    char contentOnly[MAX_STR_LEN];
} DateItem;

// ============ 图结构定义（用于菜单导航） ============
typedef struct MenuNode {
    int id;                     // 节点ID
    char name[50];              // 菜单名称
    char description[100];      // 功能描述
    int parentId;               // 父节点ID（-1表示根节点）
    int childCount;             // 子节点数量
    int children[MAX_NODES];    // 子节点ID列表
    void (*action)(StrStack*);  // 功能函数指针
} MenuNode;

typedef struct {
    MenuNode nodes[MAX_NODES];
    int nodeCount;
    int currentNode;            // 当前所在节点ID
} MenuGraph;

int MyStrLen(const char *s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

void MyStrCpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void MyStrCat(char *dest, const char *src) {
    int i = MyStrLen(dest);
    int j = 0;
    while (src[j] != '\0') {
        dest[i++] = src[j++];
    }
    dest[i] = '\0';
}

int MyStrCmp(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] < s2[i]) return -1;
        if (s1[i] > s2[i]) return 1;
        i++;
    }
    if (s1[i] == '\0' && s2[i] == '\0') return 0;
    return s1[i] == '\0' ? -1 : 1;
}

void TrimNewLine(char *s) {
    int len = MyStrLen(s);
    if (len > 0 && s[len - 1] == '\n')
        s[len - 1] = '\0';
}

int MyStrStr(const char *text, const char *key) {
    int tLen = MyStrLen(text);
    int kLen = MyStrLen(key);
    if (kLen == 0 || kLen > tLen) return 0;
    for (int i = 0; i <= tLen - kLen; i++) {
        int j;
        for (j = 0; j < kLen; j++) {
            if (text[i + j] != key[j]) break;
        }
        if (j == kLen) return 1;
    }
    return 0;
}

void GetTagPrefix(const char *src, char *prefix) {
    if (src[0] != '[') {
        prefix[0] = '\0';
        return;
    }
    int i = 0;
    while (src[i] != '\0' && src[i] != ']') {
        prefix[i] = src[i];
        i++;
    }
    if (src[i] == ']') {
        prefix[i] = ']';
        i++;
        if (src[i] == ' ') {
            prefix[i] = ' ';
            i++;
        }
    }
    prefix[i] = '\0';
}

void InitStack(StrStack *s) { s->top = -1; }
int IsFull(StrStack *s)  { return s->top == MAX_STACK_SIZE - 1; }
int IsEmpty(StrStack *s) { return s->top == -1; }
int Push(StrStack *s, const char *str) {
    if (IsFull(s)) return 0;
    s->top++;
    MyStrCpy(s->data[s->top], str);
    return 1;
}
int Pop(StrStack *s, char *str) {
    if (IsEmpty(s)) return 0;
    MyStrCpy(str, s->data[s->top]);
    s->top--;
    return 1;
}
int GetSize(StrStack *s) { return s->top + 1; }

void CopyStack(StrStack *dest, StrStack *src) {
    if (dest == src) return;
    InitStack(dest);
    for (int i = 0; i <= src->top; i++) {
        MyStrCpy(dest->data[i], src->data[i]);
    }
    dest->top = src->top;
}

void SortStackByTag(StrStack *s) {
    int size = GetSize(s);
    char tag1[MAX_STR_LEN], tag2[MAX_STR_LEN], temp[MAX_STR_LEN];
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            GetTagPrefix(s->data[j], tag1);
            GetTagPrefix(s->data[j + 1], tag2);
            if (MyStrCmp(tag1, tag2) > 0) {
                MyStrCpy(temp, s->data[j]);
                MyStrCpy(s->data[j], s->data[j + 1]);
                MyStrCpy(s->data[j + 1], temp);
            }
        }
    }
}

void ClearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int ExtractDateAndContent(const char *full, int *year, int *month, int *day, char *content) {
    const char *openBracket = strchr(full, '[');
    const char *closeBracket = strchr(full, ']');
    if (!openBracket || !closeBracket || closeBracket <= openBracket) {
        return 0;
    }
    int tagLen = closeBracket - openBracket - 1;
    if (tagLen <= 0 || tagLen >= MAX_STR_LEN) return 0;

    char tag[MAX_STR_LEN];
    int i;
    for (i = 0; i < tagLen; i++) {
        tag[i] = openBracket[1 + i];
    }
    tag[i] = '\0';

    if (sscanf(tag, "%d-%d-%d", year, month, day) != 3) {
        return 0;
    }
    const char *contentStart = closeBracket + 1;
    if (*contentStart == ' ') contentStart++;
    MyStrCpy(content, contentStart);
    return 1;
}

void addText(StrStack *s) {
    if (IsFull(s)) {
        printf("文本栈已满，无法添加！\n");
        return;
    }

    char tag[20], content[MAX_STR_LEN], full[MAX_STR_LEN];
    printf("请输入时间标签(如2026-05-22)：");
    fgets(tag, 20, stdin);
    if (tag[MyStrLen(tag) - 1] != '\n') {
        ClearInputBuffer();
    } else {
        TrimNewLine(tag);
    }
    if (MyStrStr(tag, "]")) {
        printf("错误：时间标签不能包含']'字符！\n");
        return;
    }

    int tagLen = MyStrLen(tag);
    if (tagLen > 18) {
        printf("错误：时间标签过长，请控制在18个字符以内！\n");
        return;
    }

    int maxContentLen = MAX_STR_LEN - tagLen - 4;
    if (maxContentLen <= 0) {
        printf("错误：时间标签太长，无法添加内容！\n");
        return;
    }

    printf("请输入文本内容（最长%d个字符）：", maxContentLen);
    fgets(content, maxContentLen, stdin);
    if (content[MyStrLen(content) - 1] != '\n') {
        ClearInputBuffer();
    } else {
        TrimNewLine(content);
    }
    full[0] = '[';
    full[1] = '\0';
    MyStrCat(full, tag);
    MyStrCat(full, "] ");
    MyStrCat(full, content);
    Push(s, full);
    printf("添加成功！\n");
}

void deleteText(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本可删除！\n");
        return;
    }
    int size = GetSize(s);
    printf("\n当前文本列表：\n");
    for (int i = 0; i < size; i++) {
        printf("%d. %s\n", i + 1, s->data[i]);
    }

    int idx;
    printf("请输入要删除的序号：");
    scanf("%d", &idx);
    ClearInputBuffer();
    idx--;
    if (idx < 0 || idx >= size) {
        printf("序号无效！\n");
        return;
    }

    StrStack temp;
    InitStack(&temp);
    for (int i = size - 1; i > idx; i--) {
        Push(&temp, s->data[i]);
    }
    s->top = idx - 1;
    char t[MAX_STR_LEN];
    while (!IsEmpty(&temp)) {
        Pop(&temp, t);
        Push(s, t);
    }
    printf("删除成功！\n");
}

void modifyText(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本可修改！\n");
        return;
    }
    int size = GetSize(s);
    printf("\n当前文本列表：\n");
    for (int i = 0; i < size; i++) {
        printf("%d. %s\n", i + 1, s->data[i]);
    }

    int idx;
    printf("请输入要修改的序号：");
    scanf("%d", &idx);
    ClearInputBuffer();
    idx--;
    if (idx < 0 || idx >= size) {
        printf("序号无效！\n");
        return;
    }

    char tagPrefix[30];
    GetTagPrefix(s->data[idx], tagPrefix);
    int tagPrefixLen = MyStrLen(tagPrefix);

    int maxContentLen = MAX_STR_LEN - tagPrefixLen - 1;
    if (maxContentLen <= 0) {
        printf("原标签太长，无法修改！\n");
        return;
    }

    char newContent[MAX_STR_LEN];
    printf("请输入新内容（最长%d个字符）：", maxContentLen);
    fgets(newContent, maxContentLen, stdin);
    if (newContent[MyStrLen(newContent) - 1] != '\n') {
        ClearInputBuffer();
    } else {
        TrimNewLine(newContent);
    }

    MyStrCpy(s->data[idx], tagPrefix);
    MyStrCat(s->data[idx], newContent);
    printf("修改成功！\n");
}

void searchText(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本可查找！\n");
        return;
    }
    char key[MAX_STR_LEN];
    printf("请输入查找关键词：");
    fgets(key, MAX_STR_LEN, stdin);
    TrimNewLine(key);

    int find = 0;
    int size = GetSize(s);
    printf("\n查找结果：\n");
    for (int i = 0; i < size; i++) {
        if (MyStrStr(s->data[i], key)) {
            printf("- %s\n", s->data[i]);
            find = 1;
        }
    }
    if (!find) printf("未找到匹配文本！\n");
}

void browseText(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本！\n");
        return;
    }
    int size = GetSize(s);
    int page = 1;
    int totalPage = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    char op;

    while (1) {
        int start = (page - 1) * PAGE_SIZE;
        int end = start + PAGE_SIZE;
        if (end > size) end = size;

        printf("\n--- 第 %d / %d 页 ---\n", page, totalPage);
        for (int i = start; i < end; i++) {
            printf("%d. %s\n", i + 1, s->data[i]);
        }

        printf("[n]下一页 [p]上一页 [q]退出：");
        scanf(" %c", &op);
        ClearInputBuffer();
        if (op == 'q' || op == 'Q') break;
        if ((op == 'n' || op == 'N') && page < totalPage) page++;
        else if ((op == 'p' || op == 'P') && page > 1) page--;
        else printf("无法操作！\n");
    }
}

void showAll(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本！\n");
        return;
    }
    int size = GetSize(s);
    printf("\n所有文本：\n");
    for (int i = 0; i < size; i++) {
        printf("%d. %s\n", i + 1, s->data[i]);
    }
}

void showByTime(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本！\n");
        return;
    }
    StrStack sorted;
    CopyStack(&sorted, s);
    SortStackByTag(&sorted);
    browseText(&sorted);
}

void showTreeByDate(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本！\n");
        return;
    }
    int size = GetSize(s);
    DateItem items[MAX_STACK_SIZE];
    int itemCount = 0;

    for (int i = 0; i < size; i++) {
        int y, m, d;
        char content[MAX_STR_LEN];
        if (ExtractDateAndContent(s->data[i], &y, &m, &d, content)) {
            items[itemCount].year = y;
            items[itemCount].month = m;
            items[itemCount].day = d;
            MyStrCpy(items[itemCount].contentOnly, content);
            itemCount++;
        }
    }

    if (itemCount == 0) {
        printf("没有找到有效的日期格式文本！\n");
        return;
    }

    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - 1 - i; j++) {
            if (items[j].year > items[j+1].year ||
                (items[j].year == items[j+1].year && items[j].month > items[j+1].month) ||
                (items[j].year == items[j+1].year && items[j].month == items[j+1].month && items[j].day > items[j+1].day)) {
                DateItem temp = items[j];
                items[j] = items[j+1];
                items[j+1] = temp;
            }
        }
    }

    int idx = 0;
    int prev_year = -1, prev_month = -1, prev_day = -1;
    while (idx < itemCount) {
        int cur_year = items[idx].year;
        int cur_month = items[idx].month;
        int cur_day = items[idx].day;

        if (cur_year != prev_year) {
            printf("%d\n", cur_year);
            prev_month = -1;
            prev_day = -1;
        }
        if (cur_year != prev_year || cur_month != prev_month) {
            printf("  |-- %02d\n", cur_month);
            prev_day = -1;
        }
        if (cur_year != prev_year || cur_month != prev_month || cur_day != prev_day) {
            printf("      |-- %02d\n", cur_day);
        }
        while (idx < itemCount &&
               items[idx].year == cur_year &&
               items[idx].month == cur_month &&
               items[idx].day == cur_day) {
            printf("          |-- %s\n", items[idx].contentOnly);
            idx++;
        }
        prev_year = cur_year;
        prev_month = cur_month;
        prev_day = cur_day;
    }
}
void InitMenuGraph(MenuGraph *mg) {
    mg->nodeCount = 0;
    mg->currentNode = -1;
}

int AddMenuNode(MenuGraph *mg, const char *name, const char *desc, int parentId, void (*action)(StrStack*)) {
    if (mg->nodeCount >= MAX_NODES) {
        printf("菜单节点已满！\n");
        return -1;
    }
    
    int id = mg->nodeCount;
    mg->nodes[id].id = id;
    MyStrCpy(mg->nodes[id].name, name);
    MyStrCpy(mg->nodes[id].description, desc);
    mg->nodes[id].parentId = parentId;
    mg->nodes[id].childCount = 0;
    mg->nodes[id].action = action;
    
    if (parentId >= 0) {
        if (mg->nodes[parentId].childCount < MAX_NODES) {
            mg->nodes[parentId].children[mg->nodes[parentId].childCount++] = id;
        }
    }
    
    mg->nodeCount++;
    return id;
}

int FindMenuNode(MenuGraph *mg, const char *name) {
    for (int i = 0; i < mg->nodeCount; i++) {
        if (MyStrCmp(mg->nodes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void ShowCurrentMenu(MenuGraph *mg) {
    if (mg->currentNode < 0) {
        printf("错误：未进入菜单系统！\n");
        return;
    }
    
    MenuNode *current = &mg->nodes[mg->currentNode];
    printf("\n========================================\n");
    printf("📋 %s\n", current->name);
    printf("   %s\n", current->description);
    printf("========================================\n");
    
    if (current->childCount == 0) {
        printf("   [执行功能]\n");
    } else {
        for (int i = 0; i < current->childCount; i++) {
            int childId = current->children[i];
            printf("   %d. %s\n", i + 1, mg->nodes[childId].name);
        }
    }
    
    if (current->parentId >= 0) {
        printf("   0. 返回上一级\n");
    }
    printf("========================================\n");
    printf("请选择：");
}

// 执行菜单导航
void NavigateMenu(MenuGraph *mg, StrStack *s) {
    int choice;
    char input[10];
    
    while (1) {
        if (mg->currentNode < 0) {
            printf("请先进入菜单系统！\n");
            return;
        }
        
        ShowCurrentMenu(mg);
        fgets(input, sizeof(input), stdin);
        TrimNewLine(input);

        if (MyStrCmp(input, "0") == 0) {
            if (mg->nodes[mg->currentNode].parentId >= 0) {
                mg->currentNode = mg->nodes[mg->currentNode].parentId;
                continue;
            }
        }
        
        // 尝试解析为数字
        choice = atoi(input);
        if (choice == 0 && MyStrCmp(input, "0") != 0) {
            printf("输入无效，请重新选择！\n");
            continue;
        }
        
        MenuNode *current = &mg->nodes[mg->currentNode];
        
        if (current->childCount == 0) {
            if (choice == 1) {
                if (current->action != NULL) {
                    current->action(s);
                } else {
                    printf("功能未实现！\n");
                }
                if (current->parentId >= 0) {
                    mg->currentNode = current->parentId;
                }
            } else {
                printf("无效选择！\n");
            }
        } else {
            if (choice >= 1 && choice <= current->childCount) {
                int childId = current->children[choice - 1];
                mg->currentNode = childId;
            } else {
                printf("无效选择！\n");
            }
        }
    }
}

void BuildMenuGraph(MenuGraph *mg) {
    InitMenuGraph(mg);
    
    // 创建根节点
    int rootId = AddMenuNode(mg, "📚 文本管理系统", "主菜单", -1, NULL);
    mg->currentNode = rootId;
    
    // 一级菜单：文本管理
    int textManageId = AddMenuNode(mg, "📝 文本管理", "管理所有文本内容", rootId, NULL);
    
    // 二级菜单：文本管理下的功能
    AddMenuNode(mg, "➕ 添加文本", "添加新的时间标签文本", textManageId, addText);
    AddMenuNode(mg, "➖ 删除文本", "删除指定的文本", textManageId, deleteText);
    AddMenuNode(mg, "✏️ 修改文本", "修改文本内容", textManageId, modifyText);
    AddMenuNode(mg, "🔍 查找文本", "搜索关键词", textManageId, searchText);
    
   
    int browseId = AddMenuNode(mg, "👁️ 浏览功能", "查看和浏览文本", rootId, NULL);
    
    AddMenuNode(mg, "📄 分页浏览", "分页查看所有文本", browseId, browseText);
    AddMenuNode(mg, "📋 全部显示", "显示所有文本", browseId, showAll);
    AddMenuNode(mg, "🕐 按时间排序", "按时间标签排序浏览", browseId, showByTime);
    AddMenuNode(mg, "🌳 树形展示", "按日期树形结构展示", browseId, showTreeByDate);
}
int main(void) {
    StrStack st;
    InitStack(&st);
    
    MenuGraph menuGraph;
    BuildMenuGraph(&menuGraph);
    
    printf("\n🎯 基于图结构的菜单导航系统\n");
    printf("提示：菜单采用树形结构，选择数字进入子菜单或执行功能\n");
    printf("      0 返回上一级\n\n");
    
    NavigateMenu(&menuGraph, &st);
    
    return 0;
}

