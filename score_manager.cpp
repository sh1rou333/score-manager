#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_SIZE 100
#define MAX_STR_LEN    256
#define PAGE_SIZE      5

typedef struct {
    char data[MAX_STACK_SIZE][MAX_STR_LEN];
    int top;
} StrStack;

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
void ClearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void menu() {
    printf("\n手动时间标签\n");
    printf("1. 添加文本(含时间标签)\n");
    printf("2. 删除文本\n");
    printf("3. 修改文本\n");
    printf("4. 查找文本\n");
    printf("5. 分页浏览文本\n");
    printf("6. 显示所有文本\n");
    printf("7. 按时间分类显示\n");
    printf("8. 树形结构展示所有日期\n");   // 新增菜单项
    printf("0. 退出\n");
    printf("============================================\n");
    printf("请选择：");
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

// ------------------- 新增：树形结构展示所有日期 -------------------
typedef struct {
    int year, month, day;
    char content[MAX_STR_LEN];
} DateRecord;

// 解析标签中的年月日，成功返回1，否则0
static int ParseDateFromTag(const char *tagPrefix, int *year, int *month, int *day) {
    // tagPrefix 格式为 "[2026-05-22] " 或类似
    return sscanf(tagPrefix, "[%d-%d-%d]", year, month, day) == 3;
}

void showDateTree(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本，无法展示日期树！\n");
        return;
    }

    DateRecord records[MAX_STACK_SIZE];
    int recCount = 0;

    // 遍历栈中所有文本，提取日期和纯文本内容
    for (int i = 0; i <= s->top; i++) {
        char full[MAX_STR_LEN];
        MyStrCpy(full, s->data[i]);

        char tagPrefix[MAX_STR_LEN];
        GetTagPrefix(full, tagPrefix);

        int y, m, d;
        if (ParseDateFromTag(tagPrefix, &y, &m, &d)) {
            // 定位内容起始位置：跳过 tagPrefix 的长度
            char *contentStart = full + MyStrLen(tagPrefix);
            records[recCount].year = y;
            records[recCount].month = m;
            records[recCount].day = d;
            MyStrCpy(records[recCount].content, contentStart);
            recCount++;
        } else {
            // 日期格式无效，忽略这条记录（不中断程序）
            // printf("警告：无法解析时间标签：%s\n", full);
        }
    }

    if (recCount == 0) {
        printf("没有找到任何有效的日期数据，无法展示日期树。\n");
        return;
    }

    // 冒泡排序：按年、月、日升序
    for (int i = 0; i < recCount - 1; i++) {
        for (int j = 0; j < recCount - 1 - i; j++) {
            int y1 = records[j].year, y2 = records[j+1].year;
            int m1 = records[j].month, m2 = records[j+1].month;
            int d1 = records[j].day, d2 = records[j+1].day;
            if (y1 > y2 ||
               (y1 == y2 && m1 > m2) ||
               (y1 == y2 && m1 == m2 && d1 > d2)) {
                DateRecord tmp = records[j];
                records[j] = records[j+1];
                records[j+1] = tmp;
            }
        }
    }

    // 树形打印
    int lastYear = -1, lastMonth = -1;
    printf("\n========== 日期树形结构 ==========\n");
    for (int i = 0; i < recCount; ) {
        int year = records[i].year;
        int month = records[i].month;
        int day = records[i].day;

        // 打印年份节点
        if (year != lastYear) {
            if (lastYear != -1) putchar('\n');
            printf("%d\n", year);
            lastYear = year;
            lastMonth = -1;   // 年份切换后强制重打月份
        }

        // 打印月份节点
        if (month != lastMonth) {
            printf("  %02d\n", month);
            lastMonth = month;
        }

        // 打印日期节点（当天可能有若干条文本）
        printf("    %02d\n", day);
        // 输出该日期的所有文本
        while (i < recCount &&
               records[i].year == year &&
               records[i].month == month &&
               records[i].day == day) {
            printf("      - %s\n", records[i].content);
            i++;
        }
    }
    printf("==================================\n");
}
// ------------------- 新增结束 -------------------

int main() {
    StrStack st;
    InitStack(&st);
    int choice;

    while (1) {
        menu();
        scanf("%d", &choice);
        ClearInputBuffer(); 
        switch (choice) {
            case 1: addText(&st); break;
            case 2: deleteText(&st); break;
            case 3: modifyText(&st); break;
            case 4: searchText(&st); break;
            case 5: browseText(&st); break;
            case 6: showAll(&st); break;
            case 7: showByTime(&st); break;
            case 8: showDateTree(&st); break;   // 新增调用
            case 0:
                printf("退出系统\n");
                return 0;
            default:
                printf("输入错误，请重新选择！\n");
        }
    }
    return 0;
}
