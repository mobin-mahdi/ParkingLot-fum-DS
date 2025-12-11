#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_draw.H>

#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <sstream>

// ------------------ Global state (like React useState) ------------------

static int g_numStacks = 3;
static int g_stackCapacity = 5;
static bool g_initialized = false;

static std::vector<std::vector<int>> g_stacks;       // stacks[i], top at index 0
static std::vector<int> g_entranceQueue;             // front at index 0
static int g_nextCarId = 1;

// inputs
static Fl_Input* g_numStacksInput   = nullptr;
static Fl_Input* g_capacityInput    = nullptr;
static Fl_Input* g_addCarIdInput    = nullptr;  // Add car (optional)
static Fl_Input* g_stackIndexInput  = nullptr;  // For park/sort only
static Fl_Input* g_findCarInput     = nullptr;  // Find
static Fl_Input* g_moveCarIdInput   = nullptr;  // Move car
static Fl_Input* g_targetStackInput = nullptr;  // Move target stack
static Fl_Input* g_exitCarIdInput   = nullptr;  // Exit by car ID
static Fl_Input* g_sortStackInput   = nullptr;

static std::string g_message;
static std::string g_messageType = "info";
static Fl_Box* g_messageBox = nullptr;   // if not already defined

// ------------------ Visualization widget ------------------

