int recursiveFunction(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l) {
    if (a == 0) {
        return 0;
    }
    return a + b + c + d + e + f + g + h + i + j + k + l + recursiveFunction(a - 1, b, c, d, e, f, g, h, i, j, k, l);
}

int main() {
    int result = recursiveFunction(5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
    return result;
}
