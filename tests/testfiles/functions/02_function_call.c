
int d() {
    int a = 1;
    return 1;
}
int another_function() {
    int a = 2;
    int b = 3;

    return d() + b + a;
}

int main() {
    int a = another_function();
    return a;
}