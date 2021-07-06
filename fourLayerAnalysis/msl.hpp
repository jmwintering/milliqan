//========================================================================================================================
// MSL NAMESPACE
//========================================================================================================================

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


namespace msl
{

	// returns only unique values in the std::vector
	template<class Y>
	std::vector<Y> set(std::vector<Y> inputVector){
		std::vector<Y> savedVector = {};
		for(int iElement = 0; iElement < inputVector.size(); iElement++){
			bool found = false;
			for(int iSaved = 0; iSaved < savedVector.size(); ++iSaved){
				if(inputVector[iElement] == savedVector[iSaved]) found = true;
			}
			if(!found) savedVector.push_back(inputVector[iElement]);
		}
		return savedVector;
	}

	// check if value is in std::vector
	template<class Z> 
	bool is_in(std::vector<Z> inputVector, Z value, bool isFloat = false, double eps = 0.001){
		for(auto elm: inputVector){
			if(isFloat){
				if(std::abs(value-elm) < eps) return true;
			}
			else{
				if(elm == value) return true;
			}
		}
		return false;	
	}

	// print vector elements as string
	template<class U> 
	string vectorString(std::vector<U> v){
		string s = "";
		for (int z=0u; z < v.size(); ++z){
			s += to_string(v[z]);
			if(z != v.size()-1) s += ", ";
		}
		return s;
	}
	
	// returns max element of vector
	template<class S> 
	double maxElement(std::vector<S> v){
		double max = -1e99;
		if(v.size() > 0) max = v[0];
		for(int i=0u; i < v.size(); ++i){
			if(v[i] > max) max = v[i];
		}
		return max;
	}

	// return min element of vector
	template<class R>
	double minElement(std::vector<R> v){
		double min = 1e99;
		if(v.size() > 0) min = v[0];
		for(int i=0u; i<v.size(); ++i){
			if(v[i] < min) min = v[i];
		}
		return min;
	}

	// converts string to TString
	TString tstring(string thing){
		TString thingT = thing;
		return thingT;
	}

	// checks if file exists
	bool file_exists(const std::string& name) {
		struct stat buffer;   
		return (stat (name.c_str(), &buffer) == 0); 
	}

	// sorts std::vector with thing[0] being smallest element
	template<class X> 
	std::vector<int> sort_vector(std::vector<X> thing){
		std::vector<int> temp_vector = {};
		std::vector<int> sorted_vector = {};
		int min = minElement(thing);
		int temp_min = min;
		temp_vector = thing;

		for(unsigned int j=0; j<thing.size(); ++j){
			temp_min = minElement(temp_vector);
			sorted_vector.push_back(temp_min);
			for(unsigned int i=0; i<temp_vector.size(); ++i){
				if(temp_vector[i] == temp_min){
					temp_vector.erase(temp_vector.begin()+i);
					break;
				}
			}
		}
		return sorted_vector;
	}

	// add debug statement
	void debug(bool flag, int line){ 
		if(flag) cout << BOLDRED << "line " << line << " " << RESET << endl;
	}

	// poisson rate error
	double poissRateError(double a, double b, double epsilon = 0.000000000001){
		double error = 0;
		double diff = b - 0;
		if((diff > epsilon) && (-diff < -1*epsilon)) {
			error = sqrt( ((a)/(pow(b,2)) ) + ( (pow(a,2)) / (pow(b,3))));
		}
		else{
			error = 0;
		}
		return error;
	}	

	void signalHandler(int signum){
		cout << "Interrupt signal (" << signum << ") received.\n" << std::flush;
		exit(signum);
	}	
	
} // end msl