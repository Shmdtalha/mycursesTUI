#pragma once

#include <memory>
#include <vector>
#include <map>
#include <ncurses/ncurses.h>
#include <ncurses/form.h>

// Forward declarations
class FormField;
class FormEventListener;
class FieldValidator;
class Command;

// Field attributes class
class FieldAttributes {
public:
    chtype foreground;
    chtype background;
    char padding;

    FieldAttributes() : foreground(A_NORMAL), background(A_NORMAL), padding('_') {}
    
    void setColors(chtype fg, chtype bg) {
        foreground = fg;
        background = bg;
    }
    
    void setPadding(char pad) {
        padding = pad;
    }
};

// Field validator interface
class FieldValidator {
public:
    virtual ~FieldValidator() = default;
    virtual bool validate(const FormField& field) = 0;
    virtual bool validateChar(char c) = 0;
};

// Form field class
class FormField {
private:
    std::unique_ptr<FieldValidator> validator;
    FieldAttributes attributes;
    FIELD* field;

public:
    FormField(std::unique_ptr<FieldValidator> val) 
        : validator(std::move(val)), field(nullptr) {}
    
    bool validate() {
        return validator->validate(*this);
    }
    
    void draw() {
        if (field) {
            set_field_back(field, attributes.background);
            set_field_fore(field, attributes.foreground);
            set_field_pad(field, attributes.padding);
        }
    }
    
    void handleInput(int key) {
        if (field) {
            form_driver(field->form, key);
        }
    }
    
    void setField(FIELD* f) { field = f; }
    FIELD* getField() const { return field; }
};

// Form event listener interface
class FormEventListener {
public:
    virtual ~FormEventListener() = default;
    virtual void onFormInit() = 0;
    virtual void onFormTermination() = 0;
    virtual void onFieldInit(FormField& field) = 0;
    virtual void onFieldTermination(FormField& field) = 0;
};

// Form manager class
class FormManager {
private:
    std::vector<FormField> fields;
    std::vector<std::shared_ptr<FormEventListener>> listeners;

public:
    void attachListener(std::shared_ptr<FormEventListener> listener) {
        listeners.push_back(listener);
    }
    
    void detachListener(std::shared_ptr<FormEventListener> listener) {
        auto it = std::find(listeners.begin(), listeners.end(), listener);
        if (it != listeners.end()) {
            listeners.erase(it);
        }
    }
    
    void notifyFormInit() {
        for (auto& listener : listeners) {
            listener->onFormInit();
        }
    }
    
    void notifyFormTermination() {
        for (auto& listener : listeners) {
            listener->onFormTermination();
        }
    }
    
    void addField(const FormField& field) {
        fields.push_back(field);
    }
};

// Window manager class
class WindowManager {
private:
    WINDOW* mainWindow;
    WINDOW* subWindow;

public:
    WindowManager() : mainWindow(nullptr), subWindow(nullptr) {}
    
    void createSubWindow(int rows, int cols) {
        if (mainWindow) {
            subWindow = derwin(mainWindow, rows, cols, 1, 1);
        }
    }
    
    void setBackground(chtype color) {
        if (mainWindow) {
            wbkgd(mainWindow, color);
        }
        if (subWindow) {
            wbkgd(subWindow, color);
        }
    }
    
    void refresh() {
        if (mainWindow) wrefresh(mainWindow);
        if (subWindow) wrefresh(subWindow);
    }
    
    void setMainWindow(WINDOW* win) { mainWindow = win; }
    WINDOW* getMainWindow() const { return mainWindow; }
    WINDOW* getSubWindow() const { return subWindow; }
};

// Field manager class
class FieldManager {
private:
    FormField* currentField;
    std::vector<FormField*> fields;

public:
    FieldManager() : currentField(nullptr) {}
    
    void mapFields(std::vector<FormField>& fieldList) {
        fields.clear();
        for (auto& field : fieldList) {
            fields.push_back(&field);
        }
        if (!fields.empty()) {
            currentField = fields[0];
        }
    }
    
    bool validateField(FormField* field) {
        return field->validate();
    }
    
    bool moveNext() {
        if (fields.empty()) return false;
        
        auto it = std::find(fields.begin(), fields.end(), currentField);
        if (it != fields.end() && std::next(it) != fields.end()) {
            currentField = *std::next(it);
            return true;
        }
        return false;
    }
    
    bool movePrevious() {
        if (fields.empty()) return false;
        
        auto it = std::find(fields.begin(), fields.end(), currentField);
        if (it != fields.begin()) {
            currentField = *std::prev(it);
            return true;
        }
        return false;
    }
    
    FormField* getCurrentField() const { return currentField; }
};

// Command interface
class Command {
public:
    virtual ~Command() = default;
    virtual int execute() = 0;
    virtual void undo() = 0;
};

// Key handler class
class KeyHandler {
private:
    std::map<int, std::shared_ptr<Command>> keyMap;

public:
    void registerCommand(int key, std::shared_ptr<Command> cmd) {
        keyMap[key] = cmd;
    }
    
    Command* handleKey(int key) {
        auto it = keyMap.find(key);
        if (it != keyMap.end()) {
            return it->second.get();
        }
        return nullptr;
    }
};

// Main form class
class MycursesForm {
public:
    std::unique_ptr<FormManager> formManager;
    std::unique_ptr<WindowManager> windowManager;
    std::unique_ptr<FieldManager> fieldManager;
    std::unique_ptr<KeyHandler> keyHandler;

    MycursesForm(bool withFrame) {
        formManager = std::make_unique<FormManager>();
        windowManager = std::make_unique<WindowManager>();
        fieldManager = std::make_unique<FieldManager>();
        keyHandler = std::make_unique<KeyHandler>();
        
        // Initialize ncurses window
        windowManager->setMainWindow(stdscr);
        if (withFrame) {
            windowManager->createSubWindow(LINES-4, COLS-4);
        }
    }
    
    void initialize(FormField fields[]) {
        std::vector<FormField> fieldList;
        for (int i = 0; fields[i].getField() != nullptr; ++i) {
            fieldList.push_back(fields[i]);
        }
        fieldManager->mapFields(fieldList);
    }
    
    void process() {
        int ch;
        while ((ch = getch()) != KEY_F(1)) {
            handleKey(ch);
        }
    }
    
    void handleKey(int key) {
        if (auto cmd = keyHandler->handleKey(key)) {
            cmd->execute();
        }
    }
}; 