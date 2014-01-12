/*
 Copyright (C) 2010-2014 Kristian Duske
 
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

#include "MoveBrushEdgesCommand.h"

#include "CollectionUtils.h"
#include "Model/Brush.h"
#include "Model/BrushEdge.h"
#include "Model/BrushFace.h"
#include "Model/BrushFaceGeometry.h"
#include "View/MapDocument.h"
#include "View/VertexHandleManager.h"

#include <cassert>

namespace TrenchBroom {
    namespace Controller {
        const Command::CommandType MoveBrushEdgesCommand::Type = Command::freeType();
        
        MoveBrushEdgesCommand::Ptr MoveBrushEdgesCommand::moveEdges(View::MapDocumentWPtr document, const Model::VertexToEdgesMap& edges, const Vec3& delta) {
            return Ptr(new MoveBrushEdgesCommand(document, edges, delta));
        }
        
        MoveBrushEdgesCommand::MoveBrushEdgesCommand(View::MapDocumentWPtr document, const Model::VertexToEdgesMap& edges, const Vec3& delta) :
        BrushVertexHandleCommand(Type, makeName(edges), true, true),
        m_document(document),
        m_delta(delta) {
            assert(!delta.null());
            extractEdges(edges);
        }
        
        bool MoveBrushEdgesCommand::doPerformDo() {
            View::MapDocumentSPtr document = lock(m_document);
            if (!canPerformDo(document))
                return false;
            
            const BBox3& worldBounds = document->worldBounds();
            m_snapshot = Model::Snapshot(m_brushes);
            m_newEdgePositions.clear();
            
            BrushEdgesMap::const_iterator mapIt, mapEnd;
            for (mapIt = m_brushEdges.begin(), mapEnd = m_brushEdges.end(); mapIt != mapEnd; ++mapIt) {
                Model::Brush* brush = mapIt->first;
                document->objectWillChangeNotifier(brush);
                const Edge3::List& oldEdgePositions = mapIt->second;
                const Edge3::List newEdgePositions = brush->moveEdges(worldBounds, oldEdgePositions, m_delta);
                VectorUtils::append(m_newEdgePositions, newEdgePositions);
                document->objectDidChangeNotifier(brush);
            }
            
            return true;
        }
        
        bool MoveBrushEdgesCommand::canPerformDo(View::MapDocumentSPtr document) const {
            const BBox3& worldBounds = document->worldBounds();
            BrushEdgesMap::const_iterator it, end;
            for (it = m_brushEdges.begin(), end = m_brushEdges.end(); it != end; ++it) {
                Model::Brush* brush = it->first;
                const Edge3::List& edges = it->second;
                if (!brush->canMoveEdges(worldBounds, edges, m_delta))
                    return false;
            }
            return true;
        }
        
        bool MoveBrushEdgesCommand::doPerformUndo() {
            View::MapDocumentSPtr document = lock(m_document);
            const BBox3& worldBounds = document->worldBounds();
            
            document->objectWillChangeNotifier(m_brushes.begin(), m_brushes.end());
            m_snapshot.restore(worldBounds);
            document->objectDidChangeNotifier(m_brushes.begin(), m_brushes.end());
            
            return true;
        }
        
        void MoveBrushEdgesCommand::doRemoveBrushes(View::VertexHandleManager& manager) const {
            manager.removeBrushes(m_brushes);
        }
        
        void MoveBrushEdgesCommand::doAddBrushes(View::VertexHandleManager& manager) const {
            manager.addBrushes(m_brushes);
        }
        
        void MoveBrushEdgesCommand::doSelectNewHandlePositions(View::VertexHandleManager& manager) const {
            manager.selectEdgeHandles(m_newEdgePositions);
        }
        
        void MoveBrushEdgesCommand::doSelectOldHandlePositions(View::VertexHandleManager& manager) const {
            manager.selectEdgeHandles(m_oldEdgePositions);
        }

        String MoveBrushEdgesCommand::makeName(const Model::VertexToEdgesMap& edges) {
            return String("Move ") + (edges.size() == 1 ? "Edge" : "Edges");
        }
        
        void MoveBrushEdgesCommand::extractEdges(const Model::VertexToEdgesMap& edges) {
            typedef std::pair<BrushEdgesMap::iterator, bool> BrushEdgesMapInsertResult;
            Model::VertexToEdgesMap::const_iterator vIt, vEnd;
            for (vIt = edges.begin(), vEnd = edges.end(); vIt != vEnd; ++vIt) {
                const Model::BrushEdgeList& edges = vIt->second;
                Model::BrushEdgeList::const_iterator eIt, eEnd;
                for (eIt = edges.begin(), eEnd = edges.end(); eIt != eEnd; ++eIt) {
                    Model::BrushEdge* edge = *eIt;
                    Model::Brush* brush = edge->left->face->parent();
                    const Edge3 edgePosition = edge->edgeInfo();

                    BrushEdgesMapInsertResult result = m_brushEdges.insert(std::make_pair(brush, Edge3::List()));
                    if (result.second)
                        m_brushes.push_back(brush);
                    result.first->second.push_back(edgePosition);
                    m_oldEdgePositions.push_back(edgePosition);
                }
            }
            
            assert(!m_brushes.empty());
            assert(m_brushes.size() == m_brushEdges.size());
        }
    }
}