class ParkingView : public Fl_Widget {
public:
    ParkingView(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Widget(X, Y, W, H, L) {}

    void draw() override {
        fl_push_clip(x(), y(), w(), h());
        fl_rectf(x(), y(), w(), h(), fl_rgb_color(245, 248, 255));

        int margin = 10;
        int innerX = x() + margin;
        int innerY = y() + margin;
        int innerW = w() - 2 * margin;
        int innerH = h() - 2 * margin;

        // Entrance queue
        int queueH = 80;
        fl_draw_box(FL_UP_BOX, innerX, innerY, innerW, queueH, fl_rgb_color(255, 255, 255));
        fl_color(0);
        fl_draw("Entrance Queue", innerX + 5, innerY + 15);

        int qx   = innerX + 10;
        int qy   = innerY + 30;
        int boxW = 40;
        int boxH = 30;
        int gap  = 6;

        if (g_entranceQueue.empty()) {
            fl_color(120);
            fl_draw("Queue is empty", qx, qy + 20);
        } else {
            fl_color(120);
            fl_draw("Front →", qx, qy - 5);
            qx += 60;
            for (size_t i = 0; i < g_entranceQueue.size(); ++i) {
                fl_color(fl_rgb_color(198, 210, 255));
                fl_rectf(qx, qy, boxW, boxH);
                fl_color(fl_rgb_color(80, 80, 160));
                fl_rect(qx, qy, boxW, boxH);

                std::string id = std::to_string(g_entranceQueue[i]);
                fl_color(fl_rgb_color(20, 20, 80));
                fl_draw(id.c_str(), qx + 10, qy + 20);

                qx += boxW + gap;
                if (i < g_entranceQueue.size() - 1) {
                    fl_color(120);
                    fl_draw("→", qx, qy + 20);
                    qx += 15;
                }
            }
            fl_color(120);
            fl_draw("← Rear", qx, qy - 5);
        }

        // Stacks
        int stacksY = innerY + queueH + 20;
        int stacksH = innerH - queueH - 30;

        if (!g_initialized) {
            fl_color(120);
            fl_draw("Parking lot is not initialized.", innerX + 10, stacksY + 20);
            fl_pop_clip();
            return;
        }

        int n = g_numStacks;
        if (n <= 0) {
            fl_pop_clip();
            return;
        }

        int laneW      = innerW / n;
        int laneMargin = 10;

        for (int i = 0; i < n; ++i) {
            int laneX      = innerX + i * laneW + laneMargin;
            int laneWInner = laneW - 2 * laneMargin;

            fl_color(fl_rgb_color(255, 255, 255));
            fl_rectf(laneX, stacksY, laneWInner, stacksH);
            fl_color(fl_rgb_color(200, 200, 220));
            fl_rect(laneX, stacksY, laneWInner, stacksH);

            std::string label = "Stack " + std::to_string(i + 1);
            fl_color(0);
            fl_draw(label.c_str(), laneX + 5, stacksY + 15);

            if (i < (int)g_stacks.size()) {
                auto &st = g_stacks[i];
                int maxCars = g_stackCapacity > 0 ? g_stackCapacity : (int)st.size();
                int availH  = stacksH - 30;
                int carH    = maxCars > 0 ? std::max(18, availH / maxCars - 4) : 20;
                int startY  = stacksY + 25;

                for (size_t pos = 0; pos < st.size(); ++pos) {
                    int carY = startY + pos * (carH + 4);
                    int carX = laneX + 10;
                    int carW = laneWInner - 20;

                    if (pos == 0) {
                        fl_color(fl_rgb_color(187, 247, 208));
                        fl_rectf(carX, carY, carW, carH);
                        fl_color(fl_rgb_color(21, 128, 61));
                    } else {
                        fl_color(fl_rgb_color(209, 250, 229));
                        fl_rectf(carX, carY, carW, carH);
                        fl_color(fl_rgb_color(22, 101, 52));
                    }
                    fl_rect(carX, carY, carW, carH);

                    std::string cid = std::to_string(st[pos]);
                    fl_color(fl_rgb_color(22, 101, 52));
                    fl_draw(cid.c_str(), carX + 5, carY + carH - 5);
                }
            }
        }

        fl_pop_clip();
    }
};

static ParkingView* g_parkingView = nullptr;

// ------------------ Message helper ------------------

void showMessage(const std::string& msg, const std::string& type = "info") {
    // Store in globals so the text stays alive after the function returns
    g_message     = msg;
    g_messageType = type;

    if (!g_messageBox) return;

    Fl_Color bg, fg;
    if (type == "success") {
        bg = fl_rgb_color(220, 252, 231);
        fg = fl_rgb_color(22, 101, 52);
    } else if (type == "error") {
        bg = fl_rgb_color(254, 226, 226);
        fg = fl_rgb_color(153, 27, 27);
    } else if (type == "warning") {
        bg = fl_rgb_color(254, 249, 195);
        fg = fl_rgb_color(133, 77, 14);
    } else { // info
        bg = fl_rgb_color(219, 234, 254);
        fg = fl_rgb_color(30, 64, 175);
    }

    // IMPORTANT: use g_message.c_str(), not msg.c_str(), and NEVER assign to g_messageBox
    g_messageBox->label(g_message.c_str());
    g_messageBox->box(FL_FLAT_BOX);
    g_messageBox->color(bg);
    g_messageBox->labelcolor(fg);
    g_messageBox->redraw();
}
// ------------------ Helpers ------------------

bool carExists(int carId) {
    if (std::find(g_entranceQueue.begin(), g_entranceQueue.end(), carId)
        != g_entranceQueue.end())
        return true;

    for (auto &st : g_stacks) {
        if (std::find(st.begin(), st.end(), carId) != st.end())
            return true;
    }
    return false;
}

// ------------------ Callbacks ------------------

void cb_initialize(Fl_Widget*, void*) {
    int n = std::atoi(g_numStacksInput->value());
    int c = std::atoi(g_capacityInput->value());

    if (n < 1 || c < 1) {
        showMessage("Please enter valid positive numbers", "error");
        return;
    }

    g_numStacks      = n;
    g_stackCapacity  = c;
    g_stacks.assign(g_numStacks, std::vector<int>());
    g_entranceQueue.clear();
    g_nextCarId      = 1;
    g_initialized    = true;

    showMessage("Parking lot initialized", "success");
    if (g_parkingView) g_parkingView->redraw();
}

void cb_addCar(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }

    std::string val = g_addCarIdInput->value();
    int carId;
    if (val.empty()) {
        carId = g_nextCarId++;
    } else {
        carId = std::atoi(val.c_str());
    }

