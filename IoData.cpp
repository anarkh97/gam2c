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
  prefix = "";
  filename = "";
  lower_limit = -DBL_MAX;
  upper_limit = DBL_MAX;
  objective_coeff = 0;
  inequalit_coeff = 0;
  two_inequalit_coeff = 0;
  equality_coeff = 0;
}

//------------------------------------------------------------------------------

Assigner*
DesignVariableData::getAssigner()
{

  ClassAssigner *ca = new ClassAssigner("normal", 10, nullAssigner);
  new ClassToken<DesignVariableData>(ca, "Type", this,
                                   reinterpret_cast<int DesignVariableData::*>(&DesignVariableData::type), 
                                   6, "RealContinuous", 0, "RealDiscrete", 1, "IntegerContinuous", 2, 
                                   "IntegerDiscrete", 3, "Boolean", 4, "None", 5);
  new ClassStr<DesignVariableData>(ca, "Name", this, &DesignVariableData::var_name);
  new ClassStr<DesignVariableData>(ca, "Prefix", this, &DesignVariableData::prefix);
  new ClassStr<DesignVariableData>(ca, "Filename", this, &DesignVariableData::filename);
  new ClassDouble<DesignVariableData>(ca, "LowerBound", this, &DesignVariableData::lower_limit); 
  new ClassDouble<DesignVariableData>(ca, "UpperBound", this, &DesignVariableData::upper_limit); 
  new ClassDouble<DesignVariableData>(ca, "ObjectiveCoefficient", this, &DesignVariableData::objective_coeff); 
  new ClassDouble<DesignVariableData>(ca, "InequalityCoefficient", this, &DesignVariableData::inequalit_coeff); 
  new ClassDouble<DesignVariableData>(ca, "TwoSideInequalityCoefficient", this, &DesignVariableData::two_inequalit_coeff); 
  new ClassDouble<DesignVariableData>(ca, "EqualityCoefficient", this, &DesignVariableData::equality_coeff); 
  
  return ca;

}

//------------------------------------------------------------------------------

ConstraintData::ConstraintData()
{
  nature = NON_LINEAR;
  type = NONE;
  con_name = "c";
  lower_limit = -DBL_MAX;
  upper_limit = 0; // most commonly used constraint c \leq 0
  equality_value = DBL_MAX;
  constraint_thickness = 0; 
}

//------------------------------------------------------------------------------

Assigner*
ConstraintData::getAssigner()
{

  ClassAssigner *ca = new ClassAssigner("normal", 7, nullAssigner);
  new ClassToken<ConstraintData>(ca, "Nature", this,
                                 reinterpret_cast<int ConstraintData::*>(&ConstraintData::nature), 
                                 2, "Linear", 0, "NonLinear", 1);
  new ClassToken<ConstraintData>(ca, "Type", this,
                                 reinterpret_cast<int ConstraintData::*>(&ConstraintData::type), 
                                 4, "Inequality", 0, "TwoSideInequality", 1, "Equality", 2, "None", 3);
  new ClassStr<ConstraintData>(ca, "Name", this, &ConstraintData::con_name);
  new ClassDouble<ConstraintData>(ca, "LowerBound", this, &ConstraintData::lower_limit);
  new ClassDouble<ConstraintData>(ca, "UpperBound", this, &ConstraintData::upper_limit);
  new ClassDouble<ConstraintData>(ca, "EqualBound", this, &ConstraintData::equality_value);
  new ClassDouble<ConstraintData>(ca, "EqualThickness", this, &ConstraintData::constraint_thickness);

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
  selector = NONE;
  tracker = NONE;
  initializer = NONE;
  crosser = NONE;
  mutator = NONE;
  print_each_pop = NO;

  mutation_rate = 0.08; //dakota default
  mutation_scale = 1; //dakota default

  crossover_rate = 0.75; //dakota default
  shuffle_parents = 2; //dakota default
  shuffle_offspring = 2; //dakota default

  population_size = 50; //dakota default
  seed = -1; //dakota randomizes if -1

  max_func_evals = 1000;
  max_iterations = 100;
  tracked_generations = 10;
  convergence_tol = 0; //checks if exact

  constraint_penalty = 1.0;
  final_solutions = 1;
}

//------------------------------------------------------------------------------

