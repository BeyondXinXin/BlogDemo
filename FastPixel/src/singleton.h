#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
public:
    static T &Inst()
    {
        static T value_; //静态局部变量
        return value_;
    }

private:
    Singleton();
    ~Singleton();
    Singleton(const Singleton &); //拷贝构造函数
    Singleton &operator=(const Singleton &); // =运算符重载
};

#endif // SINGLETON_H
