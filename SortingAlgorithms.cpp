/*----------------------------------------------------------------------------**
                Written by Matthew Benitez as a personal project.
  Program runs sorting algorithms on randomized vector of integers, calculates 
  runtime over user-specified number of runs on vector of user-specified size,
  then reports avg. runtime of each algorithm and total runtime over all runs.
  Program currently uses Bubble, Insertion, Selection, Merge, and Quick sorts.
**----------------------------------------------------------------------------*/

#include <iostream>
#include <math.h>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <cstring>

using namespace std;

/*----------------------------Parameters for Program--------------------------*/
// default values yield total program runtime of ~10 seconds.

// determines if initial randomized vector is to be printed (default: false)
bool wantToPrintInit = false;
// determines how many elements in array to be sorted (default: 5000)
int numbersToSort = 5000;       
// determines how many runs to do, reports avg. time (default: 15)
int runs = 15;              

// turns algorithms "on" or "off", 
// in order of Bubble, Insertion, Selection, Merge, Quick (default: 1 1 1 1 1)
bool runAlgs[] = {
/*B  I  S  M  Q */
  1, 1, 1, 1, 1
};
// helps for direct comparison of algorithms with vecSizes too large for others.

// special test mode to find vecSize where algs. become faster than one another.
// lower is starting point of vecSize (default: 100)
// upper is max cap of vecSize, where test terminates (default: 5000)
// grain is how much vecSize increments by on each iteration. (default: 100)
// testRuns is how many times each algorithm is run per vecSize. (default: 5)
//   higher value of testRuns helps to remove outlier data, but takes more time.
// test mode is also parameterized by wantToPrintInit, and runAlgs.
bool testMode = false;
int lower = 100;            
int upper = 5000;      
int grain = 100;            
int testRuns = 5;   

/*---------------------Vector and Housekeeping Functions----------------------*/
// overloaded printVector()
void printVector(vector<int> theVec){
  vector<int>::iterator start = theVec.begin();
  vector<int>::iterator finish = theVec.end();
  int index = 1;
  for(start; start!=finish; start++){
    cout << "[" << index++ << "]: " << *start << endl;
  }
}
void printVector(vector<string> theVec){
  vector<string>::iterator start = theVec.begin();
  vector<string>::iterator finish = theVec.end();
  int index = 1;
  for(start; start!=finish; start++){
    cout << "[" << index++ << "]: " << *start << endl;
  }
}
void printDivider(void){
  // standardized UI output 
  cout << "-------------------------------------------------------------------" << endl;
} // end printDivider()
vector<int> initializeVector(int vecSize){
  // creates a vector of size vecSize of rand ints in [1, vecSize*10].
  vector<int> theVec;                 
  int num;
  for(int i = 0; i < vecSize; i++){
    num = (rand() % (vecSize*10)+1); // insert a random number from the range [1,vecSize*10].
    theVec.push_back(num);
  }
  return theVec;
} // end initializeVector()
vector<string> runRace(double timeBub, double timeIns, double timeSel, double timeMer, double timeQck){
  // function to check and report which algorithms ran faster than others.
  vector<string> Places;
  vector<double> runtimes {timeBub, timeIns, timeSel, timeMer, timeQck};
  sort(runtimes.begin(), runtimes.end());
  while(!runtimes.empty()){
    // case where certain algorithms are not run, disregard them.
    if(runtimes.front() == 0){
      runtimes.erase(runtimes.begin());
      continue;
    }
    if(runtimes.front() == timeQck)
      Places.push_back("Quick Sort");
    if(runtimes.front() == timeMer)
      Places.push_back("Merge Sort");
    if(runtimes.front() == timeSel)
      Places.push_back("Selection Sort");
    if(runtimes.front() == timeIns)
      Places.push_back("Insertion Sort");
    if(runtimes.front() == timeBub)
      Places.push_back("Bubble Sort");
    runtimes.erase(runtimes.begin());
  }
  return Places;
} // end runRace();

