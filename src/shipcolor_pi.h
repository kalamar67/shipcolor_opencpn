#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ocpn_plugin.h"

#define SHIPCOLOR_PI_VERSION_MAJOR 1
#define SHIPCOLOR_PI_VERSION_MINOR 0
#define MY_API_VERSION_MAJOR 1
#define MY_API_VERSION_MINOR 16

enum ShipCategory {
    SC_UNKNOWN=0, SC_FISHING, SC_TOWING, SC_SPEEDBOAT,
    SC_PASSENGER, SC_CARGO, SC_TANKER, SC_MILITARY, SC_OTHER
};

struct ShipColor { unsigned char r, g, b; };

class shipcolor_pi : public opencpn_plugin_18 {
public:
    shipcolor_pi(void* ppimgr);
    ~shipcolor_pi();

    int       Init() override;
    bool      DeInit() override;
    int       GetAPIVersionMajor() override;
    int       GetAPIVersionMinor() override;
    int       GetPlugInVersionMajor() override;
    int       GetPlugInVersionMinor() override;
    wxString  GetCommonName() override;
    wxString  GetShortDescription() override;
    wxString  GetLongDescription() override;
    wxBitmap* GetPlugInBitmap() override;
    bool      RenderOverlay(wxDC& dc, PlugIn_ViewPort* vp) override;
    void      SetDefaults() override {}
    int       GetToolbarToolCount() override { return 0; }
    void      OnToolbarToolCallback(int id) override {}

private:
    ShipCategory GetCategory(int ship_type);
    ShipColor    GetColor(ShipCategory cat);
    void         DrawFilledTriangle(wxDC& dc, wxPoint center,
                                    double heading_deg, double scale,
                                    ShipColor color);
    wxBitmap m_logo;
};
   
