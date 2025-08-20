#include "MainWindow.hpp"
#include <gtkmm/application.h>
#include <locale>
#include <iostream>

int main(int argc, char *argv[])
{
    try {
        std::locale::global(std::locale("C"));
    } catch (const std::runtime_error& e) {
        std::cerr << "Locale ayarlanamadı: " << e.what() << std::endl;
    }

    auto app = Gtk::Application::create("tr.numanarifdeniz.stm32terminal");

    app->signal_activate().connect([&]() {
        auto controller = new MainWindow();
        
        if (controller->get_window()) {
            app->add_window(*controller->get_window());

            controller->get_window()->present();
        }
    });

    return app->run(argc, argv);
}