/*-----------------------------Sorting Algorithms-----------------------------*/
void BubbleSort(vector<int> theArray){
  bool isSorted = false;
  int arrSize = theArray.size();
  while(!isSorted){
    isSorted = true;
    for(int i = 1; i < arrSize; i++){
      if(theArray[i] < theArray[i-1]){
        theArray[i-1] += theArray[i]; // swap two numbers without temp
        theArray[i] = theArray[i-1] - theArray[i];
        theArray[i-1] = theArray[i-1] - theArray[i];
        isSorted = false;
      } // end if
    } // end for
  } // end while
} // end BubbleSort()
void InsertionSort(vector<int> theArray){
  vector<int>::iterator start = theArray.begin();
  vector<int>::iterator finish = theArray.end();
  vector<int>::iterator place = start + 1; // begin at the 1st element instead of 0th
  vector<int>::iterator tempPlace;
  int element = 0;
  int x;
  int arrSize = theArray.size();
  for(place; place!= finish; place++){
    element = *place;
    tempPlace = place;
    while((tempPlace != start) && (element < *(tempPlace-1))){
      tempPlace--; // iterate left until previous element is less than current element.
    } // end while
    theArray.erase(place); // erase original instance of element
    theArray.insert(tempPlace, element); // reinsert element into correct place.
  } // end for
} // end InsertionSort()
void SelectionSort(vector<int> theArray){
  
  //in-place comparison sort, divides array into two parts--sorted on left and unsorted on right.
  //pushes minimum element from right into the left one at a time, then shifts left over one index.
  
  int length = theArray.size();
  int startIndex = 0;
  int temp = 0;
  int min = 0;
  int minIndex = 0;
  while(startIndex < length-1){
    min = theArray[length-1];
    minIndex = length-1;
    for(int i = startIndex; i < length - 1; i++){
      if(theArray[i] < min){
        min = theArray[i];
        minIndex = i;
      }
    }
    temp = theArray[startIndex];
    theArray[startIndex] = min;
    theArray[minIndex] = temp;
    startIndex++;
  }
} // end SelectionSort()
vector<int> Merge(vector<int> left, vector<int> right){ 
  // helper function for MergeSort, last step of divide-conquer-combine paradigm.
  vector<int> result;
  while((left.size() > 0) || (right.size() > 0)){ 
    if((left.size() > 0) && (right.size() > 0)){        // if either input array is empty
      if(left.front() <= right.front()){                // simply place the other array in.
        result.push_back(left.front());
        left.erase(left.begin());
      }else{ // end if 
        result.push_back(right.front());
        right.erase(right.begin());
      } // end else
    }else if(left.size() > 0){// end if
      result.push_back(left.front());
      left.erase(left.begin());
    }else{ // end else if
      result.push_back(right.front());
      right.erase(right.begin());
    }// else
  }// end while
  return result;
}// end Merge()
vector<int> MergeSort(vector<int> theArray){
  
  // split array in half until only one element left, then sort as arrays are merged
  
  int arrSize = theArray.size();
  int x;
  if(arrSize == 0 || arrSize == 1){
    return theArray;
  }else{ // end if
    int mid;
    vector<int> left, right;
    mid = floor(arrSize/2);
    vector<int>::iterator startL = theArray.begin();
    vector<int>::iterator startR = startL + mid;
    for(int i = 0; i < mid; i++){
      left.push_back(*startL++);
      right.push_back(*startR++);
    } // end for
    if(arrSize % 2 == 1){ // if odd number of elements
      right.push_back(*startR); // add extra element to right.
    } // end if
    left = MergeSort(left);
    right = MergeSort(right);
    return Merge(left, right);
  } // end else
} // end MergeSort()
void QuickSort(vector<int> theArray, int left, int right){
  // initialize index variables
  int leftArrow = left, rightArrow = right;
  // initialize pivot as int at the middle of the array.
  int pivot = theArray[(left + right)/2];
  // temp to be used when swapping ints
  int temp = 0; 
  // begin partitioning, terminating after index variables meet.
  while(leftArrow <= rightArrow){
    while(theArray[leftArrow] < pivot)
      leftArrow++;
    while(theArray[rightArrow] > pivot)
      rightArrow--;
    if(leftArrow <= rightArrow){
      // swap the two numbers and move over the indices.
      temp = theArray[leftArrow];
      theArray[leftArrow] = theArray[rightArrow];
      theArray[rightArrow] = temp;
      leftArrow++;
      rightArrow--;
    }
  }
  // perform recursion if the array changed.
  if(left < rightArrow){
    QuickSort(theArray, left, rightArrow);
  }
  if(leftArrow < right){
    QuickSort(theArray, leftArrow, right);
  }
} // end QuickSort();

