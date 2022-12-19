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

float sign(float x)
{
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

struct CameraArcballNode : kame::squirtle::CameraNode {
    Vector3f camTarget = Vector3f::zero();
    Vector3f upVector = Vector3f(0.0f, 1.0f, 0.0f);
    int32_t lastMouseX = 0;
    int32_t lastMouseY = 0;

    Matrix4x4f getViewMatrix() override
    {
        return Matrix4x4f::createLookAt_RH(getGlobalLocation(), camTarget, upVector);
    }

    void handleRotateArcball(bool isDrag, int32_t mouseX, int mouseY, uint32_t viewportWidth, uint32_t viewportHeight)
    {
        if (isDrag)
        {
            Vector3f position = getLocation();
            Vector3f pivot = camTarget;

            Vector2f deltaAngle(2 * M_PI / viewportWidth, M_PI / viewportHeight);
            Vector2f angle((lastMouseX - mouseX) * deltaAngle.x, (lastMouseY - mouseY) * deltaAngle.y);

            Matrix4x4f viewMatrix = Matrix4x4f::transpose(getViewMatrix());
            Vector3f viewDir(-viewMatrix.m31, -viewMatrix.m32, -viewMatrix.m33);

            float cosAngle = Vector3f::dot(viewDir, upVector);
            if (cosAngle * sign(angle.y) > 0.99f)
            {
                angle.y = 0.0f;
            }

            Matrix4x4f rotationMatrixX = Matrix4x4f::createFromAxisAngle(upVector, angle.x);
            position = Vector3f::transform(position - pivot, rotationMatrixX) + pivot;

            Vector3f rightVector(viewMatrix.m11, viewMatrix.m12, viewMatrix.m13);
            Matrix4x4f rotationMatrixY = Matrix4x4f::createFromAxisAngle(rightVector, angle.y);
            Vector3f finalPosition = Vector3f::transform(position - pivot, rotationMatrixY) + pivot;

            setLocation(finalPosition);
        }

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
};

struct RotateLight : kame::squirtle::Node {
    RotateLight()
    {
        auto light0 = new kame::squirtle::LightNode();
        light0->setLocation(kame::math::Vector3f(1.5f, 1.0f, 0.0f));
        light0->diffuse = kame::math::Vector3f(1.0f, 0.0f, 0.0f);
        this->addChild(light0);

        auto light1 = new kame::squirtle::LightNode();
        light1->setLocation(kame::math::Vector3f(-1.5f, 1.0f, 0.0f));
        light1->diffuse = kame::math::Vector3f(0.0f, 0.0f, 1.0f);
        this->addChild(light1);
    }
    void onUpdate(float dt) override
    {
        auto r = this->getRotation();
        r.y += 100.0f * dt;
        this->setRotation(r);
    }
};

struct Editor {
    kame::sdl::Window win;
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
            kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(filePath);
            kame::squirtle::GltfNode* node = kame::squirtle::helper::createGltfNode(gltf);
            kame::gltf::deleteGLTF(gltf);

            std::filesystem::path path(filePath);
            path.replace_filename(path.stem().string() + "-diffuse.png");
            kame::ogl21::Texture2D* diffuse = kame::ogl21::loadTexture2D(path.string().c_str());
            setTexture(node, diffuse);

            if (node->player.hasAnimation())
            {
                node->player.playLoop();
            }

            engine.root->addChild(node);

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
        else if (node->getType() == kSquirtleLightNode)
        {
            item = parent->add(Fl_Tree_Prefs(), "Light");
        }
        else if (node->getType() == kSquirtleGltfNode)
        {
            item = parent->add(Fl_Tree_Prefs(), "GLTF");
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
        cam->farPlaneDistance = 10.0f;
        cam->setLocation(Vector3f(3.0f, 4.0f, 5.0f));
        engine.root->addChild(cam);

        auto* light0 = new kame::squirtle::LightNode();
        light0->lightType = kSquirtleDirectionalLight;
        light0->diffuse = Vector3f(0.5f, 0.5f, 0.5f);
        cam->addChild(light0);

        engine.root->addChild(new RotateLight());

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
            engine.updateNodes(state.deltaTime > 1.0f ? 1.0 / 60.0f : state.deltaTime);
            CameraArcballNode* cam = (CameraArcballNode*)engine.currentCamera;
            cam->handleRotateArcball(state.isDownLMB, state.mouseX, state.mouseY, 640, 480);
            engine.drawNodes(640, 480);
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
