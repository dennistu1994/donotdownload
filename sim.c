#include <iostream>
#include <math.h>
#include <random>
#include "events.h"
#include <algorithm>

using namespace std;

void test_exp_dist(default_random_engine generator, exponential_distribution<double> distribution, double lambda, int trials){
  double sum = 0; 

  for(int i = 0; i<trials; i++){
    sum += distribution(generator);
  }
  double mean =  sum / trials;
  cout << "after 1000 trials with lambda = " << lambda << ", the mean is " << mean <<endl;
}

EventList* generate_event_list(double lambda, double T, EventType type, default_random_engine generator, exponential_distribution<double> distribution){
  EventList* result = new EventList();
  double next_time = distribution(generator);
  while(next_time < T){
    result->put(new Event(next_time, type));
    next_time += distribution(generator);
  }
  return result;
}

EventList* generate_departure_events(EventList* arrivals, int link_rate, default_random_engine generator, exponential_distribution<double> packet_length_distribution){
  //assuming infinite queue
  
  EventList* result = new EventList();
  double next_finish = 0;
  double start;
  int L;
  EventNode* temp = arrivals->head;
  while (temp != NULL){
    start = max(temp->event->time, next_finish);
    L = round(packet_length_distribution(generator));
    next_finish = start + L/(double)link_rate;
    result->put(new Event(next_finish, Departure));
    temp = temp->next;
  }
  return result;
}


int main()
{
  double lambda = 10l;
  int average_packet_length = 5000;
  int link_rate = 1000000;
  default_random_engine generator(random_device{}());
  exponential_distribution<double> distribution(lambda);
  exponential_distribution<double> packet_length_distribution(1l/(double)average_packet_length);
  test_exp_dist(generator, distribution, lambda, 1000000);
  double T = 3;
  //generate arrival events
  EventList* arrivals = generate_event_list(lambda, T, Arrival, generator, distribution);
  //generate departure events
  EventList* departures = generate_departure_events(arrivals, link_rate, generator, packet_length_distribution);
  //generate observer events
  EventList* observers = generate_event_list(lambda, T, Observer, generator, distribution);
  arrivals->put(departures);
  arrivals->put(observers);
  
  cout << *arrivals << endl<<endl;
  return 0;
}


