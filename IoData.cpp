/************************************************************************
 * Copyright © 2020 The Multiphysics Modeling and Computation (M2C) Lab
 * <kevin.wgy@gmail.com> <kevinw3@vt.edu>
 ************************************************************************/

#include <IoData.hpp>
#include <parser/Assigner.h>
#include <parser/Dictionary.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cfloat>
#include <climits>
#include <cmath>
#include <unistd.h>
#include <bits/stdc++.h> //INT_MAX
using namespace std;

RootClassAssigner *nullAssigner = new RootClassAssigner;

//------------------------------------------------------------------------------

DesignVariableData::DesignVariableData()
{
  type = NONE;
  var_name = "x";
  lower_limit = -DBL_MAX;
  upper_limit = DBL_MAX;
}

//------------------------------------------------------------------------------

Assigner*
DesignVariableData::getAssigner()
{

  ClassAssigner *ca = new ClassAssigner("normal", 7, nullAssigner);
  //Parse tokens
  new ClassToken<DesignVariableData>(ca, "Type", this,
                                   reinterpret_cast<int DesignVariableData::*>(&DesignVariableData::type), 
                                   6, "RealContinuous", 0, "RealDiscrete", 1, "IntegerContinuous", 2, 
                                   "IntegerDiscrete", 3, "StringDiscrete", 4, "None", 5);
  //Parse integrals
  //Parse doubles
  new ClassDouble<DesignVariableData>(ca, "LowerBound", this, &DesignVariableData::lower_limit); 
  new ClassDouble<DesignVariableData>(ca, "UpperBound", this, &DesignVariableData::upper_limit); 
  //Parse strings
  new ClassStr<DesignVariableData>(ca, "Name", this, &DesignVariableData::var_name);
  //Parse objects
  new ClassSet<DesignVariableData, int> (ca, "IntDiscrete", this, &DesignVariableData::int_discrete);
  new ClassSet<DesignVariableData, double> (ca, "RealDiscrete", this, &DesignVariableData::real_discrete);
  new ClassSet<DesignVariableData, string> (ca, "StringDiscrete", this, &DesignVariableData::string_discrete);

  return ca;

}

//------------------------------------------------------------------------------

ConstraintData::ConstraintData()
{
  //nature = NON_LINEAR;
  type = NONE;
  con_name = "c";
  lower_limit = -DBL_MAX;
  upper_limit = 0; //AN: most commonly used constraint c \leq 0
  equality_value = 0;
  constraint_thickness = 0; 
}

//------------------------------------------------------------------------------

Assigner*
ConstraintData::getAssigner()
{

  ClassAssigner *ca = new ClassAssigner("normal", 6, nullAssigner);
  //Parse tokens
  //new ClassToken<ConstraintData>(ca, "Nature", this,
  //                               reinterpret_cast<int ConstraintData::*>(&ConstraintData::nature), 
  //                               2, "Linear", 0, "NonLinear", 1);
  new ClassToken<ConstraintData>(ca, "Type", this,
                                 reinterpret_cast<int ConstraintData::*>(&ConstraintData::type), 
                                 4, "Inequality", 0, "Equality", 1, "None", 2);
  //Parse integrals
  //Parse doubles
  new ClassDouble<ConstraintData>(ca, "LowerBound", this, &ConstraintData::lower_limit);
  new ClassDouble<ConstraintData>(ca, "UpperBound", this, &ConstraintData::upper_limit);
  new ClassDouble<ConstraintData>(ca, "EqualBound", this, &ConstraintData::equality_value);
  new ClassDouble<ConstraintData>(ca, "EqualThickness", this, &ConstraintData::constraint_thickness);
  //Parse strings
  new ClassStr<ConstraintData>(ca, "Name", this, &ConstraintData::con_name);

  return ca;

}

//------------------------------------------------------------------------------

EnvironmentData::EnvironmentData()
{
  tab_data_file = "dakota.dat";
  log_file = "dakota.log";
  restart_file = "dakota.rst"; 
}

//------------------------------------------------------------------------------

void
EnvironmentData::setup(const char* name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 3, father);
  new ClassStr<EnvironmentData>(ca, "DataFileName", this, 
                                &EnvironmentData::tab_data_file);
  new ClassStr<EnvironmentData>(ca, "LogFileName", this, 
                                &EnvironmentData::log_file);
  new ClassStr<EnvironmentData>(ca, "RestartFileName", this, 
                                &EnvironmentData::restart_file);
}

//------------------------------------------------------------------------------

