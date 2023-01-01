int a[10][10];
int main(){
    int i=0;
    while(i<10){
        int j=0;
        while(j<10){
            a[i][j]=i*j+i+j;
            j=j+1;
        }
        i=i+1;
    }
    i=0;
    while(i<10){
        int j=0;
        while(j<10){
            printf("%d\n",a[i][j]);
            j=j+1;
        }
        i=i+1;
    }
    return 0;
}
