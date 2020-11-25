/*
 Copyright (C) 2020 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FloatType.h"
#include "Model/Brush.h"
#include "Model/BrushNode.h"
#include "Model/Entity.h"
#include "Model/NodeContents.h"
#include "View/MapDocument.h"
#include "View/SwapNodeContentsCommand.h"
#include "View/MapDocumentTest.h"

#include <kdl/memory_utils.h>
#include <kdl/result.h>

#include <vecmath/bbox.h>
#include <vecmath/bbox_io.h>
#include <vecmath/mat.h>
#include <vecmath/mat_ext.h>
#include <vecmath/mat_io.h>
#include <vecmath/vec.h>
#include <vecmath/vec_io.h>

#include <memory>

#include "Catch2.h"

namespace TrenchBroom {
    namespace View {
        class SwapNodeContentsCommandTest : public MapDocumentTest {};

        TEST_CASE_METHOD(SwapNodeContentsCommandTest, "SwapNodeContentsCommandTest.swapBrushes") {
            auto* brushNode = createBrushNode();
            document->addNode(brushNode, document->parentForNodes());
            
            const auto originalBrush = brushNode->brush();
            const auto modifiedBrush = brushNode->brush().transform(document->worldBounds(), vm::translation_matrix(vm::vec3(16, 0, 0)), false).value();

            auto nodesToSwap = std::vector<std::pair<Model::Node*, Model::NodeContents>>{};
            nodesToSwap.emplace_back(brushNode, modifiedBrush);
            
            document->swapNodeContents("Swap Nodes", std::move(nodesToSwap));
            CHECK(brushNode->brush() == modifiedBrush);
            
            document->undoCommand();
            CHECK(brushNode->brush() == originalBrush);
        }
    }
}
