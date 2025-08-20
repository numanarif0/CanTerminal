#define WIN32_LEAN_AND_MEAN
#include "MainWindow.hpp"

#include <sstream>
#include <iomanip>
#include <chrono> 


union IntUnion {
    int32_t value;
    uint8_t bytes[4];
};
union FloatUnion {
    float value;
    uint8_t bytes[4];
};

std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto& byte : bytes) {
        ss << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::string hex_str = ss.str();
    if (!hex_str.empty()) {
        hex_str.pop_back();
        for (auto& c : hex_str) c = toupper(c);
    }
    return hex_str;
}


MainWindow::MainWindow()
{
    try {
        m_builder = Gtk::Builder::create_from_file("window.ui");
    }
    catch (const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        m_main_window = nullptr;
        return;
    }


    m_main_window = m_builder->get_widget<Gtk::Window>("main_window");
    m_combo_ports = m_builder->get_widget<Gtk::ComboBoxText>("combo_ports");
    m_button_connect = m_builder->get_widget<Gtk::Button>("button_connect");
    m_button_scan_ports = m_builder->get_widget<Gtk::Button>("button_scan_ports");
    m_log_view = m_builder->get_widget<Gtk::TextView>("log_view");
    m_entry_command = m_builder->get_widget<Gtk::Entry>("entry_command");
    m_statusbar = m_builder->get_widget<Gtk::Statusbar>("statusbar");
    m_button_show_led_window = m_builder->get_widget<Gtk::Button>("button_show_led_window");
    m_led_window = m_builder->get_widget<Gtk::Window>("led_window");
    m_toggle_led1 = m_builder->get_widget<Gtk::ToggleButton>("toggle_led1");
    m_toggle_led2 = m_builder->get_widget<Gtk::ToggleButton>("toggle_led2");
    m_toggle_led3 = m_builder->get_widget<Gtk::ToggleButton>("toggle_led3");
    m_toggle_led4 = m_builder->get_widget<Gtk::ToggleButton>("toggle_led4");

    if (!m_main_window || !m_combo_ports || !m_button_connect || !m_log_view || !m_entry_command || !m_statusbar || !m_button_scan_ports || !m_button_show_led_window || !m_led_window || !m_toggle_led1 || !m_toggle_led2 || !m_toggle_led3 || !m_toggle_led4) {
        throw std::runtime_error("UI dosyasindaki bir veya daha fazla widget'a erisilemedi.");
    }


    m_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_data_received));
    m_button_connect->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_connect_button_clicked));
    m_entry_command->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_command_entry_activated));
    m_button_show_led_window->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_show_led_window_clicked));
    m_toggle_led1->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_led_toggled));
    m_toggle_led2->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_led_toggled));
    m_toggle_led3->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_led_toggled));
    m_toggle_led4->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_led_toggled));
    

    m_combo_ports->set_sensitive(false);
    m_button_scan_ports->set_sensitive(false);
    m_combo_ports->append("PCAN_USBBUS1"); 
    m_combo_ports->set_active(0);

    m_button_show_led_window->set_sensitive(false);
    m_statusbar->push("Kontrolcü ve arayüz basariyla olusturuldu.");
}


MainWindow::~MainWindow() {
    m_stop_thread = true;
    if (m_read_thread.joinable()) {
        m_read_thread.join();
    }

 
    if (m_is_connected) {
        m_pcan.Uninitialize(PCAN_USBBUS1);
    }
}

Gtk::Window* MainWindow::get_window() {
    return m_main_window;
}


