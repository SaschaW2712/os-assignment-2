// a1767709 Kate Landon
// DRAFT??? I'm just trying to figure stuff out honestly

#include <iostream>
#include <fstream>
#include <deque>        //double ended queue
#include <vector>
#include <algorithm>

using namespace std; //so i can be lazzzyyy

const int PRINT_LOG = 0; //???? what does this do
const int QUANTUM = 5; //value of the time quantum

// VALUES TO INCREASE PRIORITY BY
const int SMALL_TIME_REMAINS = 30;

const int MAX_BETWEEN = 220;
const int LONG_BETWEEN = 30;

const int MAX_WAIT = 200;
const int LONG_WAIT = 70;

const int NOT_YET_PLAYED = 28;

const int PRIORITY_CUSTOMER = 8;

// PRIORITISER FUNCTIONS - increases priority if ...
// higher in the queue (queue sorted based on arrival time)
int queue_position(int position, int size)
{
    return position;
}

// time_left (if smaller than quantum, increase priority)
int time_left(int time_left)
{
    int value = SMALL_TIME_REMAINS;
    return (time_left <= QUANTUM) ? value : 0;
}

// time_between (if large time between plays, increase priority)
int time_between(int time_between)
{
    int too_big = MAX_BETWEEN;
    int value = LONG_BETWEEN;
    return (time_between >= too_big) ? value : 0;
}

// waiting time (if got here a while a go and not done, increase priority)
int wait_time(int time_between)
{
    int too_big = MAX_WAIT;
    int value = LONG_WAIT;
    return (time_between >= too_big) ? value : 0;
}

// haven't played (if haven't played yet, increase priority)
int first_play(int last_play)
{
    int value = NOT_YET_PLAYED;
    return (last_play < 0) ? value : 0;
}

// priority customer (if a priority customer, increase priority)
int priority (int p)
{
    int value = PRIORITY_CUSTOMER;
    return (p == 0) ? value : 0;
}


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
// PART 3: set up sorting functions
// sorts queue using a bubble sort with a given comparator
// what we want depends on how we are using the vector at the moment
//************************************************

// SORT
// pushes first value (a) to back if comparator returns true
//************************
void bubble_sort(vector<Customer*> v, bool pushBack(Customer * a, Customer * b))
{
    for(int i = 0; i < v.size()-1; i++)
    {
        for(int j = 0; j < v.size()-i-1; j++)
        {
            if(pushBack(v[j], v[j+1]))
            {
                Customer temp = *v[j];
                *v[j] = *v[j+1];
                *v[j+1] = temp;
            }
        }
    }
}
//************************
//COMPARATORS
//************************

//************
//  most_time() (NOT USED)
// true if a has more time i.e. a is pushed back when has more time remaining
// in this case we consider
//          start of vector = start of queue
//************

bool most_time(Customer* a, Customer* b)
{
    return a->slots_remaining > b->slots_remaining;
}

//************
//  least_wait()
// true if a arrived earlier .e. a is pushed back if a came first
// we increase priority as we go through the queue so we say
// in this case we consider
//          start of vector = lowest priority
//************
bool least_wait(Customer* a, Customer* b)
{
    return a->arrival_time < b->arrival_time;
}

//************
//  priority_sort()
// true if a has more priority i.e. biggest priority value at end
//      if same, true if a arrived earlier i.e. earlier arrivals at end
// in this case we consider
//          end of vector = start of queue
//************

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

//************************************************
// PART 4: queue time!!
//************************************************

//****************
//4A: prioritiser
// steps:
// 1) check there is something in the queue, if not return false
// 2) sort from newest arrivals to latest
// 3) go through queue and assign priority (where larger value == higher priority)
// 4) sort from smallest to highest priority
// priority factors
//      - ARRIVAL POSITION: increase by position in queue sorted from newest to oldest
//      - TIME REMAINING: increase priority if less than the quantum
//      - TIME BETWEEN: increase priority if large time between plays
//      - WAIT TIME: increase priority if arrival time was a LONG time ago
//      - FIRST PLAY: increase priority if have not yet played
//      - PRIORITY: increase priority if customer is priority customer
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
    bubble_sort(customers, least_wait);

    for(int i = 0; i < customers.size(); i++)
    {
        int new_priority = queue_position(i, customers.size());
        new_priority += time_left(customers[i]->slots_remaining);
        new_priority += time_between(current_time - customers[i]->playing_since);
        new_priority += wait_time(current_time - customers[i]->arrival_time);
        new_priority += first_play(customers[i]->playing_since);
        new_priority += priority(customers[i]->priority);
        customers[i]->total_priority = new_priority;
    }
    cout << "\t  5: SORT FINAL PRIORITIES" << endl;
    bubble_sort(customers, priority_sort);
    cout << "RESUME" << endl;
    return true;
}

//****************
//4B: scheduler
// for each time event,
// 1) move new arrivals from arrivals to queue
// 2) handle customer on machine
// 3) put customer on machine if none there/no one longer there
// 4) print state to out
// 5) check if there are customers left/still to come
//      if not, return from scheduler
//      else, keep going!

// CASES FOR handling customer on machine
// 1 - they have satisified their requested play time
//      kick them off (resetting customer pointer & id)
// 2 - they have no time left
//      kick them off but remember them (reset pointer but not id)
//      add them back to the queue
// 3 - they have time left
//      let them play a session

// PROCESS for adding customer to machine
// 1) call prioritiser on the queue
//      if no queue, skip rest of process
// 2) add customer at end of queue to the machine
//      if same customer as before, give them 1 time block
//      if new customer, give them the full quantum time block
// 3) let customer play a session
// 4) remove customer from queue
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
            cout << "  3: ADD CUSTOMER IF NONE" << endl;
            if(!prioritise(queue, current_time))
            {
                cout << "     NO ONE IN QUEUE" << endl;
                current_id = -1;
                time_left = QUANTUM;
            }
            else
            {
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
