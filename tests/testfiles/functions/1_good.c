int pitagoras(int a, int b) {
    int result = a * a + b * b;
    return result;
}

int main() {
    int x = 3;
    int y = 4;
    int z = pitagoras(x, y);
    return z;
}