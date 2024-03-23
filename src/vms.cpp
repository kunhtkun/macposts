#include "vms.h"
#include <cfloat>

using macposts::graph::Direction;

MNM_Link_Vms::MNM_Link_Vms (TInt ID, TInt link_ID, const macposts::Graph &graph)
{
  m_ID = ID;
  m_my_link_ID = link_ID;
  m_detour_link_ID = -1;
  m_compliance_ratio = TFlt (1);
  m_out_link_vec = std::vector<TInt> ();
  m_link_path_map = std::unordered_map<TInt, std::vector<MNM_Path *> *> ();
  hook_link (graph);
}

MNM_Link_Vms::~MNM_Link_Vms ()
{
  for (auto _it : m_link_path_map)
    {
      delete _it.second;
    }
  m_link_path_map.clear ();
  m_out_link_vec.clear ();
}

void
MNM_Link_Vms::hook_link (const macposts::Graph &graph)
{
  const auto &link = graph.get_link (m_my_link_ID);
  const auto &node = graph.get_endpoints (link).first;
  for (const auto &c : graph.connections (node, Direction::Outgoing))
    {
      m_out_link_vec.push_back (graph.get_id (c));
      std::vector<MNM_Path *> *_v = new std::vector<MNM_Path *> ();
      m_link_path_map.insert ({ graph.get_id (c), _v });
    }
}

int
MNM_Link_Vms::hook_path (Path_Table *path_table)
{
  for (auto _it : *path_table)
    {
      for (auto _it_it : *(_it.second))
        {
          for (MNM_Path *_path : _it_it.second->m_path_vec)
            {
              for (TInt _link_ID : m_out_link_vec)
                {
                  // printf("Current link is %d\n", _link_ID);
                  if (std::find (_path->m_link_vec.begin (),
                                 _path->m_link_vec.end (), _link_ID)
                      != _path->m_link_vec.end ())
                    {
                      m_link_path_map.find (_link_ID)->second->push_back (
                        _path);
                    }
                }
            }
        }
    }
  return 0;
}

TInt
MNM_Link_Vms::generate_detour_link (Path_Table *path_table,
                                    TInt next_assign_inter,
                                    MNM_Node_Factory *node_factory)
{
  TFlt _largest_diff = -DBL_MAX;
  TInt _largest_link = -1;
  TFlt _tmp_optimal, _tmp_real;
  TInt _link_ID;
  for (auto _map_it : m_link_path_map)
    {
      _link_ID = _map_it.first;
      _tmp_optimal = 0;
      _tmp_real = 0;
      for (MNM_Path *_path : *_map_it.second)
        {
          // printf("prabablity is %f\n", _path -> buffer[2] );
          _tmp_optimal
            += _path->m_buffer[2]
               * MNM::get_demand_bynode (_path->m_node_vec.front (),
                                         _path->m_node_vec.back (),
                                         next_assign_inter, node_factory);
          _tmp_real
            += _path->m_buffer[0]
               * MNM::get_demand_bynode (_path->m_node_vec.front (),
                                         _path->m_node_vec.back (),
                                         next_assign_inter, node_factory);
        }
      if (_tmp_optimal - _tmp_real > _largest_diff)
        {
          _largest_diff = _tmp_optimal - _tmp_real;
          _largest_link = _link_ID;
        }
    }
  if (_largest_link < 0)
    {
      throw std::runtime_error ("invalid state");
    }
  m_detour_link_ID = _largest_link;
  return _largest_link;
}

/**************************************************************************
                          VMS factory
**************************************************************************/

MNM_Vms_Factory::MNM_Vms_Factory ()
{
  m_link_vms_map = std::unordered_map<TInt, MNM_Link_Vms *> ();
}

MNM_Vms_Factory::~MNM_Vms_Factory ()
{
  for (auto _it : m_link_vms_map)
    {
      delete _it.second;
    }
  m_link_vms_map.clear ();
}

MNM_Link_Vms *
MNM_Vms_Factory::make_link_vms (TInt ID, TInt link_ID,
                                const macposts::Graph &graph)
{
  MNM_Link_Vms *_vms = new MNM_Link_Vms (ID, link_ID, graph);
  m_link_vms_map.insert (std::pair<TInt, MNM_Link_Vms *> (ID, _vms));
  return _vms;
}

MNM_Link_Vms *
MNM_Vms_Factory::get_link_vms (TInt ID)
{
  return m_link_vms_map.find (ID)->second;
}

int
MNM_Vms_Factory::hook_path (Path_Table *path_table)
{
  for (auto _it : m_link_vms_map)
    {
      _it.second->hook_path (path_table);
    }
  return 0;
}

namespace MNM
{
int
generate_vms_instructions (std::string file_name, MNM_Vms_Factory *vms_factory,
                           MNM_Link_Factory *link_factory)
{
  /* find file */
  std::ofstream _vms_info_file;
  _vms_info_file.open (file_name, std::ios::out);
  if (!_vms_info_file.is_open ())
    {
      throw std::runtime_error ("failed to open file: " + file_name);
    }

  std::string _info;
  MNM_Dlink *_link;
  std::string _str;
  for (auto _map_it : vms_factory->m_link_vms_map)
    {
      MNM_Link_Vms *_vms = _map_it.second;
      _link = link_factory->get_link (_vms->m_detour_link_ID);
      if (_link->m_ffs < TFlt (60.0 * 1600.0 / 3600.0))
        {
          _info = std::string ("Congestion ahead, take next exit to link ")
                  + std::to_string (_vms->m_detour_link_ID)
                  + "(The suggestion should be paired with DMS on the "
                    "arterials guiding drivers).\n";
        }
      else
        {
          if (_link->get_link_tt () > _link->m_length / _link->m_ffs)
            {
              _info = std::string ("Congestion ahead, ")
                      + std::to_string (
                        static_cast<int> (_link->get_link_tt () / 60))
                      + std::string (" minutes to get to next exit.\n");
            }
          else
            {
              _info = std::string ("Drive with care!\n");
            }
        }
      _str = std::to_string (_map_it.first) + " " + _info;
      // printf("%s\n", _str.c_str());
      _vms_info_file << _str;
    }
  _vms_info_file.close ();
  return 0;
}

}
