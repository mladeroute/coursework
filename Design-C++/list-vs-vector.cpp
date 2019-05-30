
#include <chrono>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <deque> 
#include <random>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <functional> 

using namespace std;
using my_engine = mt19937_64;
using my_distribution = uniform_int_distribution<uint64_t>;



const uint64_t SIZE = 300000;
const uint64_t MAX = SIZE*SIZE; 


/*
 * A function which generates a random number
 */
double my_random_number(uint64_t upper){


  my_engine engine {};
  engine.seed(chrono::system_clock::now().time_since_epoch().count());
  my_distribution long_range {0,(upper-1)};

  return long_range(engine);

}

/*
* A function which takes in a vector reference and function pointer to time the function
*/
template <typename T, typename R> 
double measure_ms(T& vals, R& result, void (*func)(T&, R&), void (*func2)(R&)){
//begin timer
  auto start = chrono::steady_clock::now();
//Run the function
  (*func)(vals, result);
//end timer
  auto end = chrono::steady_clock::now();
  auto diff = end - start;

//begin timer for removal function
start = chrono::steady_clock::now();

//Run the removal function
  (*func2)(result);
//end timer
  end = chrono::steady_clock::now();
  auto diff2 = end - start;
  auto diff_ms = chrono::duration<double, milli>(diff + diff2).count();

  return diff_ms;

}

/*
 * void print_cont(T& c1)
 * 
 * Function that prints the contents of a container
 */ 
template<typename T>
void print_cont(T& c1){ 
  cout << "Size: "<< c1.size() << endl; 
  cout << "Elements: "; 
  for(auto element: c1)
      cout << element << " "; 
  cout << "\n\n"; 

}
/*
 * A function that inserts the element into the appropriate place 
 *  
 * Type Requirements 
 * Type T must be a sequence container
 * Type S must be of the type stored by the sequence container passed in
 */
template<typename R , typename S> 
void simple_sort(R& result, S val){

      //Iterators used to carry out ordered insertion
      auto r_start = result.begin(); 
      auto r_mov = result.end();
      
      //Ensure a non-empty container 
      assert(!result.empty()); 

      //Ordered insertion 
      if (*(--r_mov) < val){ 
          result.push_back(val);  
      }else if (*r_start > val){ 
          result.insert(r_start, val);
      }else{
          while(*(--r_mov) > val) { }
          result.insert(++r_mov, val);  
      }
     

}


/* 
 * void insort(T& seq, R& result)
 * 
 * A function that takes in a container possesing a sequence of values and then inserts it into another container in it's natural ordering
 * at the time of insertion
 */ 
template<typename T, typename R>
void insort(T& seq, R& result){

      //Iterators for the sequence 
      auto s_start = seq.begin();  
      auto s_end = seq.end(); 

      //Ensure we have a non-empty container
      if (result.empty()){
          result.push_back(*(s_start++)); 
      }
      assert(!result.empty());

      //Begin the specialized insertion using the generic simple_sort() function
      while (s_start != s_end){
       simple_sort(result, *(s_start++));
      }
}


/* 
 * void pop_off(T& load)
 * 
 * A function which randomly picks an index and pops the value in the container off at that index. 
 */
template <typename T> 
 void pop_off (T& load){ 
      int j = 0; 
      while(load.size() > 1 ){
        int i = 0;
        int target = my_random_number((uint64_t) load.size());
        auto load_it = load.begin();

        while(i++ < target ) {++load_it; } 
        load.erase((load_it)); 
      }

       
      load.erase(load.begin());  
 }


/*
 * bool is_unique(T& v1)
 * 
 * Takes in a sequence container reference of type T and uses O(N) algorithm to check for duplicate values in the sequence
 * returns 0 if container is empty
 * returns 0 if duplicate is found
 * returns 1 if unique
 */
template <typename T> 
bool is_unique(T& v1){

      if (v1.empty()){ 
        return false; 
      }
      set<unsigned long long int> my_set; 
      for(auto element: v1){
        my_set.insert(element); 
      }

      if (my_set.size() < v1.size()){
        return false; 
      }else { 
        return true; 
      }
}

