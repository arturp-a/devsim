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

#ifndef NODESOLUTION_HH
#define NODESOLUTION_HH
#include "NodeModel.hh"
#include <string>
// need to set general node propeties, such as positive only
template <typename DoubleType>
class NodeSolution : public NodeModel
{
    public:
        NodeSolution(const std::string &, RegionPtr);
        // This model depends on this model to calculate values
        NodeSolution(const std::string &, RegionPtr, NodeModelPtr);

        static NodeModelPtr CreateNodeSolution(const std::string &, RegionPtr);
        static NodeModelPtr CreateNodeSolution(const std::string &, RegionPtr, NodeModelPtr);

        void Serialize(std::ostream &) const;

    private:
        void calcNodeScalarValues() const;
        void setInitialValues();
        // If we are an auxilary model, create our values from the parent
        mutable WeakConstNodeModelPtr parentModel;
        // Detect whether parent model still exists
        mutable std::string  parentModelName;
};

#endif