/*-------------------------Special Tester Mode Function-----------------------*/

void runTest(int lower, int upper, int grain) {
  if(lower < 1) 
    lower = 100;
  // vec and enum keep track of which algorithms outperform others in "race"
  // keeps track of the "place" of each algorithm by assigning "runner number"
  vector<string> CurrentRun;
  vector<string> LastRun;
  
  printDivider();
  cout << "Test with parameters (lower, upper, grain): " << lower << ", " << upper << ", " << grain << "." <<endl;
  printDivider();
  for(int i = lower; i <= upper; i += grain){
    numbersToSort = i;
    // Initialize timer variables for each algorithm.
    double timeBub = 0, timeIns = 0, timeSel = 0, timeMer = 0, timeQck = 1, timeTot = 0;
    // Initialize the Vector with variable size.
    vector<int> myVec = initializeVector(numbersToSort); 
    if(wantToPrintInit){
       // Print out the initial vector.
      printVector(myVec);
    }

/*-------------------Run Sorting Algorithms and Store Times------------------*/
    for(int i = 0; i < testRuns; i++){
      auto startT = chrono::steady_clock::now();
       // by default, runtime zero unless alg is run.
      auto endBub = startT;
       // runs algo and stores time.
      if(runAlgs[0] == 1){
        BubbleSort(myVec);
        endBub = chrono::steady_clock::now();
      }
      auto endIns = endBub;
      if(runAlgs[1] == 1){
        InsertionSort(myVec);
        endIns = chrono::steady_clock::now();
      }
      auto endSel = endIns;
      if(runAlgs[2] == 1){
        SelectionSort(myVec);
        endSel = chrono::steady_clock::now();
      }
      auto endMer = endSel;
      if(runAlgs[3] == 1){
        MergeSort(myVec);
        endMer = chrono::steady_clock::now();
      }
      auto endQck = endMer;
      if(runAlgs[4] == 1){
        QuickSort(myVec, 0, numbersToSort-1);
        endQck = chrono::steady_clock::now();
      }
/*----------------------Calculate Runtime of Each Algorithm-------------------*/
      auto diffBub = endBub - startT;
      auto diffIns = endIns - endBub;
      auto diffSel = endSel - endIns;
      auto diffMer = endMer - endSel;
      auto diffQck = endQck - endMer;
      auto diffTot = endQck - startT;
      timeBub += chrono::duration <double, milli> (diffBub).count();
      timeIns += chrono::duration <double, milli> (diffIns).count();
      timeSel += chrono::duration <double, milli> (diffSel).count();
      timeMer += chrono::duration <double, milli> (diffMer).count();
      timeQck += chrono::duration <double, milli> (diffQck).count();
      
/*-------------------------------Print Key Points-----------------------------*/
    }// end for testRuns
    // No need to divide times by number of runs, just compare.
    CurrentRun = runRace(timeBub,timeIns,timeSel,timeMer,timeQck);
    // only print critical points, where the positions change.
    if(CurrentRun != LastRun){
      cout << "[" << numbersToSort << "]: " << endl;
      printVector(CurrentRun);
      LastRun = CurrentRun;
    }
  }// end for grain
  printDivider();
  cout << "Final Run (" << numbersToSort << "):" << endl;
  printDivider();
  printVector(CurrentRun);
  printDivider();
} // end runTest();

