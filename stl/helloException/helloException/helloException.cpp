// helloException.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>

using namespace std;

class Expt
{
    public:
        Expt() {};
        ~Expt() {};
        const char *ShowReason() const
        {
            return "Except �쳣";
        }
        
        
};

class Demo
{
    public:
        Demo() ;
        ~Demo();
};

Demo::Demo()
{
    cout << "����Demo" << endl;
}

Demo::~Demo()
{
    cout << "����Demo" << endl;
}

void MyFunc()
{
    Demo D;
    cout << "��MyFunc()�׳�Expt���쳣" << endl;
    throw Expt();
}

class Point
{
    public:
        Point();
        ~Point();
        void InitP (float xx = 0, float yy = 0)
        {
            X = xx;
            Y = yy;
        };
        void Move (float xOff, float yOff)
        {
            X += xOff;
            Y += yOff;
        };
        float GetX()
        {
            return X;
        };
        float GetY()
        {
            return Y;
        };
        
        
    private:
        float X;
        float Y;
};



template <class T>
class Store
{
    private:
        T item;
        int haveValue;
        
    public:
        Store (void);
        T GetElem (void);
        void PutElem (T x);
        
};


template <class T>
Store<T>::Store (void) : haveValue (0)
{}

template <class T>
T Store<T>::GetElem (void)
{
    if (0 == haveValue)
    {
        cout << "no item present" << endl;
        exit (1);
    }
    
    return item;
}

template <class T>
void Store<T>::PutElem (T x)
{
        haveValue++;
        item = x;
}

int main()
{
    cout << "��Main������" << endl;
    
    try
    {
        cout << "��try���У�����MyFunc()" << endl;
        MyFunc();
    }
    
    catch (Expt E)
    {
        cout << "��try���У�����MyFunc()" << endl;
        cout << "����Expt�����쳣" << endl;
        cout << E.ShowReason() << endl;
    }
    
    catch (char* str)
    {
        cout << "�����������쳣" << endl;
    }
    
    cout << "�ص�Main������������ָ�ִ��" << endl;
    return 0;
}