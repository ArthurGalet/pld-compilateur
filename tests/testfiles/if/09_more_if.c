int main() {
    int a = 19542;
    if  (a % 3 < 2) {
        a = a / 19;
    } else {
        a = a * 19;
    }
    a = a % 1000;
    if  (a < 500) {
        a = a * 50;
    } else {
        a = a / 50;
    }
    return a;
}