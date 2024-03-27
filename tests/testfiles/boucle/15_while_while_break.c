int main() {
    int a = 0, b = 10;
    while (a < 2) {
        while (b < 11) {
            b = b + 1;
            break;
        }
        a = a + 1;
    }
    return a * b;
}