    if (carExists(carId)) {
        showMessage("Error: car already exists in system", "error");
        return;
    }

    g_entranceQueue.push_back(carId);
    g_addCarIdInput->value("");
    showMessage("Car added to entrance queue", "success");
    if (g_parkingView) g_parkingView->redraw();
}

void cb_parkFirst(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }
    if (g_entranceQueue.empty()) {
        showMessage("Entrance queue is empty", "error");
        return;
    }

    int carId = g_entranceQueue.front();
    g_entranceQueue.erase(g_entranceQueue.begin());

    for (int i = 0; i < g_numStacks; ++i) {
        if ((int)g_stacks[i].size() < g_stackCapacity) {
            g_stacks[i].insert(g_stacks[i].begin(), carId);
            showMessage("Car parked in stack " + std::to_string(i + 1), "success");
            if (g_parkingView) g_parkingView->redraw();
            return;
        }
    }

    showMessage("Parking full, car cannot be parked", "error");
    if (g_parkingView) g_parkingView->redraw();
}

void cb_parkSpecific(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }
    if (g_entranceQueue.empty()) {
        showMessage("Entrance queue is empty", "error");
        return;
    }

    int idx = std::atoi(g_stackIndexInput->value()) - 1;
    if (idx < 0 || idx >= g_numStacks) {
        showMessage("Invalid stack number", "error");
        return;
    }
    if ((int)g_stacks[idx].size() >= g_stackCapacity) {
        showMessage("Selected stack is full", "error");
        return;
    }

    int carId = g_entranceQueue.front();
    g_entranceQueue.erase(g_entranceQueue.begin());
    g_stacks[idx].insert(g_stacks[idx].begin(), carId);

    showMessage("Car parked in stack " + std::to_string(idx + 1), "success");
    g_stackIndexInput->value("");
    if (g_parkingView) g_parkingView->redraw();
}

void cb_findCar(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }

    int carId = std::atoi(g_findCarInput->value());
    for (int i = 0; i < g_numStacks; ++i) {
        auto &st = g_stacks[i];
        auto it  = std::find(st.begin(), st.end(), carId);
        if (it != st.end()) {
            int pos = (int)(it - st.begin()) + 1; // 1-based from top
            std::ostringstream oss;
            oss << "Car " << carId << " found in stack " << (i + 1)
                << " at position " << pos << " from top";
            showMessage(oss.str(), "success");
            return;
        }
    }

    showMessage("Car not found in any stack", "error");
}

void cb_moveCar(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }

    std::string carText = g_moveCarIdInput->value();
    if (carText.empty()) {
        showMessage("Please enter the Car ID to move", "error");
        return;
    }
    int carId = std::atoi(carText.c_str());

    std::string targetText = g_targetStackInput->value();
    if (targetText.empty()) {
        showMessage("Please enter the target stack number", "error");
        return;
    }
    int target = std::atoi(targetText.c_str()) - 1;

    if (target < 0 || target >= g_numStacks) {
        showMessage("Invalid target stack number", "error");
        return;
    }

    // find source stack
    int source = -1;
    for (int i = 0; i < g_numStacks; ++i) {
        auto &st = g_stacks[i];
        auto it  = std::find(st.begin(), st.end(), carId);
        if (it != st.end()) {
            source = i;
            break;
        }
    }

    if (source == -1) {
        showMessage("Car not found in any stack", "error");
        return;
    }

    auto &src = g_stacks[source];
    auto &dst = g_stacks[target];

    if (src.empty() || src.front() != carId) {
        showMessage("Cannot move: car is not at the top of its stack", "error");
        return;
    }

    if ((int)dst.size() >= g_stackCapacity) {
        showMessage("Target stack is full", "error");
        return;
    }

    src.erase(src.begin());
    dst.insert(dst.begin(), carId);

    std::ostringstream oss;
    oss << "Car " << carId << " moved from stack " << (source + 1)
        << " to stack " << (target + 1);
    showMessage(oss.str(), "success");

    g_moveCarIdInput->value("");
    g_targetStackInput->value("");
    if (g_parkingView) g_parkingView->redraw();
}

