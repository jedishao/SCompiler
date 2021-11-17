struct point
{
    int x;
    int y;
};
void main()
{
    int arr[10];
    int i;
    struct point pt;
    pt.x = 1024;
    pt.y = 768;
    for (i = 0; i < 10; i = i + 1)
    {
        /* code */
        arr[i] = i;
    }
}