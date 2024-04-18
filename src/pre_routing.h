// implement a class pre_routing
#pragma once

#include "common.h"
#include "factory.h"
#include "path.h"
#include "pmc_table.h"
#include "shortest_path.h"
#include "statistics.h"
#include "ults.h"
#include "vehicle.h"

#include <typeinfo>
#include <unordered_map>

// using namespace std;
class MNM_Pre_Routing
{
public:
  MNM_Pre_Routing (Path_Table *m_path_table,
                   MNM_OD_Factory *od_factory); // different constructors, one
                                                // for first constructing,
  // MNM_Pre_Routing(Path_Table*);// *,MNM_OD_Factory *); /
  // MNM_Pre_Routing();
  // MNM_Pre_Routing(Path_Table *m_path_table,MNM_OD_Factory
  // *od_factory,PMC_table *pmc_table); //the second one for generating routing
  // based on PMC,
  ~MNM_Pre_Routing ();
  // int init_path(Path_Table *m_path_table,MNM_OD_Factory *od_factory);
  // //initial the first pre routing scheme
  Path_Table *m_path_table; // std::unordered_map<TInt, std::unordered_map<TInt,
                            // MNM_Pathset*>*>
  std::unordered_map<TInt,
                     std::unordered_map<TInt, std::unordered_map<TInt, TFlt *>>>
    *routing_table;
  int update_routing_table_MSA (
    MNM_PMC_Table pmc_table,
    TFlt lambda); // assume single-valued PMC, update routing table
  int update_routing_table_MSA1 (MNM_PMC_Table pmc_table_lower,
                                 MNM_PMC_Table pmc_table_upper, TFlt lambda);
  int update_routing_table_MSA2 (MNM_PMC_Table pmc_table_lower,
                                 MNM_PMC_Table pmc_table_upper, TFlt lambda);
  int update_demand (MNM_OD_Factory *od_factory);
  int test_function ();
  int reassign_routing (TInt oid, TInt did, TInt pid, TInt interval,
                        TFlt lambda);
  std::string toString ();

  // MNM_Pre_Routing operator+(MNM_Pre_Routing other); // plus other pouting
  // class, for MSA optimization MNM_Pre_Routing operator*(TFlt lambda); //
  // times the num of vehs
};

// namespace MNM {
// 	int
// routing_table_multiply(std::unordered_map<TInt,std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>>
// *routing_table, 		TFlt lambda);
// }
