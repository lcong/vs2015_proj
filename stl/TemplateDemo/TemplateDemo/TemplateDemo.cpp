// TemplateDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "TemplateDemo.h"
#include <string>
#include <vector>
#include <iterator>
#include <algorithm> 

using namespace std;

template<class T>
A<T>::A()
{}

template<class T> T
A<T>::g (T a, T b)
{
    return a + b;
}

int main()
{
    A<int> a;
    cout << a.g (2, 3.2) << endl;
    A<float> b;
    cout << b.g (2, 3.2) << endl;
    string str[] = { "alex", "john", "robot" };
    vector<int> v1;
    vector<int> v2 (10);
    vector<int> v3 (10, 0);
    vector<std::string> v4 (str + 0, str + 3);
    vector<std::string>::iterator sIt = v4.begin();
    
    while (sIt != v4.end())
    {
        std::cout << * (sIt++) << ";";
    }
    
    std::cout << endl;

    int ary[] = { 1, 2, 3, 4, 5 };
    vector<int> v;
    // assign to the "v" the contains of "ary"
    v.assign (ary, ary + 5);
    copy (v.begin(), v.end(),
          ostream_iterator<int> (cout, " "));
    cout << endl;
    // replace v for 3 copies of 100
    v.assign (3, 100);
    copy (v.begin(), v.end(),
          ostream_iterator <int>(cout, " "));
    cout << endl;
    
    try
    {
        Stack<int, 20>  int20Stack;  // ���Դ洢20��intԪ�ص�ջ
        Stack<int, 40>  int40Stack;  // ���Դ洢40��intԪ�ص�ջ
        Stack<std::string, 40> stringStack; // �ɴ洢40��stringԪ�ص�ջ
        
        // ʹ�ÿɴ洢20��intԪ�ص�ջ
        for (int i = 0; i < 20; i++)
        {
            int20Stack.push (i);
        }
        
        std::cout << int20Stack.top() << std::endl;    //19
        int20Stack.pop();
        // ʹ�ÿɴ洢40��string��ջ
        stringStack.push ("hello");
        std::cout << stringStack.top() << std::endl;    //hello
        stringStack.pop();
        stringStack.pop();    //Exception: Stack<>::pop<>: empty stack
        return 0;
    }
    
    catch (std::exception const& ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;  // �˳���������ERROR���
    }
}