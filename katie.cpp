// a1767709 Kate Landon
// DRAFT??? I'm just trying to figure stuff out honestly

#include <iostream>
#include <fstream>
#include <deque>        //double ended queue
#include <vector>
#include <algorithm>

const int QUANTUM = 4; //value of the time quantum
const int PRINT_LOG = 0; //???? what does this do


using namespace std; //so i can be lazzzyyy

//************************************************
// PART 1: set up classes
//************************************************

// 1A: Customer class
//****************
class Customer
{
public:
    // FUNCTIONS ********

    Customer(string in_name = "c50", int in_prior = -1, int in_arrive = -1, int play_time = -1)
    {
        name = in_name;
        priority = in_prior;
        arrival_time = in_arrive;
        slots_remaining = play_time;
        playing_since = -1; //-1 bc haven't played yet

        //OWN CODE
        id = class_id;
        class_id++;
        playing = false;
        total_priority = 0;
    };
    void add_slot(int current_time = -1){
        slots_remaining --;
        //if given time, make it that, else just ++ old time

        if(current_time > -1)
        {
            playing_since = current_time;
        }
        else
        {
            playing_since++;
        }

    };

    // OBJECTS ********
    string name; //customer name
    int priority; // 1 = member, 0 = non-member
    int arrival_time;   // time they arrive
    int slots_remaining; //time left
    int playing_since; //time they first start playing
    bool playing; // NOT IN OG - says if playing or not

    int id; // NOT IN OG, may make event easier?
    int total_priority; //NOT IN OG, may make stuff easier?

private:
    static int class_id; //to set the customer id
};

int Customer::class_id = 0;


// 1B: Event class
//****************
class Event
{
public:
    int event_time; //time of day event takes place
    Customer * customer; //the customer?? maybe this would work better

    Event(int time, Customer *new_customer)
    {
        event_time = time;
        customer =  new_customer;
    };
};

//************************************************
// PART 2: set up input & output
// NOTE
//      file >> a >> b;
// equates to
//      file >> a;
//      file >> b;
//************************************************


void bubble_sort(vector<Customer*> v, bool isBiggerThan(Customer * a, Customer * b))
{
    for(int i = 0; i < v.size()-1; i++)
    {
        for(int j = 0; j < v.size()-i-1; j++)
        {
            if(isBiggerThan(v[j], v[j+1]))
            {
                Customer temp = *v[j];
                *v[j] = *v[j+1];
                *v[j+1] = temp;
            }
        }
    }
}

// 2A: parse input
//************************
void initialize_system(ifstream &in_file, deque<Event> &arrival_events, vector<Customer> &customers)
{
    string name;
    int priority, arrival_time, slots_requested;
    int position = 0;
    while(in_file >> name >> priority >> arrival_time >> slots_requested)
    {
        //create customer
        Customer * new_customer =  new Customer(name, priority, arrival_time, slots_requested);
        customers.push_back(*new_customer);
        //create arrival
        Event new_event(arrival_time, new_customer);
        arrival_events.push_back(new_event);

        position++;

    }

}

//2B : print output
void print_state(ofstream &out_file, int current_time, int current_id, const deque<Event> &arrival_events,  vector<Customer*> customer_queue)
{
    cout << current_time << " " << current_id << endl;
    out_file << current_time << " " << current_id << '\n';
    if (PRINT_LOG == 0)
    {
        return;
    }
    std::cout << current_time << ", " << current_id << '\n';
    for (int i = 0; i < arrival_events.size(); i++)
    {
        std::cout << "\t" << arrival_events[i].event_time << ", " << arrival_events[i].customer->id << ", ";
    }
    std::cout << '\n';
    for (int i = 0; i < customer_queue.size(); i++)
    {
        std::cout << "\t" << customer_queue[i]->id<< ", ";
    }
    std::cout << '\n';
}


//************************************************
// PART 3: queue time!!
//************************************************


//30: helper functions?
//********

//most at start, least at end
bool most_time(Customer* a, Customer* b)
{
    return a->slots_remaining > b->slots_remaining;
}

//least at start, most at end
bool least_wait(Customer* a, Customer* b)
{
    return a->arrival_time < b->arrival_time;

}

bool priority_sort(Customer * a, Customer * b)
{
    if(a->total_priority == b->total_priority)
    {
        if(a->arrival_time < b->arrival_time)
        {
            return true;
        }
        return false;
    }
    return a->total_priority > b->total_priority;
}

