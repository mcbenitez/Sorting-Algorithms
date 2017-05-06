/*----------------------------------------------------------------------------**
                  Written by Matthew Benitez as a personal project.
  Program runs sorting algorithms on randomized vector of integers and checks
  by comparing vectors produced by each sorting algorithms and ensuring they match.
  Calculates runtime over user-specified number of runs on vector of user-specified 
  size, then reports average runtime of each algorithm and total runtime over all runs.
**----------------------------------------------------------------------------*/

#include <iostream>
#include <math.h>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

/*----------------------------Parameters for Program--------------------------*/
// default values yield total program runtime of ~10 seconds.

// determines if initial randomized vector is to be printed
bool wantToPrintInit = false; // default: false

// determines if output of each algorithm is to be printed
bool wantToPrint = false;     // default: false
// *WARNING* it's almost always better to leave this false to prevent hanging.
// *WARNING* this would not be suited for >300 elements.

// determines how many elements in array to be sorted
int numbersToSort = 500000;    // default: 5000     

// determines how many runs to do, reports avg. time
int runs = 10;               // default: 20

// turns algorithms "on" or "off", in order of Bub, Ins, Sel, Mer (default: 1 1 1 1)
bool runAlgs[] = {
/*B  I  S  M*/
  0, 0, 0, 1
};
// helps for direct comparison of certain algorithms with data sizes too large for others.

// special test mode to find the vecSize where algorithms become faster than one another.
// lower is starting point of vecSize (non-negative)
// upper is max cap of vecSize, where test terminates (recommend no more than 10000)
// grain is how much vecSize increments by on each iteration.
// testRuns is how many times each algorithm is run per vecSize.
// test mode is also parameterized by wantToPrintInit, and runAlgs.
bool testMode = false;    // default: false
int lower = 100;            // default: 100 (if < 1, defaults to 50).
int upper = 5000;           // default: 5000
int grain = 100;            // default: 100
int testRuns = 3;           // default: 5

/*---------------------Vector and Housekeeping Functions----------------------*/
void printVector(vector<int> theVec){
  vector<int>::iterator start = theVec.begin();
  vector<int>::iterator finish = theVec.end(); // iterate through vector and print each element.
  int i = 0;
  for(start; start != finish; start++){ // iterate through vector and print one at a time.
    cout << "[" << i+1 << "]" << ": " << *start << endl;
    i++;
  }
  cout << endl;
} // end printVector()
void printDivider(void){
  // standardized UI output 
  cout << "-------------------------------------------------------------------" << endl;
} // end printDivider()
bool checkMatches(vector<int> vec1, vector<int> vec2){
  bool checked = true;
  int i = 1;                                    // iterates through two vectors and checks
  vector<int>::iterator place1 = vec1.begin();  // each element against those of the other vector.
  vector<int>::iterator place2 = vec2.begin();  // if vectors are not of equal size, function will 
  vector<int>::iterator finish1 = vec1.end();   // iterate through the vector of smaller size.
  vector<int>::iterator finish2 = vec2.end();
  if(vec1.size() >= vec2.size()){
    for(place2; place2 != finish2; place2++){
      if(*place2 != *place1)
        cout << "Difference at [" << i << "]" << endl;
      place1++;
      i++;
    }// end for
  }else{ // end if
    for(place1; place1 != finish1; place1++){
      if(*place1 != *place2)
        cout << "Difference at [" << i << "]" << endl;
        checked = false;
      place2++;
      i++;
    }// end for
  }// end else
  return checked;
}// end checkMatches()
bool checkAllMatches(vector<vector<int>> allVecs){
  bool allMatch = true;
  vector<vector<int>>::iterator place = allVecs.begin();
  vector<vector<int>>::iterator finish = allVecs.end();
  vector<int> firstOne = *place;  // uses checkMatches() to check first vector against all others.
  place++;                        // somewhat rudimentary--checks for error but does not find where.
  for(place; place != finish; place++){
    if(checkMatches(firstOne, *place) == false){
      allMatch = false;
    }
  }
  return allMatch;
} // end checkAllMatches()
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

