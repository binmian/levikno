#include <levikno/levikno.h>
#include <cstdio>

void func(void* userData)
{
    for (int i = 0; i < 1000; i++)
    {
        printf("thread %d: %d\n", *(int*)(userData), i);
    }
}

int main(int argc, char** argv)
{
    lvn::initContext();

    int a = 1, b = 2;

    LvnThread thread1(func, &a);
    LvnThread thread2(func, &b);

    thread1.join();
    thread2.join();


    lvn::terminateContext();

    return 0;
}
