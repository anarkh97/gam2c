/************************************************************************
 * Twins m2c IoData.
 ************************************************************************/

#ifndef _IO_DATA_H_
#define _IO_DATA_H_

#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include "parser/Assigner.h"
#include "parser/Dictionary.h"

/* not a good practice to specify namespaces in headers. */
using std::map;
using std::set;
using std::vector;

/*********************************************************************
 * class IoData reads and processes the input data provided by the user
 *********************************************************************
*/
//------------------------------------------------------------------------------

template<class DataType>
class ObjectMap {

public:
  map<int, DataType *> dataMap;

  void setup(const char *name, ClassAssigner *p) {
    SysMapObj<DataType> *smo = new SysMapObj<DataType>(name, &dataMap);
    if (p) p->addSmb(name, smo);
    else addSysSymbol(name, smo);
  }

  ~ObjectMap() {
    for(typename map<int, DataType *>::iterator it=dataMap.begin();it!=dataMap.end();++it)
      delete it->second;
  }
};

//------------------------------------------------------------------------------

struct DesignVariableData {

  enum Type {REAL_CONTINUOUS=0, REAL_DISCRETE, INTEGER_CONTINUOUS, 
             INTEGER_DISCRETE, STRING_DISCRETE, NONE} type;
  const char *var_name; //!< design variable name required by JEGA.
  double init_point; //!< initial value for this variable; if any.
  double upper_limit; //!< continuous variable upper limit; if any.
  double lower_limit; //!< continuous variable lower limit; if any.

  set<double> *real_discrete; //set of real values taken by this variable
  set<int> *int_discrete; //set of int values taken by this variable
  set<std::string> *string_discrete; //set of string values taken by this variable

  DesignVariableData();
  ~DesignVariableData() {
    if(real_discrete) delete real_discrete;
    if(int_discrete) delete int_discrete;
    if(string_discrete) delete string_discrete; 
  }

  Assigner *getAssigner();

};

//------------------------------------------------------------------------------

struct ParameterData {
  //AN: In future could be used to specify flat file for design space initialization
  const char *prefix; //!< prefix to file
  const char *filename; //!< file containing set of possible discrete values

  ObjectMap<DesignVariableData> variables;

  ParameterData();
  ~ParameterData() {};

  void setup(const char *, ClassAssigner * = 0);
};

//------------------------------------------------------------------------------

struct ConstraintData {

  //enum Nature {LINEAR = 0, NON_LINEAR} nature;
  enum Type {INEQUALITY = 0, EQUALITY, NONE} type;

  double upper_limit; //!< upper limit for one and two sided inequality constraint.
  double lower_limit; //!< lower limit for two sided inequality constraint.
  double equality_value; //!< value for equality constriant.
  double constraint_thickness; //!< constraint thickness to loosely impose equality constraint.
  const char *con_name; //!< constraint name required by JEGA.

  ConstraintData();
  ~ConstraintData() {};

  Assigner *getAssigner();
};

//------------------------------------------------------------------------------
//AN: we only care about SOGA which uses one objective function. So only label
//should be ok. The default fitness evaluator (merit_function) is used.
struct ResponseData {
  const char *objective_label;
  
  //AN: We can also communicate with driver script and our optimizer by
  //passing IoData to the optimizer directly and specifying a filename
  //that the optimizer should read in order to train/build the AI.

  //AN: for now  we use dakota's metadata to pass information from driver 
  //script to our ai model. This will be error values. 
  
  ObjectMap<ConstraintData> constraints;

  ResponseData();
  ~ResponseData() {};

  void setup(const char *, ClassAssigner * = 0);
};

//------------------------------------------------------------------------------

struct InterfaceData {
  enum EvaluationType {SYNCHRONOUS=0, ASYNCHRONOUS} mode;
  enum SchedulingType {STATIC=0, DYNAMIC} local_scheduling;
  int evaluation_concurrency;
  const char* directory_tag;
  const char* params_file_pattern;
  const char* result_file_pattern;
  const char* exe_name;

  InterfaceData();
  ~InterfaceData() {};

  void setup(const char *, ClassAssigner * = 0);
};

//------------------------------------------------------------------------------

struct EnvironmentData {

  const char *tab_data_file; //dakota's data is written here in tabular format
  const char *log_file; //dakota's output is redirected to this file
  const char *restart_file; //dakota's restart file.
  // AN: we check if restart file exists based on which we assign read/write flag.

  EnvironmentData();
  ~EnvironmentData() {};  

  void setup(const char *, ClassAssigner * = 0); 

};

//------------------------------------------------------------------------------

struct SmartLearningData {
  
  const char *feature_label; //used by metadata
  int num_neighbors; //number of design neighbors to interpolate from.

  // has info for nearest neighbor interpolation
  //vector<double> *newDesignVariable; // new design variable for doing predictions
  double tolerance; // tolerance for creating error labels

  SmartLearningData();
  ~SmartLearningData() { /*if(newDesignVariable) delete newDesignVariable;*/ };

  void setup(const char *, ClassAssigner * = 0);

};

//------------------------------------------------------------------------------

struct AlgorithmData {

  //AN: dakota infers the type of study based on response functions and handles
  //jega accordingly.

  enum Verbose {SILENT=0, NORMAL, DEBUG, QUIET, VERBOSE} verbose;
  enum Type {BASIC=0, SMART} type;
  enum Selector {DEFAULT_SELECTOR=0, ROULETTE, UNIQUE_ROULETTE, ELITIST, FAVOR_FEASIBLE} selector;
  enum Tracker {DEFAULT_TRACKER=0, AVG_FITNESS, BEST_FITNESS, NONE} tracker;
  enum Initializer {DEFAULT_INITIALIZER=0, SIMPLE_RANDOM, UNIQUE_RANDOM} initializer;  
  enum Crosser {DEFAULT_CROSSER=0, MULTI_POINT_BINARY, MULTI_POINT_PARAM_BINARY, 
                MULTI_POINT_REAL, SHUFFLE_RANDOM} crosser;
  enum Mutator {DEFAULT_MUTATOR=0, RANDOM_BIT, CAUCHY_OFFSET, NORMAL_OFFSET, UNIFORM_OFFSET,
                UNIFORM_REPLACE} mutator;
  
  enum YesNo {NO=0, YES} print_each_pop;

  SmartLearningData ml_model; //this will be used to specify ml characteristics

  //penalty for merit function
  double constraint_penalty;

  // mutation data
  double mutation_rate;
  double mutation_scale;

  // crossover data
  double crossover_rate;
  int shuffle_parents; //only required by shuffle random
  int shuffle_offspring; //only required by shuffle random
  int num_cross_points; //required by binary crossers

  //intializer data
  int population_size; //!< Initial population size
  int seed; //!< seed for random number generator.

  //convergence data
  int max_iterations;
  int max_func_evals;
  double convergence_tol; //relative to previous best
  double percent_change; //required for dakota's percent_change
  int tracked_generations; //number of generation check for percent change

  //output data
  int final_solutions; //number of best solutions given by dakota

  AlgorithmData();
  ~AlgorithmData() {};   

  void setup(const char *, ClassAssigner * = 0);

};

class IoData {

  char *cmdFileName;
  FILE *cmdFilePtr;

public:

  EnvironmentData env;
  AlgorithmData algo;
  InterfaceData driv;
  ParameterData param;
  ResponseData resp;

public:

  IoData() {}
  IoData(int, char**);
  ~IoData() {}

  void readCmdLine(int, char**);
  void setupCmdFileVariables();
  void readCmdFile();
  void finalize();

};
#endif
