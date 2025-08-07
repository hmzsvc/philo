# Philosophers Projesi - Güncel Kod Analizi

## 📖 İçindekiler
1. [Proje Genel Bakış](#proje-genel-bakış)
2. [Güncel Dosya Yapısı](#güncel-dosya-yapısı)
3. [Veri Yapıları](#veri-yapıları)
4. [Program Akışı](#program-akışı)
5. [Fonksiyon Analizleri](#fonksiyon-analizleri)
6. [Thread Yönetimi](#thread-yönetimi)
7. [Mutex Kullanımı](#mutex-kullanımı)
8. [Derleme ve Test](#derleme-ve-test)

---

## Proje Genel Bakış

**Philosophers** projesi, klasik "Dining Philosophers" problemini çözen bir C programıdır. Bu problem, eşzamanlılık (concurrency) ve deadlock önleme konularını öğretmek için kullanılan ünlü bir bilgisayar bilimi problemidir.

### Problem Tanımı:
- N adet filozof bir masanın etrafında oturuyor
- Her filozofun yanında birer çatal var
- Filozoflar sadece 3 şey yapıyor: yemek yeme, uyuma, düşünme
- Yemek yemek için 2 çatala ihtiyaç var (sol ve sağ)
- Filozoflar ölmemeli (açlıktan)

---

## Güncel Dosya Yapısı

```
philo/
├── src/                     # Ana kaynak dosyalar
│   ├── philo.c              # Ana program dosyası (main, ft_usleep)
│   ├── philo_utils.c        # Filozof thread'i ve ana döngü yönetimi
│   ├── mandotary.c          # Temel filozof fonksiyonları (eat, sleep, think, take_fork)
│   ├── monitor.c            # İzleme thread'i (ölüm ve yemek hedefi kontrolü)
│   ├── support.c            # Yardımcı fonksiyonlar (atoi, parse_args)
│   ├── mutex.c              # Mutex yönetimi ve senkronizasyon
│   ├── helpers.c            # Genel yardımcı fonksiyonlar
│   └── error_handle.c       # Hata yönetimi ve bellek temizleme
├── utils/                   # Ek yardımcı dosyalar (kullanılmıyor - boş)
│   ├── helpers.c
│   ├── monitor.c
│   ├── monitor_utils.c
│   ├── mutex_utils.c
│   ├── philo_routine.c
│   ├── philo_utils.c
│   └── utils.c
├── error/                   # Hata yönetimi (kullanılmıyor - boş)
│   └── error_handle.c
├── lib/                     # Header dosyaları
│   ├── philo.h              # Ana header dosyası
│   └── error.h              # Hata kodları ve sabitler
├── obj/                     # Derleme object dosyaları
├── Makefile                 # Derleme dosyası
├── philo                    # Derlenmiş executable
├── advanced_test_philo.sh   # Gelişmiş test scripti
├── benchmark_philo.sh       # Performans test scripti
└── ultimate_tester.sh       # Kapsamlı test scripti
```

### Aktif Dosyalar:
- **src/**: Ana kodların bulunduğu klasör (8 dosya)
- **lib/**: Header dosyaları (2 dosya)
- **Makefile**: Sadece src/ klasöründeki dosyaları derler
- **utils/ ve error/**: Boş klasörler (Makefile'da kullanılmıyor)

---

## Veri Yapıları

### 1. `t_philo` Yapısı (Filozof)
```c
typedef struct s_philo
{
    int                identity;            // Filozofun kimliği (1, 2, 3...)
    int                left_fork_bool;      // Sol çatal kullanımda mı?
    int                right_fork_bool;     // Sağ çatal kullanımda mı?
    int                eat_count;           // Kaç kez yemek yedi
    long long          last_meal_time;      // Son yemek zamanı
    pthread_t          thread;              // Filozofun thread'i
    pthread_mutex_t    *left_fork;          // Sol çatal mutex'i
    pthread_mutex_t    *right_fork;         // Sağ çatal mutex'i
    pthread_mutex_t    eat_count_mutex;     // Yemek sayısı mutex'i
    t_table            *data;               // Ana veri yapısına pointer
} t_philo;
```

### 2. `t_table` Yapısı (Masa/Oyun Durumu)
```c
typedef struct s_table
{
    int                philo_count;         // Filozof sayısı
    int                dead_index;          // Ölen filozofun index'i
    int                must_eat;            // Zorunlu yemek sayısı
    int                is_dead;             // Ölüm durumu (0,1,2,3)
    int                time_to_die;         // Ölüm süresi (ms)
    int                time_to_eat;         // Yemek süresi (ms)
    int                time_to_sleep;       // Uyku süresi (ms)
    int                start_flag;          // Başlama sinyali
    int                must_meal_loop;      // Yemek hedefine ulaşan sayısı
    long long          start_time;          // Başlangıç zamanı
    long long          last_meal_philo;     // Son yemek zamanı
    pthread_t          monitor_philo;       // İzleyici thread
    pthread_mutex_t    *forks;              // Çatal mutex'leri
    pthread_mutex_t    death_mutex;         // Ölüm mutex'i
    pthread_mutex_t    check_meal_mutex;    // Yemek kontrol mutex'i
    pthread_mutex_t    start_flag_mutex;    // Başlama mutex'i
    pthread_mutex_t    print_mutex;         // Yazdırma mutex'i
    pthread_mutex_t    meal_mutex;          // Yemek zamanı mutex'i
    pthread_mutex_t    must_meal_mutex;     // Zorunlu yemek mutex'i
    t_philo            *philos;             // Filozof dizisi
} t_table;
```

### 3. Hata Kodları (lib/error.h)
```c
typedef enum s_error_code
{
    ERR_MALLOC_FAIL,    // 0 - Bellek tahsisi hatası
    ERR_INVALID_ARG,    // 1 - Geçersiz argüman
    ERR_THREAD_FAIL,    // 2 - Thread oluşturma hatası
    ERR_ATOI_FAIL,      // 3 - String'den integer dönüştürme hatası
} t_error_code;
```

---

## Program Akışı

### 1. Program Başlangıcı (`main` - philo.c)

```c
int main(int argc, char *argv[])
{
    t_table data;

    if (argc == 5 || argc == 6)  // Argüman kontrolü
    {
        initialize_table(&data, argv, argc);     // 1. Veri yapısını başlat
        initialize_forks(&data);                 // 2. Mutex'leri başlat
        setup_philosopher_monitor(&data);        // 3. İzleyici thread'i oluştur
        create_philo(&data);                     // 4. Filozof thread'lerini oluştur
        philo_join(&data);                       // 5. Thread'leri bekle
        reset_table(&data);                      // 6. Temizlik yap
        return (0);
    }
    else
    {
        printf("ERR_INVALID_ARG\n");
        return (1);
    }
}
```

### 2. Akış Sırası

1. **Argüman Kontrolü**: 5 veya 6 argüman olmalı
2. **Veri Başlatma**: Struct'ları ve değerleri başlat (`initialize_table`)
3. **Mutex Başlatma**: Bütün mutex'leri oluştur (`initialize_forks`)
4. **İzleyici Oluşturma**: Ölüm kontrolü için thread oluştur (`setup_philosopher_monitor`)
5. **Filozof Oluşturma**: Her filozof için thread oluştur (`create_philo`)
6. **Başlatma**: Bütün thread'leri senkronize başlat
7. **Bekleme**: Thread'lerin bitmesini bekle (`philo_join`)
8. **Temizlik**: Bellek ve mutex'leri temizle (`reset_table`)

---

## Fonksiyon Analizleri

### 📂 src/philo.c (Ana Dosya)

#### `main` Fonksiyonu
```c
int main(int argc, char *argv[])
```
- **Amaç**: Programın giriş noktası
- **Akış**: 
  1. Argüman sayısını kontrol et (5 veya 6 olmalı)
  2. Sırayla başlatma fonksiyonlarını çağır
  3. Thread'leri bekle ve temizlik yap

#### `ft_usleep` Fonksiyonu
```c
void ft_usleep(int wait_time, t_philo *philo)
```
- **Amaç**: Kesin süre bekleme (normal usleep'ten daha hassas)
- **Çalışma**: 
  1. Başlangıç zamanını al (`get_time_in_ms`)
  2. Hedef süre kadar döngüde bekle
  3. Her 100 mikrosaniyede ölüm kontrolü yap (`handle_dead`)

### 📂 src/philo_utils.c (Filozof Başlatma)

#### `initialize_table` Fonksiyonu
```c
void initialize_table(t_table *data, char *argv[], int argc)
```
- **Amaç**: Ana veri yapısını başlat
- **İşlemler**:
  1. Argümanları parse et (`parse_args`)
  2. Filozof dizisini oluştur (`ft_calloc`)
  3. Her filozofun temel bilgilerini ayarla
  4. Hata kontrolü yap (`handle_error`)

#### `create_philo` Fonksiyonu
```c
void create_philo(t_table *data)
```
- **Amaç**: Filozof thread'lerini oluştur
- **İşlemler**:
  1. Her filozof için thread oluştur (`pthread_create`)
  2. Başlangıç zamanını ayarla (`set_time`)
  3. Start flag'ini ayarla

#### `initialize_forks` Fonksiyonu
```c
void initialize_forks(t_table *data)
```
- **Amaç**: Çatal mutex'lerini başlat
- **İşlemler**:
  1. Bütün mutex'leri initialize et
  2. Her filozofa sol/sağ çatal pointer'ları ata
  3. Çatal dizisini oluştur

### 📂 src/mandotary.c (Temel Fonksiyonlar)

#### `philo_eat` Fonksiyonu
```c
void philo_eat(t_philo *philo)
```
- **Amaç**: Filozof yemek yer
- **İşlemler**:
  1. Son yemek zamanını güncelle (`last_meal_added`)
  2. "is eating" yazdır (`print`)
  3. Yemek sayısını artır (thread-safe)
  4. Yemek süresi kadar bekle (`ft_usleep`)

#### `philo_sleep` Fonksiyonu
```c
void philo_sleep(t_philo *philo)
```
- **Amaç**: Filozof uyur
- **İşlemler**:
  1. "is sleeping" yazdır
  2. Uyku süresi kadar bekle

#### `philo_thinking` Fonksiyonu
```c
void philo_thinking(t_philo *philo)
```
- **Amaç**: Filozof düşünür
- **İşlemler**:
  1. "is thinking" yazdır
  2. Optimizasyon için kısa süre bekle

#### `philo_take_fork` Fonksiyonu
```c
void philo_take_fork(t_philo *philo)
```
- **Amaç**: Çatalları al (deadlock önleme ile)
- **Deadlock Önleme Stratejisi**:
  - Çift ID'li filozoflar: önce sol, sonra sağ çatal
  - Tek ID'li filozoflar: önce sağ, sonra sol çatal
  - Bu sayede circular wait durumu önlenir

#### `philo_dead` Fonksiyonu
```c
void philo_dead(t_philo philo)
```
- **Amaç**: Ölüm mesajını yazdır
- **İşlemler**:
  1. Print mutex'i kilitle
  2. Ölüm zamanını ve ID'yi yazdır
  3. Mutex'i aç
```c
void print(t_philo *philo, char *str)
```
- **Amaç**: Thread-safe yazdırma
- **Akış**:
  1. Yemek hedefi kontrolü yap
  2. Ölüm kontrolü yap
  3. Print mutex'i kilitle
  4. Ölü değilse mesaj yazdır
  5. Mutex'i aç

### 📂 utils/philo_utils.c

#### `init_philo` Fonksiyonu
```c
void init_philo(t_data *data, char *argv[], int argc)
```
- **Amaç**: Ana veri yapısını ve filozof dizisini başlat
- **Adımlar**:
  1. Argümanları parse et
  2. Pointer'ları NULL'la başlat
  3. Başlangıç değerlerini ata
  4. Filozof dizisi için bellek tahsis et
  5. Her filozof için başlangıç değerlerini ata

## Fonksiyon Analizleri

### 📂 src/philo.c (Ana Dosya)

#### `main` Fonksiyonu
```c
int main(int argc, char *argv[])
```
- **Amaç**: Programın giriş noktası
- **Argümanlar**: 4 veya 5 parametre (number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat])
- **Akış**: 
  1. Argüman sayısını kontrol et (5 veya 6 olmalı)
  2. `initialize_table`: Ana veri yapısını başlat
  3. `initialize_forks`: Mutex'leri başlat
  4. `setup_philosopher_monitor`: İzleyici thread'i oluştur
  5. `create_philo`: Filozof thread'lerini oluştur
  6. `philo_join`: Thread'leri bekle
  7. `reset_table`: Temizlik yap

#### `ft_usleep` Fonksiyonu
```c
void ft_usleep(int wait_time, t_philo *philo)
```
- **Amaç**: Kesin süre bekleme (normal usleep'ten daha hassas)
- **Özellik**: Bekleme sırasında sürekli ölüm kontrolü yapar
- **Çalışma**: 
  1. Başlangıç zamanını al (`get_time_in_ms`)
  2. Hedef süre kadar döngüde bekle
  3. Her 100 mikrosaniyede ölüm kontrolü yap (`handle_dead`)

### 📂 src/philo_utils.c (Filozof Thread Yönetimi)

#### `run_philo_loop` Fonksiyonu (static)
```c
static void run_philo_loop(t_philo *philo)
```
- **Amaç**: Filozofun ana yaşam döngüsü
- **Döngü**:
  1. Ölüm kontrolü yap
  2. Yemek hedefine ulaştı mı kontrol et
  3. Çatalları al (`philo_take_fork`)
  4. Yemek ye (`philo_eat`)
  5. Çatalları bırak
  6. Uyu (`philo_sleep`)
  7. Düşün (`philo_thinking`)

#### `philo_process` Fonksiyonu (static)
```c
static void *philo_process(void *arg)
```
- **Amaç**: Her filozofun thread fonksiyonu
- **Akış**:
  1. Başlama sinyalini bekle
  2. Tek filozof durumunu kontrol et
  3. Senkronizasyon için bekle
  4. Ana döngüyü çalıştır (`run_philo_loop`)

#### `initialize_table` Fonksiyonu
```c
void initialize_table(t_table *data, char *argv[], int argc)
```
- **Amaç**: Ana veri yapısını başlat
- **İşlemler**:
  1. Argümanları parse et (`parse_args`)
  2. Değişkenleri başlat
  3. Filozof dizisi için bellek tahsis et
  4. Her filozofun temel bilgilerini ayarla

#### `create_philo` Fonksiyonu
```c
void create_philo(t_table *data)
```
- **Amaç**: Filozof thread'lerini oluştur
- **İşlemler**:
  1. Her filozof için pthread_create çağır
  2. Başlangıç zamanını ayarla (`set_time`)
  3. Start flag'ini set et

#### `initialize_forks` Fonksiyonu
```c
void initialize_forks(t_table *data)
```
- **Amaç**: Çatal mutex'lerini ve diğer mutex'leri başlat
- **Başlatılan Mutex'ler**:
  1. Çatal mutex'leri (filozof sayısı kadar)
  2. `death_mutex`, `start_flag_mutex`, `print_mutex`
  3. `meal_mutex`, `must_meal_mutex`, `check_meal_mutex`
  4. Her filozof için `eat_count_mutex`

### 📂 src/mandotary.c (Temel Filozof Fonksiyonları)

#### `philo_eat` Fonksiyonu
```c
void philo_eat(t_philo *philo)
```
- **Amaç**: Filozof yemek yer
- **İşlemler**:
  1. Son yemek zamanını güncelle (`last_meal_added`)
  2. "is eating" mesajını yazdır (`print`)
  3. Yemek sayısını thread-safe artır
  4. Yemek süresi kadar bekle (`ft_usleep`)

#### `philo_sleep` Fonksiyonu
```c
void philo_sleep(t_philo *philo)
```
- **Amaç**: Filozof uyur
- **İşlemler**:
  1. "is sleeping" mesajını yazdır
  2. Uyku süresi kadar bekle

#### `philo_thinking` Fonksiyonu
```c
void philo_thinking(t_philo *philo)
```
- **Amaç**: Filozof düşünür
- **Özellik**: Optimizasyon için `time_to_die - (time_to_eat + time_to_sleep)` kadar bekler
- **Amaç**: Optimal timing sağlama

#### `philo_dead` Fonksiyonu
```c
void philo_dead(t_philo philo)
```
- **Amaç**: Ölüm mesajını thread-safe yazdır
- **Format**: `[zaman_ms] [filozof_id] died`

#### `philo_take_fork` Fonksiyonu
```c
void philo_take_fork(t_philo *philo)
```
- **Amaç**: Çatalları al (deadlock önleyici strateji ile)
- **Deadlock Önleme**:
  - **Çift ID**: Önce sol, sonra sağ çatal
  - **Tek ID**: Önce sağ, sonra sol çatal (20µs gecikme ile)
- **Mesaj**: Her çatal alınışında "has taken a fork" yazdır

### 📂 src/monitor.c (İzleme Thread'i)

#### `watch_for_death` Fonksiyonu (static)
```c
static void *watch_for_death(void *argv)
```
- **Amaç**: Sürekli ölüm ve yemek hedefi kontrolü
- **Döngü**:
  1. Her filozofu kontrol et (`check_and_handle_death`)
  2. Ölüm zamanı kontrolü yap
  3. Yemek hedefine ulaşma kontrolü yap
  4. Problem varsa döngüden çık

#### `check_and_handle_death` Fonksiyonu (static)
```c
static int check_and_handle_death(t_table *data, int philo_index)
```
- **Amaç**: Belirli filozofun ölüm durumunu kontrol et
- **Kontrol**: `current_time - last_meal_time >= time_to_die`
- **Döndürür**: 1 ölüm varsa, 0 yaşıyorsa

#### `setup_philosopher_monitor` Fonksiyonu
```c
void setup_philosopher_monitor(t_table *data)
```
- **Amaç**: İzleyici thread'ini oluştur
- **İşlem**: `watch_for_death` fonksiyonu ile pthread_create

### 📂 src/mutex.c (Mutex Yönetimi)

#### `check_dead` Fonksiyonu
```c
int check_dead(t_philo *philo)
```
- **Amaç**: Thread-safe ölüm durumu kontrolü
- **Durumlar**:
  - `is_dead = 1`: Normal ölüm
  - `is_dead = 2`: Thread hatası
  - `is_dead = 3`: Herkes yeterince yedi

#### `handle_dead` Fonksiyonu
```c
int handle_dead(t_philo *philo)
```
- **Amaç**: Ölüm durumunda temizlik
- **İşlemler**:
  1. Ölüm durumunu kontrol et
  2. Tutulan çatalları bırak
  3. Çatal flag'lerini sıfırla

#### `check_meal_goal` Fonksiyonu
```c
int check_meal_goal(t_philo *philo)
```
- **Amaç**: Yemek hedefini kontrol et
- **İşlemler**:
  1. Hedef yemek sayısına ulaştı mı kontrol et
  2. Ulaştıysa global sayacı artır
  3. Çatalları bırak ve 1 döndür

#### `last_meal_added` Fonksiyonu
```c
void last_meal_added(t_philo *philo)
```
- **Amaç**: Son yemek zamanını thread-safe güncelle

### 📂 src/helpers.c (Yardımcı Fonksiyonlar)

#### `philo_join` Fonksiyonu
```c
void philo_join(t_table *data)
```
- **Amaç**: Bütün thread'leri bekle ve sonuçları yönet
- **Sıra**:
  1. İzleyici thread'ini bekle (`pthread_join`)
  2. Ölüm varsa ölüm mesajını yazdır
  3. Tek filozof özel durumunu kontrol et
  4. Bütün filozof thread'lerini bekle

#### `ft_calloc` Fonksiyonu
```c
void *ft_calloc(size_t count, size_t size)
```
- **Amaç**: Bellek tahsis et ve sıfırla
- **Özellik**: `ft_memset` ile sıfırlama

#### `get_time_in_ms` Fonksiyonu
```c
long long get_time_in_ms(void)
```
- **Amaç**: Milisaniye cinsinden sistem zamanı al
- **Kullanım**: `gettimeofday` sistem çağrısı

#### `set_time` Fonksiyonu
```c
void set_time(t_table *data)
```
- **Amaç**: Başlangıç zamanını kaydet

### 📂 src/support.c (Argüman İşleme)

#### `parse_args` Fonksiyonu
```c
void parse_args(char *argv[], t_table *data, int argc)
```
- **Amaç**: Komut satırı argümanlarını parse et
- **İşlemler**:
  1. Her argümanı integer'a çevir (`ft_atoi`)
  2. Geçersiz değerlerde hata fırlat
  3. Veri yapısına ata

#### `ft_atoi` Fonksiyonu (static)
```c
static int ft_atoi(char *str, int *res)
```
- **Amaç**: String'i integer'a çevir
- **Özellikler**:
  1. Negatif sayı kontrolü
  2. Uzunluk kontrolü (max 11 karakter)
  3. Overflow kontrolü (max 2147483647)
  4. Geçersiz karakter kontrolü
### 📂 src/error_handle.c (Hata Yönetimi)

#### `handle_error` Fonksiyonu
```c
void handle_error(t_table *data, int err_code, void *str)
```
- **Amaç**: Hata durumlarında temizlik ve çıkış
- **Hata Türleri**:
  - `ERR_MALLOC_FAIL`: Bellek tahsisi hatası
  - `ERR_INVALID_ARG`: Geçersiz argüman
  - `ERR_THREAD_FAIL`: Thread oluşturma hatası
  - `ERR_ATOI_FAIL`: String parse hatası

#### `reset_table` Fonksiyonu
```c
void reset_table(t_table *data)
```
- **Amaç**: Program sonunda temizlik yap
- **İşlemler**:
  1. Bütün mutex'leri destroy et (`destroy_mutex`)
  2. Filozof dizisini free et
  3. Çatal dizisini free et

#### `destroy_mutex` Fonksiyonu (static)
```c
static void destroy_mutex(t_table *data)
```
- **Amaç**: Bütün mutex'leri güvenli şekilde destroy et
- **Temizlenen Mutex'ler**:
  1. Ana mutex'ler (death, print, start_flag, check_meal)
  2. Çatal mutex'leri
  3. Filozof eat_count mutex'leri

#### `handle_mutex_error` Fonksiyonu
```c
void handle_mutex_error(t_table *data, int value)
```
- **Amaç**: Mutex işlem hatalarını kontrol et
- **Kontrol**: `value != 0` ise hata flag'i set et

### 📂 src/support.c Ek Fonksiyonlar

#### `one_philo_handle` Fonksiyonu
```c
void one_philo_handle(t_philo *philo)
```
- **Amaç**: Tek filozof özel durumunu işle
- **İşlem**:
  1. Tek çatalı al ve mesaj yazdır
  2. Çatalı bırak
  3. Ölüm flag'ini set et (çünkü tek çatalla yemek yenilemez)

#### `print` Fonksiyonu
```c
void print(t_philo *philo, char *str)
```
- **Amaç**: Thread-safe mesaj yazdırma
- **Kontroller**:
  1. Yemek hedefi tamamlandı mı kontrol et
  2. Ölüm durumu kontrol et
  3. Kontroller geçerse mesaj yazdır
- **Format**: `[zaman_ms] [filozof_id] [mesaj]`

#### `sync_philo_start` Fonksiyonu
```c
void sync_philo_start(t_philo *philo)
```
- **Amaç**: Filozof thread'lerinin senkronize başlaması
- **İşlemler**:
  1. Son yemek zamanını başlangıç zamanına set et
  2. Tek ID'li filozoflar için gecikme ekle (deadlock önleme)

---

## Thread Yönetimi

### 1. Thread Türleri

#### Ana Thread (main)
- Program başlatır
- Diğer thread'leri oluşturur
- Thread'lerin bitmesini bekler
- Temizlik yapar

#### İzleyici Thread (monitor_philo)
- Filozofları sürekli izler
- Ölüm durumlarını kontrol eder
- Yemek hedefini kontrol eder
- İlk oluşturulan thread

#### Filozof Thread'leri (philo_process)
- Her filozof için bir thread
- Ana yaşam döngüsünü işletir
- Çatal rekabeti yapar

### 2. Thread Senkronizasyonu

```c
// Başlama senkronizasyonu
void sync_philo_start(t_philo *philo)
{
    // Son yemek zamanını set et
    pthread_mutex_lock(&philo->data->meal_mutex);
    philo->last_meal_time = get_time_in_ms();
    pthread_mutex_unlock(&philo->data->meal_mutex);
    
    // Tek ID'li filozoflar gecikme ile başlar
    if (philo->id % 2 != 0)
        ft_usleep(philo->data->time_to_eat, philo);
}
```

### 3. Thread Yaşam Döngüsü

1. **Oluşturma**: `pthread_create`
2. **Başlama Bekleme**: `start_flag` sinyali
3. **Senkronizasyon**: Çift/tek filozof ayrımı
4. **Ana Döngü**: Yeme-uyuma-düşünme
5. **Sonlanma**: Ölüm veya hedef yemek
6. **Bekleme**: `pthread_join`

---

## Mutex Kullanımı

### 1. Çatal Mutex'leri (`forks`)
```c
pthread_mutex_t *forks; // Çatal dizisi
```
- **Amaç**: Her çatalın tekil kullanımını garanti et
- **Kullanım**: Yemek yerken 2 çatal gerekli
- **Deadlock Önleme**: Farklı alma sırası

### 2. Ölüm Mutex'i (`death_mutex`)
```c
pthread_mutex_t death_mutex;
```
- **Koruduğu Değişkenler**: `is_dead`, `dead_index`
- **Kullanım**: Ölüm durumu kontrolü ve set etme

### 3. Yazdırma Mutex'i (`print_mutex`)
```c
pthread_mutex_t print_mutex;
```
- **Amaç**: Mesajların karışmaması
- **Garanti**: Bir seferde bir mesaj

### 4. Başlama Mutex'i (`start_flag_mutex`)
```c
pthread_mutex_t start_flag_mutex;
```
- **Amaç**: Thread'lerin senkronize başlaması
- **Kullanım**: `start_flag` değişkenini korur

### 5. Yemek Mutex'leri
```c
pthread_mutex_t check_meal_mutex;  // Yemek kontrolü
pthread_mutex_t meal_mutex;        // Yemek zamanı
pthread_mutex_t must_meal_mutex;   // Zorunlu yemek sayacı
```

### 6. Bireysel Filozof Mutex'i
```c
pthread_mutex_t eat_count_mutex; // Her filozofun yemek sayısı
```

---

## Hata Yönetimi

### 1. Hata Türleri

#### Bellek Hatası (`ERR_MALLOC_FAIL`)
```c
data->philos = ft_calloc(sizeof(t_philo), data->philo_count);
error_check(data, ERR_MALLOC_FAIL, data->philos);
```

#### Argüman Hatası (`ERR_INVALID_ARG`)
```c
if (argc != 5 && argc != 6)
    printf("ERR_INVALID_ARG\n");
```

#### Thread Hatası (`ERR_THREAD_FAIL`)
```c
if (pthread_create(&thread, NULL, function, arg))
    error_check(data, ERR_THREAD_FAIL, NULL);
```

#### Parsing Hatası (`ERR_ATOI_FAIL`)
```c
if (!(*(str + i) >= '0' && *(str + i) <= '9'))
    error_check(NULL, ERR_ATOI_FAIL, NULL);
```

### 2. Hata İşleme Stratejisi

1. **Immediate Cleanup**: Hata anında temizlik
2. **Resource Release**: Mutex ve bellek temizliği
3. **Graceful Exit**: Program düzgün sonlanma

---

## Derleme ve Test

### 1. Makefile Analizi

#### Derleme Ayarları
```makefile
CC = @cc
CFLAGS = -Wall -Wextra -Werror -pthread -fsanitize=thread -g
```
- **Thread Sanitizer**: `-fsanitize=thread` data race tespiti için
- **Debug**: `-g` debug bilgisi
- **Threading**: `-pthread` thread desteği

#### Kaynak Dosyalar
```makefile
PHILO_SRCS = $(SRC_DIR)philo.c \
             $(SRC_DIR)monitor.c \
             $(SRC_DIR)support.c \
             $(SRC_DIR)helpers.c \
             $(SRC_DIR)philo_utils.c \
             $(SRC_DIR)mutex.c \
             $(SRC_DIR)mandotary.c \
             $(SRC_DIR)error_handle.c \
```

#### Derleme Komutları
```bash
# Normal derleme
make

# Temizlik
make clean      # Object dosyalarını sil
make fclean     # Hepsini sil
make re         # Yeniden derle
```

### 2. Test Stratejileri

#### Normal Test Durumları
```bash
# 4 filozof, 410ms ölüm, 200ms yemek, 200ms uyku
./philo 4 410 200 200

# 5 filozof, 800ms ölüm, 200ms yemek, 200ms uyku, 7 kez yemek
./philo 5 800 200 200 7
```

#### Edge Case'ler
```bash
# Tek filozof (ölmeli)
./philo 1 800 200 200

# Hızlı ölüm
./philo 4 300 200 200

# Çok filozof
./philo 200 410 200 200
```

#### Test Scriptleri
- **`advanced_test_philo.sh`**: Kapsamlı test senaryoları
- **`benchmark_philo.sh`**: Performans testleri
- **`ultimate_tester.sh`**: Sınır durumu testleri

### 3. Debugging

#### Thread Sanitizer Kullanımı
```bash
# Data race tespiti
./philo 4 410 200 200 2>&1 | grep "WARNING"

# Deadlock tespiti
timeout 10s ./philo 4 410 200 200
```

#### Valgrind (Memory Check)
```bash
# Bellek sızıntısı kontrolü
valgrind --leak-check=full ./philo 4 410 200 200
```

---

## Özel Durumlar

### 1. Tek Filozof Durumu
```c
void one_philo_handle(t_philo *philo)
{
    if (philo->data->philo_count == 1)
    {
        pthread_mutex_lock(philo->left_fork);
        print(philo, "has taken a fork");
        pthread_mutex_unlock(philo->left_fork);
        // Ölüm flag'i set et
        pthread_mutex_lock(&philo->data->death_mutex);
        philo->data->is_dead = 1;
        pthread_mutex_unlock(&philo->data->death_mutex);
    }
}
```
- **Problem**: Tek çatalla yemek yenemiyor
- **Çözüm**: Çatalı al, bırak ve ölüm flag'i set et

### 2. Deadlock Önleme (mandotary.c)
```c
void philo_take_fork(t_philo *philo)
{
    if (philo->identity % 2 == 0)  // Çift ID
    {
        pthread_mutex_lock(philo->left_fork);
        philo->left_fork_bool = 1;
        print(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        philo->right_fork_bool = 1;
        print(philo, "has taken a fork");
    }
    else  // Tek ID
    {
        pthread_mutex_lock(philo->right_fork);
        philo->right_fork_bool = 1;
        print(philo, "has taken a fork");
        usleep(20);  // Kısa gecikme
        pthread_mutex_lock(philo->left_fork);
        philo->left_fork_bool = 1;
        print(philo, "has taken a fork");
    }
}
```
- **Strateji**: Çift ve tek ID'li filozoflar farklı sırada çatal alır
- **Amaç**: Circular wait durumunu engeller

### 3. Timing Optimizasyonu (mandotary.c)
```c
void philo_thinking(t_philo *philo)
{
    print(philo, "is thinking");
    usleep(philo->data->time_to_die - (philo->data->time_to_eat
            + philo->data->time_to_sleep));
}
```
- **Hesaplama**: Maksimum güvenli düşünme süresi
- **Amaç**: Ölümü önleyici optimal timing

### 4. Senkronizasyon (philo_utils.c)
```c
// Tek ID'li filozoflar gecikme ile başlar
if (philo->identity % 2 != 0)
    ft_usleep(philo->data->time_to_eat, philo);
```
- **Amaç**: Başlangıçta çatal rekabetini azalt
- **Etki**: Daha stabil performans

---

## Sonuç ve Kullanım Notları

Bu **Philosophers** projesi karmaşık bir concurrent programming örneğidir. Ana öğeleri:

1. **Thread Management**: Birden fazla thread'in koordinasyonu
2. **Mutex Usage**: Race condition'ları önleme
3. **Deadlock Prevention**: Farklı kilitleme sıraları
4. **Resource Sharing**: Çatal paylaşımı
5. **Timing Control**: Hassas zaman kontrolü
6. **Error Handling**: Kapsamlı hata yönetimi

### ⚠️ Önemli Notlar

1. **Thread Sanitizer**: Kodda `-fsanitize=thread` aktif, data race'leri tespit eder
2. **Tek Filozof**: Özel durum, mutlaka ölür (çatalları paylaşamaz)
3. **Timing Kritik**: `ft_usleep` kullanımı hassas zamanlama için
4. **Deadlock**: Çift/tek ID stratejisi ile önlenir

### 🔧 Geliştirme İpuçları

1. **Debug**: Thread sanitizer çıktılarını takip et
2. **Test**: Edge case'leri mutlaka test et
3. **Performance**: Büyük filozof sayıları ile test et
4. **Memory**: Valgrind ile bellek sızıntılarını kontrol et

### 📁 Güncel Durum

- **Son Güncelleme**: Ağustos 2025
- **Aktif Dosyalar**: src/ klasöründeki 8 dosya
- **Test Durumu**: Tüm test senaryoları geçiyor
- **Norm Uygunluğu**: 42 Norm standartlarına uygun

Proje, sistem programlama ve concurrent programming konularında derinlemesine bilgi gerektirir ve thread-safe programlama prensiplerini gösterir.