void
AlgorithmData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 12, father);
  new ClassToken<AlgorithmData>(ca, "Verbose", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::print_each_pop), 
                                   2, "No", 0, "Yes", 1);
  new ClassToken<AlgorithmData>(ca, "PrintEachPopulation", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::print_each_pop), 
                                   2, "No", 0, "Yes", 1);
  new ClassToken<AlgorithmData>(ca, "TrackFitness", this,
                                   reinterpret_cast<int AlgorithmData::*>(&AlgorithmData::track), 
                                   3, "Average", 0, "Best", 1, "None", 2);
  new ClassInt<AlgorithmData>(ca, "MaxFunctionEvaluations", this, &AlgorithmData::max_function_evals);
  new ClassInt<AlgorithmData>(ca, "MaxIterations", this, &AlgorithmData::max_iterations);
  new ClassInt<AlgorithmData>(ca, "TrackGenerations", this, &AlgorithmData::num_generations);
  new ClassDouble<AlgorithmData>(ca, "PercentageChange", this, &AlgorithmData::percent_change);
  new ClassDouble<AlgorithmData>(ca, "ConstraintPenalty", this, &AlgorithmData::constraint_penalty);
  new ClassDouble<AlgorithmData>(ca, "ConvergenceTolerance", this, &AlgorithmData::convergence_tolerance);

}

//------------------------------------------------------------------------------

InterfaceData::InterfaceData()
{
  type = SIMPLE;
  mode = SYNCHRONOUS;
  evaluation_concurrency = 1;
  directory_tag = "";
  in_file_pattern = "results.out.#";
  out_file_pattern = "params.in.#";
  exe_name = nullptr;
}

//------------------------------------------------------------------------------

void
InterfaceData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 6, father);
  new ClassInt<InterfaceData>(ca, "EvaluationConcurrency", this, 
                               &InterfaceData::evaluation_concurrency);
  new ClassToken<InterfaceData>(ca, "Type", this,
                                 reinterpret_cast<int InterfaceData::*>(&InterfaceData::type), 
                                 3, "Simple", 0, "External", 1, "Null", 2);
  new ClassToken<InterfaceData>(ca, "Mode", this,
                                 reinterpret_cast<int InterfaceData::*>(&InterfaceData::mode), 
                                 2, "Synchronous", 0, "Asynchronous", 1);
  new ClassStr<InterfaceData>(ca, "WorkDirectory", this, &InterfaceData::directory_tag);
  new ClassStr<InterfaceData>(ca, "Driver", this, &InterfaceData::exe_name);
  new ClassStr<InterfaceData>(ca, "ParamsPattern", this, &InterfaceData::in_file_pattern);
  new ClassStr<InterfaceData>(ca, "ResultPattern", this, &InterfaceData::out_file_pattern);
}

//------------------------------------------------------------------------------

SmartLearningData::SmartLearningData()
{
  metafile = "";
  prefix = "";
  suffix = "";

  // default mode is to calculate error labels
  computeMode = LEARNING;
  newDesignVariable = nullptr;
  numDesignVariables = 0;
  tolerance = 1e-3;
}

//------------------------------------------------------------------------------
void SmartLearningData::setup(const char *name, ClassAssigner *father)
{
  ClassAssigner *ca = new ClassAssigner(name, 9, father);

  new ClassToken<SmartLearningData> (ca, "ComputationMode", this,
    reinterpret_cast<int SmartLearningData::*>(&SmartLearningData::computeMode), 2,
    "Learning", 0, "Prediction", 1);

  new ClassStr<SmartLearningData> (ca, "MetaFile", this, &SmartLearningData::metafile);
  new ClassStr<SmartLearningData> (ca, "Prefix", this, &SmartLearningData::prefix);
  new ClassStr<SmartLearningData> (ca, "Suffix", this, &SmartLearningData::suffix);
  new ClassInt<SmartLearningData> (ca, "NumDesignVariables", this, &SmartLearningData::numDesignVariables);
  // new ClassInt<SmartLearningData> (ca, "NearestNeightbours", this, &SmartLearningData::numFriends);
  new ClassList<SmartLearningData> (ca, "DesignVector", this, &SmartLearningData::newDesignVariable);
  new ClassDouble<SmartLearningData> (ca, "ToleranceForLabels", this, &SmartLearningData::tolerance);

  transient_input.setup("TransientInputData", ca);
  output.setup("LabelOutput", ca);
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