//****************
//3A: prioritiser
//****************
bool prioritise(vector<Customer*> customers, int current_time)
{
    cout << "PRIORITISE FUNCTION" << endl;
    cout << "\t  1: CHECK QUEUE EXISTS" << endl;
    if(customers.empty())
    {
        cout << "\t    NO QUEUE TO SORT" << endl;
        cout << "RESUME" << endl;
        return false;
    }

    cout << "\t  2: SORT BY LONGEST WAITING TIME" << endl;
    for(int i = 0; i < customers.size(); i++)
    {
        cout << "PRE" << i << " " << customers[i]->id << endl;
    }
//    sort(customers.begin(), customers.end(), least_wait);
    bubble_sort(customers, least_wait);

    for(int i = 0; i < customers.size(); i++)
    {
 //       cout << "\t     SETTING " <<  i << endl;
      //  int new_priority = current_time-customers[i]->arrival_time;
        int new_priority = i*(3/2) + (i%(3/2));
        if(customers[i]->slots_remaining <= QUANTUM)
        {
            new_priority += 130;
        }
        if((current_time-customers[i]->playing_since)>150)
        {
            new_priority += 8;
        }
        if(customers[i]->playing_since < 0)
        {
            new_priority += 28;
        }
        if(customers[i]->priority == 1)
        {
            new_priority += 7;
        }
        customers[i]->total_priority = new_priority;
    }
    cout << "\t  5: SORT FINAL PRIORITIES" << endl;
    bubble_sort(customers, priority_sort);
    cout << "RESUME" << endl;
    return true;
}

//****************
//3B: scheduler
//****************
bool schedule(deque<Event> &arrival_events, ofstream &out) {
    vector <Customer*> queue; // sorted queue
    Customer *current_customer = nullptr; //who is now on the machine, nullptr means no one
    int current_id = -1;
    int current_time = 0;

    int time_left = QUANTUM;

    while(current_time >= 0)
    {
        cout << "ROUND " << current_time << endl;
        cout << "  1: ADD ARRIVALS" << endl;
        while(!arrival_events.empty() && (current_time == arrival_events[0].event_time))
        {
            cout << "     ADDING ARRIVAL " << arrival_events[0].customer->id << endl;
            Customer * point = arrival_events[0].customer;
            queue.push_back(point);
            cout << "     ADDING QUEUE " << queue[queue.size()-1]->id << endl;
            arrival_events.pop_front();
        }
        cout << "  2: HANDLE CURRENT CUSTOMER IF PRESENT" << endl;
        if(current_customer != nullptr)
        {
            if(current_customer->slots_remaining <= 0)
            {
                cout << "    FINISHED CUSTOMER" << endl;
                free(current_customer);
                current_customer = nullptr;
                current_id = -1;
            }
            else
            {
                cout << "    CUSTOMER IS:" << endl;
                current_customer->playing = true;
                if(time_left <= 0)
                {
                    cout << "      LEAVING" << endl;
                    queue.push_back(current_customer);
                    time_left = QUANTUM;
                    current_customer = nullptr;
                }
                else
                {
                    time_left--;
                    current_customer->add_slot();
                    cout << "      STAYING." << endl;
                }
            }
        }
        if(current_customer == nullptr)
        {
            int size = 0;

            cout << "  3: ADD CUSTOMER IF NONE" << endl;
            if(!prioritise(queue, current_time))
            {
                cout << "     NO ONE IN QUEUE" << endl;
                current_id = -1;
                time_left = QUANTUM;
            }
            else
            {
                size = queue.size();
                cout << "     QUEUE HAS PEOPLE" << endl;
                current_customer = queue[queue.size()-1];
     if(queue.size()>2){           cout << "NEXT PRIOR" << queue[queue.size()-2]->total_priority << endl;}

                if(current_customer->id == current_id)
                {
                    cout << "     SAME CUSTOMER" << endl;
                    time_left = 1;
                }
                else
                {
                    cout << "     NEW CUSTOMER" << endl;
                    current_id = current_customer->id;
                    time_left = QUANTUM;
                }
                current_customer->add_slot(current_time);
                time_left--;
                queue.pop_back();
                cout << "        PRIORITY" << current_customer->total_priority << endl;


            }
        }
        print_state(out, current_time, current_id, arrival_events, queue);
        current_time++;
        cout << "  4: CHECK IF THERE ARE ANY CUSTOMERS STILL WAITING" << endl;
        if(arrival_events.empty() && (queue.empty() && (current_customer == nullptr)))
        {
            current_time = -10;
        }


    }

    return 1;

}



//FINAL : call main
//: get command line files
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Provide input and output file names." << endl;
        return -1;
    }
    ifstream in_file(argv[1]);
    ofstream out_file(argv[2]);
    if((!in_file) || (!out_file))
    {
        cerr << "Cannot open one of the files." << endl;
        return -1;
    }

    deque<Event> arrival_events; //new arrivals
    vector<Customer> customers; //customer info

    //NOTE: bc function uses &, no need for pointers
    initialize_system(in_file, arrival_events, customers);

    schedule(arrival_events, out_file);

    return 1;

}
