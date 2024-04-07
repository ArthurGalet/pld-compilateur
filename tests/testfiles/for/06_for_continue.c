int main(){
    int i;
    for (i=0; i<3; i++){
        if(i>1){
            continue;
            i += 5;
        }
    }
    return i;
}