AlgorithmData::AlgorithmData()
{
  verbose = NORMAL;
  type = BASIC;
  selector = DEFAULT_SELECTOR;
  tracker = DEFAULT_TRACKER;
  initializer = DEFAULT_INITIALIZER;
  crosser = DEFAULT_CROSSER;
  mutator = DEFAULT_MUTATOR;
  print_each_pop = NO;

  mutation_rate = 0.08; //dakota default
  mutation_scale = 1; //dakota default

  crossover_rate = 0.75; //dakota default
  num_cross_points = 4; //dakota default
  shuffle_parents = 2; //dakota default
  shuffle_offspring = 2; //dakota default

  population_size = 50; //dakota default
  seed = 0; //dakota randomizes if 0

  max_func_evals = 1000;
  max_iterations = 100;
  tracked_generations = 10;
  percent_change = 0; //checks if exact
  convergence_tol = 1e-3;

  constraint_penalty = 1.0;
  final_solutions = 1;
}

//------------------------------------------------------------------------------

void
AlgorithmData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 23, father);
  //Parse tokens
  new ClassToken<AlgorithmData>(ca, "Verbose", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::verbose), 
                                   5, "Hush", 0, "Normal", 1, "Debug", 2, "Low", 3, "High", 4);
  new ClassToken<AlgorithmData>(ca, "Type", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::type), 
                                   2, "Basic", 0, "WithAI", 1);
  new ClassToken<AlgorithmData>(ca, "SelectionType", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::selector), 
                                   5, "Default", 0, "Roulette", 1, "UniqueRoulette", 2, "Elitist", 3,
                                   "FavorFeasible", 4);
  new ClassToken<AlgorithmData>(ca, "ConvergenceType", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::tracker), 
                                   3, "Default", 0, "Average", 1, "Best", 2);
  new ClassToken<AlgorithmData>(ca, "InitializationType", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::initializer), 
                                   3, "Default", 0, "Random", 1, "UniqueRandom", 2);
  new ClassToken<AlgorithmData>(ca, "CrossoverType", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::crosser), 
                                   5, "Default", 0, "MPBinary", 1, "MPParamBinary", 2, "MPReal", 3, "Shuffle", 4);
  new ClassToken<AlgorithmData>(ca, "MutationType", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::mutator), 
                                   6, "Default", 0, "BitRandom", 1, "OffsetCauchy", 2, "OffsetNormal", 3, 
                                   "OffsetUniform", 4, "ReplaceUniform", 5);
  new ClassToken<AlgorithmData>(ca, "PrintEachPopulation", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::print_each_pop), 
                                   2, "No", 0, "Yes", 1);
  //Parse integrals
  new ClassInt<AlgorithmData>(ca, "NumberOfParents", this, &AlgorithmData::shuffle_parents);
  new ClassInt<AlgorithmData>(ca, "NumberOfOffspring", this, &AlgorithmData::shuffle_offspring);
  new ClassInt<AlgorithmData>(ca, "PopulationSize", this, &AlgorithmData::population_size);
  new ClassInt<AlgorithmData>(ca, "NumberOfCrossPoints", this, &AlgorithmData::num_cross_points);
  new ClassInt<AlgorithmData>(ca, "Seed", this, &AlgorithmData::seed);
  new ClassInt<AlgorithmData>(ca, "MaxIterations", this, &AlgorithmData::max_iterations);
  new ClassInt<AlgorithmData>(ca, "MaxObjectiveEvals", this, &AlgorithmData::max_func_evals);
  new ClassInt<AlgorithmData>(ca, "NumberOfGensTracked", this, &AlgorithmData::tracked_generations);
  new ClassInt<AlgorithmData>(ca, "NumberOfFinalSolutions", this, &AlgorithmData::final_solutions);
  //Parse doubles
  new ClassDouble<AlgorithmData>(ca, "ConstraintPenalty", this, &AlgorithmData::constraint_penalty);
  new ClassDouble<AlgorithmData>(ca, "MutationRate", this, &AlgorithmData::mutation_rate);
  //AN: right now not allowing mutation scale updates.
  new ClassDouble<AlgorithmData>(ca, "CrossoverRate", this, &AlgorithmData::crossover_rate);
  new ClassDouble<AlgorithmData>(ca, "PercentageChange", this, &AlgorithmData::percent_change);
  new ClassDouble<AlgorithmData>(ca, "ConvergenceTol", this, &AlgorithmData::convergence_tol);
  //Parse strings

  //Parse objects
  ml_model.setup("SmartDesigner", ca);
}

//------------------------------------------------------------------------------

