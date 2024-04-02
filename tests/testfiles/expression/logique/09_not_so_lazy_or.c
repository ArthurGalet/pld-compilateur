int main() {
    int a;
    int b = (a = 0) || (a = 5);
    return a + b;
}