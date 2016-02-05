#include <iomanip>
using namespace std;

class SimResult{
  public:
    double avg_pckts_in_sys;
    double p_idle;
    double p_loss;

    SimResult(double avg_pckts_in_sys, double p_idle, double p_loss){
      this->avg_pckts_in_sys = avg_pckts_in_sys;
      this->p_idle = p_idle;
      this->p_loss = p_loss;
    };
};

std::ostream & operator<<(std::ostream & Str, const SimResult& result) {
  Str<<"average packets in the system: "<<result.avg_pckts_in_sys<<endl;
  Str<<"the system is idle "<<result.p_idle * 100 <<"% of the time"<<endl;
  Str<< std::setprecision(5) <<"the system dropped "<<result.p_loss * 100 <<"% of the arrived packets"<<endl;
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

    //assumes all three queues are sorted
    EventNode* get_next_event(EventList* arrivals, EventList* observers, EventList* departures){
      EventList* result_list = NULL;
      double next_time = numeric_limits<double>::max();
      if(arrivals != NULL && arrivals->size > 0){
        result_list = arrivals;
        next_time = arrivals->get_head_time();
      }
      if(observers != NULL && observers->size > 0 && (observers->get_head_time() < next_time)){
        result_list = observers;
        next_time = observers->get_head_time();
      }
      if(departures != NULL && departures->size > 0 && (departures->get_head_time() < next_time)){
        result_list = departures;
        //don't need to update next_time
      }
      if(result_list != NULL){
        return result_list->pop_head();
      } else {
        return NULL;
      }
    };

    SimResult* simulate(EventList* arrivals, EventList* observers, int link_rate, default_random_engine generator, exponential_distribution<double> packet_length_distribution, double interval, bool finite_queue, int queue_size){
      EventList* departures = new EventList();
      double start, next_available = 0;
      double mark = 0, en = 0, fraction;
      int L;
      EventNode* temp = get_next_event(arrivals, observers, departures);
      while(temp != NULL){
        if(temp->event->time > mark+interval){
          cout << mark << "s finshed"<< endl;
          cout.flush();
          mark += interval;
        }
      //  cout<<*(temp->event);
        switch(temp->event->event_type){
          case Arrival:
            this->gen++;
            if(finite_queue && (this->Nsys == queue_size+1)){
              //drop packet
              this->drops += 1;
            } else {
              this->Na += 1;
              this->Nsys+=1;
              //schedule departure
              start = max(temp->event->time, next_available);
              L = round(packet_length_distribution(generator));
              //cout << " packet length is "<<L<<" bits";
              next_available = start + L/(double)link_rate;
              departures->put(new Event(next_available, Departure));
            }
            break;
          case Departure:
            this->Nd += 1;
            this->Nsys-=1;
            break;
          case Observer:
            this->No +=1;
            //cout<<"asd "<<Nsys<<endl;
            fraction = 1/(double)this->No;
            en = en * (1l-fraction) + this->Nsys * fraction;
            if(this->Nsys == 0){
              this->idle_count += 1;
            }
            break;
        }
        //cout<<endl;
        //cout.flush();
        temp = get_next_event(arrivals, observers, departures);
      }
      cout<<"simulation finished"<<endl;
      cout.flush();
      return new SimResult(en, (double)this->idle_count/this->No, (double)this->drops/this->gen);
    };

    SimResult* simulate_infinite_queue(EventList* arrivals, EventList* observers, int link_rate, default_random_engine generator, exponential_distribution<double> packet_length_distribution, double interval){
      EventList* departures = new EventList();
      double start, next_available = 0;
      double mark = 0, en = 0, fraction;
      int L;
      EventNode* temp = get_next_event(arrivals, observers, departures);
      while(temp != NULL){
        if(temp->event->time > mark+interval){
          cout << mark << "s finshed, current en "<<en<< endl;
          cout.flush();
          mark += interval;
        }
      //  cout<<*(temp->event);
        switch(temp->event->event_type){
          case Arrival:
            this->Na += 1;
            this->Nsys += 1;
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
            fraction = 1/(double)this->No;
            en = en * (1l-fraction) + Nsys * fraction;
            if(this->Nsys == 0){
              this->idle_count += 1;
            }
            break;
        }
        //cout<<endl;
        //cout.flush();
        temp = get_next_event(arrivals, observers, departures);
      }
      cout<<"simulation finished"<<endl;
      cout.flush();
      return new SimResult(en, (double)this->idle_count/this->No, 0l);
    };
};
