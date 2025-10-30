#pragma once
#include <vector>
#include <pylon/PylonIncludes.h>
#include <opencv2/opencv.hpp>
#include <afxstr.h> // CString

struct CameraInfo {
    CString model;
    CString serial;
    bool opened = false;
    bool connected = false;
};

class CameraManager {
public:
    CameraManager();
    ~CameraManager();

    bool EnumerateDevices(std::vector<CameraInfo>& list);
    const CameraInfo& GetCameraInfo(int index) const;

    bool Open(int index);
    bool Start();     // Start grabbing
    bool Stop();      // Stop grabbing
    bool Close();     // Close camera
    bool RetrieveFrame(cv::Mat& frame);
    bool IsConnected() const { return m_camera.IsGrabbing(); }

private:
    Pylon::CInstantCamera m_camera;
    Pylon::DeviceInfoList_t m_devices;
    std::vector<CameraInfo> m_camList;
    Pylon::CGrabResultPtr m_ptrGrabResult;
};
