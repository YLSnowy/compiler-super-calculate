int main(){
    int i=0;
    while(i<10){
        printf("Test:%d\n",i);
        if(i>5){
            printf("Break\n");
            break;
        }
        i=i+1;
    }
    return 0;
}