void cb_exitCar(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }

    std::string carStr = g_exitCarIdInput->value();
    if (carStr.empty()) {
        showMessage("Please enter the Car ID to exit", "error");
        return;
    }
    int carId = std::atoi(carStr.c_str());

    // 1) try to remove if car is at top of some stack
    for (int i = 0; i < g_numStacks; ++i) {
        auto &st = g_stacks[i];
        if (!st.empty() && st.front() == carId) {
            st.erase(st.begin());
            std::ostringstream oss;
            oss << "Car " << carId << " exited from top of stack " << (i + 1);
            showMessage(oss.str(), "success");
            g_exitCarIdInput->value("");
            if (g_parkingView) g_parkingView->redraw();
            return;
        }
    }

    // 2) check if exists deeper
    bool exists = false;
    for (auto &st : g_stacks) {
        if (std::find(st.begin(), st.end(), carId) != st.end()) {
            exists = true;
            break;
        }
    }

    if (exists) {
        showMessage("Cannot remove: car is not at the top of its stack", "error");
    } else {
        showMessage("Car not found in any stack", "error");
    }
}
void cb_sortStack(Fl_Widget*, void*) {
    if (!g_initialized) {
        showMessage("Initialize parking lot first", "error");
        return;
    }

    if (!g_sortStackInput) {
        showMessage("Internal error: sort input not created", "error");
        return;
    }

    std::string s = g_sortStackInput->value();
    if (s.empty()) {
        showMessage("Please enter stack number to sort", "error");
        return;
    }

    int idx = std::atoi(s.c_str()) - 1;
    if (idx < 0 || idx >= g_numStacks) {
        showMessage("Invalid stack number", "error");
        return;
    }

    auto &st = g_stacks[idx];
    std::sort(st.begin(), st.end());
    showMessage("Sorted stack " + std::to_string(idx + 1), "success");
    if (g_parkingView) g_parkingView->redraw();
}

void cb_reset(Fl_Widget*, void*) {
    g_initialized = false;
    g_stacks.clear();
    g_entranceQueue.clear();
    g_nextCarId = 1;
    showMessage("System reset. Please initialize again.", "info");
    if (g_parkingView) g_parkingView->redraw();
}

// ------------------ main (GUI) ------------------

