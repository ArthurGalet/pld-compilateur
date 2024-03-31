int main(){
    int a = 1, b = 1;

    if (a) {
        a = -1;
        if (a) {
            b = 2;
        }
    }
    return a * b;
}