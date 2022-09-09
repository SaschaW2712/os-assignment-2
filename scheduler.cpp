// a1234567, Stark, Tony
// a1773086, Ritossa, William
// a1799837, Williams, Sascha
// a1740459, Sparnon, Abigail
// very cool team
/*
created by Andrey Kan
andrey.kan@adelaide.edu.au
2021
*/
#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <chrono> // to measure run time

using namespace std;
using namespace std::chrono;

const int TIME_ALLOWANCE = 8;  // allow to use up to this number of time slots at once
const int PRINT_LOG = 0; // print detailed execution trace

class Customer
{
public:
    std::string name;
    int priority;
    int arrival_time;
    int slots_remaining; // how many time slots are still needed to fulfill their desired play time
    int playing_since;

    Customer(std::string par_name, int par_priority, int par_arrival_time, int par_slots_remaining)
    {
        name = par_name;
        priority = par_priority;
        arrival_time = par_arrival_time;
        slots_remaining = par_slots_remaining;
        playing_since = -1; // this is updated to the correct start time when the customer gets on the machine
    }
};

class Event
{
public:
    int event_time;
    int customer_id;  // each event involes exactly one customer

    Event(int par_event_time, int par_customer_id)
    {
        event_time = par_event_time;
        customer_id = par_customer_id;
    }
};


// Reads the input file line by line and initializes customers and arrival_events.
void initialize_system(
    std::ifstream &in_file,
    std::deque<Event> &arrival_events,
    std::vector<Customer> &customers)
{
    std::string name;
    int priority, arrival_time, slots_requested;

    // read input file line by line
    int customer_id = 0;
    while (in_file >> name >> priority >> arrival_time >> slots_requested)
    {
        Customer customer_from_file(name, priority, arrival_time, slots_requested);
        customers.push_back(customer_from_file);

        // new customer arrival event
        Event arrival_event(arrival_time, customer_id);
        arrival_events.push_back(arrival_event);

        customer_id++;
    }
}


// Prints the current state of the arcade machine to out_file.
// This is called at the end of each scheduler "time slot" loop iteration.
void print_state(
    std::ofstream &out_file,
    int current_time,
    int current_id,
    const std::deque<Event> &arrival_events,
    const std::deque<int> &customer_queue)
{
    out_file << current_time << " " << current_id << '\n';
    if (PRINT_LOG == 0)
    {
        return;
    }
    std::cout << current_time << ", " << current_id << '\n';
    for (int i = 0; i < arrival_events.size(); i++)
    {
        std::cout << "\t" << arrival_events[i].event_time << ", " << arrival_events[i].customer_id << ", ";
    }
    std::cout << '\n';
    for (int i = 0; i < customer_queue.size(); i++)
    {
        std::cout << "\t" << customer_queue[i] << ", ";
    }
    std::cout << '\n';
}

void abigails_superior_rrscheduler(){
    std::cout << "Scheduled B-)" <<std::endl;
    return;
}

void schedule_baseline(
    std::deque<Event> &arrival_events,
    std::vector<Customer> &customers,
    std::ofstream &out_file
) {
    int current_id = -1; // who is using the machine now, -1 means nobody
    int time_out = -1; // time when current customer will be preempted
    std::deque<int> queue; // waiting queue

    bool all_done = false;

    // step by step simulation of each time slot
    for (int current_time = 0; !all_done; current_time++)
    {
        // move newly arrived customers from arrivals to the queue
        while (!arrival_events.empty() && (current_time == arrival_events[0].event_time))
        {
            queue.push_back(arrival_events[0].customer_id);
            arrival_events.pop_front();
        }

        // check if we need to take a customer off the machine
        if (current_id >= 0)
        {
            if (current_time == time_out)
            {
                int last_run = current_time - customers[current_id].playing_since;
                customers[current_id].slots_remaining -= last_run;

                if (customers[current_id].slots_remaining > 0)
                {
                    // customer is not done yet, waiting for the next chance to play
                    queue.push_back(current_id);
                }

                current_id = -1; // the machine is free now
            }
        }

        // if machine is empty, schedule a new customer
        if (current_id == -1)
        {
            if (!queue.empty()) // is anyone waiting?
            {
                current_id = queue.front();
                queue.pop_front();

                if (TIME_ALLOWANCE > customers[current_id].slots_remaining)
                {
                    time_out = current_time + customers[current_id].slots_remaining;
                }
                else
                {
                    time_out = current_time + TIME_ALLOWANCE;
                }

                customers[current_id].playing_since = current_time;
            }
        }

        print_state(out_file, current_time, current_id, arrival_events, queue);

        // exit loop when there are no new arrivals, no waiting and no playing customers
        all_done = (arrival_events.empty() && queue.empty() && (current_id == -1));
    }
}

int main(int argc, char *argv[])
{
    // Get timepoint at start of algorithm
    auto start = high_resolution_clock::now();


    // Validate input files
    if (argc != 3)
    {
        std::cerr << "Provide input and output file names." << std::endl;
        return -1;
    }
    std::ifstream in_file(argv[1]);
    std::ofstream out_file(argv[2]);
    if ((!in_file) || (!out_file))
    {
        std::cerr << "Cannot open one of the files." << std::endl;
        return -1;
    }

    // Initialize the events and customers from the input file
    std::deque<Event> arrival_events; // new customer arrivals
    std::vector<Customer> customers; // information about each customer
    initialize_system(in_file, arrival_events, customers);


    // Schedule and output results using the baseline algorithm
    // IMPORTANT: to be replaced with the new algorithm
    schedule_baseline(arrival_events, customers, out_file);


    // Get timepoint at end of algorithm and calculate duration
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);

    if (duration.count() > 0)
    {
        auto durationMins = duration_cast<minutes>(stop - start);
        cout << "Run time exceeds 5 minutes (" << durationMins.count() << " mins)" << endl;
    } else {
        cout << "Run time: " << duration.count() << "s" << endl;
    }


    return 0;
}











