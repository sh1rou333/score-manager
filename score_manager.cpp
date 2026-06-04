#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 100
#define MAX_STR_LEN    256
#define PAGE_SIZE      5

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
void ClearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void menu(void) {
    printf("\n手动时间标签\n");
    printf("1. 添加文本(含时间标签)\n");
    printf("2. 删除文本\n");
    printf("3. 修改文本\n");
    printf("4. 查找文本\n");
    printf("5. 分页浏览文本\n");
    printf("6. 显示所有文本\n");
    printf("7. 按时间分类显示\n");
    printf("8. 树形结构展示所有日期\n");
    printf("0. 退出\n");
    printf("============================================\n");
    printf("请选择：");
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

int main(void) {
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
            case 8: showTreeByDate(&st); break;//新增功能
            case 0:
                printf("退出系统\n");
                return 0;
            default:
                printf("输入错误，请重新选择！\n");
        }
    }
    return 0;
}
