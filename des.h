using namespace std;

class SimResult{
  public:
    double avg_pckts_in_sys;

    SimResult(double avg_pckts_in_sys){
      this->avg_pckts_in_sys = avg_pckts_in_sys;
    };
};

std::ostream & operator<<(std::ostream & Str, const SimResult& result) {
  Str<<"average packets in the system: "<<result.avg_pckts_in_sys<<endl;
  return Str;
}

class DES{
  public:
    int Na = 0;
    int Nd = 0;
    int No = 0;
    int Nsys = 0;
    int idle_count = 0;
    int drops = 0;
    int gen = 0;

    void simulate_finite_queue(EventList* events, int link_rate, default_random_engine generator, exponential_distribution<double> packet_length_distribution, int queue_size){
      //finite queue
      double start, next_available = 0;
      int L;
      EventNode* temp = events->head;
      while(temp != NULL){
        //cout<<*(temp->event);
        switch(temp->event->event_type){
          case Arrival:
            if(this->Nsys == queue_size){
              //drop packet
              this->Nd += 1;
            } else {
              this->Na += 1;
              this->Nsys += 1;
              //schedule departure
              start = max(temp->event->time, next_available);
              L = round(packet_length_distribution(generator));
              //cout << " packet length is "<<L<<" bits";
              next_available = start + L/(double)link_rate;
              events->put_and_sort(new Event(next_available, Departure));
            }
            break;
          case Departure:
            this->Nd += 1;
            this->Nsys-=1;
            break;
          case Observer:
            this->No +=1;
            if(this->Nsys == 0){
              this->idle_count += 1;
            }
            break;
        }
        cout<<endl;
        temp = temp->next;
      }
    };

    //assumes all three queues are sorted
    EventNode* get_next_event(EventList* arrivals, EventList* observers, EventList* departures){
      //cout<<"next arrival: "<<arrivals->get_head_time();
      //cout<<", next observer: "<<observers->get_head_time();
      //cout<<", next departure: "<<departures->get_head_time();
      EventList* result_list = NULL;
      if(arrivals != NULL && arrivals->size > 0){
        result_list = arrivals;
      }
      if(observers != NULL && observers->size > 0 && (observers->get_head_time() < result_list->get_head_time())){
        result_list = observers;
      }
      if(departures != NULL && departures->size > 0 && (departures->get_head_time() < result_list->get_head_time())){
        result_list = departures;
      }
      if(result_list != NULL){
        return result_list->pop_head();
      } else {
        return NULL;
      }
    };

    SimResult* simulate_infinite_queue(EventList* arrivals, EventList* observers, int link_rate, default_random_engine generator, exponential_distribution<double> packet_length_distribution){
      EventList* departures = new EventList();
      int pckts_in_sys_sum = 0;
      //finite queue
      double start, next_available = 0;
      double mark = 0;
      int L;
      EventNode* temp = get_next_event(arrivals, observers, departures);
      while(temp != NULL){
        if(temp->event->time > mark+100){
          cout << mark << endl;
          cout.flush();
          mark += 10;
        }
        cout<<*(temp->event);
        switch(temp->event->event_type){
          case Arrival:
            this->Na += 1;
            //schedule departure
            start = max(temp->event->time, next_available);
            L = round(packet_length_distribution(generator));
            //cout << " packet length is "<<L<<" bits";
            next_available = start + L/(double)link_rate;
            departures->put(new Event(next_available, Departure));
            break;
          case Departure:
            this->Nd += 1;
            break;
          case Observer:
            this->No +=1;
            this->Nsys = Na - Nd;

            //cout << " Nsys is "<< Nsys;
            pckts_in_sys_sum += Nsys;
            if(this->Nsys == 0){
              this->idle_count += 1;
            }
            break;
        }
        cout<<endl;
        cout.flush();
        temp = get_next_event(arrivals, observers, departures);
      }
      cout<<"simulation finished"<<endl;
      cout.flush();
      return new SimResult(((double)pckts_in_sys_sum)/No);
    };
};