int main(int argc, char** argv) {
    Fl_Window* win = new Fl_Window(1300, 900, "Parking Lot Manager (FLTK)");

    // Header
    Fl_Group* header = new Fl_Group(10, 10, 1280, 60);
    {
        Fl_Box* title = new Fl_Box(20, 15, 400, 25, "Parking Lot Manager");
        title->labelfont(FL_BOLD);
        title->labelsize(20);

        Fl_Box* subtitle = new Fl_Box(20, 40, 400, 20, "DSA Project - Parking Lot GUI");
        subtitle->labelsize(12);
        subtitle->labelcolor(fl_rgb_color(100, 100, 120));

        Fl_Button* resetBtn = new Fl_Button(1120, 20, 150, 30, "Reset System");
        resetBtn->callback(cb_reset);
    }
    header->end();

    // Message
    g_messageBox = new Fl_Box(10, 80, 1280, 40, "");
    g_messageBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
    showMessage("Welcome! Initialize the parking lot to begin.", "info");

    // Left scrollable control panel
    int leftX = 10;
    int leftY = 130;
    int leftW = 360;
    int leftH = 740;

    Fl_Scroll* scroll = new Fl_Scroll(leftX, leftY, leftW, leftH);
    scroll->box(FL_NO_BOX);
    scroll->begin();
    {
        int x = leftX;
        int y = leftY + 10;

        // Setup
        Fl_Box* setupLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Parking Lot Setup");
        setupLabel->labelfont(FL_BOLD);
        y += 25;

        g_numStacksInput = new Fl_Input(x + 170, y, 80, 25, "Stacks:");
        g_numStacksInput->value("3");
        y += 30;

        g_capacityInput = new Fl_Input(x + 170, y, 80, 25, "Capacity:");
        g_capacityInput->value("5");
        y += 35;

        Fl_Button* initBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Initialize Parking Lot");
        initBtn->callback(cb_initialize);
        y += 45;

        // Add Car
        Fl_Box* addLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Add Car to Entrance");
        addLabel->labelfont(FL_BOLD);
        y += 25;

        Fl_Box* addCarIdLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Car ID (optional, empty = auto):");
        addCarIdLabel->labelsize(12);
        addCarIdLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        y += 22;

        g_addCarIdInput = new Fl_Input(x + 10, y, leftW - 20, 25);
        y += 35;

        Fl_Button* addCarBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Add to Entrance Queue");
        addCarBtn->callback(cb_addCar);
        y += 45;

        // Park Car
        Fl_Box* parkLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Park Car");
        parkLabel->labelfont(FL_BOLD);
        y += 25;

        Fl_Button* parkFirstBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Park in First Available");
        parkFirstBtn->callback(cb_parkFirst);
        y += 35;

        g_stackIndexInput = new Fl_Input(x + 170, y, 80, 25, "Stack #:");
        y += 30;

        Fl_Button* parkSpecificBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Park in Specific Stack");
        parkSpecificBtn->callback(cb_parkSpecific);
        y += 45;

        // Find Car
        Fl_Box* findLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Find Car");
        findLabel->labelfont(FL_BOLD);
        y += 25;

        g_findCarInput = new Fl_Input(x + 10, y, leftW - 20, 25, "Car ID:");
        y += 35;

        Fl_Button* findBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Find Car");
        findBtn->callback(cb_findCar);
        y += 45;

        // Move Car
        Fl_Box* moveLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Move Car to Another Stack");
        moveLabel->labelfont(FL_BOLD);
        y += 25;

        Fl_Box* moveCarLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Car ID to Move:");
        moveCarLabel->labelsize(12);
        moveCarLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        y += 22;

        g_moveCarIdInput = new Fl_Input(x + 10, y, leftW - 20, 25);
        y += 35;

        Fl_Box* targetLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Target Stack #:");
        targetLabel->labelsize(12);
        targetLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        y += 22;

        g_targetStackInput = new Fl_Input(x + 10, y, leftW - 20, 25);
        y += 35;

        Fl_Button* moveBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Move Car");
        moveBtn->callback(cb_moveCar);
        y += 45;

        // Sort
        Fl_Box* sortLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Stack # to Sort:");
        sortLabel->labelsize(12);
        sortLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        y += 22;

        g_sortStackInput = new Fl_Input(x + 10, y, leftW - 20, 25);
        y += 35;

        Fl_Button* sortBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Sort Stack");
        sortBtn->callback(cb_sortStack);
        y += 40;

        //exit
        Fl_Box* exitLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Exit Car from Top (by Car ID)");
        exitLabel->labelfont(FL_BOLD);
        y += 25;

        Fl_Box* exitCarIdLabel = new Fl_Box(x + 10, y, leftW - 20, 20, "Car ID to Exit:");
        exitCarIdLabel->labelsize(12);
        exitCarIdLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        y += 22;

        g_exitCarIdInput = new Fl_Input(x + 10, y, leftW - 20, 25);
        y += 35;

        Fl_Button* exitBtn = new Fl_Button(x + 10, y, leftW - 20, 30, "Exit Car from Top");
        exitBtn->callback(cb_exitCar);
        y += 40;

        // scroll's content may extend beyond leftH; scrollbar appears automatically
    }
    scroll->end();

    // Right visualization area
    g_parkingView = new ParkingView(390, 130, 900, 740, "Parking Visualization");

    win->end();
    win->resizable(g_parkingView);
    win->show(argc, argv);

    return Fl::run();
}
