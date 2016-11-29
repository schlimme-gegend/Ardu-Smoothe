This library can be used to calculate running averages of sensor input. 
It uses bit shifting to efficently divide the sum of the calculated 
samples. Due to this the library is limited to sample sizes that equal 
powers of two. The maximal sample size is 256 items. The minimal size 
is 2 (1 works as well but doesn't really make sense).

The class is using a template to define the type of data stored and the 
sample size like this:

Smoothe<int,16> sixteen_integers;
Smoothe<char,64> fourtysix_chars;

Note that other values than powers of two will not throw errors but 
will produce unpredictable results.

Class properties
----------------
bool dirty: Indicates if data has been appended since the last fetch.

Class methods
-------------
Constructor(bool initialize_raw_data = true): If no parameter is passed 
	the raw data will not initialized with zeroes. Passing a false will 
	prevent this.

void fill_with(T value): Writes the passedvalue to allfildsof raw data. 
	This can be used to initialize the array with an arbitrary value, 
	especially when chaining several instances, this is useful to speed up 
	the process in the first rounds of averaging:

	Smoothe<byte,16> collector;
	Smoothe<byte,16> aggregator;
	bool first_round = true;

	void loop(){
		...
		if(collector.current_index() == 0){
			if(first_round){
				aggregator.fill_with(collector.fetch());
			}
			else{
				aggregator.append(collector.fetch());
			}
		}
	}

	This way the long term aggregation starts from a value that is somewhat 
	related to the sensor output. 

void append(T value): This is used to add a sample point to the raw data 
	array. Takes a single argument of the type defined by the template.
	
T fetch(): This returns the sum of all raw data divided by sample size. 
	Division is implemented as right shifting which looks like strange 
	binary magic until on realizes that for decimal numbers division or 
	multiplication by powers of ten can be achieved by just moving the 
	decimal point left or right.

char current_index(): Returns the next position of the raw data array 
that will be written to.


Notes on sample size:
---------------------
The RAM of the Arduino ist very limited (2kB for the UNO) and a 
Smoothe<long,256> will eat half of this. To keep memory requirements 
small, it is is advisable to use the smallest data type that is 
suitable for the data you are expecting. 

Another way of keeping memory requirements in check is to chain 
together two or more instances of Smoothe:

Smoothe<byte,16> collector;
Smoothe<byte,16> aggregator;

void loop(){
	...
	collector.append(Whatever sensor data you want);
	...
	if(collector.current_index() == 0){
		aggregator.append(collector.fetch());
	}
	...
	if(aggregator.dirty){
		Do stuff;
	}
}

This way the aggregator is updated every time the collector has been 
completely filled. The above example allows one to calculate the 
running average of 256 samples while using only 32 bytes (plus some 
overhead for class variables). More than two instances can be chained, 
the sample sizes of the instances are multiplied.

Notes on booleans
-----------------
Although I'm not sure why anyone would want to do this, it is possible 
to run an average on booleans. 
Since 'true' is represented as 1 and integer math drops fractions, any 
array that is not exclusively filled with ones will be evaluated to 0 
after division. 
In order to correct for this, the data has to be cast to the smallest 
available numerical data type (usually byte) and multiplied by two 
before appending. 

bool bit;
Smoothe<byte,32> some_booleans;

void loop(){
	...
	some_booleans.append(byte(bit*2);
	...
	average = bool(some_booleans.fetch());
	...
}

Note that this method has a slight bias towards 'true', depending on the 
sample size. For four samples this looks like this:

raw data	average int	bool
{0,0,0,0}	0		0	false
{0,0,0,2}	0.5		0	false
{0,0,2,2}	1		1	true
{0,2,2,2}	1.5		1	true
{2,2,2,2}	2		2	true





