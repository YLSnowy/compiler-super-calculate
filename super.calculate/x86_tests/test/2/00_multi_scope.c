int a=10;
int main(){
    int a;
    a=1;
    int i=0;
    while(i<10){
        int a=i*10+i%3;
        printf("Test %d %d\n",i,a);
        i=i+1;
    }
    printf("Test %d\n",a);
    return 0;
}