void MainWindow::on_connect_button_clicked() {
    const TPCANHandle channel = PCAN_USBBUS1;
    const TPCANBaudrate baudrate = PCAN_BAUD_250K;

    if (m_is_connected) { 
        m_stop_thread = true;
        if (m_read_thread.joinable()) {
            m_read_thread.join();
        }
        
        m_pcan.Uninitialize(channel);
        
        m_is_connected = false;
        m_button_connect->set_label("Bağlan");
        m_statusbar->push("Bağlantı kesildi.");
        m_led_window->hide();
        m_button_show_led_window->set_sensitive(false);

    } else {
    
        TPCANStatus status = m_pcan.Initialize(channel, baudrate);

        if (status == PCAN_ERROR_OK) {
            m_is_connected = true;
            m_button_connect->set_label("Bağlantıyı Kes");
            m_statusbar->push("PCAN_USBBUS1 kanalına 250 kbit/s ile bağlandı.");
            m_button_show_led_window->set_sensitive(true);
            
            m_stop_thread = false;
            m_read_thread = std::thread(&MainWindow::read_thread_function, this);
        } else {
            char error_text[256];
            m_pcan.GetErrorText(status, 0, error_text);
            m_statusbar->push("Hata: PCAN kanalı başlatılamadı! " + std::string(error_text));
        }
    }
}


void MainWindow::on_command_entry_activated()
{
    if (!m_is_connected) {
        m_statusbar->push("Hata: Veri göndermek için önce bağlanmalısınız.");
        return;
    }
    if (m_waiting_for_ack) {
        m_statusbar->push("Önceki komutun cevabı bekleniyor...");
        return;
    }
    
    std::string text_input = m_entry_command->get_text();
    if (text_input.empty()) return;

    TPCANMsg message_to_send;
    message_to_send.MSGTYPE = PCAN_MESSAGE_STANDARD;
    const TPCANHandle channel = PCAN_USBBUS1;


    if (text_input.find('.') != std::string::npos) 
    {
        float value_to_send;
        try {
            size_t pos;
            value_to_send = std::stof(text_input, &pos);
            if (pos < text_input.length()) {
                throw std::invalid_argument("Geçersiz karakterler bulundu.");
            }
        }
        catch (const std::exception& e) {
            m_statusbar->push("Hata: Geçerli bir float değeri girilmedi!");
            return;
        }

        message_to_send.ID = CAN_ID_SET_FLOAT_VALUE;
        message_to_send.LEN = 8;
        FloatUnion float_converter;
        float_converter.value = value_to_send;
        for (int i = 0; i < 4; ++i) {
            message_to_send.DATA[i] = float_converter.bytes[i];
        }
    } 
    else 
    {
        int32_t value_to_send;
        try {
            value_to_send = std::stoi(text_input);
        }
        catch (const std::exception& e) {
            m_statusbar->push("Hata: Geçerli bir tamsayı girilmedi!");
            return;
        }

        message_to_send.ID = CAN_ID_SET_LEDS; 
        message_to_send.LEN = 8;
        IntUnion int_converter;
        int_converter.value = value_to_send;
        for (int i = 0; i < 4; ++i) {
            message_to_send.DATA[i] = int_converter.bytes[i];
             //message_to_send.DATA[i] = 2;
        }
    }

    TPCANStatus status = m_pcan.Write(channel, &message_to_send);

    if (status != PCAN_ERROR_OK) {
        m_statusbar->push("Hata: CAN mesajı gönderilemedi!");
        return;
    }


    std::vector<uint8_t> data_bytes(message_to_send.DATA, message_to_send.DATA + message_to_send.LEN);
    std::stringstream id_stream;
    id_stream << "0x" << std::hex << std::uppercase << message_to_send.ID;
    std::string log_str = "TX > (ID: " + id_stream.str() + ", Data: " + bytes_to_hex_string(data_bytes) + ")\n";

    auto text_buffer = m_log_view->get_buffer();
    text_buffer->insert(text_buffer->end(), log_str);
    m_statusbar->push("Komut gönderildi, cevap bekleniyor...");
    m_entry_command->set_text("");


    m_waiting_for_ack = true;
    m_timeout_connection = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &MainWindow::on_command_timeout), 
        200 
    );
}



