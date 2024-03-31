int main() {
    int a = 0;
    while (a < 10) {
        if (a < 1) {
            a = a + 5;
            continue;
        }
        a = a + 1;
    }
    return a;
}