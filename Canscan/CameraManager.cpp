#include "pch.h"
#include "CameraManager.h"
using namespace Pylon;

CameraManager::CameraManager() { PylonInitialize(); }
CameraManager::~CameraManager() { Close(); PylonTerminate(); }

bool CameraManager::EnumerateDevices(std::vector<CameraInfo>& list) {
    try {
        list.clear(); m_camList.clear(); m_devices.clear();
        CTlFactory::GetInstance().EnumerateDevices(m_devices);
        for (auto& dev : m_devices) {
            CameraInfo info;
            info.model = CString(dev.GetModelName().c_str());
            info.serial = CString(dev.GetSerialNumber().c_str());
            m_camList.push_back(info);
            list.push_back(info);
        }
        return !list.empty();
    }
    catch (...) { return false; }
}

const CameraInfo& CameraManager::GetCameraInfo(int index) const {
    ASSERT(index >= 0 && index < (int)m_camList.size());
    return m_camList[index];
}

bool CameraManager::Open(int index) {
    if (index < 0 || index >= (int)m_devices.size()) return false;
    try {
        m_camera.Attach(CTlFactory::GetInstance().CreateDevice(m_devices[index]));
        m_camera.Open();
        return true;
    }
    catch (const GenericException& e) {
        AfxMessageBox(CString(L"Open failed: ") + CString(e.GetDescription()));
        return false;
    }
}

bool CameraManager::Start() {
    try {
        if (!m_camera.IsOpen()) m_camera.Open();
        m_camera.StartGrabbing(GrabStrategy_LatestImageOnly);
        return true;
    }
    catch (const GenericException& e) {
        AfxMessageBox(CString(L"Start failed: ") + CString(e.GetDescription()));
        return false;
    }
}

bool CameraManager::Stop() {
    if (m_camera.IsGrabbing()) m_camera.StopGrabbing();
    return true;
}

bool CameraManager::Close() {
    Stop();
    if (m_camera.IsOpen()) m_camera.Close();
    return true;
}

bool CameraManager::RetrieveFrame(cv::Mat& frame) {
    if (!m_camera.IsGrabbing()) return false;
    try {
        if (m_camera.RetrieveResult(1000, m_ptrGrabResult, TimeoutHandling_Return)) {
            if (!m_ptrGrabResult->GrabSucceeded()) return false;
            cv::Mat gray(m_ptrGrabResult->GetHeight(), m_ptrGrabResult->GetWidth(),
                CV_8UC1, (uint8_t*)m_ptrGrabResult->GetBuffer());
            cv::resize(gray, frame, cv::Size(640, 480), 0, 0, cv::INTER_AREA);
            return true;
        }
    }
    catch (...) { /* ignore */ }
    return false;
}
