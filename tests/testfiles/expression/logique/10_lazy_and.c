int main() {
    int a;
    int b = (a = 0) && (a = 3);
    return a + b;
}