/*--------------------------------Begin Main()--------------------------------*/
int main() {
  if(testMode){
    runTest(lower, upper, grain);
    return 0;
  }
  // Initialize timer variables for each algorithm.
  double timeSel = 0, timeBub = 0, timeIns = 0, timeMer = 0, timeQck = 0, timeTot = 0;
  // Initialize the Vector with variable size.
  vector<int> myVec = initializeVector(numbersToSort); 

  printDivider();
  if(wantToPrintInit){
    cout << "[Initial Vector:]";
    printVector(myVec); // Print out the initial vector.
    printDivider();
  } // print initial vector of randomized ints.
  cout << "Running sorting algorithm(s) on a " << numbersToSort << "-element array " << runs << " time(s):" << endl;
  printDivider();
  
/*-------------------Run Sorting Algorithms and Store Times------------------*/
  for(int i = 0; i < runs; i++){
    auto startT = chrono::steady_clock::now();
    
     // by default, runtime zero unless algorithm is run.
    auto endBub = startT;
    if(runAlgs[0] == 1){
      BubbleSort(myVec);
      endBub = chrono::steady_clock::now();
    } // runs algorithm and stores time.
    
    auto endIns = endBub;
    if(runAlgs[1] == 1){
      InsertionSort(myVec);
      endIns = chrono::steady_clock::now();
    } // runs algorithm and stores time.
    
    auto endSel = endIns;
    if(runAlgs[2] == 1){
      SelectionSort(myVec);
      endSel = chrono::steady_clock::now();
    } // runs algorithm and stores time.
    
    auto endMer = endSel;
    if(runAlgs[3] == 1){
      MergeSort(myVec);
      endMer = chrono::steady_clock::now();
    } // runs algorithm and stores time.
    
    auto endQck = endMer;
    if(runAlgs[4] == 1){
      QuickSort(myVec, 0, numbersToSort-1);
      endQck = chrono::steady_clock::now();
    } // runs algorithm and stores time.
/*----------------------Calculate Runtime of Each Algorithm-------------------*/
    auto diffBub = endBub - startT;
    auto diffIns = endIns - endBub;
    auto diffSel = endSel - endIns;
    auto diffMer = endMer - endSel;
    auto diffQck = endQck - endMer;
    auto diffTot = endQck - startT;
    timeBub += chrono::duration <double, milli> (diffBub).count();
    timeIns += chrono::duration <double, milli> (diffIns).count();
    timeSel += chrono::duration <double, milli> (diffSel).count();
    timeMer += chrono::duration <double, milli> (diffMer).count();
    timeQck += chrono::duration <double, milli> (diffQck).count();
    timeTot += chrono::duration <double> (diffTot).count();
  } // main driver, run sorting algorithms.

/*---------------Calculate Runtime in Hours, Minutes, and Seconds-------------*/
  // hours = seconds / 3600
  double hourTot = floor(timeTot/3600); 
  // minutes = seconds / 60
  double minsTot = floor(timeTot/60);   
  // minutes after hours = mins % 60
  double hrmnTot = fmod(minsTot, (double) 60); 
  // seconds after minutes = seconds % 60
  double secsTot = fmod(timeTot,(double)60);   
  
/*--------------------------------Print Runtimes------------------------------*/
  cout << "Runtime for Bubble Sort:    " << timeBub/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Insertion Sort: " << timeIns/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Selection Sort: " << timeSel/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Merge Sort:     " << timeMer/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Quick Sort:     " << timeQck/runs << " milliseconds (10^-3)." << endl;
  printDivider();
  cout << "Above runtimes are averaged over the " << runs << " run(s)." << endl;
  cout << "Average runtime of 1 run:    " << (timeTot/runs) << " seconds." << endl;
  cout << "Total runtime over all runs: " << timeTot << " seconds"; // always print runtime in seconds.
  if(timeTot > 60 && timeTot < 3600){
    cout << "," << endl << "or " << minsTot << " minute(s) and " << secsTot << " second(s)";
  } // in the event runtime is over a minute but less than an hour.
  if(timeTot > 3600){ 
    cout << "," << endl << "or " << hourTot << " hour(s), " << hrmnTot << " minute(s) and " << secsTot << " second(s)";
  } // in the event runtime is over an hour.
  cout << "." << endl; 
  printDivider();
  // Print all output with correct formatting.
}

/*--------------------------------End of File---------------------------------*/