void MainWindow::read_thread_function()
{
    const TPCANHandle channel = PCAN_USBBUS1;
    TPCANMsg can_message;
    TPCANTimestamp timestamp;

    while (!m_stop_thread)
    {
        TPCANStatus status = m_pcan.Read(channel, &can_message, &timestamp);

        if (status == PCAN_ERROR_OK)
        {
            {
                std::lock_guard<std::mutex> lock(m_queue_mutex);
                m_shared_can_messages.push(can_message);
            }
            m_dispatcher.emit();
        }
        else if (status != PCAN_ERROR_QRCVEMPTY)
        {
            
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}


void MainWindow::on_data_received()
{
    std::queue<TPCANMsg> local_queue;
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        local_queue.swap(m_shared_can_messages);
    }

    while (!local_queue.empty())
    {
        TPCANMsg msg = local_queue.front();
        local_queue.pop();
        handle_can_message(msg);
    }
}


void MainWindow::handle_can_message(const TPCANMsg& msg)
{
    if (m_waiting_for_ack) {
        m_timeout_connection.disconnect();
        m_waiting_for_ack = false;
    }

    auto text_buffer = m_log_view->get_buffer();
    std::string log_message = "RX < ";

    switch(msg.ID)
    {
        case CAN_ID_ACK_OK:
            log_message += "Komut ONAYLANDI.";
            m_statusbar->push("İşlem başarılı.");
            break;
        case CAN_ID_ACK_FAIL:
            log_message += "Komut HATALI.";
            m_statusbar->push("Cihazdan HATA cevabı alındı!");
            break;
        default:
            log_message += "Bilinmeyen ID.";
            break;
    }

    std::vector<uint8_t> data_bytes(msg.DATA, msg.DATA + msg.LEN);
    
    std::stringstream id_stream;
    id_stream << "0x" << std::hex << std::uppercase << msg.ID;
    
    log_message += " (ID: " + id_stream.str() + ", Data: " + bytes_to_hex_string(data_bytes) + ")\n";
    text_buffer->insert(text_buffer->end(), log_message);
    m_log_view->scroll_to(text_buffer->get_mark("insert"));
}

bool MainWindow::on_command_timeout()
{
    m_waiting_for_ack = false; 
    m_statusbar->push("HATA: Cihazdan cevap alınamadı (Zaman Aşımı)!");
    return false; 
}

void MainWindow::on_show_led_window_clicked()
{
    if (m_led_window) {
        m_led_window->present();
    }
}

void MainWindow::on_led_toggled()
{
    if (!m_is_connected) return; 

    int32_t led_mask = 0;
    if (m_toggle_led1->get_active()) { led_mask |= 1; }
    if (m_toggle_led2->get_active()) { led_mask |= 2; }
    if (m_toggle_led3->get_active()) { led_mask |= 4; }
    if (m_toggle_led4->get_active()) { led_mask |= 8; }

    send_led_command(led_mask);
}


void MainWindow::send_led_command(int32_t led_mask)
{
    if (!m_is_connected) {
        m_statusbar->push("Hata: LED komutu göndermek için önce bağlanmalısınız.");
        return;
    }
    if (m_waiting_for_ack) {
        m_statusbar->push("Önceki komutun cevabı bekleniyor...");
        return;
    }

    const TPCANHandle channel = PCAN_USBBUS1;
    TPCANMsg message_to_send;


    message_to_send.ID = CAN_ID_SET_LEDS;
    message_to_send.MSGTYPE = PCAN_MESSAGE_STANDARD;
    message_to_send.LEN = 4;

    IntUnion int_converter;
    int_converter.value = led_mask;
    for (int i = 0; i < 4; ++i) {
        message_to_send.DATA[i] = int_converter.bytes[i];
    }
    
    
    m_pcan.Write(channel, &message_to_send);

    
    std::vector<uint8_t> data_bytes(message_to_send.DATA, message_to_send.DATA + 4);
    std::stringstream id_stream;
    id_stream << "0x" << std::hex << std::uppercase << message_to_send.ID;
    std::string log_str = "TX > (ID: " + id_stream.str() + ", Data: " + bytes_to_hex_string(data_bytes) + ")\n";

    auto text_buffer = m_log_view->get_buffer();
    text_buffer->insert(text_buffer->end(), log_str);
    m_statusbar->push("LED durumu gönderildi, cevap bekleniyor...");

    
    m_waiting_for_ack = true;
    m_timeout_connection = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &MainWindow::on_command_timeout), 
        200 
    );
}