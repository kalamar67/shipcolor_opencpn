#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ocpn_plugin.h"   // OpenCPN Plugin API

#define SHIPCOLOR_PI_VERSION_MAJOR 1
#define SHIPCOLOR_PI_VERSION_MINOR 0

// -------------------------------------------------------
// AIS Ship Type kodları (ITU-R M.1371-5)
// -------------------------------------------------------
enum ShipCategory {
    SC_UNKNOWN     = 0,
    SC_FISHING     = 1,   // 30
    SC_TOWING      = 2,   // 21-24
    SC_SPEEDBOAT   = 3,   // 31-32
    SC_PASSENGER   = 4,   // 60-69
    SC_CARGO       = 5,   // 70-79
    SC_TANKER      = 6,   // 80-89
    SC_MILITARY    = 7,   // 35
    SC_OTHER       = 8
};

// -------------------------------------------------------
// Renk yapısı
// -------------------------------------------------------
struct ShipColor {
    unsigned char r, g, b;
};

// -------------------------------------------------------
// Plugin sınıfı
// -------------------------------------------------------
class shipcolor_pi : public opencpn_plugin_18 {
public:
    shipcolor_pi(void* ppimgr);
    ~shipcolor_pi();

    // --- Zorunlu OpenCPN API metodları ---
    int         Init() override;
    bool        DeInit() override;
    int         GetAPIVersionMajor() override;
    int         GetAPIVersionMinor() override;
    int         GetPlugInVersionMajor() override;
    int         GetPlugInVersionMinor() override;
    wxString    GetCommonName() override;
    wxString    GetShortDescription() override;
    wxString    GetLongDescription() override;
    wxBitmap*   GetPlugInBitmap() override;

    // --- Render callback ---
    bool RenderOverlay(wxDC& dc, PlugIn_ViewPort* vp) override;
    bool RenderOverlayMultiCanvas(wxDC& dc, PlugIn_ViewPort* vp, int canvas_index) override;

private:
    ShipCategory  GetCategory(int ship_type);
    ShipColor     GetColor(ShipCategory cat);
    void          DrawFilledTriangle(wxDC& dc, 
                                     wxPoint center, 
                                     double heading_deg,
                                     double scale,
                                     ShipColor color);

    wxBitmap m_logo;
};
