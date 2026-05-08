#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TEXT_NUM 100    
#define MAX_TEXT_LEN 256 
#define PAGE_SIZE 5

char texts[MAX_TEXT_NUM][MAX_TEXT_LEN];
int count = 0;             

void menu() {
    printf("\n====== 文本管理系统 ======\n");
    printf("1. 添加文本\n");
    printf("2. 删除文本\n");
    printf("3. 修改文本\n");
    printf("4. 查找文本\n");
    printf("5. 显示所有文本\n");
    printf("6. 分页浏览文本\n");
    printf("0. 退出\n");
    printf("==========================\n");
    printf("请选择：");
}

void addText() {
    if (count >= MAX_TEXT_NUM) {
        printf("文本数量已达上限，无法添加！\n");
        return;
    }
    printf("请输入文本内容：");
    getchar();
    fgets(texts[count], MAX_TEXT_LEN, stdin);
    texts[count][strcspn(texts[count], "\n")] = '\0';
    count++;
    printf("添加成功！\n");
}

void deleteText() {
    if (count == 0) {
        printf("暂无文本可删除！\n");
        return;
    }

    int i, index;
    printf("\n当前文本列表：\n");
    for (i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, texts[i]);
    }

    printf("请输入要删除的序号：");
    scanf("%d", &index);
    index--;

    if (index < 0 || index >= count) {
        printf("序号无效！\n");
        return;
    }
    for (i = index; i < count - 1; i++) {
        strcpy(texts[i], texts[i + 1]);
    }
    count--;
    printf("删除成功！\n");
}

void modifyText() {
    if (count == 0) {
        printf("暂无文本可修改！\n");
        return;
    }

    int i, index;
    printf("\n当前文本列表：\n");
    for (i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, texts[i]);
    }

    printf("请输入要修改的文本序号：");
    scanf("%d", &index);
    index--;

    if (index < 0 || index >= count) {
        printf("序号无效！\n");
        return;
    }

    getchar();
    printf("请输入新的文本内容：");
    fgets(texts[index], MAX_TEXT_LEN, stdin);
    texts[index][strcspn(texts[index], "\n")] = '\0';

    printf("修改成功！\n");
}

void searchText() {
    if (count == 0) {
        printf("暂无文本可查找！\n");
        return;
    }

    char key[MAX_TEXT_LEN];
    printf("请输入查找关键词：");
    getchar();
    fgets(key, MAX_TEXT_LEN, stdin);
    key[strcspn(key, "\n")] = '\0';

    int found = 0;
    printf("\n查找结果：\n");
    for (int i = 0; i < count; i++) {
        if (strstr(texts[i], key) != NULL) {
            printf("- %s\n", texts[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("未找到包含关键词的文本。\n");
    }
}

void browseText() {
    if (count == 0) {
        printf("暂无文本可浏览！\n");
        return;
    }

    int page = 1;
    int totalPages = (count + PAGE_SIZE - 1) / PAGE_SIZE;
    char op;

    while (1) {
        int start = (page - 1) * PAGE_SIZE;
        int end = start + PAGE_SIZE;
        if (end > count) end = count;

        printf("\n--- 第 %d / %d 页 ---\n", page, totalPages);
        for (int i = start; i < end; i++) {
            printf("%d. %s\n", i + 1, texts[i]);
        }

        printf("\n操作说明：[n]下一页  [p]上一页  [q]退出浏览\n请输入操作：");
        scanf(" %c", &op);

        if (op == 'q' || op == 'Q') {
            break;
        } else if ((op == 'n' || op == 'N') && page < totalPages) {
            page++;
        } else if ((op == 'p' || op == 'P') && page > 1) {
            page--;
        } else {
            printf("无法执行该操作！\n");
        }
    }
}

void showAll() {
    if (count == 0) {
        printf("暂无文本！\n");
        return;
    }

    printf("\n所有文本：\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, texts[i]);
    }
}

int main() {
    int choice;
    while (1) {
        menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: addText(); break;
            case 2: deleteText(); break;
            case 3: modifyText(); break;
            case 4: searchText(); break;
            case 5: showAll(); break;
            case 6: browseText(); break;
            case 0:
                printf("退出系统。\n");
                return 0;
            default:
                printf("输入错误，请重新选择！\n");
        }
    }
    return 0;
}