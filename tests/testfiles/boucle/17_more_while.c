int main() {
    int a = 0;
    while (a < 5) {
        a = a + 1;
    }
    a = a * 2;
    while(a != 1) {
        a = a / 2;
    }
    a = a * 7;
    while ((a % 50 != 3)) {
        a = a * 19;
    }
    return a;
}