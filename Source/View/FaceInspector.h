/*
 Copyright (C) 2010-2012 Kristian Duske
 
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
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __TrenchBroom__FaceInspector__
#define __TrenchBroom__FaceInspector__

#include <wx/panel.h>

#include "Model/BrushTypes.h"
#include "Model/FaceTypes.h"

class SpinControl;
class wxButton;
class wxListBox;
class wxGLContext;
class wxSpinDoubleEvent;
class wxStaticText;

namespace TrenchBroom {
    namespace Model {
        class Texture;
    }
    
    namespace View {
        class DocumentViewHolder;
        class SingleTextureViewer;
        class TextureBrowser;
        class TextureSelectedCommand;
        
        class FaceInspector : public wxPanel {
        protected:
            DocumentViewHolder& m_documentViewHolder;
            
            SingleTextureViewer* m_textureViewer;
            wxStaticText* m_textureNameLabel;
            
            SpinControl* m_xOffsetEditor;
            SpinControl* m_yOffsetEditor;
            SpinControl* m_xScaleEditor;
            SpinControl* m_yScaleEditor;
            SpinControl* m_rotationEditor;
            
            TextureBrowser* m_textureBrowser;
            wxListBox* m_textureCollectionList;
            wxButton* m_addTextureCollectionButton;
            wxButton* m_removeTextureCollectionsButton;
            
            wxWindow* createFaceEditor();
            wxWindow* createTextureBrowser();
        public:
            FaceInspector(wxWindow* parent, DocumentViewHolder& documentViewHolder);
            
            void updateFaceAttributes();
            void updateSelectedTexture();
            void updateTextureBrowser();
            void updateTextureCollectionList();
            
            void OnXOffsetChanged(wxSpinDoubleEvent& event);
            void OnYOffsetChanged(wxSpinDoubleEvent& event);
            void OnXScaleChanged(wxSpinDoubleEvent& event);
            void OnYScaleChanged(wxSpinDoubleEvent& event);
            void OnRotationChanged(wxSpinDoubleEvent& event);
            void OnTextureSelected(TextureSelectedCommand& event);

            void OnAddTextureCollectionPressed(wxCommandEvent& event);
            void OnRemoveTextureCollectionsPressed(wxCommandEvent& event);
            void OnUpdateRemoveTextureCollectionsButton(wxUpdateUIEvent& event);
            
            DECLARE_EVENT_TABLE()
        };
    }
}

#endif /* defined(__TrenchBroom__FaceInspector__) */
