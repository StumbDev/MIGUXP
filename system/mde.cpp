
#include <ncurses.h>
#include <menu.h>
#include <panel.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

class Window {
private:
    WINDOW* win;
    PANEL* panel;
    int height, width, starty, startx;
    std::string title;

public:
    Window(int h, int w, int y, int x, const std::string& t) 
        : height(h), width(w), starty(y), startx(x), title(t) {
        win = newwin(height, width, starty, startx);
        box(win, 0, 0);
        panel = new_panel(win);
        mvwprintw(win, 0, (width - title.length()) / 2, title.c_str());
        wrefresh(win);
    }

    ~Window() {
        del_panel(panel);
        delwin(win);
    }

    WINDOW* get() { return win; }
    void refresh() { wrefresh(win); }
    void clear() { wclear(win); box(win, 0, 0); }
    void hide() { hide_panel(panel); }
    void show() { show_panel(panel); }
};

class MDEApplication {
protected:
    std::unique_ptr<Window> window;
    bool running = true;

public:
    virtual void init() = 0;
    virtual void handle_input(int ch) = 0;
    virtual void update() = 0;
    virtual bool is_running() { return running; }
    virtual ~MDEApplication() = default;
};

class TextEditor : public MDEApplication {
private:
    std::vector<std::string> buffer;
    int cursor_y = 1, cursor_x = 1;
    
public:
    void init() override {
        window = std::make_unique<Window>(15, 40, 2, 2, "MDE Text Editor");
        buffer.push_back("");
    }

    void handle_input(int ch) override {
        switch(ch) {
            case KEY_UP:
                if (cursor_y > 1) cursor_y--;
                break;
            case KEY_DOWN:
                if (cursor_y < buffer.size()) cursor_y++;
                break;
            case KEY_LEFT:
                if (cursor_x > 1) cursor_x--;
                break;
            case KEY_RIGHT:
                if (cursor_x < buffer[cursor_y-1].length() + 1) cursor_x++;
                break;
            case 27: // ESC
                running = false;
                break;
            case '\n':
                buffer.insert(buffer.begin() + cursor_y, "");
                cursor_y++;
                cursor_x = 1;
                break;
            default:
                if (isprint(ch)) {
                    if (cursor_x > buffer[cursor_y-1].length())
                        buffer[cursor_y-1] += ch;
                    else
                        buffer[cursor_y-1].insert(cursor_x-1, 1, ch);
                    cursor_x++;
                }
        }
    }

    void update() override {
        window->clear();
        for (size_t i = 0; i < buffer.size(); i++) {
            mvwprintw(window->get(), i+1, 1, buffer[i].c_str());
        }
        wmove(window->get(), cursor_y, cursor_x);
        window->refresh();
    }
};

class FileManager : public MDEApplication {
private:
    std::vector<std::string> files;
    int selected = 0;

public:
    void init() override {
        window = std::make_unique<Window>(15, 30, 2, 45, "MDE File Manager");
        // TODO: Add actual file listing
        files = {"file1.txt", "file2.txt", "directory1/"};
    }

    void handle_input(int ch) override {
        switch(ch) {
            case KEY_UP:
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
                if (selected < files.size() - 1) selected++;
                break;
            case 27: // ESC
                running = false;
                break;
        }
    }

    void update() override {
        window->clear();
        for (size_t i = 0; i < files.size(); i++) {
            if (i == selected)
                wattron(window->get(), A_REVERSE);
            mvwprintw(window->get(), i+1, 1, files[i].c_str());
            if (i == selected)
                wattroff(window->get(), A_REVERSE);
        }
        window->refresh();
    }
};

class MDE {
private:
    std::vector<std::unique_ptr<MDEApplication>> apps;
    std::unique_ptr<Window> menuWindow;
    int current_app = 0;
    bool running = true;

    void init_colors() {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
    }

public:
    void init() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(1);
        init_colors();

        menuWindow = std::make_unique<Window>(3, COLS, 0, 0, "MDE Menu");
        wbkgd(menuWindow->get(), COLOR_PAIR(1));
        mvwprintw(menuWindow->get(), 1, 1, "F1:Editor F2:Files F10:Exit");
        menuWindow->refresh();

        // Initialize applications
        apps.push_back(std::make_unique<TextEditor>());
        apps.push_back(std::make_unique<FileManager>());

        for (auto& app : apps) {
            app->init();
        }
    }

    void run() {
        while (running) {
            int ch = getch();
            
            switch(ch) {
                case KEY_F(1):
                    current_app = 0;
                    apps[current_app]->init();
                    break;
                case KEY_F(2):
                    current_app = 1;
                    apps[current_app]->init();
                    break;
                case KEY_F(10):
                    running = false;
                    break;
                default:
                    if (apps[current_app]->is_running())
                        apps[current_app]->handle_input(ch);
            }

            if (apps[current_app]->is_running())
                apps[current_app]->update();
            
            update_panels();
            doupdate();
        }
    }

    ~MDE() {
        endwin();
    }
};

int main() {
    MDE desktop;
    desktop.init();
    desktop.run();
    return 0;
}