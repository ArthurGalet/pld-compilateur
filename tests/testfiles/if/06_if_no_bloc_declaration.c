int main() {
    int a = 1;
    while (a)
        int b = (a = 0);
    return a;
}