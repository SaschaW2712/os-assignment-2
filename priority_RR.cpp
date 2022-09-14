//
// Created by kt on 9/09/22.
//
#include <iostream>
#include <vector>
#include <deque>

using namespace std;


bool first_response(Customer * customer)
{
    if(customer.playing_since <= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool small_time(Customer customer, int quantum)
{
    if(customer.slots_remaining <= quantum)
    {
        return true;
    }
    else
    {
        return false;
    }
}


 priority_queue(vector<Customer> &customers, int current, int quantum)
{
    vector<pair<int, Customer>> q;
    for(int i = 0; i < customers.size(); i++)
    {
        if(customers[i].slots_remaining>=0)
        {
            int priority = 0;
            if(first_response(customers[i]))
            {
                priority += 5;
            }
            if(small_time(customers[i], quantum))
            {
                priority += 4;
            }
            if(customers[i].priority == 1)
            {
                priority += 3;
            }
            q.push_back(make_pair(priority, customers[i]));

        }
    }
    sort(q.begin(), q.end());
    return q;
}

//
