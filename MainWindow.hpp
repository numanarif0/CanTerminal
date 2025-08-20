#pragma once

#include <gtkmm.h>
#include <windows.h>   
#include <iostream>
#include "pcan.hpp"
#include <thread>      
#include <mutex>
#include <queue> 



const uint8_t CAN_ID_SET_LEDS = 0xA1;
const uint8_t CAN_ID_SET_FLOAT_VALUE = 0xA2; 

//receive kısmı
const uint8_t CAN_ID_ACK_OK = 0xB1; 
const uint8_t CAN_ID_ACK_FAIL = 0xB2; 
class MainWindow
{
public:
    MainWindow();
    virtual ~MainWindow();

    Gtk::Window* get_window();

protected:
    void on_connect_button_clicked();
    void on_command_entry_activated();
     void handle_can_message(const TPCANMsg& msg);

    void read_thread_function();
    void on_data_received();
    bool on_command_timeout();
  
 

    void on_show_led_window_clicked();
    void on_led_toggled();
    void send_led_command(int32_t led_mask);


    Glib::RefPtr<Gtk::Builder> m_builder;

    Gtk::Window* m_main_window = nullptr;
    Gtk::ComboBoxText* m_combo_ports = nullptr;
    Gtk::Button* m_button_connect = nullptr;
    Gtk::Button* m_button_scan_ports = nullptr;
    Gtk::TextView* m_log_view = nullptr;
    Gtk::Entry* m_entry_command = nullptr;
    Gtk::Statusbar* m_statusbar = nullptr;

    Gtk::Button* m_button_show_led_window = nullptr;
    Gtk::Window* m_led_window = nullptr;
    Gtk::ToggleButton* m_toggle_led1 = nullptr;
    Gtk::ToggleButton* m_toggle_led2 = nullptr;
    Gtk::ToggleButton* m_toggle_led3 = nullptr;
    Gtk::ToggleButton* m_toggle_led4 = nullptr;
    
    PCAN m_pcan;
    bool m_is_connected = false;

    std::thread m_read_thread;
    std::atomic<bool> m_stop_thread {false};
    std::queue<TPCANMsg> m_shared_can_messages;
    std::mutex m_queue_mutex;
    Glib::Dispatcher m_dispatcher;
    std::atomic<bool> m_waiting_for_ack {false};
    sigc::connection m_timeout_connection;


 
};