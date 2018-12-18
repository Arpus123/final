#ifndef QUEUE_THREAD
#define QUEUE_THREAD
#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include <condition_variable>
using namespace std;

template <class T>
class Queue{
	size_t N;
	T* array;
	size_t put_index=0;
	size_t get_index=0;
	mutex m;
	condition_variable cv_put;
	condition_variable cv_get;
	public:
		void put (const T&);
		void get (T *);
		Queue(size_t _N=4):N{_N},array{new T[N]}{};
		~Queue(){delete [] array;};
		Queue & operator =(const Queue &)=delete;
		Queue (const Queue &)=delete;

	};

template <class T> 
void Queue<T>::put(const T& input){
	unique_lock <mutex> lck(m); //Lock el mutex
	while (!(put_index-get_index<N)){ //Mientras haya mas de N elementos entre los indices
		cv_put.wait(lck); //Espera dormida y libera el lck
		}
	array[put_index%N]=input; //Inserta el elemento
	put_index++; //sube el indice de put
	lck.unlock(); //desbloquea el mutex 
	cv_get.notify_one(); //Notifica a los que esperan para hacer get
	};

template <class T> 
void Queue<T>::get(T* output){
	unique_lock <mutex> lck(m); //Lock el mutex
	while (!(put_index>get_index)){ //mientras el indice de put no sea mayor que el de get
		cv_get.wait(lck); //Espera la señal
		}	
	*output=array[get_index%N]; 
	get_index++;
	lck.unlock();
	cv_put.notify_one(); //avisa a los que esperan un push
	};
#endif
