int main() {
    int a;
    int b = (a = 3) && (a = 0);
    return a + b;
}