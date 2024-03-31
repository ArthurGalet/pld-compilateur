int main(){
    int a = 1;
    int b;
    if (a) {
        a = -1;
        if (a) {
            b = 2;
        }
    }
    return a;
}