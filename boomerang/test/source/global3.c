
long long a = 123456789012345;
int b = 7;

void foo2()
{
    b = 12;
    printf("a = %lld\n", a);
}

void foo1()
{
    foo2();
}

int main()
{
    foo1();
    printf("b = %i\n", b);
    return 0;
}



