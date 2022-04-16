#include <cstdio>

int main() {
    char path[1024];
    long x;
    printf("input the file path: ");  // ../chapters/3.2/answer.txt
    scanf("%s", path);
    FILE* fp = fopen(path, "r");
    fscanf(fp, "%ld", &x);  // NOLINT(cert-err34-c)
    fclose(fp);
    printf("%lX", x);
    return 0;
}
