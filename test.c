#include <iostream>
#include <math.h>
#include <random>
#include "events.h"
#include "des.h"
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
  double rho = 0.25l;
  int average_packet_length = 12000;
  int link_rate = 1000000;
  double lambda = rho*link_rate/(double)average_packet_length;

  int queue_size = 100000;
  default_random_engine generator(random_device{}()+rand());
  exponential_distribution<double> distribution(lambda);
  exponential_distribution<double> packet_length_distribution(1l/(double)average_packet_length);
  test_exp_dist(generator, distribution, lambda, 1000000);
  double T = 1 ;
  //generate arrival events
  EventList* arrivals = generate_event_list(lambda, T, Arrival, generator, distribution);
  cout << "finished generating arrivals" << endl;
  cout.flush();
  //generate departure events
  //EventList* departures = generate_departure_events(arrivals, link_rate, generator, packet_length_distribution);
  //generate observer events
  generator.seed(random_device{}()+rand());
  EventList* observers = generate_event_list(lambda * 2, T, Observer, generator, distribution);
  cout << "finished generating observers" << endl;
  cout.flush();
  DES des;
  generator.seed(random_device{}());
  SimResult* result = des.simulate_infinite_queue(arrivals, observers, link_rate, generator, packet_length_distribution);
  //cout<<(*result)<<endl;
  return 0;
}
