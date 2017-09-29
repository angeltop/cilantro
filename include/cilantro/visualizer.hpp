#pragma once

#include <cilantro/renderables.hpp>
#include <cilantro/point_cloud.hpp>
#include <pangolin/display/display_internal.h>

class Visualizer {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Visualizer(const std::string &window_name, const std::string &display_name);
    ~Visualizer();

    Visualizer& addPointCloud(const std::string &name, const PointCloud &cloud, const RenderingProperties &rp = RenderingProperties());
    Visualizer& addPointCloud(const std::string &name, const std::vector<Eigen::Vector3f> &points, const RenderingProperties &rp = RenderingProperties());
    Visualizer& addPointCloudColors(const std::string &name, const std::vector<Eigen::Vector3f> &colors);
    Visualizer& addPointCloudValues(const std::string &name, const std::vector<float> &point_values);

    Visualizer& addPointCloudNormals(const std::string &name, const std::vector<Eigen::Vector3f> &points, const std::vector<Eigen::Vector3f> &normals, const RenderingProperties &rp = RenderingProperties());
    Visualizer& addPointCloudNormals(const std::string &name, const PointCloud &cloud, const RenderingProperties &rp = RenderingProperties());

    Visualizer& addPointCorrespondences(const std::string &name, const std::vector<Eigen::Vector3f> &points_src, const std::vector<Eigen::Vector3f> &points_dst, const RenderingProperties &rp = RenderingProperties());
    Visualizer& addPointCorrespondences(const std::string &name, const PointCloud &cloud_src, const PointCloud &cloud_dst, const RenderingProperties &rp = RenderingProperties());

    Visualizer& addCoordinateSystem(const std::string &name, float scale = 1.0f, const Eigen::Matrix4f &tf = Eigen::Matrix4f::Identity(), const RenderingProperties &rp = RenderingProperties());

    Visualizer& addTriangleMesh(const std::string &name, const PointCloud &cloud, const std::vector<std::vector<size_t> > &faces, const RenderingProperties &rp = RenderingProperties());
    Visualizer& addTriangleMesh(const std::string &name, const std::vector<Eigen::Vector3f> &vertices, const std::vector<std::vector<size_t> > &faces, const RenderingProperties &rp = RenderingProperties());
    Visualizer& addTriangleMeshVertexNormals(const std::string &name, const std::vector<Eigen::Vector3f> &vertex_normals);
    Visualizer& addTriangleMeshFaceNormals(const std::string &name, const std::vector<Eigen::Vector3f> &face_normals);
    Visualizer& addTriangleMeshVertexColors(const std::string &name, const std::vector<Eigen::Vector3f> &vertex_colors);
    Visualizer& addTriangleMeshFaceColors(const std::string &name, const std::vector<Eigen::Vector3f> &face_colors);
    Visualizer& addTriangleMeshVertexValues(const std::string &name, const std::vector<float> &vertex_values);
    Visualizer& addTriangleMeshFaceValues(const std::string &name, const std::vector<float> &face_values);

    inline Visualizer& clear() { renderables_.clear(); return *this; }
    inline Visualizer& remove(const std::string &name) { renderables_.erase(name); return *this; }

    Visualizer& clearRenderArea();
    Visualizer& render();
    inline Visualizer& finishFrame() { gl_context_->MakeCurrent(); pangolin::FinishFrame(); return *this; }
    inline Visualizer& spinOnce() { clearRenderArea(); render(); finishFrame(); return *this; }

    inline bool wasStopped() const { return gl_context_->quit; }

    bool getVisibilityStatus(const std::string &name) const;
    Visualizer& setVisibilityStatus(const std::string &name, bool visible);
    inline Visualizer& toggleVisibilityStatus(const std::string &name) { setVisibilityStatus(name, !getVisibilityStatus(name)); return *this; }

    RenderingProperties getRenderingProperties(const std::string &name) const;
    Visualizer& setRenderingProperties(const std::string &name, const RenderingProperties &rp);

    std::vector<std::string> getObjectNames() const;

    inline Eigen::Vector3f getClearColor() const { return clear_color_; }
    inline Visualizer& setClearColor(const Eigen::Vector3f &color) { clear_color_ = color; return *this; }
    inline Visualizer& setClearColor(float r, float g, float b) { clear_color_ = Eigen::Vector3f(r,g,b); return *this; }

    Visualizer& setProjectionMatrix(int w, int h, pangolin::GLprecision fu, pangolin::GLprecision fv, pangolin::GLprecision u0, pangolin::GLprecision v0, pangolin::GLprecision zNear, pangolin::GLprecision zFar);

    Visualizer& registerKeyboardCallback(const std::vector<int> &keys, std::function<void(Visualizer&,int,void*)> func, void *cookie);

    pangolin::TypedImage getRenderImage(float scale = 1.0f, bool rgba = false);
    Visualizer& saveRenderAsImage(const std::string &file_name, float scale, float quality, bool rgba = false);

    Visualizer& startVideoRecording(const std::string &uri, size_t fps, bool record_on_render = false, float scale = 1.0f, bool rgba = false);
    Visualizer& recordVideoFrame();
    Visualizer& stopVideoRecording();
    inline bool isRecording() const { return !!video_recorder_; }

    inline pangolin::PangolinGl* getPangolinGLContext() const { return gl_context_; }
    inline pangolin::View* getDisplay() const { return display_; }

private:
    pangolin::PangolinGl *gl_context_;
    pangolin::View *display_;

    std::shared_ptr<pangolin::OpenGlRenderState> gl_render_state_;
    std::shared_ptr<pangolin::Handler3D> input_handler_;
    pangolin::OpenGlMatrix initial_model_view_;
    std::shared_ptr<pangolin::VideoOutput> video_recorder_;
    size_t video_fps_;
    float video_scale_;
    bool video_rgba_;
    bool video_record_on_render_;

    static Eigen::Vector3f no_color_;
    static Eigen::Vector3f default_color_;
    Eigen::Vector3f clear_color_;

    std::map<std::string, std::shared_ptr<Renderable> > renderables_;

    static void point_size_callback_(Visualizer &viz, int key);
    static void reset_view_callback_(Visualizer &viz);
    static void wireframe_toggle_callback_(Visualizer &viz);

    struct {
        inline bool operator()(const std::pair<Renderable*, float> &p1, const std::pair<Renderable*, float> &p2) const {
            if (p1.first->renderingProperties.opacity == 1.0f) {
                return true;
            } else if (p2.first->renderingProperties.opacity == 1.0f) {
                return false;
            } else {
                return p1.second > p2.second;
            }
        }
    } render_priority_comparator_;
};
