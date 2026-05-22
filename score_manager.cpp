#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_SIZE 100
#define MAX_STR_LEN    256
#define PAGE_SIZE      5

typedef struct {
    char data[MAX_STACK_SIZE][MAX_STR_LEN];
    int top;
} StrStack;

void InitStack(StrStack *s) {
    s->top = -1;
}

int IsFull(StrStack *s) {
    return s->top == MAX_STACK_SIZE - 1;
}

int IsEmpty(StrStack *s) {
    return s->top == -1;
}

int MyStrLen(char *s) {
    int len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

// 带长度限制的安全字符串拷贝，防止越界
void MyStrCpy(char *dest, char *src, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// 清空输入缓冲区
void CleanBuffer()
{
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// 去除末尾换行符，增加空串防护
void TrimNewLine(char *s) {
    int len = MyStrLen(s);
    if (len <= 0) return;
    if (s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

int MyStrStr(char *text, char *key) {
    int tLen = MyStrLen(text);
    int kLen = MyStrLen(key);
    if (kLen == 0 || kLen > tLen)
        return 0;

    for (int i = 0; i <= tLen - kLen; i++) {
        int j;
        for (j = 0; j < kLen; j++) {
            if (text[i + j] != key[j])
                break;
        }
        if (j == kLen)
            return 1;
    }
    return 0;
}

int Push(StrStack *s, char *str) {
    if (IsFull(s)) return 0;
    s->top++;
    MyStrCpy(s->data[s->top], str, MAX_STR_LEN);
    return 1;
}

int Pop(StrStack *s, char *str) {
    if (IsEmpty(s)) return 0;
    MyStrCpy(str, s->data[s->top], MAX_STR_LEN);
    s->top--;
    return 1;
}

int GetSize(StrStack *s) {
    return s->top + 1;
}

void menu() {
    printf("\n==== 无库函数 栈版文本管理 ====\n");
    printf("1. 添加文本\n");
    printf("2. 删除文本\n");
    printf("3. 修改文本\n");
    printf("4. 查找文本\n");
    printf("5. 分页浏览文本\n");
    printf("6. 显示所有文本\n");
    printf("0. 退出\n");
    printf("==============================\n");
    printf("请选择：");
}

void addText(StrStack *s) {
    if (IsFull(s)) {
        printf("文本栈已满，无法添加！\n");
        return;
    }
    char buf[MAX_STR_LEN];
    CleanBuffer();
    printf("请输入文本内容：");
    fgets(buf, MAX_STR_LEN, stdin);
    TrimNewLine(buf);
    Push(s, buf);
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
    idx--;
    if (idx < 0 || idx >= size) {
        printf("序号无效！\n");
        return;
    }
    CleanBuffer();
    char buf[MAX_STR_LEN];
    printf("请输入新内容：");
    fgets(buf, MAX_STR_LEN, stdin);
    TrimNewLine(buf);
    MyStrCpy(s->data[idx], buf, MAX_STR_LEN);
    printf("修改成功！\n");
}

void searchText(StrStack *s) {
    if (IsEmpty(s)) {
        printf("暂无文本可查找！\n");
        return;
    }
    char key[MAX_STR_LEN];
    CleanBuffer();
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
    if (!find) {
        printf("未找到匹配文本！\n");
    }
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
        if (op == 'q' || op == 'Q') break;

        if (op == 'n' || op == 'N')
        {
            if (page < totalPage)
                page++;
            else
                printf("已经是最后一页，无法翻页\n");
        }
        else if (op == 'p' || op == 'P')
        {
            if (page > 1)
                page--;
            else
                printf("已经是第一页，无法翻页\n");
        }
        else
        {
            printf("输入指令无效，请重新操作\n");
        }
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

int main() {
    StrStack st;
    InitStack(&st);
    int choice;

    while (1) {
        menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: addText(&st); break;
            case 2: deleteText(&st); break;
            case 3: modifyText(&st); break;
            case 4: searchText(&st); break;
            case 5: browseText(&st); break;
            case 6: showAll(&st); break;
            case 0:
                printf("退出系统\n");
                return 0;
            default:
                printf("输入错误，请重新选择！\n");
                CleanBuffer();
                break;
        }
    }
    return 0;
}