/*
 * vector<uint64_t> build_random_val(int count)
 * 
 * A function which builds a vector with count unique elements using C-style for loop
 */

vector<uint64_t> build_random_val(int count) {
  vector<uint64_t> temp; 
  temp.reserve(count); 
  for (int i = 0; i < count; i++){
    temp.emplace_back(my_random_number(count*count));
  }
  
   
  return temp;
}

/*
 * 
 * void generate_unique(T& data)
 * 
 * Generate a container filled with unique values
 */
template <typename T> 
void generate_unique(T& data, int size){ 
  
  while(!is_unique(data)){
    data = build_random_val(size); 
  }
}


/*
 * void map_insort(T& seq, R& result)
 * 
 * A function which places the elements of the sequence into the map using an O(lg N ) algorithm 
 */
template <typename T, typename R> 
void map_insort(T& seq, R& result){ 
   
  for(auto val: seq){ 
    result.insert( {val, val} );  
  }
}



/*
 * MAIN METHOD 
 */ 
int main() {
  int n = 100000; 
  while(n < SIZE){
    
    /* Output the size N of the containers we will be generating */ 
    cout << "\n N = " << n <<endl; 
    /* 
     * Construct 3 vectors which hold random values with a different seed for each random number 
     * Ensure these 3 vectors holding the unordered random numbers are unique
     * Continue with measurement testing
     */ 

    vector<uint64_t> data_1 = {};
    vector<uint64_t> data_2 = {};
    vector<uint64_t> data_3 = {}; 
  
    
    generate_unique(data_1, n); 
    generate_unique(data_2, n); 
    generate_unique(data_3, n); 


    vector<vector<uint64_t> *> ref_vec; 
    ref_vec.push_back(&data_1); 
    ref_vec.push_back(&data_2);
    ref_vec.push_back(&data_3);
    

    /* 
     * Begin the measurements
     */ 
  
    /* Call insort on the first vector using data as the elements to be inserted */
    {
      int i = 0;
      for(vector<uint64_t> * item: ref_vec){
        ++i; 
        vector<uint64_t> v1;
        //v1.reserve(SIZE); 
        cout << "Vector Trial " << i << endl;  
        cout << "Vector time in ms: " << measure_ms(*item, v1, insort, pop_off) << endl;
        
      }
    }

    cout << "\n\n"; 

    /* Call insort on list */ 
    {
      int i = 0;
      for(vector<uint64_t> * item: ref_vec){
        ++i; 
        list<uint64_t> l1 = {}; 
        cout << "List Trial " << i << endl;  
        cout << "List time in ms: " << measure_ms(*item, l1, insort, pop_off) << endl;
        
      }
    }

    cout << "\n\n"; 

    /* Call map_insort on map */ 
    {
      int i = 0;
      for(vector<uint64_t> * item: ref_vec){
        ++i; 
        map<uint64_t, uint64_t> my_map;  
        cout << "Map Trial " << i << endl;  
        cout << "Map time in ms: " << measure_ms(*item, my_map, map_insort, pop_off) << endl;
        
      }
    }

    cout << "\n\n"; 
    /* Alternative Experiment */ 

    /*Check the effects on the time for vector by reserving the memory space needed for N*/ 
    {
      int i = 0;
      for(vector<uint64_t> * item: ref_vec){
        ++i; 
        vector<uint64_t> v1;
        v1.reserve(n); 
        cout << "Experiment Vector Trial " << i << endl;  
        cout << "Experiment Vector time in ms: " << measure_ms(*item, v1, insort, pop_off) << endl;
        
      }
    }
    cout << "\n\n"; 
    /*Check the effects on the time for a deque*/ 
    {
      int i = 0;
      for(vector<uint64_t> * item: ref_vec){
        ++i; 
        deque<uint64_t> my_deque;
        cout << "Experiment Deque Trial " << i << endl;  
        cout << "Experiment Deque time in ms: " << measure_ms(*item, my_deque, insort, pop_off) << endl;
        
      }
    }
    
    n +=100000; 
  }
  return 0;
}
