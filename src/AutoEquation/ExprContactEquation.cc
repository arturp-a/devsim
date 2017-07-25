/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "ExprContactEquation.hh"
#include "Region.hh"

#include "ObjectCache.hh"
#include "ModelExprData.hh"

#include "ObjectHolder.hh"

#include <cmath>
#include <ostream>

template <typename DoubleType>
ExprContactEquation<DoubleType>::ExprContactEquation(
            const std::string &eq,// nodemodel
            const std::string &var, // variable
            ContactPtr c,
            RegionPtr  r,
            const std::string &nmi,// nodemodel
            const std::string &emi, // edgemodel
            const std::string &eemi, // elementedgemodel
            const std::string &nmc,// nodemodel
            const std::string &emc, // edgemodel
            const std::string &eemc, // elementedgemodel
            const std::string &nmq,// nodemodel
            const std::string &emq ,// edgemodel
            const std::string &eemq // elementedgemodel
            ) :  ContactEquation<DoubleType>(eq, var, c, r),
                 nodemodel_int(nmi),
                 edgemodel_int(emi),
                 elementedgemodel_int(eemi),
                 nodemodel_current(nmc),
                 edgemodel_current(emc),
                 elementedgemodel_current(eemc),
                 nodemodel_charge(nmq),
                 edgemodel_charge(emq),
                 elementedgemodel_charge(eemq)
{
}


