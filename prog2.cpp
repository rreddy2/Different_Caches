//Program2
#include<iostream>
#include<fstream>
#include<string>
#include <stdio.h>   
#include <stdlib.h>
#include<vector>
#include<sstream>
#include<math.h> 
#include<sys/time.h>
#include <chrono>
#include<ctime>
#include <iomanip>



using namespace std;

/*
	Direct Mapping
		cache line = 32 bytes
		cache size = 1 KB, 4 KB, 16 KB, 32 KB		
*/
class cacheLine
{
	public:
		bool validBit = false;
		long tag;
};

/*
	index offset does index through every line of cache
	tag offset does index through table index
	and tag is stored in the cache
*/
long DirectMapping(vector<string>instructions, vector<long>addresses, int cacheSize)
{
		int counter = 0;
		int cacheLineSize = 32;
		int numBlocks = 0;	
		
		long lineIndex = 0;
		long cacheIndex = 0;
		long blockAddress = 0;
		
		numBlocks  = cacheSize/cacheLineSize; 
		//two dimensional cache with *numBlocks* rows and *cacheLineSize* columns filled with 0s
		vector<vector<long> > cache(numBlocks, vector <long> (cacheLineSize, 0));

		for(int i = 0; i < addresses.size(); i++)
		{
			//figuring out cacheIndex and lineIndex using blockAddress
			blockAddress = addresses[i] / cacheLineSize;
			cacheIndex = blockAddress % numBlocks; 
			lineIndex = addresses[i] % cacheLineSize;

			//check whether its a hit or not. if its not, replace the cache table index with blockAddress
			if(cache[cacheIndex][lineIndex] == addresses[i])
			{
				counter++;	
			}
			else
			{
				for( int i = 0; i < cacheLineSize; i++)
				{
					cache[cacheIndex][i] = blockAddress * cacheLineSize+i;				
				}
			}
			
		}
	return counter;
}

