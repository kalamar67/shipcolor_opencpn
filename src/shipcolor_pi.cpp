#include "shipcolor_pi.h"
#include <cmath>

DECL_EXP opencpn_plugin* create_pi(void* ppimgr) {
    return new shipcolor_pi(ppimgr);
}
DECL_EXP void destroy_pi(opencpn_plugin* p) {
    delete p;
}

shipcolor_pi::shipcolor_pi(void* ppimgr)
    : opencpn_plugin_18(ppimgr) {}

shipcolor_pi::~shipcolor_pi() {}

int  shipcolor_pi::GetAPIVersionMajor()    { return MY_API_VERSION_MAJOR; }
int  shipcolor_pi::GetAPIVersionMinor()    { return MY_API_VERSION_MINOR; }
int  shipcolor_pi::GetPlugInVersionMajor() { return SHIPCOLOR_PI_VERSION_MAJOR; }
int  shipcolor_pi::GetPlugInVersionMinor() { return SHIPCOLOR_PI_VERSION_MINOR; }

wxString shipcolor_pi::GetCommonName()       { return wxT("ShipColor"); }
wxString shipcolor_pi::GetShortDescription() { return wxT("Gemi tipine gore renkli AIS hedefleri"); }
wxString shipcolor_pi::GetLongDescription()  { return wxT("AIS ship_type alanina gore renkli ucgen"); }
wxBitmap* shipcolor_pi::GetPlugInBitmap()    { return &m_logo; }

int shipcolor_pi::Init() {
    return WANTS_OVERLAY_CALLBACK;
}

bool shipcolor_pi::DeInit() { return true; }

ShipCategory shipcolor_pi::GetCategory(int t) {
    if (t == 30)             return SC_FISHING;
    if (t >= 21 && t <= 24) return SC_TOWING;
    if (t == 31 || t == 32) return SC_SPEEDBOAT;
    if (t == 35)             return SC_MILITARY;
    if (t >= 60 && t <= 69) return SC_PASSENGER;
    if (t >= 70 && t <= 79) return SC_CARGO;
    if (t >= 80 && t <= 89) return SC_TANKER;
    if (t > 0)               return SC_OTHER;
    return SC_UNKNOWN;
}

ShipColor shipcolor_pi::GetColor(ShipCategory cat) {
    switch (cat) {
        case SC_TANKER:    return {220, 30,  30};
        case SC_CARGO:     return {139, 90,  43};
        case SC_PASSENGER: return {30,  100, 220};
        case SC_FISHING:   return {230, 210, 20};
        case SC_SPEEDBOAT: return {230, 130, 20};
        case SC_TOWING:    return {140, 60,  180};
        case SC_MILITARY:  return {20,  20,  20};
        case SC_OTHER:     return {150, 150, 150};
        default:           return {180, 180, 180};
    }
}

void shipcolor_pi::DrawFilledTriangle(wxDC& dc, wxPoint center,
                                       double heading_deg, double scale,
                                       ShipColor color) {
    const double H = 12.0 * scale;
    const double W =  7.0 * scale;
    double rad = heading_deg * M_PI / 180.0;

    wxPoint pts[3] = {
        wxPoint(center.x + (int)( H * sin(rad)), center.y + (int)(-H * cos(rad))),
        wxPoint(center.x + (int)(-W * cos(rad)), center.y + (int)(-W * sin(rad))),
        wxPoint(center.x + (int)( W * cos(rad)), center.y + (int)( W * sin(rad)))
    };

    wxColour wx_color(color.r, color.g, color.b);
    wxColour border((unsigned char)(color.r * 0.6),
                    (unsigned char)(color.g * 0.6),
                    (unsigned char)(color.b * 0.6));

    dc.SetBrush(wxBrush(wx_color, wxBRUSHSTYLE_SOLID));
    dc.SetPen(wxPen(border, 1));
    dc.DrawPolygon(3, pts);
}

bool shipcolor_pi::RenderOverlay(wxDC& dc, PlugIn_ViewPort* vp) {
    if (!vp) return false;

    ArrayOfPlugIn_AIS_Targets* targets = GetAISTargetArray();
    if (!targets) return true;

    for (size_t i = 0; i < targets->GetCount(); i++) {
        PlugIn_AIS_Target* t = targets->Item(i);
        if (!t) continue;
        if (t->Lat > 90.0 || t->Lat < -90.0) continue;
        if (t->Lon > 180.0 || t->Lon < -180.0) continue;

        wxPoint screen_pos;
        GetCanvasPixLL(vp, &screen_pos, t->Lat, t->Lon);

        if (screen_pos.x < -50 || screen_pos.x > vp->pix_width  + 50) continue;
        if (screen_pos.y < -50 || screen_pos.y > vp->pix_height + 50) continue;

        ShipCategory cat   = GetCategory(t->ShipType);
        ShipColor    color = GetColor(cat);

        double heading = t->HDG;
        if (heading > 360.0 || heading < 0.0) heading = t->COG;
        if (heading > 360.0 || heading < 0.0) heading = 0.0;

        double scale = vp->view_scale_ppm * 500.0;
        if (scale < 0.8) scale = 0.8;
        if (scale > 3.0) scale = 3.0;

        DrawFilledTriangle(dc, screen_pos, heading, scale, color);
    }

    delete targets;
    return true;
}
