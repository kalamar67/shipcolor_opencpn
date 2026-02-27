#include "shipcolor_pi.h"
#include <cmath>

// OpenCPN plugin export macro
DECL_EXP opencpn_plugin* create_pi(void* ppimgr) {
    return new shipcolor_pi(ppimgr);
}
DECL_EXP void destroy_pi(opencpn_plugin* p) {
    delete p;
}

// -------------------------------------------------------
// Constructor / Destructor
// -------------------------------------------------------
shipcolor_pi::shipcolor_pi(void* ppimgr)
    : opencpn_plugin_18(ppimgr)
{
}

shipcolor_pi::~shipcolor_pi() {}

// -------------------------------------------------------
// OpenCPN API Kimlik Bilgileri
// -------------------------------------------------------
int  shipcolor_pi::GetAPIVersionMajor()    { return MY_API_VERSION_MAJOR; }
int  shipcolor_pi::GetAPIVersionMinor()    { return MY_API_VERSION_MINOR; }
int  shipcolor_pi::GetPlugInVersionMajor() { return SHIPCOLOR_PI_VERSION_MAJOR; }
int  shipcolor_pi::GetPlugInVersionMinor() { return SHIPCOLOR_PI_VERSION_MINOR; }

wxString shipcolor_pi::GetCommonName() {
    return wxT("ShipColor");
}
wxString shipcolor_pi::GetShortDescription() {
    return wxT("Gemi tipine göre renkli AIS hedefleri");
}
wxString shipcolor_pi::GetLongDescription() {
    return wxT("AIS Type 5 ship_type alanına göre her gemi tipini\n"
               "farklı renkli dolu üçgen ile gösterir.\n\n"
               "Tanker: Kırmızı\n"
               "Kargo: Kahverengi\n"
               "Yolcu/Feribot: Mavi\n"
               "Balıkçı: Sarı\n"
               "Hız Teknesi: Turuncu\n"
               "Römorkör: Mor\n"
               "Savaş Gemisi: Siyah\n"
               "Diğer: Gri");
}

wxBitmap* shipcolor_pi::GetPlugInBitmap() {
    return &m_logo;
}

// -------------------------------------------------------
// Init / DeInit
// -------------------------------------------------------
int shipcolor_pi::Init() {
    // Render overlay için capability flag
    return (WANTS_OVERLAY_CALLBACK |
            WANTS_OPENGL_OVERLAY_CALLBACK |
            WANTS_CURSOR_LATLON);
}

bool shipcolor_pi::DeInit() {
    return true;
}

// -------------------------------------------------------
// AIS Ship Type → Kategori
// -------------------------------------------------------
ShipCategory shipcolor_pi::GetCategory(int ship_type) {
    if (ship_type == 30)              return SC_FISHING;
    if (ship_type >= 21 && ship_type <= 24) return SC_TOWING;
    if (ship_type == 31 || ship_type == 32) return SC_SPEEDBOAT;
    if (ship_type == 35)              return SC_MILITARY;
    if (ship_type >= 60 && ship_type <= 69) return SC_PASSENGER;
    if (ship_type >= 70 && ship_type <= 79) return SC_CARGO;
    if (ship_type >= 80 && ship_type <= 89) return SC_TANKER;
    if (ship_type > 0)                return SC_OTHER;
    return SC_UNKNOWN;
}

// -------------------------------------------------------
// Kategori → RGB Renk
// -------------------------------------------------------
ShipColor shipcolor_pi::GetColor(ShipCategory cat) {
    switch (cat) {
        case SC_TANKER:    return {220,  30,  30};  // Kırmızı
        case SC_CARGO:     return {139,  90,  43};  // Kahverengi
        case SC_PASSENGER: return { 30, 100, 220};  // Mavi
        case SC_FISHING:   return {230, 210,  20};  // Sarı
        case SC_SPEEDBOAT: return {230, 130,  20};  // Turuncu
        case SC_TOWING:    return {140,  60, 180};  // Mor
        case SC_MILITARY:  return { 20,  20,  20};  // Siyah
        case SC_OTHER:     return {150, 150, 150};  // Gri
        default:           return {180, 180, 180};  // Açık gri (unknown)
    }
}

