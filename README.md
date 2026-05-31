<div align="center">

# CAN Terminal

**PCAN Tabanlı CAN-Bus Haberleşme Terminali**

PEAK-System PCAN donanımı üzerinden CAN-Bus haberleşmesi yapmak için geliştirilmiş, GTKmm tabanlı Windows masaüstü uygulaması.

</div>

---

## Hakkında

CAN Terminal, bir PCAN arayüzü aracılığıyla CAN-Bus üzerindeki cihazlarla mesaj alışverişi yapmayı sağlayan grafik arayüzlü bir terminal uygulamasıdır. PEAK-System'in **PCAN-Basic** API'sini kullanarak CAN kanalına bağlanır, mesaj gönderir/alır ve mesaj filtreleme gibi işlemleri destekler.

PCAN-Basic kütüphanesi (`PCANBasic.dll`) çalışma zamanında dinamik olarak yüklenir; böylece uygulama, sürücüyle gevşek bağlı kalır.

---

## Özellikler

- **CAN kanalına bağlanma** — Seçilen baud hızında PCAN kanalını başlatır.
- **Mesaj gönderme / alma** — CAN çerçevelerini iletir ve gelen mesajları okur.
- **Mesaj filtreleme** — Belirli ID aralıklarına göre gelen mesajları süzer.
- **Dinamik DLL yükleme** — PCAN-Basic fonksiyonları `LoadLibrary` ile çalışma zamanında bağlanır.
- **Hata metni desteği** — PCAN durum kodları okunabilir mesajlara çevrilir.

---

## Teknoloji Yığını

- **Dil:** C++17
- **Arayüz:** [GTKmm 4.0](https://gtkmm.org/)
- **CAN katmanı:** PEAK-System [PCAN-Basic](https://www.peak-system.com/) API
- **Derleme:** Makefile (g++)

> **Not:** Uygulama Win32 ve PCAN-Basic'e bağlı olduğundan **Windows** içindir ve çalışması için PCAN-Basic SDK ile uyumlu bir PEAK CAN donanımı gerekir.

---

## Proje Yapısı

```
CanTerminal/
├── main.cpp            # Uygulama giriş noktası (GTKmm Application)
├── MainWindow.hpp/.cpp # Arayüz mantığı
├── pcan.hpp/.cpp       # PCAN-Basic API sarmalayıcısı (dinamik DLL)
├── window.ui           # GTK arayüz tanımı
└── makefile            # Derleme yapılandırması
```

---

## Kurulum ve Çalıştırma

### Gereksinimler
- Windows üzerinde MSYS2 / MinGW (g++) ortamı
- GTKmm 4.0 (`pkg-config` ile bulunabilir olmalı)
- PCAN-Basic SDK (`PCANBasic.h` ve `PCANBasic` kütüphanesi)
- Bir PEAK-System CAN donanımı (ör. PCAN-USB)

### Derleme

`makefile` içindeki PCAN-Basic `Include` ve kütüphane yollarını kendi kurulumunuza göre güncelledikten sonra:

```bash
make
```

Bu komut `arayuz_app.exe` dosyasını üretir.

### Çalıştırma

```bash
./arayuz_app.exe
```

---

## İletişim

- **E-posta:** numanarifdeniz@gmail.com
- **GitHub:** [github.com/numanarif0](https://github.com/numanarif0)
- **LinkedIn:** [linkedin.com/in/numanarifdeniz](https://linkedin.com/in/numanarifdeniz)
