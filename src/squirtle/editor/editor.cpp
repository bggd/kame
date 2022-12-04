#include <kame/kame.hpp>

#include <FL/FL.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Tree.H>

#include <filesystem>

using namespace kame::math;

struct CameraArcballNode : kame::squirtle::CameraNode {
    Vector3f camTarget = Vector3f::Zero();

    Matrix4x4f getViewMatrix() override
    {
        return Matrix4x4f::createLookAt_RH(getGlobalLocation(), camTarget, Vector3f(0.0f, 1.0f, 0.0f));
    }
};

struct Editor {
    kame::sdl2::Window win;
    kame::squirtle::Engine engine;
    Fl_Window* mainWindow = nullptr;
    Fl_Window* outliner = nullptr;
    Fl_Tree* tree = nullptr;

    static void importGltf(Fl_Widget*, void* data)
    {
        ((Editor*)data)->OnImportGltf();
    }

    void setTexture(kame::squirtle::Node* node, kame::ogl21::Texture2D* diffuse)
    {
        if (node->getType() == kSquirtleMeshNode)
        {
            ((kame::squirtle::MeshNode*)node)->diffuse = diffuse;
        }
        for (kame::squirtle::Node* child : node->getChildren())
        {
            setTexture(child, diffuse);
        }
    }

    void OnImportGltf()
    {
        char* filePath = fl_file_chooser("Import GLTF 2.0?", "*.gltf", nullptr, 0);
        if (filePath)
        {
            kame::squirtle::Node* node = new kame::squirtle::Node();
            assert(node);

            kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(filePath);
            kame::squirtle::loadMeshNodeFromGLTF(node, gltf);
            kame::gltf::deleteGLTF(gltf);

            std::filesystem::path path(filePath);
            path.replace_filename(path.stem().string() + "-diffuse.png");
            kame::ogl21::Texture2D* diffuse = kame::ogl21::loadTexture2D(path.string().c_str());
            setTexture(node, diffuse);

            for (kame::squirtle::Node* child : node->getChildren())
            {
                engine.root->addChild(child);
            }

            node->children.clear();
            delete node;

            addTreeItem();
            outliner->redraw();
        }
    }

    void addTreeItemRecursive(Fl_Tree_Item* parent, kame::squirtle::Node* node)
    {
        Fl_Tree_Item* item = nullptr;
        if (node->getType() == kSquirtleNode)
        {
            item = parent->add(Fl_Tree_Prefs(), "Node");
        }
        else if (node->getType() == kSquirtleCameraNode)
        {
            item = parent->add(Fl_Tree_Prefs(), "Camera");
        }
        else if (node->getType() == kSquirtleMeshNode)
        {
            item = parent->add(Fl_Tree_Prefs(), "Mesh");
        }
        for (kame::squirtle::Node* child : node->getChildren())
        {
            addTreeItemRecursive(item, child);
        }
    }

    void addTreeItem()
    {
        if (tree->first())
        {
            tree->first()->clear_children();
        }
        tree->begin();
        addTreeItemRecursive(tree->first(), engine.root);
        tree->end();
    }

    void initEditor(int argc, char** argv)
    {
        kame::kameInit();
        win.setOgl21DebugMode(true);
        win.setFpsCap(1.0 / 60.0);
        win.openWindow("Viewport");
        win.setVsync(true);

        engine.initSquirtle();
        auto* cam = new CameraArcballNode();
        cam->fov = 45.0f;
        cam->aspectRatio = 640.0f / 480.0f;
        cam->nearPlaneDistance = 0.1f;
        cam->farPlaneDistance = 100.0f;
        cam->setLocation(Vector3f(5.0f, 6.0f, 7.0f));
        engine.root->addChild(cam);

        mainWindow = new Fl_Window(256, 30, "Squirtle Editor");
        Fl_Menu_Bar* menuBar = new Fl_Menu_Bar(0, 0, 256, 30);
        menuBar->add("&File/&Import/&GLTF 2.0(.gltf)", 0, (Fl_Callback*)Editor::importGltf, (void*)this);
        mainWindow->end();
        mainWindow->show(argc, argv);

        outliner = new Fl_Window(340, 180, "Outliner");
        tree = new Fl_Tree(0, 0, outliner->w(), outliner->h());
        addTreeItem();
        outliner->end();
        outliner->show(argc, argv);
    }

    void runEditor()
    {
        for (;;)
        {
            win.update();
            auto state = win.getState();
            if (state.isCloseRequest)
                break;
            engine.updateNodes(state.deltaTime);
            kame::ogl21::setViewport(0, 0, 640, 480);
            kame::ogl21::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, Vector4f(0.5, 0.5, 0.5, 1));
            engine.drawNodes();
            win.swapWindow();
            Fl::check();
        }
    }

    void shutdownEditor()
    {
        engine.shutdownSqurtile();

        win.closeWindow();
        kame::kameShutdown();
    }
};

int main(int argc, char** argv)
{
    Editor editor;
    editor.initEditor(argc, argv);
    editor.runEditor();
    editor.shutdownEditor();
    return 0;
}
