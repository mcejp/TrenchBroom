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

#include "MoveBrushFacesCommand.h"

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
        const Command::CommandType MoveBrushFacesCommand::Type = Command::freeType();
        
        MoveBrushFacesCommand::Ptr MoveBrushFacesCommand::moveFaces(View::MapDocumentWPtr document, const Model::VertexToFacesMap& faces, const Vec3& delta) {
            return Ptr(new MoveBrushFacesCommand(document, faces, delta));
        }
        
        MoveBrushFacesCommand::MoveBrushFacesCommand(View::MapDocumentWPtr document, const Model::VertexToFacesMap& faces, const Vec3& delta) :
        BrushVertexHandleCommand(Type, makeName(faces), true, true),
        m_document(document),
        m_delta(delta) {
            assert(!delta.null());
            extractFaces(faces);
        }
        
        bool MoveBrushFacesCommand::doPerformDo() {
            View::MapDocumentSPtr document = lock(m_document);
            if (!canPerformDo(document))
                return false;
            
            const BBox3& worldBounds = document->worldBounds();
            m_snapshot = Model::Snapshot(m_brushes);
            m_newFacePositions.clear();
            
            BrushFacesMap::const_iterator mapIt, mapEnd;
            for (mapIt = m_brushFaces.begin(), mapEnd = m_brushFaces.end(); mapIt != mapEnd; ++mapIt) {
                Model::Brush* brush = mapIt->first;
                document->objectWillChangeNotifier(brush);
                const Polygon3::List& oldFacePositions = mapIt->second;
                const Polygon3::List newFacePositions = brush->moveFaces(worldBounds, oldFacePositions, m_delta);
                VectorUtils::append(m_newFacePositions, newFacePositions);
                document->objectDidChangeNotifier(brush);
            }
            
            return true;
        }
        
        bool MoveBrushFacesCommand::canPerformDo(View::MapDocumentSPtr document) const {
            const BBox3& worldBounds = document->worldBounds();
            BrushFacesMap::const_iterator it, end;
            for (it = m_brushFaces.begin(), end = m_brushFaces.end(); it != end; ++it) {
                Model::Brush* brush = it->first;
                const Polygon3::List& faces = it->second;
                if (!brush->canMoveFaces(worldBounds, faces, m_delta))
                    return false;
            }
            return true;
        }
        
        bool MoveBrushFacesCommand::doPerformUndo() {
            View::MapDocumentSPtr document = lock(m_document);
            const BBox3& worldBounds = document->worldBounds();
            
            document->objectWillChangeNotifier(m_brushes.begin(), m_brushes.end());
            m_snapshot.restore(worldBounds);
            document->objectDidChangeNotifier(m_brushes.begin(), m_brushes.end());
            
            return true;
        }
        
        void MoveBrushFacesCommand::doRemoveBrushes(View::VertexHandleManager& manager) const {
            manager.removeBrushes(m_brushes);
        }
        
        void MoveBrushFacesCommand::doAddBrushes(View::VertexHandleManager& manager) const {
            manager.addBrushes(m_brushes);
        }
        
        void MoveBrushFacesCommand::doSelectNewHandlePositions(View::VertexHandleManager& manager) const {
            manager.selectFaceHandles(m_newFacePositions);
        }
        
        void MoveBrushFacesCommand::doSelectOldHandlePositions(View::VertexHandleManager& manager) const {
            manager.selectFaceHandles(m_oldFacePositions);
        }
        
        String MoveBrushFacesCommand::makeName(const Model::VertexToFacesMap& faces) {
            return String("Move ") + (faces.size() == 1 ? "Face" : "Faces");
        }
        
        void MoveBrushFacesCommand::extractFaces(const Model::VertexToFacesMap& faces) {
            typedef std::pair<BrushFacesMap::iterator, bool> BrushFacesMapInsertResult;
            Model::VertexToFacesMap::const_iterator vIt, vEnd;
            for (vIt = faces.begin(), vEnd = faces.end(); vIt != vEnd; ++vIt) {
                const Model::BrushFaceList& faces = vIt->second;
                Model::BrushFaceList::const_iterator eIt, eEnd;
                for (eIt = faces.begin(), eEnd = faces.end(); eIt != eEnd; ++eIt) {
                    Model::BrushFace* face = *eIt;
                    Model::Brush* brush = face->parent();
                    Model::BrushFaceGeometry* side = face->side();
                    assert(side != NULL);
                    const Polygon3 facePosition = side->faceInfo();
                    
                    BrushFacesMapInsertResult result = m_brushFaces.insert(std::make_pair(brush, Polygon3::List()));
                    if (result.second)
                        m_brushes.push_back(brush);
                    result.first->second.push_back(facePosition);
                    m_oldFacePositions.push_back(facePosition);
                }
            }
            
            assert(!m_brushes.empty());
            assert(m_brushes.size() == m_brushFaces.size());
        }
    }
}
