int main(){
    int i=0;
    while(i<10){
        printf("%d\n",i);
        i = i+1;
        if(i<5){
            printf("Continue\n");
            continue;
        }
        break;
    }
    return 0;
}