/*-----------------------------Sorting Algorithms-----------------------------*/
vector<int> BubbleSort(vector<int> theArray){
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
  return theArray;
} // end BubbleSort()
vector<int> InsertionSort(vector<int> theArray){
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
  return theArray;
} // end InsertionSort()
vector<int> SelectionSort(vector<int> theArray){
  
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
  return theArray;
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

/*-------------------------Special Tester Mode Function-----------------------*/
void runTest(int lower, int upper, int grain) {
  if(lower < 1) 
    lower = 50;
  bool MerBeatSel = 0, MerBeatIns = 0, MerBeatBub = 0,
       SelBeatMer = 0, SelBeatIns = 0, SelBeatBub = 0,
       InsBeatMer = 0, InsBeatSel = 0, InsBeatBub = 0,
       BubBeatMer = 0, BubBeatIns = 0, BubBeatSel = 0;
  bool firstRun = 1;
  
  printDivider();
  cout << "Test with parameters (lower, upper, grain): " << lower << ", " << upper << ", " << grain << "." <<endl;
  printDivider();
  for(int i = lower; i <= upper; i += grain){
    numbersToSort = i;
    double timeSel = 0, timeBub = 0, timeIns = 0, timeMer = 0, timeTot = 0; // Initialize timer variables for each algorithm.
    vector<int> myVec = initializeVector(numbersToSort); // Initialize the Vector with variable size.
    vector<int> myVecSel, myVecMer, myVecBub, myVecIns; // Declare output storage.
    if(wantToPrintInit){
      printVector(myVec); // Print out the initial vector.
    } // print initial vector of randomized ints.
  
/*-------------------Run Sorting Algorithms and Store Times------------------*/
    for(int i = 0; i < testRuns; i++){
      auto startT = chrono::steady_clock::now();
      
      auto endBub = startT; // by default, runtime zero unless alg is run.
      if(runAlgs[0] == 1){
        myVecBub = BubbleSort(myVec);
        endBub = chrono::steady_clock::now();
      } // runs algo and stores time.
      
      auto endIns = endBub; // by default, runtime zero unless alg is run.
      if(runAlgs[1] == 1){
        myVecIns = InsertionSort(myVec);
        endIns = chrono::steady_clock::now();
      } // runs algo and stores time.
      
      auto endSel = endIns; // by default, runtime zero unless alg is run.
      if(runAlgs[2] == 1){
        myVecSel = SelectionSort(myVec);
        endSel = chrono::steady_clock::now();
      } // runs algo and stores time.
      
      auto endMer = endSel; // by default, runtime zero unless alg is run.
      if(runAlgs[3] == 1){
        myVecMer = MergeSort(myVec);
        endMer = chrono::steady_clock::now();
      } // runs algo and stores time.
    
  /*----------------------Calculate Runtime of Each Algorithm-------------------*/
      auto diffBub = endBub - startT;
      auto diffIns = endIns - endBub;
      auto diffSel = endSel - endIns;
      auto diffMer = endMer - endSel;
      auto diffTot = endMer - startT;
      timeBub += chrono::duration <double, milli> (diffBub).count();
      timeIns += chrono::duration <double, milli> (diffIns).count();
      timeSel += chrono::duration <double, milli> (diffSel).count();
      timeMer += chrono::duration <double, milli> (diffMer).count();
      timeTot += chrono::duration <double> (diffTot).count();
    } // main driver, run sorting algorithms.
  
/*-------------------------------Print Key Points-----------------------------*/
    if(firstRun){
      cout << "Initial state: " << endl;
      if(timeBub < timeIns){
        cout << "Bubble sort beat Insertion Sort at vecSize " << numbersToSort <<   "." << endl;
      }
      if(timeBub < timeSel){
        cout << "Bubble sort beat Selection Sort at vecSize " << numbersToSort <<  "." << endl;
      }
      if(timeBub < timeMer){
        cout << "Bubble sort beat Merge Sort at vecSize " << numbersToSort <<   "." << endl;
      }
      if(timeIns < timeBub){
        cout << "Insertion sort beat Bubble Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeIns < timeSel){
        cout << "Insertion sort beat Selection Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeIns < timeMer){
        cout << "Insertion sort beat Merge Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeSel < timeBub){
        cout << "Selection sort beat Bubble Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeSel < timeIns){
        cout << "Selection sort beat Insertion Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeSel < timeMer){
        cout << "Selection sort beat Merge Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeMer < timeBub){
        cout << "Merge sort beat Bubble Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeMer < timeIns){
        cout << "Merge sort beat Insertion Sort at vecSize " << numbersToSort << "." << endl;
      }
      if(timeMer < timeSel){
        cout << "Merge sort beat Selection Sort at vecSize " << numbersToSort << "." << endl;
      }
      firstRun = 0;
      printDivider();
    } // on first run, check and print all placings.
    
    if((BubBeatIns == 0) && (timeBub < timeIns)){
      cout << "At vecsize " << numbersToSort << ", Bubble Sort beat Insertion Sort." << endl;
      BubBeatIns = 1;
    }
    if((BubBeatSel == 0) && (timeBub < timeSel)){
      cout << "At vecsize " << numbersToSort << ", Bubble Sort beat Selection Sort." << endl;
      BubBeatSel = 1;
    }
    if((BubBeatMer == 0) && (timeBub < timeMer)){
      cout << "At vecsize " << numbersToSort << ", Bubble Sort beat Merge Sort." << endl;
      BubBeatMer = 1;
    }
    
    if((InsBeatBub == 0) && (timeIns < timeBub)){
      cout << "At vecsize " << numbersToSort << ", Insertion Sort beat Bubble Sort." << endl;
      InsBeatBub = 1;
    }
    if((InsBeatSel == 0) && (timeIns < timeSel)){
      cout << "At vecsize " << numbersToSort << ", Insertion Sort beat Selection Sort." << endl;
      InsBeatSel = 1;
    }
    if((InsBeatMer == 0) && (timeIns < timeMer)){
      cout << "At vecsize " << numbersToSort << ", Insertion Sort beat Merge Sort." << endl;
      InsBeatMer = 1;
    }
    
    if((SelBeatBub == 0) && (timeSel < timeBub)){
      cout << "At vecsize " << numbersToSort << ", Selection Sort beat Bubble Sort." << endl;
      SelBeatBub = 1;
    }
    if((SelBeatIns == 0) && (timeSel < timeIns)){
      cout << "At vecsize " << numbersToSort << ", Selection Sort beat Insertion Sort" << endl;
      SelBeatIns = 1;
    }
    if((SelBeatMer == 0) && (timeSel < timeMer)){
      cout << "At vecsize " << numbersToSort << ", Selection Sort beat Merge Sort." << endl;
      SelBeatMer = 1;
    }
    
    if((MerBeatBub == 0) && (timeMer < timeBub)){
      cout << "At vecsize " << numbersToSort << ", Merge Sort beat Bubble Sort." << endl;
      MerBeatBub = 1;
    }
    if((MerBeatIns == 0) && (timeMer < timeIns)){
      cout << "At vecsize " << numbersToSort << ", Merge Sort beat Insertion Sort." << endl;
      MerBeatIns = 1;
    }
    if((MerBeatSel == 0) && (timeMer < timeSel)){
      cout << "At vecsize " << numbersToSort << ", Merge Sort beat Selection Sort." << endl;
      MerBeatSel = 1;
    }
  }
  printDivider();
  cout << "Program terminated properly at vecsize " << numbersToSort << "." << endl;
  printDivider();
} // end runTest()

/*--------------------------------Begin Main()--------------------------------*/
int main() {
  if(testMode){
    runTest(lower, upper, grain);
    return 0;
  }
  double timeSel = 0, timeBub = 0, timeIns = 0, timeMer = 0, timeTot = 0; // Initialize timer variables for each algorithm.
  vector<int> myVec = initializeVector(numbersToSort); // Initialize the Vector with variable size.
  vector<int> myVecSel, myVecMer, myVecBub, myVecIns; // Declare output storage.
  printDivider();
  cout << "Running sorting algorithm(s) over a " << numbersToSort << "-element array " << runs << " time(s):" << endl;
  printDivider();
  if(wantToPrintInit){
    printVector(myVec); // Print out the initial vector.
  } // print initial vector of randomized ints.
  
/*-------------------Run Sorting Algorithms and Store Times------------------*/
  for(int i = 0; i < runs; i++){
    auto startT = chrono::steady_clock::now();
    
    auto endBub = startT; // by default, runtime zero unless alg is run.
    if(runAlgs[0] == 1){
      myVecBub = BubbleSort(myVec);
      endBub = chrono::steady_clock::now();
    } // runs algo and stores time.
    
    auto endIns = endBub; // by default, runtime zero unless alg is run.
    if(runAlgs[1] == 1){
      myVecIns = InsertionSort(myVec);
      endIns = chrono::steady_clock::now();
    } // runs algo and stores time.
    
    auto endSel = endIns; // by default, runtime zero unless alg is run.
    if(runAlgs[2] == 1){
      myVecSel = SelectionSort(myVec);
      endSel = chrono::steady_clock::now();
    } // runs algo and stores time.
    
    auto endMer = endSel; // by default, runtime zero unless alg is run.
    if(runAlgs[3] == 1){
      myVecMer = MergeSort(myVec);
      endMer = chrono::steady_clock::now();
    } // runs algo and stores time.
  
/*----------------------Calculate Runtime of Each Algorithm-------------------*/
    auto diffBub = endBub - startT;
    auto diffIns = endIns - endBub;
    auto diffSel = endSel - endIns;
    auto diffMer = endMer - endSel;
    auto diffTot = endMer - startT;
    timeBub += chrono::duration <double, milli> (diffBub).count();
    timeIns += chrono::duration <double, milli> (diffIns).count();
    timeSel += chrono::duration <double, milli> (diffSel).count();
    timeMer += chrono::duration <double, milli> (diffMer).count();
    timeTot += chrono::duration <double> (diffTot).count();
  } // main driver, run sorting algorithms.

/*---------------Calculate Runtime in Hours, Minutes, and Seconds-------------*/
  double hourTot = floor(timeTot/3600); // hours = seconds / 3600
  double minsTot = floor(timeTot/60);   // minutes = seconds / 60
  double hrmnTot = fmod(minsTot, (double) 60); // minutes after hours = mins % 60
  double secsTot = fmod(timeTot,(double)60);   // seconds after minutes = seconds % 60
  
/*--------------------------------Print Runtimes------------------------------*/
  cout << "Runtime for Bubble Sort:     " << timeBub/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Insertion Sort:  " << timeIns/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Selection Sort:  " << timeSel/runs << " milliseconds (10^-3)." << endl;
  cout << "Runtime for Merge Sort:      " << timeMer/runs << " milliseconds (10^-3)." << endl;
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
  cout << "." << endl; // formatting.
  
/*--------------------------Print Outputs if Desired--------------------------*/
  if(wantToPrint){ // wantToPrint can be set at top to print or not.
    cout << "Bubble Sort:" << endl;
    printVector(myVecBub);
    cout << "Insertion Sort:" << endl;
    printVector(myVecIns);
    cout << "Selection Sort:" << endl;
    printVector(myVecSel);
    cout << "Merge Sort:" << endl;
    printVector(myVecMer);
  } // end if

/*------------------Prepare and Check That All Arrays Match-------------------*/
  vector<vector<int>> allMyVecs;
  allMyVecs.push_back(myVecBub);
  allMyVecs.push_back(myVecIns);
  allMyVecs.push_back(myVecSel);
  allMyVecs.push_back(myVecMer);

  if(checkAllMatches(allMyVecs)){
    cout << "All arrays match, algorithms succeded." << endl;
  }else{
    cout << "There is a problem! Algorithms failed!" << endl;
  }
  printDivider();
} // end main()

/*--------------------------------End of File---------------------------------*/
