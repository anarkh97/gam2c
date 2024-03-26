#include <DakotaModel.hpp>

#include <DkLibraryEnvironment.hpp>
#include <MyJEGAOptimizer.hpp>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <cassert>

namespace bfs = boost::filesystem;

void
DkLibraryEnvironment::update_db(const IoData &iod)
{

  // Create containers with default values
  Dakota::DataEnvironment den;
  Dakota::DataMethod dme;
  Dakota::DataModel dmo;
  Dakota::DataInterface dif;
  Dakota::DataVariables dvb;
  Dakota::DataResponses drp;

  const EnvironmentData &env = iod.env;
  const AlgorithmData &algo = iod.algo;
  const InterfaceData &driv = iod.driv;
  const ParameterData &param = iod.param;
  const ResponseData &resp = iod.resp;

  //AN: for our use world rank should always be 0
  //However, following dakota's library_mode.cpp just in case.
  Dakota::ParallelLibrary &parallel_lib = this->parallel_library();
  int world_rank = parallel_lib.world_rank();
  if(world_rank == 0) {
    Dakota::DataEnvironmentRep &denr = *den.data_rep();
    Dakota::DataMethodRep &dmer = *dme.data_rep();
    Dakota::DataModelRep &dmor = *dmo.data_rep();
    Dakota::DataInterfaceRep &difr = *dif.data_rep();
    Dakota::DataVariablesRep &dvbr = *dvb.data_rep();
    Dakota::DataResponsesRep &drpr = *drp.data_rep();

    // set environment properties
    denr.tabularDataFlag = true;
    denr.tabularDataFile = env.tab_data_file;
    denr.outputFile = env.log_file;
    denr.writeRestart = env.restart_file;

    // need to check if restart file exists
    bfs::path restart_file(std::string(env.restart_file));
    if(bfs::exists(restart_file)) {
      denr.readRestart = env.restart_file;
    }

    // set method properties
    dmer.methodName = Dakota::SOGA;
    switch(algo.verbose) {
      case AlgorithmData::SILENT: dmer.methodOutput = Dakota::SILENT_OUTPUT; break;
      case AlgorithmData::NORMAL: dmer.methodOutput = Dakota::NORMAL_OUTPUT; break;
      case AlgorithmData::DEBUG: dmer.methodOutput = Dakota::DEBUG_OUTPUT; break;
      case AlgorithmData::QUIET: dmer.methodOutput = Dakota::QUIET_OUTPUT; break;
      case AlgorithmData::VERBOSE: dmer.methodOutput = Dakota::VERBOSE_OUTPUT; break;
      default: dmer.methodOutput = Dakota::NORMAL_OUTPUT; break;
    } 
    dmer.randomSeed = algo.seed;
    dmer.maxIterations = algo.max_iterations;
    dmer.maxFunctionEvals = algo.max_func_evals;
    dmer.numFinalSolutions = algo.final_solutions;
    dmer.percentChange = algo.percent_change;
    dmer.numGenerations = algo.tracked_generations;
    dmer.convergenceTolerance = algo.convergence_tol;
    dmer.constraintPenalty = algo.constraint_penalty;
    dmer.fitnessType = "merit_function";
    switch(algo.tracker) {
      case AlgorithmData::AVG_FITNESS: dmer.convergenceType = "average_fitness_tracker"; break;
      case AlgorithmData::BEST_FITNESS: dmer.convergenceType = "best_fitness_tracker"; break;
      case AlgorithmData::DEFAULT_TRACKER:
        if(algo.verbose == AlgorithmData::VERBOSE) {
          Cout << "GAM2C: Assigning default fitness tracker (best fitness)."
               << std::endl;
        }
        dmer.convergenceType = "best_fitness_tracker";
        break;
    }
    switch(algo.selector) {
      case AlgorithmData::ROULETTE: dmer.replacementType = "roulette_wheel"; break;
      case AlgorithmData::UNIQUE_ROULETTE: dmer.replacementType = "unique_roulette_wheel"; break;
      case AlgorithmData::ELITIST: dmer.replacementType = "elitist"; break;
      case AlgorithmData::FAVOR_FEASIBLE: dmer.replacementType = "favor_feasible"; break;
      case AlgorithmData::DEFAULT_SELECTOR: 
        if(algo.verbose == AlgorithmData::VERBOSE) {
          Cout << "GAM2C: Assigning default selection operator (elitist)."
               << std::endl;
        }
        dmer.replacementType = "elitist"; 
        break;
    }
    switch(algo.initializer) {
      case AlgorithmData::SIMPLE_RANDOM: dmer.initializationType = "random"; break;
      case AlgorithmData::UNIQUE_RANDOM: dmer.initializationType = "unique_random"; break;
      case AlgorithmData::DEFAULT_INITIALIZER: 
        if(algo.verbose == AlgorithmData::VERBOSE) {
          Cout << "GAM2C: Assigning default initialization operator (unique random)."
               << std::endl;
        }
        dmer.initializationType = "unique_random"; 
        break;
    }
    dmer.populationSize = algo.population_size;
    switch(algo.crosser) {
      case AlgorithmData::MULTI_POINT_BINARY: dmer.crossoverType = "multi_point_binary"; break;
      case AlgorithmData::MULTI_POINT_PARAM_BINARY: dmer.crossoverType = "multi_point_parametrized_binary"; break;
      case AlgorithmData::MULTI_POINT_REAL: dmer.crossoverType = "multi_point_real"; break;
      case AlgorithmData::SHUFFLE_RANDOM: dmer.crossoverType = "shuffle_random"; break;
      case AlgorithmData::DEFAULT_CROSSER: 
        if(algo.verbose == AlgorithmData::VERBOSE) {
          Cout << "GAM2C: Assigning default crossover operator (random shuffle)."
               << std::endl;
        }
        dmer.crossoverType = "shuffle_random"; 
        break;
    }
    if(algo.crosser == AlgorithmData::MULTI_POINT_BINARY 
       || algo.crosser == AlgorithmData::MULTI_POINT_PARAM_BINARY 
       || algo.crosser == AlgorithmData::MULTI_POINT_REAL) {
      dmer.numCrossPoints = algo.num_cross_points;
    }
    else if(algo.crosser == AlgorithmData::SHUFFLE_RANDOM) {
      dmer.numParents = algo.shuffle_parents;
      dmer.numOffspring = algo.shuffle_offspring;
    }
    else {
      if(algo.verbose == AlgorithmData::VERBOSE) {
        Cout << "GAM2C: Using default number of parents (2) and offspring (2) for crossover."
             << std::endl;
      }
    }
    dmer.crossoverRate = algo.crossover_rate;
    switch(algo.mutator) {
      case AlgorithmData::RANDOM_BIT: dmer.mutationType = "bit_random"; break;
      case AlgorithmData::CAUCHY_OFFSET: dmer.mutationType = "offset_cauchy"; break;
      case AlgorithmData::NORMAL_OFFSET: dmer.mutationType = "offset_noraml"; break;
      case AlgorithmData::UNIFORM_OFFSET: dmer.mutationType = "offset_uniform"; break;
      case AlgorithmData::UNIFORM_REPLACE: dmer.mutationType = "replace_uniform"; break;
      case AlgorithmData::DEFAULT_MUTATOR: 
        if(algo.verbose == AlgorithmData::VERBOSE) {
          Cout << "GAM2C: Assigning default mutation operator (uniform replace)."
               << std::endl;
        }
        dmer.mutationType = "replace_uniform";
        break;
    }
    dmer.mutationRate = algo.mutation_rate;
    dmer.mutationScale = 1.0;
    dmer.printPopFlag = (algo.print_each_pop == AlgorithmData::YES);

    // set model properties
    dmor.modelType = "simulation"; //dakota default; but specifying here for reference

    // set interface properties
    difr.interfaceType = Dakota::FORK_INTERFACE;
    difr.fileTagFlag = true;
    difr.fileSaveFlag = true;
    difr.useWorkdir = true;
    difr.dirTag = true;
    difr.dirSave = true;
    difr.asynchFlag = (driv.mode == InterfaceData::ASYNCHRONOUS);
    if(difr.asynchFlag) difr.asynchLocalEvalConcurrency = driv.evaluation_concurrency;
    if(difr.asynchFlag) {
      difr.asynchLocalEvalScheduling = 
        (driv.local_scheduling == InterfaceData::STATIC) ? Dakota::STATIC_SCHEDULING : Dakota::DYNAMIC_SCHEDULING;
    }
    difr.workDir = driv.directory_tag;
    difr.parametersFile = driv.params_file_pattern;
    difr.resultsFile = driv.result_file_pattern;
    if(!driv.exe_name) {
      Cerr << "GAM2C: Error: analysis script not specified.\n\tBye!"
           << std::endl;
      abort_handler(Dakota::PARSE_ERROR);
    }
    difr.analysisDrivers.push_back(driv.exe_name);

    // set variable properties
    dvbr.varsView = Dakota::DESIGN_VIEW; // dakota infers; but we hard code to avoid conflicts.
    dvbr.varsDomain = Dakota::MIXED_DOMAIN; //ensure that discrete variables are treated that way; default for SOGA
    //AN: can specify initialization flat_file here 
    

    // set response properties
  }
/*
  // get the model attached to top method
  Dakota::Model &modelWithTopMethod = this->topLevelIterator.iterated_model();
  // get the communicator attached to environment
  Dakota::ParLevLIter w_pl_iter = parallelLib.w_parallel_level_iterator();
  // create our iterator
  std::shared_ptr<Dakota::Iterator> iter_rep = 
  	std::make_shared<MyJEGAOptimizer>(this->probDescDB, modelWithTopMethod);
  // assign to top method
  this->topLevelIterator.assign_rep(iter_rep);	
  this->topLevelIterator.init_communicators(w_pl_iter);
*/
}
