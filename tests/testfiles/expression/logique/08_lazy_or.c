int main() {
    int a;
    int b = (a = 5) || (a = 0);
    return a + b;
}