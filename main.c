#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Strings/Headers/String.h"
#include "Lists/Headers/Vector.h"
#include "FilesHandler/Headers/TxtFileLoader.h"
#include <pthread.h>


typedef struct Param {
    int m1R, m1C, m2R, m2C, *fMatrix, *sMatrix, *answerMatrix, r, c;
} Param;

void *calElement(void *p) {
    Param *param = (Param *) p;
    int m1R = param->m1R, m1C = param->m1C, m2R = param->m2R, m2C = param->m2C,
            *fMatrix = param->fMatrix, *sMatrix = param->sMatrix, *answerMatrix = param->answerMatrix, r = param->r, c = param->c;
    int sum = 0;

    for (int i = 0; i < m1C; i++) {
        sum += fMatrix[r * m1C + i] * sMatrix[i * m2C + c];
    }
    answerMatrix[r * m2C + c] = sum;
    pthread_exit(NULL);

}

void *calRow(void *p) {
    Param *param = (Param *) p;
    int m1R = param->m1R, m1C = param->m1C, m2R = param->m2R, m2C = param->m2C,
            *fMatrix = param->fMatrix, *sMatrix = param->sMatrix, *answerMatrix = param->answerMatrix, r = param->r;

    int sum;
    for (int i = 0; i < m2C; i++) {
        sum = 0;
        for (int j = 0; j < m1C; j++) {
            sum += fMatrix[r * m1C + j] * sMatrix[j * m2C + i];
        }
        answerMatrix[r * m2C + i] = sum;
    }

    pthread_exit(NULL);
}


int main() {

    double time_spent;
    clock_t begin, end;
    int m1R, m1C, m2R, m2C, currThread;

    char filePath[] = "C:\\Users\\mosta\\CLionProjects\\MatrixMultiplication\\input.txt";
    TxtFileLoader *txtFileLoader = txtFileLoaderInitialization(filePath);
    Vector *fileLines = txtLoaderReadFileLines(txtFileLoader);
    if (vectorGetLength(fileLines) < 4) {
        fprintf(stderr, "The file isn't valid.");
        exit(-1);
    }

    String *fMatrixSizeLine = vectorGet(fileLines, 0);
    Vector *fMatrixSizeSplitted = stringSplit(fMatrixSizeLine, " ");
    if (vectorGetLength(fMatrixSizeSplitted) != 2) {
        fprintf(stderr, "The file isn't valid.");
        exit(-1);
    }

    m1R = atoi(((String *) vectorGet(fMatrixSizeSplitted, 0))->string);
    m1C = atoi(((String *) vectorGet(fMatrixSizeSplitted, 1))->string);
    destroyString(fMatrixSizeLine);
    destroyVector(fMatrixSizeSplitted);
    Vector *splittedLine;

    int fMatrix[m1R][m1C];
    for (int i = 1; i <= m1R; i++) {
        splittedLine = stringSplit(vectorGet(fileLines, i), " ");
        if (vectorGetLength(splittedLine) != m1C) {
            fprintf(stderr, "The file isn't valid.");
            exit(-1);
        }
        for (int j = 0; j < m1C; j++)
            fMatrix[i - 1][j] = atoi(((String  *) vectorGet(splittedLine, j))->string);

        destroyVector(splittedLine);
    }


    String *sMatrixSizeLine = vectorGet(fileLines, m1R + 1);
    Vector *sMatrixSizeSplitted = stringSplit(sMatrixSizeLine, " ");
    if (vectorGetLength(sMatrixSizeSplitted) != 2) {
        fprintf(stderr, "The file isn't valid.");
        exit(-1);
    }

    m2R = atoi(((String *) vectorGet(sMatrixSizeSplitted, 0))->string);
    m2C = atoi(((String *) vectorGet(sMatrixSizeSplitted, 1))->string);
    destroyString(sMatrixSizeLine);
    destroyVector(sMatrixSizeSplitted);

    int sMatrix[m2R][m2C];
    for (int i = m1R + 2; i <= m2R + m1R + 1; i++) {
        splittedLine = stringSplit(vectorGet(fileLines, i), " ");
        if (vectorGetLength(splittedLine) != m2C) {
            fprintf(stderr, "The file isn't valid.");
            exit(-1);
        }
        for (int j = 0; j < m2C; j++)
            sMatrix[i - m1R - 2][j] = atoi(((String  *) vectorGet(splittedLine, j))->string);

        destroyVector(splittedLine);
    }

    destroyTxtFileLoader(txtFileLoader);
    destroyVector(fileLines);

    int answerMatrix[m1R][m2C];
    pthread_t threads_id[m1R * m2C];
    Param params[m1R * m2C];
    for (int i = 0; i < m1R * m2C; i++) {
        params[i].m1R = m1R;
        params[i].m1C = m1C;
        params[i].m2R = m2R;
        params[i].m2C = m2C;
        params[i].fMatrix = (int *) fMatrix;
        params[i].sMatrix = (int *) sMatrix;
        params[i].answerMatrix = (int *) answerMatrix;
    }

    // start of method 1:
    begin = clock();
    for (int i = 0; i < m1R; i++) {
        for (int j = 0; j < m2C; j++) {
            currThread = i * m2C + j;
            params[currThread].r = i;
            params[currThread].c = j;
            pthread_create(threads_id + currThread, NULL, calElement, params + currThread);
        }
    }

    for (int i = 0; i < m1R * m2C; i++) {
        if (pthread_join(threads_id[i], NULL))
            fprintf(stderr, "Some thing went wrong.");

    }

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Answer by first procedure:\n");
    for (int i = 0; i < m1R; i++) {
        for (int j = 0; j < m2C; j++) {
            printf("%d%c", answerMatrix[i][j], j == m2C - 1 ? '\n' : ' ');
        }
    }

    printf("Time spent: %f s\n\n", time_spent);
    // finish of method 1.

    // start of method 2:
    begin = clock();

    for (int i = 0; i < m1R; i++) {
        currThread = i;
        params[currThread].r = i;
        pthread_create(threads_id + currThread, NULL, calRow, params + currThread);
    }

    for (int i = 0; i < m1R; i++) {
        if (pthread_join(threads_id[i], NULL))
            fprintf(stderr, "Some thing went wrong.");

    }

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Answer by second procedure:\n");
    for (int i = 0; i < m1R; i++) {
        for (int j = 0; j < m2C; j++) {
            printf("%d%c", answerMatrix[i][j], j == m2C - 1 ? '\n' : ' ');
        }
    }

    printf("Time spent: %f s\n", time_spent);
    // end of method 2.

    return 0;
}
