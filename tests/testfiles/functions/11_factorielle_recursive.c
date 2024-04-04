int factorielle(int n){
    if (n == 0) {
        return 1;
    } else {
        return n * factorielle(n - 1);
    }
}

int main(){
    return factorielle(5);
}