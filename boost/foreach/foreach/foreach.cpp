// foreach.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

// Copyright (c) 2015
// Author: Chrono Law

#include <iostream>
#include <string>

using namespace std;

#include <boost/foreach.hpp>
#include <boost/assign.hpp>

//////////////////////////////////////////

void case1()
{
    using namespace boost::assign;
    vector<int> v = (list_of (1), 12, 13, 14, 15);
    BOOST_FOREACH (auto x, v)
    {
        cout << x << ",";
    }
    cout << endl;
    string str ("boost foreach");
    BOOST_FOREACH (auto & c, str)
    {
        cout << c << "-";
    }
    cout << endl;
    set<int> s = list_of (10) (20) (30);
    int x;
    BOOST_FOREACH (x, s)
    {
        if (++x % 7 == 0)
        {
            cout << x << endl;
            break;
        }
    }
}

//////////////////////////////////////////

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

void case2()
{
    int ar[] = { 1, 2, 3, 4, 5 };
    
    foreach (auto& x, ar)
    { cout << x << " "; }
    
    cout << endl;
    map<int, string> m1 = { { 1, "111" }, { 2, "222" }, { 3, "333" } };
    m1.insert (pair<int, string> (4, "444"));
    
    foreach (auto& x, m1)
    { cout << x.first << x.second << endl; }

	map<int, string> m2 = { };
	m2.insert(pair<int, string>(101, "101101101"));

	m2.insert(pair<int, string>(102, "102102102"));

	m2.insert(pair<int, string>(103, "103103103"));

	m2.swap(m1);

	cout << "after swap" << endl;

	foreach(auto& x, m1)
	{
		cout << x.first << x.second << endl;
	}

	
	foreach(auto& x, m2)
	{
		cout << x.first << x.second << endl;
	}

	


    vector< vector<int> > v = { { 1, 2 }, { 3, 4 } };
    
    foreach (auto& row, v)
    {
        reverse_foreach (auto & z, row)
        cout << z << ",";
        cout << endl;
    }
}

//////////////////////////////////////////

#include <boost/array.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/unordered_set.hpp>

void case3()
{
    using namespace boost::assign;
    boost::array<int, 5> ar = (list_of (1), 2, 3, 4, 5);
    
    foreach (auto x, ar)
    { cout << x << " "; }
    
    cout << endl;
    pair<decltype (ar.begin()), decltype (ar.end()) >
    rng (ar.begin(), ar.end() - 2);
    
    foreach (auto x, rng)
    { cout << x << " "; }
    
    cout << endl;
    boost::circular_buffer<int> cb = list_of (1) (2) (3);
    
    foreach (auto x, cb)
    { cout << x << " "; }
    
    cout << endl;
    boost::unordered_set<double> us = list_of (3.14) (2.717) (0.618);
    
    foreach (auto x, us)
    { cout << x << " "; }
    
    cout << endl;
}
//////////////////////////////////////////

int main()
{
    case1();
    case2();
    case3();
}
