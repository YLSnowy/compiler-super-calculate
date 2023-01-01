int a[10];
int main(){
    int i=0;
    while(i<10){
        a[i]=i+1;
        i=i+1;
    }
    i=0;
    while(i<10){
        printf("%d\n",a[i]);
        i=i+1;
    }
    return 0;
}