InterfaceData::InterfaceData()
{
  mode = SYNCHRONOUS;
  local_scheduling = STATIC;
  evaluation_concurrency = 1;
  directory_tag = "eval";
  //patterns handled by dakota; we just provide names right now
  params_file_pattern = "params.in";
  result_file_pattern = "results.out";
  exe_name = "";
}

//------------------------------------------------------------------------------

void
InterfaceData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 7, father);
  //Parse tokens
  new ClassToken<InterfaceData>(ca, "EvaluationType", this,
                                 reinterpret_cast<int InterfaceData::*>(&InterfaceData::mode), 
                                 2, "Synchronous", 0, "Asynchronous", 1);
  new ClassToken<InterfaceData>(ca, "SchedulingType", this,
                                 reinterpret_cast<int InterfaceData::*>(&InterfaceData::local_scheduling), 
                                 2, "WaitForAll", 0, "DontWaitForAll", 1);
  //Parse integrals
  new ClassInt<InterfaceData>(ca, "EvaluationConcurrency", this, 
                               &InterfaceData::evaluation_concurrency);
  //Parse double

  //Parse string
  new ClassStr<InterfaceData>(ca, "WorkDirectory", this, &InterfaceData::directory_tag);
  new ClassStr<InterfaceData>(ca, "AnalysisScript", this, &InterfaceData::exe_name);
  new ClassStr<InterfaceData>(ca, "ParamFileName", this, &InterfaceData::params_file_pattern);
  new ClassStr<InterfaceData>(ca, "ResultFileName", this, &InterfaceData::result_file_pattern);
  //Parse objects
}

//------------------------------------------------------------------------------

ParameterData::ParameterData()
{
  prefix = "";
  filename = "";
}

//------------------------------------------------------------------------------

void
ParameterData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 1, father);
  //AN: we can use prefix and filename to specify a file to read initial variables from
  variables.setup("DesignVariable", ca);
}

//------------------------------------------------------------------------------

ResponseData::ResponseData()
{
  objective_label = "objective";
  feature_label = "error";
}

//------------------------------------------------------------------------------

void
ResponseData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 3, father);

  //Parse tokens
  //Parse integrals
  //Parse doubles
  //Parse string
  ClassStr<ResponseData>(ca, "ObjectiveLabel", this, &ResponseData::objective_label);
  ClassStr<ResponseData>(ca, "FeatureLabel", this, &ResponseData::feature_label);
  //Parse object
  constraints.setup("Constraint", ca);
}

//------------------------------------------------------------------------------

SmartLearningData::SmartLearningData()
{
  num_neighbors = 3;
  tolerance = 1e-3;
}

//------------------------------------------------------------------------------

void 
SmartLearningData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 2, father);

  new ClassInt<SmartLearningData> (ca, "NearestNeightbours", this, &SmartLearningData::num_neighbors);
  new ClassDouble<SmartLearningData> (ca, "ToleranceForLabels", this, &SmartLearningData::tolerance);
}

//------------------------------------------------------------------------------

IoData::IoData(int argc, char** argv)
{
  readCmdLine(argc, argv);
  readCmdFile();
}

//------------------------------------------------------------------------------

void IoData::readCmdLine(int argc, char** argv)
{
  if(argc==1) {
    fprintf(stdout,"\033[0;31m*** Error: Input file not provided!\n\033[0m");
    exit(-1);
  }
  cmdFileName = argv[1];
}

//------------------------------------------------------------------------------

void IoData::readCmdFile()
{
  extern FILE *yyCmdfin;
  extern int yyCmdfparse();

  setupCmdFileVariables();
//  cmdFilePtr = freopen(cmdFileName, "r", stdin);
  yyCmdfin = cmdFilePtr = fopen(cmdFileName, "r");

  if (!cmdFilePtr) {
    fprintf(stdout,"\033[0;31m*** Error: could not open \'%s\'\n\033[0m", cmdFileName);
    exit(-1);
  }

  int error = yyCmdfparse();
  if (error) {
    fprintf(stdout,"\033[0;31m*** Error: command file contained parsing errors.\n\033[0m");
    exit(error);
  }
  fclose(cmdFilePtr);
}

//------------------------------------------------------------------------------
// This function is supposed to be called after creating any MPI communicator.
void IoData::finalize()
{

  //READ ADDITIONAL FILES

}

//------------------------------------------------------------------------------

void IoData::setupCmdFileVariables()
{
  env.setup("Environment");
  algo.setup("GeneticAlgorithm");
  driv.setup("Driver");
  param.setup("Parameters");
  resp.setup("Responses");
}

//------------------------------------------------------------------------------
