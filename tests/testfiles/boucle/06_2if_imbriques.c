int main(){
    int a = 1;
    if (a) {
        a = -1;
        if (a) {
            a = 2;
        }
    }
    return a;
}