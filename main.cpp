#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

// ---- Bank Account Classes ----
class Account {
protected:
    std::string owner;
    double balance;

public:
    Account(std::string name, double initialBalance)
        : owner(name), balance(initialBalance) {}

    virtual void deposit(double amount) { balance += amount; }
    virtual bool withdraw(double amount) {
        if (amount > balance) return false;
        balance -= amount;
        return true;
    }

    virtual std::string info() const {
        return owner + " | Balance: " + std::to_string(balance);
    }

    virtual ~Account() {}
};

class SavingsAccount : public Account {
    double interestRate;
public:
    SavingsAccount(std::string name, double initialBalance, double rate)
        : Account(name, initialBalance), interestRate(rate) {}

    void addInterest() { balance += balance * interestRate; }

    std::string info() const override {
        return "[Savings] " + owner + " | Balance: " + std::to_string(balance);
    }
};

class CheckingAccount : public Account {
    double fee;
public:
    CheckingAccount(std::string name, double initialBalance, double fee)
        : Account(name, initialBalance), fee(fee) {}

    bool withdraw(double amount) override {
        if (amount + fee > balance) return false;
        balance -= (amount + fee);
        return true;
    }

    std::string info() const override {
        return "[Checking] " + owner + " | Balance: " + std::to_string(balance);
    }
};


int main() {
    
    if (!glfwInit()) return 1;
    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(800, 600, "Bank System GUI", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    
    std::vector<std::unique_ptr<Account>> accounts;
    static char name[64] = "";
    static double initAmount = 0;
    static double rateOrFee = 0;
    static int accountType = 0; 

    static int selectedAccount = -1;
    static double transAmount = 0;

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Bank Account Manager");

        // Add Account
        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        ImGui::InputDouble("Initial Amount", &initAmount);
        ImGui::InputDouble("Rate/Fee", &rateOrFee);
        ImGui::RadioButton("Savings", &accountType, 0); ImGui::SameLine();
        ImGui::RadioButton("Checking", &accountType, 1);

        if (ImGui::Button("Add Account")) {
            if (accountType == 0)
                accounts.push_back(std::make_unique<SavingsAccount>(name, initAmount, rateOrFee));
            else
                accounts.push_back(std::make_unique<CheckingAccount>(name, initAmount, rateOrFee));
        }

        ImGui::Separator();

        // Accounts list
        ImGui::Text("Accounts:");
        for (int i = 0; i < accounts.size(); i++) {
            if (ImGui::Selectable(accounts[i]->info().c_str(), selectedAccount == i)) {
                selectedAccount = i;
            }
        }

        // transaction controls
        if (selectedAccount >= 0 && selectedAccount < accounts.size()) {
            ImGui::Separator();
            ImGui::Text("Selected: %s", accounts[selectedAccount]->info().c_str());
            ImGui::InputDouble("Amount", &transAmount);

            if (ImGui::Button("Deposit")) {
                accounts[selectedAccount]->deposit(transAmount);
            }
            ImGui::SameLine();
            if (ImGui::Button("Withdraw")) {
                accounts[selectedAccount]->withdraw(transAmount);
            }
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
