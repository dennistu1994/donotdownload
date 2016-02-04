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
        std::cout<<*(temp->event)<<std::endl;
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
        temp = temp->next;
      }
    };
    
    void simulate_infinite_queue(EventList* events, int link_rate, default_random_engine generator, exponential_distribution<double> packet_length_distribution){
      //finite queue
      double start, next_available = 0;
      int L;
      EventNode* temp = events->head;
      while(temp != NULL){
        std::cout<<*(temp->event)<<std::endl;
        switch(temp->event->event_type){
          case Arrival:
            this->Na += 1;
            this->Nsys += 1;
            //schedule departure 
            start = max(temp->event->time, next_available);
            L = round(packet_length_distribution(generator));
            next_available = start + L/(double)link_rate;
            events->put_and_sort(new Event(next_available, Departure));
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
        temp = temp->next;
      }
    };
};