/*
	similar to direct mapping but have to select between
	a couple indexes while using LRU
*/
long setAssociative(vector<string>instructions, vector<long>addresses, int numSets)
{		
	//using pre-existing libraries to have a time for every address
	std::chrono::high_resolution_clock::time_point startTime;
	startTime = chrono::high_resolution_clock::now();
	
	int counter = 0;
	int cacheSize = 16384;
	int cacheLineSize = 32;
	int numBlocks = cacheSize/cacheLineSize/numSets;	
		
	long lineIndex = 0;
	long cacheIndex = 0;
	long blockAddress = 0;
	int i,j,k;
	
	//a struct that can hold time and a long vector for the cacheLine
	struct cacheLru
	{	
		double time; //keeps track of when its last used
		vector<long> line;
	};
	
	struct cacheLru l;
	l.time = -1;
	l.line.resize(cacheLineSize,-1);
	std::chrono::high_resolution_clock::time_point start;
	
	//2 dimensional cache filled with cacheLru
	vector< vector<struct cacheLru> > cache(numSets, vector<struct cacheLru> (numBlocks, l));
	
	for(i = 0; i < addresses.size(); i++)
	{	
		//similar to direct mapping
		blockAddress = addresses[i] / cacheLineSize;
		cacheIndex = blockAddress % numBlocks;
		lineIndex = addresses[i] % cacheLineSize;
		bool hit = false;
		
		for(j = 0; j < cache.size(); j++)
		{
			//if it's a hit, then replace the time variable with current time so it can accurately be replaced if need be
			if(cache[j][cacheIndex].line[lineIndex] == addresses[i])
			{
				counter++;
				cache[j][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
				hit = true;
				break; 
			}
		}
		//if it's a cache miss
		if(hit == false)
		{
			double low = 999999999999999999; 
			int indexToRemove = -1;

			for(j = 0; j < cache.size(); j++)
			{
				//finding the lowest time so you can remove it
				if(cache[j][cacheIndex].time < low)
				{ 
					low = cache[j][cacheIndex].time;
					indexToRemove = j; 								
				}
			}
			//replace the lowest time with the current time
			cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
			
			//replace the lowest time with the blockAddress that will replace it.
			for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
			{
				cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
			}			
		}
	}
	return counter;
}

// same exact as set associative but instead of numSets use (cacheSize/cacheLineSize)
long fullyAssociativeLRU(vector<string>instructions, vector<long>addresses)
{
	std::chrono::high_resolution_clock::time_point startTime;
	startTime = chrono::high_resolution_clock::now();
	
	int counter = 0;
	int cacheSize = 16384;
	int cacheLineSize = 32;
	int numBlocks = cacheSize/cacheLineSize/(cacheSize/cacheLineSize);	
		
	long lineIndex = 0;
	long cacheIndex = 0;
	long blockAddress = 0;
	int i,j,k;
	
	struct cacheLru
	{	
		double time; //keeps track of when its last used
		vector<long> line;
	};
	
	struct cacheLru l;
	l.time = -1;
	l.line.resize(cacheLineSize,-1);
	std::chrono::high_resolution_clock::time_point start;
	
	vector< vector<struct cacheLru> > cache((cacheSize/cacheLineSize), vector<struct cacheLru> (numBlocks, l));
	
	for(i = 0; i < addresses.size(); i++)
	{	
		blockAddress = addresses[i] / cacheLineSize;
		cacheIndex = blockAddress % numBlocks;
		lineIndex = addresses[i] % cacheLineSize;
		bool hit = false;
		
		for(j = 0; j < cache.size(); j++)
		{
			if(cache[j][cacheIndex].line[lineIndex] == addresses[i])
			{
				counter++;
				cache[j][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
				hit = true; 
				break; 
			}
		}

		if(hit == false)
		{
			double low = 999999999999999999; 
			int indexToRemove = -1;

			for(j = 0; j < cache.size(); j++)
			{
				if(cache[j][cacheIndex].time < low)
				{ 
					low = cache[j][cacheIndex].time;
					indexToRemove = j; 								
				}
			}
			cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
 
			for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
			{
				cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
			}			
		}
	}
	return counter;


}	

//
long fullyAssociativeHotCold(vector<string>instructions, vector<long> addresses)
{	
	int counter = 0;
	int cacheSize = 16384;
	int cacheLineSize = 32;
	int numBlocks = cacheSize/cacheLineSize;	
		
	long lineIndex = 0;
	long cacheIndex = 0;
	long blockAddress = 0;
	int i,j,k;
	
	struct cacheLru
	{	
		double time; //keeps track of when its last used
		vector<long> line;
	};
	
	struct cacheLru l;
	l.time = 0;
	l.line.resize(cacheLineSize,-1);
	
	vector<int> tree(numBlocks-1,0);
	vector<struct cacheLru> cache(numBlocks, l);
	
	for(i = 0; i < addresses.size(); i++)
	{	
		blockAddress = (addresses[i]/cacheLineSize) * cacheLineSize;
		lineIndex = addresses[i] % cacheLineSize;
		bool hit = false;
		int hitIndex = -1;
		int treeIndex = -1;
		
		for(j = 0; j < cache.size(); j++)
		{
			if(cache[j].line[lineIndex] == addresses[i])
			{
				hitIndex = j;
				hit = true;
				break;
			}
		}
		
		if(hit == true)
		{
			counter++;
			treeIndex = numBlocks + hitIndex - 1;
			
			while(treeIndex != 0)
			{
				if(treeIndex % 2 != 0)
				{
					treeIndex = (treeIndex - 1)/2;
					tree[treeIndex] = 1;
				}
				else
				{
					treeIndex = (treeIndex - 2)/2;
					tree[treeIndex] = 0;
				}
			}
		}
		else
		{
			int remove = -1;
			treeIndex = 0;
			
			for(j = 0; j < log2(numBlocks); j++)
			{
				if(tree[treeIndex] == 1)
				{
					tree[treeIndex] = 0;
					treeIndex = (treeIndex*2) + 2;
				}
				else
				{
					tree[treeIndex] = 1;
					treeIndex = (treeIndex*2) + 1;
				}
			}
			remove = treeIndex - (numBlocks - 1);
			
			for(j = 0; j < cache[remove].line.size(); j++)
			{
				cache[remove].line[j] = blockAddress + j;
			}
		}
		
	}
	return counter;
	
}

//same as setAssociative but ignore misses for store
//change the if statement to check misses
long setAssociativeNoAllocation(vector<string>instructions, vector<long>addresses, int numSets)
{
	std::chrono::high_resolution_clock::time_point startTime;
	startTime = chrono::high_resolution_clock::now();
	
	int counter = 0;
	int cacheSize = 16384;
	int cacheLineSize = 32;
	int numBlocks = cacheSize/cacheLineSize/numSets;	
		
	long lineIndex = 0;
	long cacheIndex = 0;
	long blockAddress = 0;
	int i,j,k;
	
	struct cacheLru
	{	
		double time; //keeps track of when its last used
		vector<long> line;
	};
	
	struct cacheLru l;
	l.time = -1;
	l.line.resize(cacheLineSize,-1);
	std::chrono::high_resolution_clock::time_point start;
	
	vector< vector<struct cacheLru> > cache(numSets, vector<struct cacheLru> (numBlocks, l));
	
	for(i = 0; i < addresses.size(); i++)
	{	
		blockAddress = addresses[i] / cacheLineSize;
		cacheIndex = blockAddress % numBlocks;
		lineIndex = addresses[i] % cacheLineSize;
		bool hit = false;
		
		for(j = 0; j < cache.size(); j++)
		{
			if(cache[j][cacheIndex].line[lineIndex] == addresses[i])
			{
				counter++;
				cache[j][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
				hit = true; 
				break; 
			}
		}
		
		//the one line that is different from setAssociative function
		//because you have to ignore misses for stores
		if((hit == false) && (instructions[i] != "S"))	
		{
			double low = 999999999999999999; 
			int indexToRemove = -1;

			for(j = 0; j < cache.size(); j++)
			{
				if(cache[j][cacheIndex].time < low)
				{ 
					low = cache[j][cacheIndex].time;
					indexToRemove = j; 								
				}
			}
			cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
 
			for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
			{
				cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
			}			
		}
	}
	return counter;
}	

//once again similar to setAssociative but after processing everything
//have to include prefetching
long setAssociativenextLinePrefetching(vector<string>instructions, vector<long>addresses, int numSets)
{
	std::chrono::high_resolution_clock::time_point startTime;
	startTime = chrono::high_resolution_clock::now();
	
	int counter = 0;
	int cacheSize = 16384;
	int cacheLineSize = 32;
	int numBlocks = cacheSize/cacheLineSize/numSets;

	if(numSets == 2)
	{
		counter++;
	}
		
	long lineIndex = 0;
	long cacheIndex = 0;
	long blockAddress = 0;
	int i,j,k;
	
	struct cacheLru
	{	
		double time; //keeps track of when its last used
		vector<long> line;
	};
	
	struct cacheLru l;
	l.time = -1;
	l.line.resize(cacheLineSize,-1);
	std::chrono::high_resolution_clock::time_point start;
	
	vector< vector<struct cacheLru> > cache(numSets, vector<struct cacheLru> (numBlocks, l));
	
	for(i = 0; i < addresses.size(); i++)
	{	
		blockAddress = addresses[i] / cacheLineSize;
		cacheIndex = blockAddress % numBlocks;
		lineIndex = addresses[i] % cacheLineSize;
		bool hit = false;
		
		for(j = 0; j < cache.size(); j++)
		{
			if(cache[j][cacheIndex].line[lineIndex] == addresses[i])
			{
				counter++;
				cache[j][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
				hit = true; 
				break; 
			}
		}

		if(hit == false)
		{
			double low = 999999999999999999; 
			int indexToRemove = -1;

			for(j = 0; j < cache.size(); j++)
			{
				if(cache[j][cacheIndex].time < low)
				{ 
					low = cache[j][cacheIndex].time;
					indexToRemove = j; 								
				}
			}
			cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
 
			for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
			{
				cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
			}			
		}
		
		//same as setAssociative until the end where you take care of prefetching====================
		bool prefetch = false;
		//increment so we can look at the next line
		cacheIndex = (cacheIndex+1) % numBlocks;
		blockAddress++;
		
		//very similar to what we did above, do what you did with the first line to the second line as well
		for(j = 0; j < cache.size(); j++)
		{
			if(cache[j][cacheIndex].line[0] == blockAddress * cacheLineSize)
			{
				prefetch = true;
				cache[j][cacheIndex].time = std::chrono::duration<double,milli> (std::chrono::high_resolution_clock::now() - start).count();
				break;
			}
		}
		
		//copy and paste what happens if there's no hit
		if(prefetch == false)
		{
			double low = 999999999999999999; 
			int indexToRemove = -1;

			for(j = 0; j < cache.size(); j++)
			{
				if(cache[j][cacheIndex].time < low)
				{ 
					low = cache[j][cacheIndex].time;
					indexToRemove = j; 								
				}
			}
			cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
 
			for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
			{
				cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
			}				
		}
	}
	return counter;
}

//similar to setAssociativenextLinePrefetching but instead of after the setAssociative code
//the prefetching happens if there's a miss
long prefetchOnMiss(vector<string> instructions, vector<long>addresses, int numSets)
{
	std::chrono::high_resolution_clock::time_point startTime;
	startTime = chrono::high_resolution_clock::now();
	
	int counter = 0;
	int cacheSize = 16384;
	int cacheLineSize = 32;
	int numBlocks = cacheSize/cacheLineSize/numSets;
		
	long lineIndex = 0;
	long cacheIndex = 0;
	long blockAddress = 0;
	int i,j,k;
	
	struct cacheLru
	{	
		double time; //keeps track of when its last used
		vector<long> line;
	};
	
	struct cacheLru l;
	l.time = -1;
	l.line.resize(cacheLineSize,-1);
	std::chrono::high_resolution_clock::time_point start;
	
	vector< vector<struct cacheLru> > cache(numSets, vector<struct cacheLru> (numBlocks, l));
	
	for(i = 0; i < addresses.size(); i++)
	{	
		blockAddress = addresses[i] / cacheLineSize;
		cacheIndex = blockAddress % numBlocks;
		lineIndex = addresses[i] % cacheLineSize;
		bool hit = false;
		
		for(j = 0; j < cache.size(); j++)
		{
			if(cache[j][cacheIndex].line[lineIndex] == addresses[i])
			{
				counter++;
				cache[j][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
				hit = true; 
				break; 
			}
		}

		if(hit == false)
		{
			double low = 999999999999999999; 
			int indexToRemove = -1;

			for(j = 0; j < cache.size(); j++)
			{
				if(cache[j][cacheIndex].time < low)
				{ 
					low = cache[j][cacheIndex].time;
					indexToRemove = j; 								
				}
			}
			cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
 
			for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
			{
				cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
			}				
			//same code as prefetch above but this one is inside the miss because we only check
			//if there is a miss
			bool prefetch = false;
			cacheIndex = (cacheIndex+1) % numBlocks;
			blockAddress++;
		
			for(j = 0; j < cache.size(); j++)
			{
				if(cache[j][cacheIndex].line[0] == blockAddress * cacheLineSize)
				{
					prefetch = true;
					cache[j][cacheIndex].time = std::chrono::duration<double,milli> (std::chrono::high_resolution_clock::now() - start).count();
					break;
				}
			}
		
			//copy and paste what happens if there's no hit
			if(prefetch == false)
			{
				double low = 999999999999999999; 
				int indexToRemove = -1;

				for(j = 0; j < cache.size(); j++)
				{
					if(cache[j][cacheIndex].time < low)
					{ 
						low = cache[j][cacheIndex].time;
						indexToRemove = j; 								
					}
				}
				cache[indexToRemove][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
 
				for(k = 0; k < cache[indexToRemove][cacheIndex].line.size(); k++)
				{
					cache[indexToRemove][cacheIndex].line[k] = blockAddress * cacheLineSize + k;
				}				
			}
		}
	}
	return counter;

}

int main(int argc, char **argv)
{
	ifstream file;
	ofstream outputFile;
	int i = 0;
	string line;
	vector<string> instructions;
	vector<long> addresses;
	long convertedString;
	
	file.open(argv[1]);
	
	if(file.is_open())
	{
		while(getline(file,line))
		{
			instructions.push_back(line.substr(0,1));
			
			stringstream ss;
			ss << hex << line.substr(2,-1);
			ss >> convertedString;
			
			addresses.push_back(convertedString);
		}
	}
	else
	{
		cout << "Unable to open text file" << endl;
		exit(0);
	}

	long direct1 = DirectMapping(instructions, addresses, 1024);
	long direct4 = DirectMapping(instructions, addresses, 4096);
	long direct16 = DirectMapping(instructions, addresses, 16384);
	long direct32 = DirectMapping(instructions, addresses, 32768);
	
	long setAssociative2 = setAssociative(instructions, addresses, 2);
	long setAssociative4 = setAssociative(instructions, addresses, 4);
	long setAssociative8 = setAssociative(instructions, addresses, 8);
	long setAssociative16 = setAssociative(instructions, addresses, 16);
	
	long fullAssociative = fullyAssociativeLRU(instructions, addresses);
	
	long fullAssociativeHotCold = fullyAssociativeHotCold(instructions, addresses);
	
	long noAllocation2 = setAssociativeNoAllocation(instructions, addresses,2);
	long noAllocation4 = setAssociativeNoAllocation(instructions, addresses, 4);
	long noAllocation8 = setAssociativeNoAllocation(instructions, addresses, 8);
	long noAllocation16 = setAssociativeNoAllocation(instructions, addresses, 16);
	
	long prefetch2 = setAssociativenextLinePrefetching(instructions, addresses, 2);
	long prefetch4 = setAssociativenextLinePrefetching(instructions, addresses, 4);
	long prefetch8 = setAssociativenextLinePrefetching(instructions, addresses, 8);
	long prefetch16 = setAssociativenextLinePrefetching(instructions, addresses, 16);
	
	long prefetchMiss2 = prefetchOnMiss(instructions, addresses, 2);
	long prefetchMiss4 = prefetchOnMiss(instructions, addresses, 4);
	long prefetchMiss8 = prefetchOnMiss(instructions, addresses, 8);
	long prefetchMiss16 = prefetchOnMiss(instructions, addresses, 16);
		
	outputFile.open(argv[2]);
	
	if(outputFile.is_open())
	{
		outputFile << direct1 << "," << addresses.size() << "; " << direct4 << "," << addresses.size() << "; "
		 << direct16 << "," << addresses.size() << "; " << direct32 << "," << addresses.size() << ";" << endl;
	
		outputFile << setAssociative2 << "," << addresses.size() << "; " << setAssociative4 << "," << addresses.size() << "; "
		 << setAssociative8 << "," << addresses.size() << "; " << setAssociative16 << "," << addresses.size() << ";" << endl;
	
		outputFile << fullAssociative << "," << addresses.size() << "; " << endl;
	
		outputFile << fullAssociativeHotCold << "," << addresses.size() << "; " << endl;

		outputFile << noAllocation2 << "," << addresses.size() << "; " << noAllocation4 << "," << addresses.size() << "; "
		 << noAllocation8 << "," << addresses.size() << "; " << noAllocation16 << "," << addresses.size() << ";" << endl;
		
	
		outputFile << prefetch2 << "," << addresses.size() << "; " << prefetch4 << "," << addresses.size() << "; "
		 << prefetch8 << "," << addresses.size() << "; " << prefetch16 << "," << addresses.size() << ";" << endl;

			
		outputFile << prefetchMiss2 << "," << addresses.size() << "; " << prefetchMiss4 << "," << addresses.size() << "; "
		 << prefetchMiss8 << "," << addresses.size() << "; " << prefetchMiss16 << "," << addresses.size() << ";" << endl;
	}
	else
	{
		cout << "Unable to open file" << endl;
	}
	outputFile.close();
	
	
	
}