template <typename DoubleType>
void ExprContactEquation<DoubleType>::DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr<DoubleType> model_cache = ModelExprDataCachePtr<DoubleType>(new ModelExprDataCache<DoubleType>()); 
    Region &r = const_cast<Region &>(ContactEquation<DoubleType>::GetRegion());
    r.SetModelExprDataCache(model_cache);

    const std::string &NodeVolumeModel = ContactEquation<DoubleType>::GetRegion().GetNodeVolumeModel();
    const std::string &EdgeCoupleModel = ContactEquation<DoubleType>::GetRegion().GetEdgeCoupleModel();
    const std::string &ElementEdgeCoupleModel = ContactEquation<DoubleType>::GetRegion().GetElementEdgeCoupleModel();

    if (w == dsMathEnum::PERMUTATIONSONLY)
    {
        if (!nodemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleNodeEquation(nodemodel_int, m, v, p, w, NodeVolumeModel);
        }
    }
    else if (t == dsMathEnum::DC)
    {
        if (!nodemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleNodeEquation(nodemodel_int, m, v, p, w, NodeVolumeModel);
        }

        if (!edgemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleEdgeEquation(edgemodel_int, m, v, w, EdgeCoupleModel);
        }

        if (!elementedgemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleElementEdgeEquation(elementedgemodel_int, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
        }

        const std::string &circuitnode = ContactEquation<DoubleType>::GetCircuitNode();
        if (!circuitnode.empty())
        {
            if (!nodemodel_current.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleNodeEquationOnCircuit(nodemodel_current, m, v, w, NodeVolumeModel);
            }
            if (!edgemodel_current.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleEdgeEquationOnCircuit(edgemodel_current, m, v, w, EdgeCoupleModel);
            }
            if (!elementedgemodel_current.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleElementEdgeEquationOnCircuit(elementedgemodel_current, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
            }
        }
    }
    else if (t == dsMathEnum::TIME)
    {
    // In a circuit equation, we need to account for displacement flux
    /**
    //// Need time domain equivalent for internal models
        if (!nodemodel_int.empty())
        {
            ContactEquation<DoubleType>::AssembleNodeEquation(nodemodel_int, m, v, p);
        }

        if (!edgemodel_int.empty())
        {
            ContactEquation<DoubleType>::AssembleEdgeEquation(edgemodel_int, m, v, p);
        }
    */

        //// TODO: We really rely on there being a nodemodel in DCAssemble to set permutation entries
        const std::string &circuitnode = ContactEquation<DoubleType>::GetCircuitNode();
        if (!circuitnode.empty())
        {
            if (!nodemodel_charge.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleNodeEquationOnCircuit(nodemodel_charge, m, v, w, NodeVolumeModel);
            }
            if (!edgemodel_charge.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleEdgeEquationOnCircuit(edgemodel_charge, m, v, w, EdgeCoupleModel);
            }
            if (!elementedgemodel_charge.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleElementEdgeEquationOnCircuit(elementedgemodel_charge, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
            }
        }
    }
}

template <typename DoubleType>
void ExprContactEquation<DoubleType>::calcCurrent()
{
    /// handle cylindrical
    DoubleType ch = ContactEquation<DoubleType>::integrateNodeModelOverNodes(nodemodel_current, ContactEquation<DoubleType>::GetRegion().GetNodeVolumeModel());
    ch += ContactEquation<DoubleType>::integrateEdgeModelOverNodes(edgemodel_current, ContactEquation<DoubleType>::GetRegion().GetEdgeCoupleModel());
    ch += ContactEquation<DoubleType>::integrateElementEdgeModelOverNodes(elementedgemodel_current, ContactEquation<DoubleType>::GetRegion().GetElementEdgeCoupleModel(), 1.0, -1.0);
    ContactEquation<DoubleType>::SetCurrent(ch);
}

// Modeled after the Expr Equation
// Need to separate on a per contact basis
template <typename DoubleType>
void ExprContactEquation<DoubleType>::calcCharge()
{
    DoubleType ch = ContactEquation<DoubleType>::integrateNodeModelOverNodes(nodemodel_charge, ContactEquation<DoubleType>::GetRegion().GetNodeVolumeModel());
    ch += ContactEquation<DoubleType>::integrateEdgeModelOverNodes(edgemodel_charge, ContactEquation<DoubleType>::GetRegion().GetEdgeCoupleModel());
    ch += ContactEquation<DoubleType>::integrateElementEdgeModelOverNodes(elementedgemodel_charge, ContactEquation<DoubleType>::GetRegion().GetElementEdgeCoupleModel(), 1.0, -1.0);
    ContactEquation<DoubleType>::SetCharge(ch);
}

template <typename DoubleType>
void ExprContactEquation<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND contact_equation "
        "-device \"" << ContactEquation<DoubleType>::GetDeviceName()
        << "\" -contact \"" << ContactEquation<DoubleType>::GetContactName()
        << "\" -name \"" << ContactEquation<DoubleType>::GetName()
        << "\" -variable_name \"" <<  ContactEquation<DoubleType>::GetVariable()
        << "\" -edge_charge_model \"" << edgemodel_charge
        << "\" -edge_current_model \"" << edgemodel_current
        << "\" -edge_model \"" << edgemodel_int
        << "\" -element_charge_model \"" << elementedgemodel_charge
        << "\" -element_current_model \"" << elementedgemodel_current
        << "\" -element_model \"" << elementedgemodel_int
        << "\" -node_charge_model \"" << nodemodel_charge
        << "\" -node_current_model \"" << nodemodel_current
        << "\" -node_model \"" << nodemodel_int
        << "\" -circuit_node \"" << ContactEquation<DoubleType>::GetCircuitNode()
        << "\"";
}

template <typename DoubleType>
void ExprContactEquation<DoubleType>::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(ContactEquation<DoubleType>::GetDeviceName());
  omap["contact"] = ObjectHolder(ContactEquation<DoubleType>::GetContactName());
  omap["name"] = ObjectHolder(ContactEquation<DoubleType>::GetName());
  omap["variable_name"] = ObjectHolder( ContactEquation<DoubleType>::GetVariable());
  omap["edge_charge_model"] = ObjectHolder(edgemodel_charge);
  omap["edge_current_model"] = ObjectHolder(edgemodel_current);
  omap["edge_model"] = ObjectHolder(edgemodel_int);
  omap["element_charge_model"] = ObjectHolder(elementedgemodel_charge);
  omap["element_current_model"] = ObjectHolder(elementedgemodel_current);
  omap["element_model"] = ObjectHolder(elementedgemodel_int);
  omap["node_charge_model"] = ObjectHolder(nodemodel_charge);
  omap["node_current_model"] = ObjectHolder(nodemodel_current);
  omap["node_model"] = ObjectHolder(nodemodel_int);
  omap["circuit_node"] = ObjectHolder(ContactEquation<DoubleType>::GetCircuitNode());
}

template class ExprContactEquation<double>;
