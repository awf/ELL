////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Stack.tcc (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoordinateListTools.h"

// utilities
#include "XMLSerialization.h"
#include "Files.h"

// stl
#include <algorithm>
#include <string>
#include <memory>
#include <algorithm>

namespace layers
{
    template <typename LayerType>
    const LayerType& Stack::GetLayer(uint64 layerIndex) const
    {
        return dynamic_cast<const LayerType&>(*_layers[layerIndex]);
    }

    template<typename StackType>
    StackType Stack::Load(const std::string& inputMapFile)
    {
        auto inputMapFStream = utilities::OpenIfstream(inputMapFile);
        utilities::XMLDeserializer deserializer(inputMapFStream);

        StackType map;
        deserializer.Deserialize(map);
        return map;
    }

}