// -------------------------------------------------------
// Dolu Üçgen Çizimi (heading yönünde)
// -------------------------------------------------------
void shipcolor_pi::DrawFilledTriangle(wxDC& dc,
                                       wxPoint center,
                                       double heading_deg,
                                       double scale,
                                       ShipColor color)
{
    // OpenCPN varsayılan AIS üçgen boyutu ile aynı
    const double H = 12.0 * scale;  // üçgen yüksekliği (px)
    const double W =  7.0 * scale;  // üçgen genişliği (px)

    double rad = heading_deg * M_PI / 180.0;

    // Üçgen noktaları (yerel koordinat, kuzey = yukarı)
    // Burun (ön)
    double nx =  H * sin(rad);
    double ny = -H * cos(rad);
    // Sol kanat
    double lx = -W * cos(rad);
    double ly = -W * sin(rad);
    // Sağ kanat
    double rx =  W * cos(rad);
    double ry =  W * sin(rad);

    wxPoint pts[3] = {
        wxPoint(center.x + (int)nx, center.y + (int)ny),
        wxPoint(center.x + (int)lx, center.y + (int)ly),
        wxPoint(center.x + (int)rx, center.y + (int)ry)
    };

    wxColour wx_color(color.r, color.g, color.b);

    // Dolgu
    dc.SetBrush(wxBrush(wx_color, wxBRUSHSTYLE_SOLID));
    // Kenar — aynı renk, biraz daha koyu
    wxColour border(
        (unsigned char)(color.r * 0.6),
        (unsigned char)(color.g * 0.6),
        (unsigned char)(color.b * 0.6)
    );
    dc.SetPen(wxPen(border, 1));
    dc.DrawPolygon(3, pts);
}

// -------------------------------------------------------
// Ana Render Callback
// -------------------------------------------------------
bool shipcolor_pi::RenderOverlay(wxDC& dc, PlugIn_ViewPort* vp) {
    if (!vp) return false;

    // OpenCPN AIS hedef listesini al
    ArrayOfPlugIn_AIS_Targets* targets = GetAISTargetArray();
    if (!targets) return true;

    for (size_t i = 0; i < targets->GetCount(); i++) {
        PlugIn_AIS_Target* t = targets->Item(i);
        if (!t) continue;

        // Geçersiz konum filtrele
        if (t->Lat > 90.0 || t->Lat < -90.0) continue;
        if (t->Lon > 180.0 || t->Lon < -180.0) continue;

        // Ekran koordinatına çevir
        wxPoint screen_pos;
        GetCanvasPixLL(vp, &screen_pos, t->Lat, t->Lon);

        // Görünür alan kontrolü
        if (screen_pos.x < -50 || screen_pos.x > vp->pix_width  + 50) continue;
        if (screen_pos.y < -50 || screen_pos.y > vp->pix_height + 50) continue;

        // Ship type → renk
        ShipCategory cat   = GetCategory(t->ShipType);
        ShipColor    color = GetColor(cat);

        // Heading: COG veya True Heading
        double heading = t->HDG;
        if (heading > 360.0 || heading < 0.0)
            heading = t->COG;  // HDG yoksa COG kullan
        if (heading > 360.0 || heading < 0.0)
            heading = 0.0;

        // Zoom'a göre ölçek
        double scale = vp->view_scale_ppm * 500.0;
        if (scale < 0.8) scale = 0.8;
        if (scale > 3.0) scale = 3.0;

        DrawFilledTriangle(dc, screen_pos, heading, scale, color);
    }

    delete targets;
    return true;
}

bool shipcolor_pi::RenderOverlayMultiCanvas(wxDC& dc, 
                                             PlugIn_ViewPort* vp, 
                                             int canvas_index) {
    return RenderOverlay(dc, vp);
}
