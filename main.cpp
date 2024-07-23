#include "pch.h"

//
// Credit/Reference: https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/DisplayCoreCustomCompositor/cpp/DisplayCoreCustomCompositor.cpp
//

#include <iostream>
#include <iomanip>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Display.Core.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <windows.devices.display.core.interop.h>

namespace winrt
{
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Devices::Display;
    using namespace winrt::Windows::Devices::Display::Core;
    using namespace winrt::Windows::Graphics;
    using namespace winrt::Windows::Graphics::DirectX;
    using namespace winrt::Windows::Foundation::Collections;
}

/*LUID LuidFromAdapterId(winrt::Windows::Graphics::DisplayAdapterId id)
{
    return { id.LowPart, id.HighPart };
}*/

std::wstring ConnectorName[] = { L"Unknown",
                                L"HD15",
                                L"AnalogTV",
                                L"DVI",
                                L"HDMI",
                                L"LVDS",
                                L"SDI",
                                L"DisplayPort"
};

static std::wstring GetConnectorName(winrt::Windows::Devices::Display::DisplayMonitorPhysicalConnectorKind connector)
{
    if ((int)connector > 7)
        return L"Undefined";
    else
        return ConnectorName[(int)connector];
}

bool PrintEDID(const winrt::com_array<uint8_t>& edidBuffer)
{
    uint32_t size = edidBuffer.size();

    if (size > 0)
    {
        for (uint32_t i = 0; i < size; i++)
        {
            //if (size > 128 && (i > 0) && (i % 128 == 0)) // block boundary
            //    std::wcout << std::endl;
            
            if (i % 16 == 0) // line end
                std::wcout << std::endl;

            std::wcout << std::setw(2) << std::uppercase << std::setfill(L'0') << std::hex << edidBuffer[i] << " ";
        }

        std::wcout << std::endl << std::endl;
        return true;
    }
    
    std::wcout << L"Empty EDID!" << std::endl;
    return false;
}

int main()
{
    winrt::init_apartment();

    std::wcout << "--------------------------------------------------" << std::endl;
    std::wcout << "Windows EDID read utility v1.0 - Intel Corporation" << std::endl;
    std::wcout << "--------------------------------------------------" << std::endl << std::endl;

    // Create a DisplayManager instance for owning targets and managing the displays
    auto manager = winrt::DisplayManager::Create(winrt::DisplayManagerOptions::None);
    winrt::IVectorView<winrt::DisplayTarget> targets = manager.GetCurrentTargets();

    for (auto&& target : targets)
    {
        // You can look at a DisplayMonitor to inspect the EDID of the device
        winrt::DisplayMonitor monitor = target.TryGetMonitor();

        if (monitor)
        {
            winrt::com_array<uint8_t> edidBuffer = monitor.GetDescriptor(winrt::DisplayMonitorDescriptorKind::Edid);

            winrt::DisplayAdapter adapter = target.Adapter();

            if (adapter)
            {
                // reset number formating
                std::wcout << std::resetiosflags(std::ios_base::basefield);

                std::wcout << L"Monitor: " << monitor.DisplayName().c_str() << std::endl;
                std::wcout << L"NativeResolution: " << monitor.NativeResolutionInRawPixels().Width << "x" << monitor.NativeResolutionInRawPixels().Height << std::endl;
                std::wcout << L"Monitor Device ID: " << monitor.DeviceId().c_str() << std::endl;
                std::wcout << L"Display Adapter Device ID: " << monitor.DisplayAdapterDeviceId().c_str() << std::endl;

                std::wcout << std::setw(8) << std::uppercase << std::setfill(L'0') << std::hex;
                std::wcout << L"Adapter LUID: " << monitor.DisplayAdapterId().HighPart << monitor.DisplayAdapterId().LowPart << std::endl;
                std::wcout << L"DisplayAdapterTargetId: " << monitor.DisplayAdapterTargetId() << std::endl;
                std::wcout << L"Connector: " << GetConnectorName(monitor.PhysicalConnector()) << std::endl;
                std::wcout << L"EDID..." << std::endl;

                PrintEDID(edidBuffer);
            }
        }
    }

    return 0;
}
