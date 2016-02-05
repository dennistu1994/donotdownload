#include <iostream>
#include <random>
using namespace std;


double get_exp_from_uni(double x, double lambda){
  return -log(1l-x)/lambda;
}

int main(){
  double lambda;
  cout<<"enter lambda"<<endl;
  cin >> lambda;
  default_random_engine generator(random_device{}()+rand());
  uniform_real_distribution<double> distribution(0.0, 1.0);
  double uni, rv, sum = 0;
  for(int i = 0; i < 1000; i++){
    uni = distribution(generator);
    rv = get_exp_from_uni(uni, lambda);
    sum += rv;
  }
  cout << "the average is " << sum/1000l<<endl;
  cout << "the inverse of the average is " << 1000l/sum<<endl;
};
